using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

using DiskSearchEngine.Hashtable;

using fpos_t = System.Int64;

namespace DiskSearchEngine
{
    /// <summary>
    /// 
    /// </summary>
    internal static class Extensions
    {
        public static bool IsEmptyOrNull( this string value )
        {
            return (string.IsNullOrEmpty( value ));
        }
        public static string InSingleQuote( this string value )
        {
            if ( value.IsEmptyOrNull() )
                return (value);

            return ('\'' + value + '\'');
        }

        public static void ThrowIfEmptyOrNull( this string value, string paramName )
        {
            if ( value.IsEmptyOrNull() )
                throw (new ArgumentNullException(paramName));
        }
        public static void ThrowIfNull( this object value, string paramName )
        {
            if ( value == null )
                throw (new ArgumentNullException(paramName));
        }
        
        public static void IncremetByKey( this Dictionary< int, int > dictionary, int key )
        {
            int value = 0;
            if ( dictionary.TryGetValue( key, out value ) )
            {
                dictionary[ key ] = ++value;
            }
            else
            {
                dictionary.Add( key, 1 );
            }
        }

        public static bool StartsWithEncodingPreamble( this BinaryReader binaryReader, Encoding encoding )
        {
            byte[] preamble = encoding.GetPreamble();
            if ( preamble.Length == 0 )
                return (false);

            byte[] buffer = new byte[ preamble.Length ];

            int readBytes = binaryReader.Read( buffer, 0, buffer.Length );

            if ( preamble.Length != readBytes )
                return (false);

            for ( int i = 0; i < readBytes; i++ )
            {
                if ( preamble[ i ] != buffer[ i ] )
                    return (false);
            }

            return (true);
        }

        public static void SeekFromBegin( this BinaryWriter binaryWriter, fpos_t position )
        {
            binaryWriter.BaseStream.Seek( position, SeekOrigin.Begin );
        }
        public static void SeekFromBegin( this BinaryReader binaryReader, fpos_t position )
        {
            binaryReader.BaseStream.Seek( position, SeekOrigin.Begin );
        }
        public static void SeekFromBegin( this FileStream fileStream    , fpos_t position )
        {
            fileStream.Seek( position, SeekOrigin.Begin );
        }
        public static void SeekFromCurrent( this FileStream fileStream  , fpos_t position )
        {
            fileStream.Seek( position, SeekOrigin.Current );
        }

        public static void SeekAndWrite( this BinaryWriter binaryWriter, fpos_t position, byte[] buffer )
        {
            binaryWriter.SeekFromBegin( position );
            binaryWriter.Write        ( buffer   );
        }
        public static DiskSlot SeekAndReadDiskSlot( this BinaryReader binaryReader, fpos_t position, ReadBuffer< DiskSlot > readBuffer )
        {
            binaryReader.SeekFromBegin( position );

            return (binaryReader.ReadDiskSlot( readBuffer ));
        }
        public static DiskTag  SeekAndReadDiskTag ( this BinaryReader binaryReader, fpos_t position, ReadBuffer< DiskTag > readBuffer )
        {
            binaryReader.SeekFromBegin( position );

            return (binaryReader.ReadDiskTag( readBuffer ));
        }
        public static DiskSlotInt32 SeekAndReadDiskSlotInt32( this BinaryReader binaryReader, fpos_t position, ReadBuffer< DiskSlotInt32 > readBuffer )
        {
            binaryReader.SeekFromBegin( position );

            return (binaryReader.ReadDiskSlotInt32( readBuffer ));
        }
        public static DiskTagInt32  SeekAndReadDiskTagInt32 ( this BinaryReader binaryReader, fpos_t position, ReadBuffer< DiskTagInt32 > readBuffer )
        {
            binaryReader.SeekFromBegin( position );

            return (binaryReader.ReadDiskTagInt32( readBuffer ));
        }
        public static string SeekAndRead4Searching( this TextLineReader textLineReader, fpos_t position )
        {
            textLineReader.SeekFromBegin( position );

            return (textLineReader.ReadLine4Searching());
        }

