using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

using Config = Logging.Properties.Settings;

namespace Logging
{
    /// <summary>
    /// Функция логирования в текстовый файл
    /// при первом обращении создается новый файл с именем - текущая дата и время
    /// </summary>
    public class Logger
    {
        private static Dictionary<string, NamedLogger> _loggerList = new Dictionary<string, NamedLogger>();

        private static NamedLogger _defaultLogger;
        private static string _defaultLoggerName = "";

        public static string DefaultLoggerName
        {
            get
            {
                return _defaultLoggerName;
            }
            set
            {
                _defaultLoggerName = value ?? "";
                _defaultLogger = new NamedLogger( _defaultLoggerName, null );
            }
        }
        public static NamedLogger DefaultLogger
        {
            get
            {
                if ( _defaultLogger == null )
                {
                    _defaultLogger = new NamedLogger( _defaultLoggerName, null );
                }
                return _defaultLogger;
            }
        }

        public static void Write( Exception ex )
        {
            DefaultLogger.Write( ex );
        }
        public static void WriteInfo( string format, params object[] list )
        {
            DefaultLogger.WriteInfo( format, list );
        }
        public static void WriteError( string format, params object[] list )
        {
            DefaultLogger.WriteError( format, list );
        }
        public static void Write( MessageLevel level, string format, params object[] list )
        {
            DefaultLogger.Write( level, format, list );
        }
        public static void WriteRaw( string format, params object[] list )
        {
            DefaultLogger.WriteRaw( format, list );
        }
        public static NamedLogger GetLogger( string name, Encoding encoding = null )
        {
            NamedLogger logger;
            lock ( _loggerList )
            {
                if ( !_loggerList.TryGetValue( name, out logger ) )
                {
                    logger = new NamedLogger( name, encoding );
                    _loggerList.Add( name, logger );
                }
            }
            return logger;
        }
        public static void CloseLogger( NamedLogger logger )
        {
            CloseLogger( logger.Name );
        }
        public static void CloseLogger( string name )
        {
            NamedLogger logger;
            lock ( _loggerList )
            {
                if ( _loggerList.TryGetValue( name, out logger ) )
                {
                    _loggerList.Remove( name );
                }
            }
            if ( logger != null )
            {
                logger.Close();
            }
        }
    }

    public class NamedLogger : IDisposable
    {
        public string Name;
        private StreamWriter _logFile;

        internal NamedLogger( string name, Encoding encoding )
        {
            this.Name = name;
            string dirName = Path.Combine( AppDomain.CurrentDomain.BaseDirectory, "logs" );
            if ( !Directory.Exists( dirName ) )
                Directory.CreateDirectory( dirName );

            while ( true )
            {
                string fileName = Path.Combine( dirName,
                    string.Format( "{0}{1}.log", name, DateTime.Now.ToString( "yyyy-MM-dd_HH-mm-ss" ) ) );
                if ( File.Exists( fileName ) )
                {
                    Thread.Sleep( 1000 );
                    continue;
                }
                try
                {
                    int enc;
                    if ( encoding == null )
                    {
                        encoding = int.TryParse( Config.Default.Encoding, out enc ) ?
                            Encoding.GetEncoding( enc ) :
                            Encoding.GetEncoding( Config.Default.Encoding );
                    }
                    _logFile = new StreamWriter(
                        new FileStream( fileName, FileMode.Append, FileAccess.Write, FileShare.Read ),
                        encoding );
                    _logFile.AutoFlush = true;
                }
                catch ( IOException )
                {
                    Thread.Sleep( 1000 );
                    continue;
                }
                break;
            }
        }

        public bool AutoFlush
        {
            get { return _logFile.AutoFlush; }
            set { _logFile.AutoFlush = value; }
        }
        public void Flush()
        {
            lock ( _logFile )
            {
                _logFile.Flush();
            }
        }
        public void Write( Exception ex )
        {
            string s = ex.ToString().Replace( "\r", "" ).Replace( "\n", "\r" );
            lock ( _logFile )
            {
                _logFile.WriteLine( "{0} {1}: {2}", DateTime.Now.ToString( "yyyy-MM-dd HH:mm:ss.fff" ), MessageLevel.ERROR, s );
            }
        }
        public void WriteInfo( string format, params object[] list )
        {
            Write( MessageLevel.INFO, format, list );
        }
        public void WriteError( string format, params object[] list )
        {
            Write( MessageLevel.ERROR, format, list );
        }
        public void Write( MessageLevel level, string format, params object[] list )
        {
            try
            {
                string s = string.Format( format, list ).Replace( "\r", "" ).Replace( "\n", "\r" );
                lock ( _logFile )
                {
                    _logFile.WriteLine( "{0} {1}: {2}", DateTime.Now.ToString( "yyyy-MM-dd HH:mm:ss.fff" ), level, s );
                }
            }
            catch
            {
                lock ( _logFile )
                {
                    _logFile.WriteLine( string.Format( "LoggingError: Format={0}; ParamCount={1}", format, (list == null) ? 0 : list.Length ) );
                }
            }
        }
        public void WriteRaw( string format, params object[] list )
        {
            try
            {
                lock ( _logFile )
                {
                    _logFile.Write( format, list );
                }
            }
            catch
            {
                lock ( _logFile )
                {
                    _logFile.WriteLine( string.Format( "LoggingError: Format={0}; ParamCount={1}", format, (list == null) ? 0 : list.Length ) );
                }
            }
        }
        internal void Close()
        {
            lock ( _logFile )
            {
                if ( _logFile != null )
                {
                    _logFile.Close();
                    _logFile = null;
                }
            }
        }

        #region IDisposable Members

        public void Dispose()
        {
            Close();
        }

        #endregion
    }

    public enum MessageLevel
    {
        INFO,
        ERROR
    }
}
