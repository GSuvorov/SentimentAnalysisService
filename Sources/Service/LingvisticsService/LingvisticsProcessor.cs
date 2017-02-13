using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Remoting;
using System.Threading;

using Lingvistics.Client;
using Config = Lingvistics.Properties.Settings;

#if WITH_OM_TM
using TextMining.Core;
#endif

namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
    public sealed class LingvisticsProcessor : ILingvisticsProcessor
    {
        private static readonly log4net.ILog Logger = LOG.GetLogger();
#if WITH_OM_TM
        private const string PROCESSOR_FILENAME = "LingvisticsWorkProcess-with-OM-TM.exe";
#else
        private const string PROCESSOR_FILENAME = "LingvisticsWorkProcess.exe";
#endif
        private static readonly string ProcessorWorkingDirectory = Path.GetDirectoryName( Path.GetDirectoryName( AppDomain.CurrentDomain.BaseDirectory ) );
        private static readonly string ProcessorFilePath = Path.Combine( ProcessorWorkingDirectory, PROCESSOR_FILENAME );
        private static readonly string ProcessorUri = Path.GetFileNameWithoutExtension( PROCESSOR_FILENAME ).ToLower();

        private volatile bool         _Closed;
        private bool                  _Busy;
        private ProcessingType        _ProcessingType;
        private Process               _Process;
        private ILingvisticsProcessor _ILingvisticsProcessor;
        private DateTime              _LastDateTimeAction = DateTime.Now;

        public LingvisticsProcessor()
        {
            if ( !File.Exists( ProcessorFilePath ) )
            {
                throw (new Exception( "ЕБАТЬ ВАШУ МАТЬ - '" + ProcessorFilePath + "' - НЕТ ТАКОГО ФАЙЛА" ));
            }

            WindowsEvent startEvent = null;
            try
            {
                _Process = Process.Start(
                    new ProcessStartInfo()
                    {
                        FileName         = ProcessorFilePath,
                        WorkingDirectory = ProcessorWorkingDirectory,
                        Arguments        = "LingvisticsServicePID=" + Process.GetCurrentProcess().Id, //"",//string.Format("{0} {1}", _port, _uri),
                        CreateNoWindow   = true,
                        UseShellExecute  = false
                    }
                );
                var url = string.Format( "ipc://LP:{0}/{1}", _Process.Id, ProcessorUri );

                //ожидание запуска и сетевой инициализации процессора
                startEvent = new WindowsEvent( string.Format( "LPStartEvent_{0}", _Process.Id ) );
                while ( !startEvent.Wait( 10 ) )
                {
                    if ( _Process.HasExited )
                    {
                        throw (new ApplicationException( string.Format( "Proc({0}) stopped", _Process.Id ) ));
                    }
                }

                _ILingvisticsProcessor = (ILingvisticsProcessor) Activator.GetObject( typeof(ILingvisticsProcessor), url );
            }
            catch ( Exception ex )
            {
                Logger.Error( ex.ToString() );
                throw (new ApplicationException( "Error on start new proc" ));
            }
            finally
            {
                if ( startEvent != null )
                {
                    startEvent.Close();
                }
            }
            Logger.InfoFormat( "New proc({0}) start", _Process.Id );
        }

        public bool Busy
        {
            get { return _Busy; }
        }
        public int ProcessID
        {
            get { return (_Process == null) ? 0 : _Process.Id; }
        }
        public ProcessingType ProcessingType
        {
            get { return _ProcessingType; }
        }
        public DateTime LastDateTimeAction
        {
            get { return _LastDateTimeAction; }
        }
        public void Lock( ProcessingType processingType )
        {
            _Busy = true;
            _ProcessingType = processingType;
        }
        public void Free()
        {
            _LastDateTimeAction = DateTime.Now;
            if ( !_Closed )
            {
                _Busy = false;
            }
        }
        public bool Closed
        {
            get
            {
                if ( !_Closed && ((_Process != null) && _Process.HasExited) )
                {
                    FinallyClose();
                }
                return (_Process == null || _Process.HasExited);
            }
        }

        private void FinallyClose()
        {
            lock ( this )
            {
                if ( _Closed ) return;
                _Closed = true;
                if ( _Process != null )
                {
                    try
                    {
                        if ( !_Process.HasExited && _ILingvisticsProcessor != null )
                        {
                            Logger.InfoFormat( "Proc({0}) start stopping", _Process.Id );
                            try
                            {
                                var dtEnd = DateTime.Now.AddSeconds( Config.Default.ProcessorCloseTimeOutInSeconds );
                                var t = new Thread( () =>
                                    {
                                        try
                                        {
                                            _ILingvisticsProcessor.Close();
                                        }
                                        catch 
                                        {
                                            ;
                                        }
                                    }
                                );
                                t.Start();
                                while ( !_Process.HasExited && DateTime.Now < dtEnd )
                                {
                                    Thread.Sleep( 10 );
                                }
                            }
                            catch ( Exception ex )
                            {
                                Logger.ErrorFormat( "Proc({0}) error on stopping\n{1}", _Process.Id, ex.Message );
                            }
                        }
                        if ( !_Process.HasExited )
                        {
                            _Process.Kill();
                            Logger.InfoFormat( "Proc({0}) force stopped", _Process.Id );
                        }
                        else
                        {
                            Logger.InfoFormat( "Proc({0}) stopped", _Process.Id );
                        }
                    }
                    catch ( Exception ex )
                    {
                        Logger.ErrorFormat( "Proc({0}) error on force stopped\n{1}", _Process.Id, ex.Message );
                    }
                }
                _ILingvisticsProcessor = null;
            }
        }

        private TResult CallLinguisticFunction< TResult >( Func< ILingvisticsProcessor, TResult > function )
        {
            if ( _ProcessingType == ProcessingType.Heavy )
            {
                var workTimer = new Timer( state =>
                    {
                        Logger.ErrorFormat( "Proc({0}) working timeout happens", _Process.Id );
                        FinallyClose();
                    }, null, 1000 * Config.Default.ProcessorWorkTimeOutInSeconds, Timeout.Infinite );

                try
                {
                    return function( _ILingvisticsProcessor );
                }
                catch ( Exception ex )
                {
                    if ( _Closed )
                    {
                        throw (new TimeoutException( "Превышено максимально допустимое время выполнения лингвистической функции" ));
                    }
                    if ( ex is RemotingException || ex is SEHException )
                    {
                        ThreadPool.QueueUserWorkItem( ( state ) => FinallyClose() );
                        while ( !_Closed )
                        {
                            Thread.Sleep( 1 );
                        }
                    }
                    throw;
                }
                finally
                {
                    workTimer.Dispose();
                }
            }
            else
            {
                return (function( _ILingvisticsProcessor ));
            }
        }

        #region [.ILingvisticProcessor.]
        public void Close()
        {
            if ( _Closed ) return;

            //Процесс уже завершен - синхронный вызов
            if ( _Process == null || _Process.HasExited )
            {
                FinallyClose();
            }
            //иначе - асинхронный вызов
            else
            {
                var th = new Thread( () => FinallyClose() );
                th.Start();
            }
        }
        #endregion

        #region [.ILingvisticServer.]
        public string[] GetAllWordForms( string word )
        {
            return CallLinguisticFunction( f => f.GetAllWordForms( word ) );
        }

        public Tuple< string[], string[] > GetAllWordFormsWithPartsOfSpeech( string word )
        {
            return CallLinguisticFunction( f => f.GetAllWordFormsWithPartsOfSpeech( word ) );
        }

        public string GetNormalWordForm( string word )
        {
            return CallLinguisticFunction( f => f.GetNormalWordForm( word ) );
        }

        public string[] GetAllNormalWordForm( string word )
        {
            return CallLinguisticFunction( f => f.GetAllNormalWordForm( word ) );
        }

        public LingvisticsResult ProcessText( string                   text, 
                                              bool                     afterSpellChecking, 
                                              DateTime                 baseDate,
                                              LingvisticsResultOptions options              = LingvisticsResultOptions.All,
                                              SelectEntitiesMode       mode                 = SelectEntitiesMode.Full,
                                              bool                     generateAllSubthemes = false )
        {
            return CallLinguisticFunction( f => f.ProcessText( text, afterSpellChecking, baseDate, options, mode, generateAllSubthemes ) );
        }
        public LingvisticsResult ProcessText( LingvisticsTextInput input )
        {
            return CallLinguisticFunction( f => f.ProcessText( input ) );
        }

        public LingvisticsResult ProcessRDF( string rdf, LingvisticsResultOptions options, bool generateAllSubthemes = false )
        {
            return CallLinguisticFunction( f => f.ProcessRDF( rdf, options, generateAllSubthemes ) );
        }
        public LingvisticsResult ProcessRDF( LingvisticsRDFInput input )
        {
            return CallLinguisticFunction( f => f.ProcessRDF( input ) );
        }

        public PTSResult ProcessPTS( string xml, bool buildSemanticNetwork, string language )
        {
            return CallLinguisticFunction( f => f.ProcessPTS( xml, buildSemanticNetwork, language ) );
        }
        #endregion
    }
}
