using System;
using System.IO;

namespace DiskSearchEngine.Hashtable
{
    /// <summary>
    /// 
    /// </summary>
    public sealed class HashtableSearcher : IDisposable
    {
        private delegate bool IsExistsFunction( string searchText );

        #region [.Private field's.]
        private TextLineReader _DataFileTextLineReader;
        private BinaryReader   _IndexFileBinaryReader;

        private ReadBuffer< DiskSlot > _DiskSlotReadBuffer;
        private ReadBuffer< DiskTag  > _DiskTagReadBuffer;

        private ReadBuffer< DiskSlotInt32 > _DiskSlotInt32ReadBuffer;
        private ReadBuffer< DiskTagInt32  > _DiskTagInt32ReadBuffer;

        private IsExistsFunction     _IsExistsFunction;
        private NormlizeTextFunction _NormlizeTextFunction;
        #endregion

        #region [.ctor().]
        public HashtableSearcher( string dataFileFullName, NormlizeTextFunction normlizeTextFunction = null )
        {
            if ( dataFileFullName.IsEmptyOrNull() )
                throw (new ArgumentNullException("dataFileFullName"));


            var indexFileFullName = IndexFileHelper.GetIndexFileFullName( dataFileFullName );

            this.IndexHeader = HashtableIndexer.GetIndexHeaderByIndexFile( indexFileFullName );

            if ( !this.IndexHeader.IsSignValid() )
                throw (new DiskSearchEngineException("Invalid sign of index file: '" + indexFileFullName + "'."));
            if ( !this.IndexHeader.IsCorrectSizeAndModifyTime( dataFileFullName ) )
                throw (new DiskSearchEngineException("Data file was modify after last indexing."));


            this._DataFileTextLineReader = new TextLineReader
                ( 
                dataFileFullName, 
                this.IndexHeader.DataFileEncoding,
                this.IndexHeader.DataRecordMaxBytesLenght + this.IndexHeader.DataFileEncoding.GetMaxByteCount( 10 )
                );
            this._IndexFileBinaryReader = IndexFileHelper.CreateBinaryReaderRandomAccess( indexFileFullName );

            this._DiskSlotReadBuffer = new ReadBuffer< DiskSlot >();
            this._DiskTagReadBuffer  = new ReadBuffer< DiskTag >();

            this._DiskSlotInt32ReadBuffer = new ReadBuffer< DiskSlotInt32 >();
            this._DiskTagInt32ReadBuffer  = new ReadBuffer< DiskTagInt32 >();

            this._NormlizeTextFunction = normlizeTextFunction;

            switch ( this.IndexHeader.HashtableSlotType )
            {
                case HashtableIndexFileHeader.SlotType.Int64:
                    this._IsExistsFunction = IsExistsDiskRoutine;
                break;

                case HashtableIndexFileHeader.SlotType.Int32:
                    this._IsExistsFunction = IsExistsDiskRoutineIn32;
                break;

                default:
                    throw (new ArgumentException("this.IndexHeader.HashtableSlotType"));
            }
        }
        #endregion

        public HashtableIndexFileHeader IndexHeader
        {
            get;
            private set;
        }

        public bool IsExists( string searchText )
        {
            if ( searchText.IsEmptyOrNull() )
                throw (new ArgumentNullException("searchText"));
            
            //return (IsExistsDiskRoutine( IndexFileHelper.NormlizeText( searchText ) ));
            return (_IsExistsFunction( searchText ) );
        }        

