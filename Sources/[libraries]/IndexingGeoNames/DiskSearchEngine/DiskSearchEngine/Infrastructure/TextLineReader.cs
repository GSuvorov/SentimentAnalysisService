//#define _REALLOC_BUFFER_ON_FLY

using System;
using System.IO;
using System.Text;

namespace DiskSearchEngine
{
    /// <summary>
    /// 
    /// </summary>
    internal sealed class TextLineReader : IDisposable
    {
        #region [.field's.]
        private const int READ_BUFFER_SIZE_IN_BYTES = 1024;
        public const char N_CARRIAGE_TRANSFER = '\n';
        public const char R_CARRIAGE_TRANSFER = '\r';

        private BinaryReader _BinaryReader;

        private long _StreamLength;
        private long _StreamPosition;

        private Encoding _Encoding;
        private Decoder  _Decoder;

        private byte[] _ReadBuffer;
        private char[] _CharBuffer;
        private int    _CharBufferLen = 0;
        private int    _CharBufferPos = 0;
        private StringBuilder _Accumulator;

        private int _R_CarriageTransferLen;  //lenght of '\r'
        private int _N_CarriageTransferLen;  //lenght of '\n'
        private int _RN_CarriageTransferLen; //lenght of '\r\n'
        #endregion  
    
        #region [.ctor().]
        public TextLineReader( string fileFullName, Encoding fileEncoding )
            : this( fileFullName, fileEncoding, READ_BUFFER_SIZE_IN_BYTES )
        {            
        }

        public TextLineReader( string fileFullName, Encoding fileEncoding, int readBufferSizeInBytes )
        {
            if ( fileFullName.IsEmptyOrNull() )
                throw (new ArgumentNullException("fileFullName"));
            if ( fileEncoding == null )
                throw (new ArgumentNullException("fileEncoding"));
            if ( readBufferSizeInBytes < 0 )
                throw (new ArgumentException("readBufferSizeInBytes"));


            _BinaryReader = DataFileHelper.CreateBinaryReaderRandomAccess( fileFullName );
            _Encoding     = fileEncoding;
            _Decoder      = _Encoding.GetDecoder();
            _R_CarriageTransferLen  = _Encoding.GetBytes(new[] { R_CARRIAGE_TRANSFER }).Length;
            _N_CarriageTransferLen  = _Encoding.GetBytes(new[] { N_CARRIAGE_TRANSFER }).Length;
            _RN_CarriageTransferLen = _Encoding.GetBytes(new[] { R_CARRIAGE_TRANSFER, N_CARRIAGE_TRANSFER }).Length;

            if ( readBufferSizeInBytes <= 0 ) readBufferSizeInBytes = READ_BUFFER_SIZE_IN_BYTES;
            _ReadBuffer  = new byte[ readBufferSizeInBytes ];
            _CharBuffer  = new char[ _Encoding.GetMaxCharCount( _ReadBuffer.Length ) ];
            _Accumulator = new StringBuilder( _ReadBuffer.Length );            

            _StreamLength = _BinaryReader.BaseStream.Length;

            //If file-pointer in begin of file try trim possible encoding preamble
            _StreamPosition = _BinaryReader.StartsWithEncodingPreamble( _Encoding )
                            ? _Encoding.GetPreamble().Length : 0;
            SeekFromBegin( _StreamPosition );
        }
        #endregion

        #region [.public method's.]
        public long StreamPosition
        {
            //get { return (_BinaryReader.BaseStream.Position); }
            get { return (_StreamPosition); }
        }

        public bool EndOfStream
        {
            //get { return (_BinaryReader.BaseStream.Position == _Length); }
            get { return (_StreamPosition == _StreamLength); }
        }

        public void SeekFromBegin( long position )
        {
            _StreamPosition = position;
            _BinaryReader.BaseStream.Seek( position, SeekOrigin.Begin );

            ClearReadBuffer();
        }

        public string ReadLine4Indexing()
        {
            return (ReadLine());
        }

        public string ReadLine4Searching()
        {
            return (ReadLineWithoutStreamPositionCalculate());
        }
        #endregion

        #region [.private method's.]
        private void SetEndOfStream()
        {
            _StreamPosition = _StreamLength;
        }
        /*private bool EndOfStreamInternal
        {
            get { return (_BinaryReader.BaseStream.Position == _StreamLength); }
        }*/
        private bool ReadBufferIsEmpty
        {
            get { return (_CharBufferPos == _CharBufferLen); }
        }
        private bool ReadDiskDataIntoBuffer()
        {
            int readBytesCount = _BinaryReader.Read( _ReadBuffer, 0, _ReadBuffer.Length );

            _CharBufferPos = 0;
            _CharBufferLen = _Decoder.GetChars( _ReadBuffer, 0, readBytesCount, _CharBuffer, 0 );
            //_Decoder.Reset();

            return (_CharBufferLen > 0);
        }
        private void ClearReadBuffer()
        {
            _CharBufferLen = 0;
            _CharBufferPos = 0;

            //Very important! without this may be hallucy (hallucination)!
            _Decoder = _Encoding.GetDecoder();
        }  

