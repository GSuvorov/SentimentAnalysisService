using System;
using System.Text;
using System.Threading;

using fpos_t = System.Int64;

namespace DiskSearchEngine.Hashtable
{
    /// <summary>
    /// 
    /// </summary>
    public static class HashtableIndexer
    {
        private static readonly object _SyncRoot = new object();

        public static HashtableIndexFileHeader GetIndexHeaderByDataFile( string dataFileFullName )
        {
            return (GetIndexHeaderByIndexFile( IndexFileHelper.GetIndexFileFullName( dataFileFullName ) ));
        }
        public static HashtableIndexFileHeader GetIndexHeaderByIndexFile( string indexFileFullName )
        {
            if ( indexFileFullName.IsEmptyOrNull() )
                throw (new ArgumentNullException("indexFileFullName"));


            using ( var indexFileBinaryReader = IndexFileHelper.CreateBinaryReaderSequentialScan( indexFileFullName ) )
            {
                indexFileBinaryReader.SeekFromBegin( 0 );

                HashtableIndexFileHeader indexHeader = HashtableIndexFileHeader.DeserializeIndexHeader( indexFileBinaryReader );

                if ( !indexHeader.IsSignValid() )
                    throw (new DiskSearchEngineException("Invalid sign of index file: '" + indexFileFullName + "'."));

                return (indexHeader);
            }
        }

        /// <summary>
        /// Int64 file-pointer version
        /// </summary>
        /// <param name="dataFileFullName"></param>
        /// <param name="dataFileEncoding"></param>
        /// <param name="hashTableSize"></param>
        public static void BuildIndex( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize, NormlizeTextFunction normlizeTextFunction = null )
        {
            dataFileFullName.ThrowIfEmptyOrNull("dataFileFullname");
            dataFileFullName.ThrowIfNull       ("dataFileEncoding");
            if ( hashTableSize <= 2 ) throw (new ArgumentException("hashTableSize"));


            if ( !Monitor.TryEnter( _SyncRoot ) )
            {
                throw (new DiskSearchEngineException("Already run in other thread."));
            }

            try
            {
                BuildIndexInMemory( dataFileFullName, dataFileEncoding, hashTableSize, normlizeTextFunction ); //.RoundBeforeGreaterBorder() );
            }
            finally
            {
                Monitor.Exit( _SyncRoot );
            }
        }

