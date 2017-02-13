using System.IO;
using System.Text;

namespace DiskSearchEngine
{
    /// <summary>
    /// 
    /// </summary>
    internal static class IndexFileHelper
    {
        private const int WRITE_BUFFER_SIZE_IN_BYTES = 0x1000;

        public const string INDEX_FILE_SIGN = "FuskingRotter";

        public const string INDEX_FILE_EXTENSION = "index";

        public static readonly Encoding INDEX_FILE_ENCODING = Encoding.ASCII; //UTF8;

        public static string GetIndexFileFullName( string dataFileFullName )
        {
            return (Path.ChangeExtension( dataFileFullName, IndexFileHelper.INDEX_FILE_EXTENSION ));
        }

        public static uint HashFunction( string value, uint hashTableSize )
        {
		    uint __hash = 0;
		    foreach ( var _ch in value )
		    {
			    __hash = (123 * __hash + _ch) % hashTableSize;
		    }
		    return (__hash);
        }
        /*public static uint HashFunction( ref string value, uint hashTableSize )
        {
            uint hash = 0;
            foreach ( var _ch in value )
            {
                hash = (123 * hash + _ch);
            }

            // The .NET framework tends to produce pretty bad hash codes.
            // Scramble them up to be much more random!
            hash += ~(hash << 15);
            hash ^= (hash >> 10);
            hash += (hash << 3);
            hash ^= (hash >> 6);
            hash += ~(hash << 11);
            hash ^= (hash >> 16);
            hash &= 0x7FFFFFFF;
            if (hash == 0)
                hash = 0x7FFFFFFF;     // Make sure it isn't zero.

            return (hash % hashTableSize);
        }*/

        /*public static BinaryWriter CreateBinaryWriter( string indexFileFullName )
        {
            return 
            (
                new BinaryWriter
                ( 
                    new StreamWriter( indexFileFullName, false, Consts.INDEX_FILE_ENCODING ).BaseStream 
                )
            );
        }
        public static BinaryReader CreateBinaryReader( string indexFileFullName )
        {
            return 
            (
                new BinaryReader
                ( 
                    new StreamReader( indexFileFullName, Consts.INDEX_FILE_ENCODING ).BaseStream 
                )
            );
        }*/

        public static BinaryWriter CreateBinaryWriterRandomAccess( string indexFileFullName )
        {
            var fs = new FileStream
            (
                indexFileFullName,
                FileMode.OpenOrCreate,
                FileAccess.Write,
                FileShare.None,
                WRITE_BUFFER_SIZE_IN_BYTES,
                FileOptions.RandomAccess
            );

            fs.SetLength( 0 );

            return (new BinaryWriter( fs ));

            /*return 
            (
                new BinaryWriter
                ( 
                    new FileStream
                    (
                        indexFileFullName, 
                        FileMode.OpenOrCreate, 
                        FileAccess.Write, 
                        FileShare.None,
                        WRITE_BUFFER_SIZE_IN_BYTES, 
                        FileOptions.RandomAccess 
                    )
                )
            );*/
        }

        public static BinaryReader CreateBinaryReaderSequentialScan( string indexFileFullName )
        {
            return 
            (
                new BinaryReader
                ( 
                    new FileStream
                    (
                        indexFileFullName, 
                        FileMode.Open, 
                        FileAccess.Read, 
                        FileShare.Read,
                        WRITE_BUFFER_SIZE_IN_BYTES, 
                        FileOptions.SequentialScan 
                    )
                )
            );
        }
        public static BinaryReader CreateBinaryReaderRandomAccess  ( string indexFileFullName )
        {
            return 
            (
                new BinaryReader
                ( 
                    new FileStream
                    (
                        indexFileFullName, 
                        FileMode.Open, 
                        FileAccess.Read, 
                        FileShare.Read,
                        WRITE_BUFFER_SIZE_IN_BYTES,
                        FileOptions.RandomAccess 
                    )
                )
            );
        }

        public static FileStream CreateFileStreamReadWrite( string indexFileFullName )
        {
            var fs = new FileStream
            (
                indexFileFullName,
                FileMode.OpenOrCreate,
                FileAccess.ReadWrite,
                FileShare.None,
                WRITE_BUFFER_SIZE_IN_BYTES,
                FileOptions.RandomAccess
            );

            fs.SetLength( 0 );

            return (fs);
        }

        /*public static void Serialize( this BinaryWriter indexFileBinaryWriter, object obj )
        {
            Consts.Formatter.Serialize( indexFileBinaryWriter.BaseStream, obj );
        }
        public static T Deserialize< T >( this BinaryReader indexFileBinaryReader )
        {
            return ((T) Consts.Formatter.Deserialize( indexFileBinaryReader.BaseStream ));
        }*/
    }
}