        //-private DiskTag diskTag = new DiskTag();
        private bool IsExistsDiskRoutine( string searchText )
        {
            //Normlize text if allowed
            if ( _NormlizeTextFunction != null )
            {
                searchText = _NormlizeTextFunction( searchText );
            }

            //Calculate hash-code
	        uint hashCode = IndexFileHelper.HashFunction( searchText, this.IndexHeader.HashtableSize );


            //Search in hashtable
            //Read structure [DiskHtRecord_t] from index file
            DiskSlot diskSlot = _IndexFileBinaryReader.SeekAndReadDiskSlot
                ( 
                this.IndexHeader.SizeOf + hashCode * DiskSlot.SizeOf, 
                _DiskSlotReadBuffer
                );

            //Free slot => search text not found
            if ( diskSlot.PositionInDataFile == Consts.FREE )
            {
                // == NOT FOUND
                return (false); /*"ни хуяшки нету :("*/
            } 

            //Set file-pointer in position into data-file
            //Read text from data-file
            var text = _DataFileTextLineReader.SeekAndRead4Searching( diskSlot.PositionInDataFile );
            //Normlize text if allowed
            if ( _NormlizeTextFunction != null )
            {
                text = _NormlizeTextFunction( text );
            }
            //Input text equal text in data-file => search text found (with first step)
            if ( text == searchText )
            {
                // == FOUND ! first step
                return (true);
            }


            //Search in hashtable tag chain's
            var diskTag = new DiskTag() { NextTagOffset = diskSlot.FirstTagOffset };
            while ( 0 < diskTag.NextTagOffset )
            {
                diskTag = _IndexFileBinaryReader.SeekAndReadDiskTag( diskTag.NextTagOffset, _DiskTagReadBuffer );

                text = _DataFileTextLineReader.SeekAndRead4Searching( diskTag.PositionInDataFile );
                //Normlize text if allowed
                if ( _NormlizeTextFunction != null )
                {
                    text = _NormlizeTextFunction( text );
                }
                if ( text == searchText )
                {
                    // == FOUND !
                    return (true);
                }
            }

            // == NOT FOUND
            return (false);
        }

        /// <summary>
        /// Int32 file-pointer version
        /// </summary>
        /// <param name="searchText"></param>
        /// <returns></returns>
        private bool IsExistsDiskRoutineIn32( string searchText )
        {
            //Normlize text if allowed
            if ( _NormlizeTextFunction != null )
            {
                searchText = _NormlizeTextFunction( searchText );
            }

            //Calculate hash-code
	        uint hashCode = IndexFileHelper.HashFunction( searchText, this.IndexHeader.HashtableSize );


            //Search in hashtable
            //Read structure [DiskHtRecord_t] from index file
            DiskSlotInt32 diskSlot = _IndexFileBinaryReader.SeekAndReadDiskSlotInt32
                ( 
                this.IndexHeader.SizeOf + hashCode * DiskSlotInt32.SizeOf, 
                _DiskSlotInt32ReadBuffer
                );

            //Free slot => search text not found
            if ( diskSlot.PositionInDataFile == Consts.FREE_INT32 )
            {
                // == NOT FOUND
                return (false); /*"ни хуяшки нету :("*/
            } 

            //Set file-pointer in position into data-file
            //Read text from data-file
            var text = _DataFileTextLineReader.SeekAndRead4Searching( diskSlot.PositionInDataFile );
            //Normlize text if allowed
            if ( _NormlizeTextFunction != null )
            {
                text = _NormlizeTextFunction( text );
            }
            //Input text equal text in data-file => search text found (with first step)
            if ( text == searchText )
            {
                // == FOUND ! first step
                return (true);
            }


            //Search in hashtable tag chain's
            DiskTagInt32 diskTag = new DiskTagInt32() { NextTagOffset = diskSlot.FirstTagOffset };
            while ( 0 < diskTag.NextTagOffset )
            {
                diskTag = _IndexFileBinaryReader.SeekAndReadDiskTagInt32( diskTag.NextTagOffset, _DiskTagInt32ReadBuffer );

                text = _DataFileTextLineReader.SeekAndRead4Searching( diskTag.PositionInDataFile );
                //Normlize text if allowed
                if ( _NormlizeTextFunction != null )
                {
                    text = _NormlizeTextFunction( text );
                }
                if ( text == searchText )
                {
                    // == FOUND !
                    return (true);
                }
            }

            // == NOT FOUND
            return (false);
        }

        #region [.IDisposable.]
        public void Dispose()
        {
            if (_DataFileTextLineReader != null)
            {
                _DataFileTextLineReader.Dispose();
                _DataFileTextLineReader = null;
            }

            if (_IndexFileBinaryReader != null)
            {
                _IndexFileBinaryReader.BaseStream.Dispose();
                _IndexFileBinaryReader = null;
            }

            if ( _DiskSlotReadBuffer != null )
            {
                _DiskSlotReadBuffer.Dispose();
                _DiskSlotReadBuffer = null;
            }
            if ( _DiskTagReadBuffer != null )
            {
                _DiskTagReadBuffer.Dispose();
                _DiskTagReadBuffer = null;
            }

            if ( _DiskSlotInt32ReadBuffer != null )
            {
                _DiskSlotInt32ReadBuffer.Dispose();
                _DiskSlotInt32ReadBuffer = null;
            }
            if ( _DiskTagInt32ReadBuffer != null )
            {
                _DiskTagInt32ReadBuffer.Dispose();
                _DiskTagInt32ReadBuffer = null;
            }
        }
        #endregion
    }
}