        public static DiskSlot ReadDiskSlot( this BinaryReader binaryReader, ReadBuffer< DiskSlot > readBuffer )
        {
            var result = binaryReader.Read( readBuffer.Buffer, 0, readBuffer.Size );
            if ( result != readBuffer.Size )
                throw (new DiskSearchEngineException("Error reading [DiskSlot] in file (result != buffer.Length)."));

            return (readBuffer.ConvertFromBuffer());
        }
        public static DiskTag  ReadDiskTag ( this BinaryReader binaryReader, ReadBuffer< DiskTag > readBuffer )
        {
            var result = binaryReader.Read( readBuffer.Buffer, 0, readBuffer.Size );
            if ( result != readBuffer.Size )
                throw (new DiskSearchEngineException("Error reading [DiskSlot] in file (result != buffer.Length)."));

            return (readBuffer.ConvertFromBuffer());
        }
        public static DiskSlotInt32 ReadDiskSlotInt32( this BinaryReader binaryReader, ReadBuffer< DiskSlotInt32 > readBuffer )
        {
            var result = binaryReader.Read( readBuffer.Buffer, 0, readBuffer.Size );
            if ( result != readBuffer.Size )
                throw (new DiskSearchEngineException("Error reading [DiskSlot] in file (result != buffer.Length)."));

            return (readBuffer.ConvertFromBuffer());
        }
        public static DiskTagInt32  ReadDiskTagInt32 ( this BinaryReader binaryReader, ReadBuffer< DiskTagInt32 > readBuffer )
        {
            var result = binaryReader.Read( readBuffer.Buffer, 0, readBuffer.Size );
            if ( result != readBuffer.Size )
                throw (new DiskSearchEngineException("Error reading [DiskSlot] in file (result != buffer.Length)."));

            return (readBuffer.ConvertFromBuffer());
        }

        public static byte[] StructureToByteArray( this object obj )
        {
            //BitConverter.GetBytes()

            int len = Marshal.SizeOf( obj );

            byte[] arr = new byte[ len ];

            IntPtr ptr = Marshal.AllocHGlobal( len );

            Marshal.StructureToPtr( obj, ptr, true );

            Marshal.Copy( ptr, arr, 0, len );

            Marshal.FreeHGlobal( ptr );

            return arr;

        }
        /*public static T ByteArrayToStructure< T >( byte[] buffer )
        {

            int len = Marshal.SizeOf( typeof(T) );

            IntPtr ptr = Marshal.AllocHGlobal( len );

            Marshal.Copy( buffer, 0, ptr, len );

            var obj = (T)Marshal.PtrToStructure( ptr, typeof(T) );

            Marshal.FreeHGlobal( ptr );

            return (obj);
        }*/
        /*public static DiskTag ByteArrayToDiskTag( byte[] buffer, IntPtr ptr )
        {
            //IntPtr ptr = Marshal.AllocHGlobal( DiskTag.SizeOf );

            Marshal.Copy( buffer, 0, ptr, DiskTag.SizeOf );

            var diskTag = (DiskTag) Marshal.PtrToStructure( ptr, typeof(DiskTag) );

            //Marshal.FreeHGlobal( ptr );

            return (diskTag);
        }
        public static DiskSlot ByteArrayToDiskSlot( byte[] buffer, IntPtr ptr )
        {
            //IntPtr ptr = Marshal.AllocHGlobal( DiskSlot.SizeOf );

            Marshal.Copy( buffer, 0, ptr, DiskSlot.SizeOf );

            var diskSlot = (DiskSlot) Marshal.PtrToStructure( ptr, typeof(DiskSlot) );

            //Marshal.FreeHGlobal( ptr );

            return (diskSlot);
        }*/

        public static void WriteBytes( this FileStream fileStream, byte[] buffer )
        {
            fileStream.Write( buffer, 0, buffer.Length );
        }
        public static DiskSlotInt32 SeekAndReadDiskSlotInt32( this FileStream fileStream, fpos_t position, ReadBuffer< DiskSlotInt32 > readBuffer )
        {
            fileStream.SeekFromBegin( position );

            return (fileStream.ReadDiskSlotInt32( readBuffer ));
        }
        public static DiskTagInt32  SeekAndReadDiskTagInt32 ( this FileStream fileStream, fpos_t position, ReadBuffer< DiskTagInt32  > readBuffer )
        {
            fileStream.SeekFromBegin( position );

            return (fileStream.ReadDiskTagInt32( readBuffer ));
        }
        public static DiskSlotInt32 ReadDiskSlotInt32( this FileStream fileStream, ReadBuffer< DiskSlotInt32 > readBuffer )
        {
            var result = fileStream.Read( readBuffer.Buffer, 0, readBuffer.Size );
            if ( result != readBuffer.Size )
                throw (new DiskSearchEngineException("Error reading [DiskSlot] in file (result != buffer.Length)."));

            return (readBuffer.ConvertFromBuffer());
        }
        public static DiskTagInt32  ReadDiskTagInt32 ( this FileStream fileStream, ReadBuffer< DiskTagInt32  > readBuffer )
        {
            var result = fileStream.Read( readBuffer.Buffer, 0, readBuffer.Size );
            if ( result != readBuffer.Size )
                throw (new DiskSearchEngineException("Error reading [DiskSlot] in file (result != buffer.Length)."));

            return (readBuffer.ConvertFromBuffer());
        }
        public static void SeekAndWrite( this FileStream fileStream, fpos_t position, byte[] buffer )
        {
            fileStream.SeekFromBegin( position );
            fileStream.WriteBytes   ( buffer   );
        }
        public static void SeekFromCurrentAndWrite( this FileStream fileStream, fpos_t position, byte[] buffer )
        {
            fileStream.SeekFromCurrent( position );
            fileStream.WriteBytes     ( buffer   );
        }
        
    }
}