        /// <summary>
        /// Int64 file-pointer version
        /// </summary>
        /// <param name="dataFileFullName"></param>
        /// <param name="dataFileEncoding"></param>
        /// <param name="hashTableSize"></param>
        private static void BuildIndexInMemory( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize, NormlizeTextFunction normlizeTextFunction )
        {
            //
            HashtableIndexFileHeader indexHeader = new HashtableIndexFileHeader
                (
                dataFileFullName,
                dataFileEncoding, 
                hashTableSize 
                );
            //
            MemorySlot[] hashTable = new MemorySlot[ indexHeader.HashtableSize ];
            
            //
            using ( var dataFileTextLineReader = new TextLineReader( indexHeader.DataFileFullName, indexHeader.DataFileEncoding ) )
            {

            #region [.2 calc index in memory.]
                int dataRecordCount     = 0;
                int dataRecordMaxLenght = 0;
		        while ( !dataFileTextLineReader.EndOfStream )
		        {
                    fpos_t position = dataFileTextLineReader.StreamPosition;
                
			        var text = dataFileTextLineReader.ReadLine4Indexing();
			        if ( text.IsEmptyOrNull() )	
                    {
                        continue;
                    }
                    //Normlize text if allowed
                    if ( normlizeTextFunction != null )
                    {
                        text = normlizeTextFunction( text );

			            if ( text.IsEmptyOrNull() )	
                        {
                            continue;
                        }
                    }

			        uint hashCode = IndexFileHelper.HashFunction( ref text, indexHeader.HashtableSize );

			        MemorySlot memorySlot = hashTable[ hashCode ];
                    if ( memorySlot == null )
                    {
                        hashTable[ hashCode ] = new MemorySlot( position );
                    }
			        else
			        {
				        MemoryTag newMemoryTag = new MemoryTag( position );

				        if ( memorySlot.FirstMemoryTag == null )
				        {
					        memorySlot.FirstMemoryTag = newMemoryTag;
				        }
				        else
				        {
                            MemoryTag memoryTag = memorySlot.FirstMemoryTag;
					        while ( memoryTag.NextMemoryTag != null )
					        {
						        memoryTag = memoryTag.NextMemoryTag;
					        }
					        memoryTag.NextMemoryTag = newMemoryTag;
				        }
			        }

                    dataRecordCount++;
                    
                    if ( dataRecordMaxLenght < text.Length ) dataRecordMaxLenght = text.Length; //dataRecordMaxLenght = Math.Max( dataRecordMaxLenght, text.Length );
		        }

                indexHeader.SetDataRecordCount    ( dataRecordCount );
                indexHeader.SetDataRecordMaxBytesLenght( indexHeader.DataFileEncoding.GetMaxByteCount( dataRecordMaxLenght ) );

            #endregion

            }

        #region [.Calulate Tag collision statistica.]
            foreach ( var memorySlot in hashTable ) 
            {
                if ( memorySlot == null )
                {
                    //empty slot's in hash-table
                    indexHeader.TagCollisionStatistica.IncremetByKey( -1 );
                }
                else
                {
                    //Current tag chain depth
                    int currentTagChainDepth = 0;

                    //Get first memory-tag
		            MemoryTag memoryTag = memorySlot.FirstMemoryTag;
		            while ( memoryTag != null ) 
		            {	
                        //Get next tag
			            memoryTag = memoryTag.NextMemoryTag;

                        //Current tag chain depth
                        currentTagChainDepth++;
		            }

                    //
                    indexHeader.TagCollisionStatistica.IncremetByKey( currentTagChainDepth );
                }
            }
        #endregion
            
            //
            using ( var indexFileBinaryWriter = IndexFileHelper.CreateBinaryWriterRandomAccess( indexHeader.GetIndexFileFullName() ) )
            {

		    #region [.3 write hash table on disk.]

		        DiskSlot diskSlot = new DiskSlot();
                DiskTag  diskTag  = new DiskTag();
                byte[] emptyDiskSlotBytes = DiskSlot.GetEmptyDiskSlot().StructureToByteArray();		        

                //Write header in index file
                indexFileBinaryWriter.SeekFromBegin( 0 );
                indexHeader.SerializeIndexHeader( indexFileBinaryWriter );

                fpos_t tagAreaOffset = indexHeader.SizeOf + indexHeader.HashtableSize * DiskSlot.SizeOf;

                uint hashTableItemIndex = 0;
		        foreach ( var memorySlot in hashTable ) 
		        {
                    //Empty hash-table slot
                    if ( memorySlot == null )
                    {
                        //Write in index-file empty slot
                        indexFileBinaryWriter.SeekAndWrite
                            ( 
                            indexHeader.SizeOf + hashTableItemIndex * DiskSlot.SizeOf,
                            emptyDiskSlotBytes 
                            );
                    }
                    //Have a data hash-table slot
                    else
                    {
                        //Calc data-slot properties
			            diskSlot.PositionInDataFile =  memorySlot.PositionInDataFile; 
                        diskSlot.FirstTagOffset     = ( memorySlot.FirstMemoryTag != null ) ? tagAreaOffset : 0;

                        //Write in index-file data-slot
                        indexFileBinaryWriter.SeekAndWrite
                            ( 
                            indexHeader.SizeOf + hashTableItemIndex * DiskSlot.SizeOf,
                            diskSlot.StructureToByteArray() 
                            );

                        //Calc Tag collision statistica
                        int currentTagChainDepth = 0;

                        //Get first memory-tag
			            MemoryTag memoryTag = memorySlot.FirstMemoryTag;
			            while ( memoryTag != null ) 
			            {	
                            //Calc disk-tag properties
				            diskTag.PositionInDataFile = memoryTag.PositionInDataFile;
                            diskTag.NextTagOffset      = ( memoryTag.NextMemoryTag != null ) ? tagAreaOffset + DiskTag.SizeOf : 0;
                            
                            //Write in index-file
                            indexFileBinaryWriter.SeekAndWrite
                                ( 
                                tagAreaOffset, 
                                diskTag.StructureToByteArray() 
                                );
                            //Calc disk-tag offset
                            tagAreaOffset += DiskTag.SizeOf;
                            //Get next tag
				            memoryTag = memoryTag.NextMemoryTag;

                            //Calc Tag collision statistica
                            currentTagChainDepth++;
			            }

                        /*
                        //Calulate Tag collision statistica
                        //---indexHeader.TagCollisionStatistica.IncremetByKey( currentTagChainDepth /*memoryHash.GetTagChainDepth()* / );
                        */
                    }

                    hashTableItemIndex++;
		        }

            #endregion

            }

            //Free memory
            hashTable = null;
            GC.Collect();
        }


