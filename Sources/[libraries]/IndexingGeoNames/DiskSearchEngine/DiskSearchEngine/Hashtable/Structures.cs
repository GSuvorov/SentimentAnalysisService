using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;

using fpos_t = System.Int64;

namespace DiskSearchEngine
{
    /// <summary>
    /// 
    /// </summary>
    internal static class Consts
    {
        public const fpos_t FREE = -1;

        public const Int32 FREE_INT32 = -1;
    }

    /// <summary>
    /// 
    /// </summary>
    public delegate string NormlizeTextFunction( string text );

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class IndexFileHeader
	{       
	    private	byte[] _SignBuffer;

        public string   DataFileFullName
        {
            get;
            private set;
        }
        public fpos_t   DataFileSize
        {
            get;
            private set;
        }
		public DateTime DataFileLastWriteTime
        {
            get;
            private set;
        }
        public Encoding DataFileEncoding
        {
            get;
            private set;
        }
		public int      DataRecordCount
        {
            get;
            private set;
        }
        public int      DataRecordMaxBytesLenght
        {
            get;
            private set;
        }

		private void SetSizeAndModifyTime( string dataFileFullName )
		{
            var fileInfo = new FileInfo( dataFileFullName );

            this.DataFileLastWriteTime = fileInfo.LastWriteTime;
            this.DataFileSize          = fileInfo.Length;
		}

        private IndexFileHeader()
        {
			this._SignBuffer = IndexFileHelper.INDEX_FILE_ENCODING.GetBytes( IndexFileHelper.INDEX_FILE_SIGN );
			for ( var i = 0; i < _SignBuffer.Length; i++ )
			{  
				this._SignBuffer[ i ] += 0xFF;
            }
        }
		internal IndexFileHeader( string dataFileFullName, Encoding dataFileEncoding ) : this()
		{
            if ( dataFileFullName.IsEmptyOrNull() )
            {
                throw (new ArgumentNullException("dataFileFullName"));    
            }

            this.DataFileFullName = dataFileFullName;
            this.DataFileEncoding = dataFileEncoding;
            this.SetSizeAndModifyTime( dataFileFullName );            
        }

        internal bool IsSignValid()
		{
			return (IndexFileHelper.INDEX_FILE_ENCODING.GetString( (new IndexFileHeader())._SignBuffer ) 
                        == 
                    IndexFileHelper.INDEX_FILE_ENCODING.GetString( this._SignBuffer ));
		}
		internal bool IsCorrectSizeAndModifyTime( string dataFileFullName )
		{
            var fileInfo = new FileInfo( dataFileFullName );

            return (fileInfo.LastWriteTime == this.DataFileLastWriteTime 
                        && 
                    fileInfo.Length == this.DataFileSize);
		}
        internal void SetDataRecordCount( int dataRecordCount )
        {
            this.DataRecordCount = dataRecordCount;
        }
        internal void SetDataRecordMaxBytesLenght( int dataRecordMaxBytesLenght )
        {
            this.DataRecordMaxBytesLenght = dataRecordMaxBytesLenght;
        }
        internal string GetIndexFileFullName()
        {
            return (IndexFileHelper.GetIndexFileFullName( this.DataFileFullName ));
        }
	}
}

namespace DiskSearchEngine.Hashtable
{
    /// <summary>
    /// Int64 file-pointer version
    /// </summary>
    internal sealed class MemorySlot
    {
	    public fpos_t    PositionInDataFile;
        public MemoryTag FirstMemoryTag;

	    public MemorySlot() : this( Consts.FREE )
        {
        }
	    public MemorySlot( fpos_t position ) 
        {
            PositionInDataFile = position;
            FirstMemoryTag     = null;
        }

        /*public int GetTagChainDepth()
        {
            if ( FirstMemoryTag == null )
                return (0);
            return (FirstMemoryTag.GetTagChainDepth());
        }*/
    }
    internal sealed class MemoryTag
    {        
        public fpos_t    PositionInDataFile;
        public MemoryTag NextMemoryTag;

	    public MemoryTag() : this( Consts.FREE )
        {
        }
	    public MemoryTag( fpos_t position ) 
        {
            PositionInDataFile = position;
            NextMemoryTag      = null;
        }