        private string ReadLine()
        {
#if DEBUG
            var for_test_StreamPosition = _StreamPosition;
#endif

            //If no data in byte-buffer
            if ( ReadBufferIsEmpty )
            {
                //Read data from disk in bytes buffer
                if ( !ReadDiskDataIntoBuffer() )
                {
                    SetEndOfStream();
                    //No more data
                    return (null);
                }
            }

            //---_Accumulator.Length = 0;

            while ( true )
            {
                for ( int i = _CharBufferPos; i < _CharBufferLen; i++ )
                {
                    char ch = _CharBuffer[ i ];
                    switch ( ch )
                    {
                        //Carriage Transfer is found
                        case R_CARRIAGE_TRANSFER: // '\r'
                        case N_CARRIAGE_TRANSFER: // '\n'
                            var text = default(string);
                            if ( _Accumulator.Length != 0 )
                            {
                                _Accumulator.Append( _CharBuffer, _CharBufferPos, i - _CharBufferPos );

                                text = _Accumulator.ToString(); 
                                       _Accumulator.Length = 0;

                                _StreamPosition += _Encoding.GetBytes( text ).Length;
                            }
                            else
                            {
                                text = new string( _CharBuffer, _CharBufferPos, i - _CharBufferPos ); 

                                _StreamPosition += _Encoding.GetBytes( _CharBuffer, _CharBufferPos, i - _CharBufferPos ).Length;
                            }

                            _CharBufferPos = i + 1;
                            if ( ch == R_CARRIAGE_TRANSFER/*'\r'*/ ) 
                            {
                                if ( ((_CharBufferPos < _CharBufferLen) || ReadDiskDataIntoBuffer())
                                        &&
                                     (_CharBuffer[ _CharBufferPos ] == N_CARRIAGE_TRANSFER/*'\n'*/) )
                                {
                                    _CharBufferPos++;

                                    _StreamPosition += _RN_CarriageTransferLen; //"\r\n"
                                }
                                else
                                {
                                    _StreamPosition += _R_CarriageTransferLen; //"\r"
                                }
                            }
                            else
                            {
                                _StreamPosition += _N_CarriageTransferLen; //"\n"
                            }

#if DEBUG
                            CheckDebug( text, for_test_StreamPosition );
#endif

                            return (text);                       
                    }
                }

                //Carriage Transfer not found
                _Accumulator.Append( _CharBuffer, _CharBufferPos, _CharBufferLen - _CharBufferPos );

               //Read data from disk in bytes buffer
                if ( !ReadDiskDataIntoBuffer() )
                {
                    SetEndOfStream();
                    //No more data
                    var text = _Accumulator.ToString();
                               _Accumulator.Length = 0;

                    _StreamPosition += _Encoding.GetBytes( text ).Length;

#if DEBUG
                    CheckDebug( text, for_test_StreamPosition );
#endif

                    return (text);
                }

                //if end of stream => return null & set position to equal lenght
                /*?!?if ( EndOfStreamInternal )
                {
                    SetEndOfStream();
                    return (null);
                }*/
            }
        }

        private string ReadLineWithoutStreamPositionCalculate()
        {
            //If no data in byte-buffer
            if ( ReadBufferIsEmpty )
            {
                //Read data from disk in bytes buffer
                if ( !ReadDiskDataIntoBuffer() )
                {
                    SetEndOfStream();
                    //No more data
                    return (null);
                }
            }

            _Accumulator.Length = 0;

            while ( true )
            {
                for ( int i = _CharBufferPos; i < _CharBufferLen; i++ )
                {
                    char ch = _CharBuffer[ i ];
                    switch ( ch )
                    {
                        //Carriage Transfer is found
                        case R_CARRIAGE_TRANSFER: // '\r'
                        case N_CARRIAGE_TRANSFER: // '\n'
                            var charBufferPos = _CharBufferPos;

                            _CharBufferPos = i + 1;
                            if ( ch == R_CARRIAGE_TRANSFER/*'\r'*/ ) 
                            {
                                if ( ((_CharBufferPos < _CharBufferLen) || ReadDiskDataIntoBuffer())
                                        &&
                                     (_CharBuffer[ _CharBufferPos ] == N_CARRIAGE_TRANSFER/*'\n'*/) )
                                {
                                    _CharBufferPos++;
                                }
                            }

                            if ( _Accumulator.Length != 0 )
                            {
                                _Accumulator.Append( _CharBuffer, charBufferPos, i - charBufferPos );

                                return (_Accumulator.ToString()); 
                            }
                            else
                            {
                                return (new string( _CharBuffer, charBufferPos, i - charBufferPos )); 
                            }
                    }
                }

                //Carriage Transfer not found
                _Accumulator.Append( _CharBuffer, _CharBufferPos, _CharBufferLen - _CharBufferPos );

               //Read data from disk in bytes buffer
                if ( !ReadDiskDataIntoBuffer() )
                {
                    SetEndOfStream();
                    //No more data
                    return (_Accumulator.ToString());
                }

                //if end of stream => return null & set position to equal lenght
                /*?!?if ( EndOfStreamInternal )
                {
                    SetEndOfStream();
                    return (null);
                }*/
            }
        }

#if DEBUG
        private void CheckDebug( string valueToCheck, long for_test_StreamPosition )
        {
            var pos = _BinaryReader.BaseStream.Position;
            _BinaryReader.BaseStream.Position = for_test_StreamPosition;

            byte[] temp = new byte[ _Encoding.GetBytes( valueToCheck ).Length ];
            int readBytes = _BinaryReader.BaseStream.Read( temp, 0, temp.Length );

            var text = _Encoding.GetString( temp, 0, readBytes );

            //System.Diagnostics.Debug.Assert( text == valueToCheck );
            if ( text != valueToCheck )
                System.Diagnostics.Debugger.Break();

            _BinaryReader.BaseStream.Position = pos;
        }
#endif
        #endregion

        #region [.IDisposable.]
        public void Dispose()
        {
            if ( _BinaryReader != null )
            {
                _BinaryReader.BaseStream.Dispose();
                _BinaryReader = null;
            }
        }
        #endregion
    }
}