        /// <summary>
        /// Int32 file-pointer version
        /// </summary>
        /// <param name="dataFileFullName"></param>
        /// <param name="dataFileEncoding"></param>
        /// <param name="hashTableSize"></param>
        public static void BuildIndexInt32( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize, NormlizeTextFunction normlizeTextFunction = null )
        {
            dataFileFullName.ThrowIfEmptyOrNull("dataFileFullname");
            dataFileFullName.ThrowIfNull       ("dataFileEncoding");
            if ( hashTableSize <= 2 ) throw (new ArgumentException("hashTableSize"));


            if ( !Monitor.TryEnter( _SyncRoot ) )
            {
                throw (new DiskSearchEngineException("Already run in other thread."));
            }

            try
            {
                BuildIndexInt32InMemory( dataFileFullName, dataFileEncoding, hashTableSize, normlizeTextFunction ); //.RoundBeforeGreaterBorder() );
            }
            finally
            {
                Monitor.Exit( _SyncRoot );
            }
        }

        /// <summary>
        /// Int32 file-pointer version
        /// </summary>
        /// <param name="dataFileFullName"></param>
        /// <param name="dataFileEncoding"></param>
        /// <param name="hashTableSize"></param>
        private static void BuildIndexInt32InMemory( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize, NormlizeTextFunction normlizeTextFunction )
        {
            //
            var indexHeader = new HashtableIndexFileHeader
                ( 
                dataFileFullName,
                dataFileEncoding, 
                hashTableSize, 
                HashtableIndexFileHeader.SlotType.Int32 
                );
            //
            var hashTable = new MemorySlotInt32[ indexHeader.HashtableSize ];

            //
            using ( var dataFileTextLineReader = new TextLineReader( indexHeader.DataFileFullName, indexHeader.DataFileEncoding ) )
            {
                #region [.2 calc index in memory.]
                int dataRecordCount     = 0;
                int dataRecordMaxLenght = 0;
		        while ( !dataFileTextLineReader.EndOfStream )
		        {
                    Int64 _posInt64 = dataFileTextLineReader.StreamPosition;

                    //Guard from file-pointer > MemorySlotInt32.MaxValue
                    if ( _posInt64 > MemorySlotInt32.MaxValue )
                        throw (new InvalidOperationException("file-pointer more then allow possible value => file-pointer: " + _posInt64 + ", possible value: " + MemorySlotInt32.MaxValue));

                    Int32 position = (Int32) _posInt64;
                
			        var text = dataFileTextLineReader.ReadLine4Indexing();
			        if ( text.IsEmptyOrNull() )	
                    {
                        continue;
                    }
                    //Normlize text if allowed
                    if ( normlizeTextFunction != null )
                    {
                        text = normlizeTextFunction( text );

			            if ( text.IsEmptyOrNull() )	
                        {
                            continue;
                        }
                    }

			        uint hashCode = IndexFileHelper.HashFunction( ref text, indexHeader.HashtableSize );

			        MemorySlotInt32 memorySlot = hashTable[ hashCode ];
                    if ( memorySlot == null )
                    {
                        hashTable[ hashCode ] = new MemorySlotInt32( position );
                    }
			        else
			        {
				        MemoryTagInt32 newMemoryTag = new MemoryTagInt32( position );

				        if ( memorySlot.FirstMemoryTag == null )
				        {
					        memorySlot.FirstMemoryTag = newMemoryTag;
				        }
				        else
				        {
                            MemoryTagInt32 memoryTag = memorySlot.FirstMemoryTag;
					        while ( memoryTag.NextMemoryTag != null )
					        {
						        memoryTag = memoryTag.NextMemoryTag;
					        }
					        memoryTag.NextMemoryTag = newMemoryTag;
				        }
			        }

                    dataRecordCount++;
                    
                    if ( dataRecordMaxLenght < text.Length ) dataRecordMaxLenght = text.Length; //dataRecordMaxLenght = Math.Max( dataRecordMaxLenght, text.Length );
		        }

                indexHeader.SetDataRecordCount( dataRecordCount );
                indexHeader.SetDataRecordMaxBytesLenght( indexHeader.DataFileEncoding.GetMaxByteCount( dataRecordMaxLenght ) );
                #endregion
            }

            #region [.Calulate Tag collision statistica.]
            foreach ( var memorySlot in hashTable ) 
            {
                if ( memorySlot == null )
                {
                    //empty slot's in hash-table
                    indexHeader.TagCollisionStatistica.IncremetByKey( -1 );
                }
                else
                {
                    //Current tag chain depth
                    int currentTagChainDepth = 0;

                    //Get first memory-tag
		            MemoryTagInt32 memoryTag = memorySlot.FirstMemoryTag;
		            while ( memoryTag != null ) 
		            {	
                        //Get next tag
			            memoryTag = memoryTag.NextMemoryTag;

                        //Current tag chain depth
                        currentTagChainDepth++;
		            }

                    //
                    indexHeader.TagCollisionStatistica.IncremetByKey( currentTagChainDepth );
                }
            }
            #endregion

            //
            using ( var indexFileBinaryWriter = IndexFileHelper.CreateBinaryWriterRandomAccess( indexHeader.GetIndexFileFullName() ) )
            {
		        #region [.3 write hash table on disk.]

		        var diskSlot = new DiskSlotInt32();
                var  diskTag  = new DiskTagInt32();
                byte[] emptyDiskSlotBytes = DiskSlotInt32.GetEmptyDiskSlot().StructureToByteArray();

                //Write header in index file
                indexFileBinaryWriter.SeekFromBegin( 0 );
                indexHeader.SerializeIndexHeader( indexFileBinaryWriter );

                Int64 tagAreaOffset = (indexHeader.SizeOf + indexHeader.HashtableSize * DiskSlotInt32.SizeOf);

                //Guard from tagAreaOffset > MemorySlotInt32.MaxValue
                if (tagAreaOffset > MemorySlotInt32.MaxValue)
                    throw (new InvalidOperationException("tag area offset more then allow possible value => tag area offset: " + tagAreaOffset + ", possible value: " + MemorySlotInt32.MaxValue));

                #region [.iterate over memory-slot's.]
                uint hashTableItemIndex = 0;
		        foreach ( var memorySlot in hashTable ) 
		        {
                    //Empty hash-table slot
                    if ( memorySlot == null )
                    {
                        //Write in index-file empty slot
                        indexFileBinaryWriter.SeekAndWrite
                            ( 
                            indexHeader.SizeOf + hashTableItemIndex * DiskSlotInt32.SizeOf,
                            emptyDiskSlotBytes 
                            );
                    }
                    //Have a data hash-table slot
                    else
                    {
                        //Calc data-slot properties
			            diskSlot.PositionInDataFile =  memorySlot.PositionInDataFile; 
                        diskSlot.FirstTagOffset     = ( memorySlot.FirstMemoryTag != null ) ? (Int32) tagAreaOffset : 0;

                        //Write in index-file data-slot
                        indexFileBinaryWriter.SeekAndWrite
                            ( 
                            indexHeader.SizeOf + hashTableItemIndex * DiskSlotInt32.SizeOf,
                            diskSlot.StructureToByteArray() 
                            );

                        //Calc Tag collision statistica
                        int currentTagChainDepth = 0;

                        //Get first memory-tag
			            MemoryTagInt32 memoryTag = memorySlot.FirstMemoryTag;
			            while ( memoryTag != null ) 
			            {	
                            //Calc disk-tag properties
				            diskTag.PositionInDataFile = memoryTag.PositionInDataFile;
                            diskTag.NextTagOffset      = ( memoryTag.NextMemoryTag != null ) ? (Int32) tagAreaOffset + DiskTagInt32.SizeOf : 0;
                            
                            //Write in index-file
                            indexFileBinaryWriter.SeekAndWrite
                                ( 
                                tagAreaOffset, 
                                diskTag.StructureToByteArray() 
                                );
                            //Calc disk-tag offset
                            tagAreaOffset += DiskTagInt32.SizeOf;

                            //Guard from tagAreaOffset > MemorySlotInt32.MaxValue
                            if (tagAreaOffset > MemorySlotInt32.MaxValue)
                                throw (new InvalidOperationException("tag area offset more then allow possible value => tag area offset: " + tagAreaOffset + ", possible value: " + MemorySlotInt32.MaxValue));

                            //Get next tag
				            memoryTag = memoryTag.NextMemoryTag;

                            //Calc Tag collision statistica
                            currentTagChainDepth++;
			            }

                        /*
                        //Calulate Tag collision statistica
                        //---indexHeader.TagCollisionStatistica.IncremetByKey( currentTagChainDepth /*memoryHash.GetTagChainDepth()* / );
                        */
                    }

                    hashTableItemIndex++;
                }
                #endregion

                #endregion
            }

            //Free memory
            hashTable = null;
            GC.Collect();
        }


