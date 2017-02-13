using System;
using System.IO;

namespace DiskSearchEngine
{
    /// <summary>
    /// 
    /// </summary>
    internal static class DataFileHelper
    {
        private const int READ_BUFFER_SIZE_IN_BYTES = 0x1000;

        /*public static BinaryReader CreateBinaryReader( string dataFileFullName, Encoding dataFileEncoding  )
        {
            return 
            (
                new BinaryReader
                ( 
                    new StreamReader( dataFileFullName, dataFileEncoding ).BaseStream 
                )
            );
        }

        public static BinaryReader CreateBinaryReaderSequentialScan( string dataFileFullName )
        {
            return 
            (
                new BinaryReader
                ( 
                    new FileStream
                    ( 
                        dataFileFullName, 
                        FileMode.Open, 
                        FileAccess.Read, 
                        FileShare.None, 
                        Consts.READ_WRITE_BUFFER_SIZE, 
                        FileOptions.SequentialScan 
                    )
                )
            );
        }*/

        public static BinaryReader CreateBinaryReaderRandomAccess( string dataFileFullName )
        {
            return 
            (
                new BinaryReader
                ( 
                    new FileStream
                    ( 
                        dataFileFullName, 
                        FileMode.Open, 
                        FileAccess.Read, 
                        FileShare.Read,
                        READ_BUFFER_SIZE_IN_BYTES, 
                        FileOptions.RandomAccess 
                    )
                )
            );
        }

        public static Int64 GetFileSize( string dataFileFullName )
        {
            return ((new FileInfo( dataFileFullName )).Length);
        }
    }
}