        /*public int GetTagChainDepth()
        {
            //return (1 + ((NextMemoryTag != null) ? NextMemoryTag.GetTagChainDepth() : 0));
            var count = 1;
            var tag = NextMemoryTag;
            while ( tag != null )
            {
                tag = tag.NextMemoryTag;
                count++;
            }
            return (count);
        }*/
    }

    internal struct DiskSlot
    {
        public fpos_t PositionInDataFile;
        public fpos_t FirstTagOffset;

        public static DiskSlot GetEmptyDiskSlot()
        {
            return (new DiskSlot() { FirstTagOffset = 0, PositionInDataFile = Consts.FREE });
        }

        public static readonly int SizeOf = Marshal.SizeOf( typeof(DiskSlot) );
    }
    internal struct DiskTag
    {
        public fpos_t PositionInDataFile;
        public fpos_t NextTagOffset;

        public static readonly int SizeOf = Marshal.SizeOf( typeof(DiskTag) );
    }

    /// <summary>
    /// Int32 file-pointer version
    /// </summary>
    internal sealed class MemorySlotInt32
    {
	    public Int32          PositionInDataFile;
        public MemoryTagInt32 FirstMemoryTag;

	    public MemorySlotInt32() : this( Consts.FREE_INT32 )
        {
        }
	    public MemorySlotInt32( Int32 position ) 
        {
            PositionInDataFile = position;
            FirstMemoryTag     = null;
        }

        public static Int32 MaxValue
        {
            get { return (Int32.MaxValue); }
        }
    }
    internal sealed class MemoryTagInt32
    {        
        public Int32          PositionInDataFile;
        public MemoryTagInt32 NextMemoryTag;

	    public MemoryTagInt32() : this( Consts.FREE_INT32 )
        {
        }
	    public MemoryTagInt32( Int32 position ) 
        {
            PositionInDataFile = position;
            NextMemoryTag      = null;
        }        
    }

    internal struct DiskSlotInt32
    {
        public Int32 PositionInDataFile;
        public Int32 FirstTagOffset;

        public static DiskSlotInt32 GetEmptyDiskSlot()
        {
            return (new DiskSlotInt32() { FirstTagOffset = 0, PositionInDataFile = Consts.FREE_INT32 });
        }

        public static readonly int SizeOf = Marshal.SizeOf( typeof(DiskSlotInt32) );
    }
    internal struct DiskTagInt32
    {
        public Int32 PositionInDataFile;
        public Int32 NextTagOffset;

        public static readonly int SizeOf = Marshal.SizeOf( typeof(DiskTagInt32) );
    }

    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="T"></typeparam>
    internal sealed class ReadBuffer< T > : IDisposable
    {
        public ReadBuffer()
        {
            this.Size = Marshal.SizeOf( typeof(T) );

            this.Buffer = new byte[ this.Size ];
            this.Ptr    = Marshal.AllocHGlobal( this.Size );
        }

        public byte[] Buffer
        {
            get;
            private set;
        }
        private IntPtr Ptr
        {
            get;
            set;
        }

        public int Size
        {
            get;
            private set;
        }

        public T ConvertFromBuffer()
        {
            Marshal.Copy( this.Buffer, 0, this.Ptr, this.Size );

            var obj = (T) Marshal.PtrToStructure( this.Ptr, typeof(T) );

            return (obj);
        }