        //======================================================//
        public static void BuildIndexAutomatic( string dataFileFullName, Encoding dataFileEncoding, NormlizeTextFunction normlizeTextFunction = null )
        {
            dataFileFullName.ThrowIfEmptyOrNull("dataFileFullname");
            dataFileFullName.ThrowIfNull       ("dataFileEncoding");


            uint allRecordCount = HashtableEngineHelper.CalulateAllRecordCountInDataFile( dataFileFullName, dataFileEncoding );

            uint hashTableSize = allRecordCount.NearestPrimeNumber();

            BuildIndexAutomaticInternal( dataFileFullName, dataFileEncoding, hashTableSize, allRecordCount, normlizeTextFunction );
        }
        public static void BuildIndexAutomatic( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize, NormlizeTextFunction normlizeTextFunction = null )
        {
            dataFileFullName.ThrowIfEmptyOrNull("dataFileFullname");
            dataFileFullName.ThrowIfNull       ("dataFileEncoding");
            if ( hashTableSize <= 2 ) throw (new ArgumentException("hashTableSize"));

            /*
            Int64 fileSize = DataFileHelper.GetFileSize( dataFileFullName );

            uint maxPossibleRecordCount = (uint) (fileSize / (1 + 1)); /*1-char-on-data + 1-char-on-\n * /
            */

            uint maxPossibleRecordCount = hashTableSize;

            BuildIndexAutomaticInternal( dataFileFullName, dataFileEncoding, hashTableSize, maxPossibleRecordCount, normlizeTextFunction );
        }

        private static void BuildIndexAutomaticInternal( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize, uint allRecordCount, NormlizeTextFunction normlizeTextFunction )
        {
            Int64 fileSize = DataFileHelper.GetFileSize( dataFileFullName );

            var buildIndexAs32 = (fileSize < MemorySlotInt32.MaxValue);
            if ( buildIndexAs32 )
            {
                var maxPossibleOffsetInIndexFile = (hashTableSize  * DiskSlotInt32.SizeOf) +
                                                   (allRecordCount * DiskTagInt32 .SizeOf);

                buildIndexAs32 = (maxPossibleOffsetInIndexFile < MemorySlotInt32.MaxValue); 
            }


            if ( buildIndexAs32 )
            {
                BuildIndexInt32( dataFileFullName, dataFileEncoding, hashTableSize, normlizeTextFunction );
            }
            else
            {
                BuildIndex( dataFileFullName, dataFileEncoding, hashTableSize, normlizeTextFunction );
            }
        }

        //======================================================//
        public static void BuildIndexInt32OnDisk( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize, NormlizeTextFunction normlizeTextFunction = null )
        {
            //create header-of-disk-hashtable
            HashtableIndexFileHeader indexHeader = new HashtableIndexFileHeader
                ( 
                dataFileFullName,
                dataFileEncoding, 
                hashTableSize, 
                HashtableIndexFileHeader.SlotType.Int32 
                );

            //create index-file
            using ( var indexFileStream = IndexFileHelper.CreateFileStreamReadWrite( indexHeader.GetIndexFileFullName() ) )
            using ( var diskSlotInt32ReadBuffer = new ReadBuffer< DiskSlotInt32 >() )
            using ( var diskTagInt32ReadBuffer  = new ReadBuffer< DiskTagInt32  >() )
            {
                //Write header in index file
                indexFileStream.SeekFromBegin( 0 );
                indexHeader.SerializeIndexHeader( indexFileStream );

                Int64 tagAreaOffset = (indexHeader.SizeOf + indexHeader.HashtableSize * DiskSlotInt32.SizeOf);

                //Guard from tagAreaOffset > MemorySlotInt32.MaxValue
                if (tagAreaOffset > MemorySlotInt32.MaxValue)
                    throw (new InvalidOperationException("tag area offset more then allow possible value => tag area offset: " + tagAreaOffset + ", possible value: " + MemorySlotInt32.MaxValue));

                //fill disk-hashtable empty slot's
                byte[] emptyDiskSlotBytes = DiskSlotInt32.GetEmptyDiskSlot().StructureToByteArray();
                for ( var i = 0U; i < hashTableSize; i++ )
                {
                    indexFileStream.WriteBytes( emptyDiskSlotBytes );
                }

                DiskTagInt32 diskTag = new DiskTagInt32();
                
                //open data-file
                using ( var dataFileTextLineReader = new TextLineReader( indexHeader.DataFileFullName, indexHeader.DataFileEncoding ) )
                {
                #region [.build index on disk.]
                    int dataRecordCount     = 0;
                    int dataRecordMaxLenght = 0;
		            while ( !dataFileTextLineReader.EndOfStream )
		            {
                        Int64 _posInt64 = dataFileTextLineReader.StreamPosition;

                        //Guard from file-pointer > MemorySlotInt32.MaxValue
                        if ( _posInt64 > MemorySlotInt32.MaxValue )
                            throw (new InvalidOperationException("file-pointer more then allow possible value => file-pointer: " + _posInt64 + ", possible value: " + MemorySlotInt32.MaxValue));

                        Int32 position = (Int32) _posInt64;
                    
			            var text = dataFileTextLineReader.ReadLine4Indexing();
			            if ( text.IsEmptyOrNull() )	
                        {
                            continue;
                        }
                        //Normlize text if allowed
                        if ( normlizeTextFunction != null )
                        {
                            text = normlizeTextFunction( text );

			                if ( text.IsEmptyOrNull() )	
                            {
                                continue;
                            }
                        }

			            uint hashCode = IndexFileHelper.HashFunction( ref text, indexHeader.HashtableSize );

                    #region [..]
                        DiskSlotInt32 diskSlot = indexFileStream.SeekAndReadDiskSlotInt32
                            ( 
                            indexHeader.SizeOf + hashCode * DiskSlotInt32.SizeOf, 
                            diskSlotInt32ReadBuffer
                            );

                        //Free slot => search text not found
                        if ( diskSlot.PositionInDataFile == Consts.FREE_INT32 )
                        {
                            //Calc data-slot properties
			                diskSlot.PositionInDataFile = position; 
                            diskSlot.FirstTagOffset     = 0;
                            
                            //Write in index-file data-slot
                            indexFileStream.SeekFromCurrentAndWrite
                                (
                                -diskSlotInt32ReadBuffer.Size, 
                                 diskSlot.StructureToByteArray() 
                                );                            
                        }
                        else
                        {
                            if ( diskSlot.FirstTagOffset == 0 )
                            {
                                diskSlot.FirstTagOffset = (Int32) tagAreaOffset;

                                //ReWrite in index-file data-slot
                                indexFileStream.SeekFromCurrentAndWrite
                                    (
                                    -diskSlotInt32ReadBuffer.Size, 
                                     diskSlot.StructureToByteArray() 
                                    );  
                            }
                            else //if ( diskSlot.FirstTagOffset != 0 )
                            {
                                //Search in hashtable tag chain's
                                diskTag.NextTagOffset = diskSlot.FirstTagOffset;
                                while ( 0 < diskTag.NextTagOffset )
                                {
                                    diskTag = indexFileStream.SeekAndReadDiskTagInt32( diskTag.NextTagOffset, diskTagInt32ReadBuffer );
                                }

                                //Calc disk-tag properties
                                diskTag.PositionInDataFile = position;
                                diskTag.NextTagOffset      = (Int32) tagAreaOffset;

                                //ReWrite previous tag in chain
                                indexFileStream.SeekFromCurrentAndWrite
                                    (
                                    -diskTagInt32ReadBuffer.Size,
                                     diskTag.StructureToByteArray() 
                                    );
                            }

                            //Calc disk-tag properties
                            diskTag.PositionInDataFile = position;
                            diskTag.NextTagOffset      = 0;

                            //Write in index-file
                            indexFileStream.SeekAndWrite
                                ( 
                                tagAreaOffset, 
                                diskTag.StructureToByteArray() 
                                );

                            //Calc disk-tag offset
                            tagAreaOffset += DiskTagInt32.SizeOf;

                            //Guard from tagAreaOffset > MemorySlotInt32.MaxValue
                            if (tagAreaOffset > MemorySlotInt32.MaxValue)
                                throw (new InvalidOperationException("tag area offset more then allow possible value => tag area offset: " + tagAreaOffset + ", possible value: " + MemorySlotInt32.MaxValue));
                        }
                    #endregion

                        dataRecordCount++;
                        
                        if ( dataRecordMaxLenght < text.Length ) dataRecordMaxLenght = text.Length; //dataRecordMaxLenght = Math.Max( dataRecordMaxLenght, text.Length );
		            }

                    indexHeader.SetDataRecordCount( dataRecordCount );
                    indexHeader.SetDataRecordMaxBytesLenght( indexHeader.DataFileEncoding.GetMaxByteCount( dataRecordMaxLenght ) );

                #endregion
                }

                //one more time - Write header in index file
                indexFileStream.SeekFromBegin( 0 );
                indexHeader.SerializeIndexHeader( indexFileStream );
            }
        }
    }
}