    #region [.IDisposable.]
        public void Dispose()
        {
            if ( Buffer != null )
            {
                Buffer = null;
            }
            if ( Ptr != IntPtr.Zero )
            {
                Marshal.FreeHGlobal( Ptr );
                Ptr = IntPtr.Zero;
            }
        }
    #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public sealed class HashtableIndexFileHeader : IndexFileHeader
	{
        public enum SlotType
        {
            Int64, //default
            Default = Int64,
            Int32,
        }

        public uint HashtableSize
        {
            get;
            private set;
        }
        public SlotType HashtableSlotType
        {
            get;
            private set;
        }
        public Dictionary< int, int > TagCollisionStatistica
        {
            get;
            private set;
        }

        public HashtableIndexFileHeader( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize )
            : this( dataFileFullName, dataFileEncoding, hashTableSize, SlotType.Default )
        {
        }
		public HashtableIndexFileHeader( string dataFileFullName, Encoding dataFileEncoding, uint hashTableSize, SlotType slotType ) 
            : base( dataFileFullName, dataFileEncoding )
        {
            this.HashtableSize     = hashTableSize;
            this.HashtableSlotType = slotType;

            this.TagCollisionStatistica = new Dictionary< int, int >();

            this._SizeOf = GetSize4BinaryFormatter( Formatter );
        }

        [NonSerialized]
        private long _SizeOf;
        internal long SizeOf
        {
            get { return (_SizeOf); }
        }

        private long GetSize4BinaryFormatter( BinaryFormatter formatter )
        {
            using ( var ms = new MemoryStream() )
            {
                formatter.Serialize( ms, this );

                return (ms.Length);
            }
        }

        [NonSerialized]
        private static readonly BinaryFormatter Formatter = new BinaryFormatter()
        {
            Context = new StreamingContext( StreamingContextStates.File )
        };

        /*internal static void SerializeIndexHeader( BinaryWriter indexFileBinaryWriter, HashtableIndexFileHeader indexHeader )
        {
            Formatter.Serialize( indexFileBinaryWriter.BaseStream, indexHeader );
        }*/
        internal void SerializeIndexHeader( BinaryWriter indexFileBinaryWriter )
        {
            Formatter.Serialize( indexFileBinaryWriter.BaseStream, this );

            //Important! may changed after some member modify (such as [TagCollisionStatistica])
            this._SizeOf = this.GetSize4BinaryFormatter( Formatter );
        }
        internal void SerializeIndexHeader( FileStream indexFileStream )
        {
            Formatter.Serialize( indexFileStream, this );

            //Important! may changed after some member modify (such as [TagCollisionStatistica])
            this._SizeOf = this.GetSize4BinaryFormatter( Formatter );
        }
        public static HashtableIndexFileHeader DeserializeIndexHeader( BinaryReader indexFileBinaryReader )
        {
            var indexHeader = (HashtableIndexFileHeader) Formatter.Deserialize( indexFileBinaryReader.BaseStream );

            //Important! set only in .ctor() or here!
            indexHeader._SizeOf = indexHeader.GetSize4BinaryFormatter( Formatter );

            return (indexHeader);
        }

        public override string ToString()
        {
            var sb = new StringBuilder();

            sb.Append( "Data file                        : " + this.DataFileFullName.InSingleQuote()                    + Environment.NewLine );
            sb.Append( "Data file encoding               : " + this.DataFileEncoding.HeaderName.InSingleQuote()         + Environment.NewLine );
            sb.Append( "Data file last write time        : " + this.DataFileLastWriteTime.ToString().InSingleQuote()    + Environment.NewLine );
            sb.Append( "Data file size                   : " + this.DataFileSize.ToString().InSingleQuote()             + Environment.NewLine );
            sb.Append( "Data file record count           : " + this.DataRecordCount.ToString().InSingleQuote()          + Environment.NewLine );
            sb.Append( "Data file record max bytes lenght: " + this.DataRecordMaxBytesLenght.ToString().InSingleQuote() + Environment.NewLine );
            sb.Append( "Hash-table size                  : " + this.HashtableSize.ToString().InSingleQuote()            + Environment.NewLine );
            sb.Append( "Hash-table slot type             : " + this.HashtableSlotType.ToString().InSingleQuote()        + Environment.NewLine );
            sb.Append( "Tag collision statistica         : " + Environment.NewLine + 
                       "  ([tag chain depth] : [count]                )"  + Environment.NewLine + 
                       "  (             [-1] : [count of empty slot's])"  + Environment.NewLine + 
                       "  --------------------------------"  + Environment.NewLine );
            var max_len = TagCollisionStatistica.Any() ? TagCollisionStatistica.Max( _ => _.Key.ToString().Length ) : 0;
            foreach ( var _ in TagCollisionStatistica.OrderBy( _ => _.Key ) )
            {
                sb.Append( "    " + new string(' ', Math.Max( max_len, _.Key.ToString().Length ) - _.Key.ToString().Length ) + 
                            _.Key + " : " + _.Value + Environment.NewLine );
            }

            sb.Append( "  --------------------------------"  + Environment.NewLine );

            return (sb.ToString());
        }
	}
}
