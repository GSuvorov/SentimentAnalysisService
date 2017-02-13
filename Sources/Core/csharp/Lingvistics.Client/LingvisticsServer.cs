using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text.RegularExpressions;
using System.Threading;
using SystemTimer = System.Timers.Timer;

#if WITH_OM_TM
using TextMining.Core;
#else
using Linguistics.Core;
#endif

namespace Lingvistics.Client
{
    /// <summary>
    /// 
    /// </summary>
    public sealed class LingvisticsServer : ILingvisticsServer
    {
        //Время ожидания при отсутствии лингвистических сервисов
        private const int    SERVER_TIMEOUT_IN_SECONDS = 60; //60c
        private const string DEFAULT_HOST              = "localhost";
        private const int    DEFAULT_PORT              = 45312;
        private const string DEFAULT_URI               = "LingvisticsServer";

        private static readonly Regex RegexHostPort = new Regex(@"\s*(?<HOST>(\w{1,256})|(\d{1,3}\.\d{1,3}.\d{1,3}.\d{1,3})):(?<PORT>\d{1,5})\s*", RegexOptions.IgnoreCase);
        private static readonly Regex RegexUrl      = new Regex(@"\s*(?<URL>\w{3,5}://\w{1,256}:\d{1,5}/\w+)\s*", RegexOptions.IgnoreCase);

        /// <summary>
        /// 
        /// </summary>
        private sealed class LingvisticsServerInstance : IDisposable
        {
            public const int STATUS_PERIOD = 1000 * 10; //10c

            public string Url { get; private set; }
            public ILingvisticsService _ILingvisticsService { get; private set; }

            private bool                  _Active;
            private volatile ServerStatus _Status;
            private double                _LoadingStatus;
            private readonly SystemTimer  _StatusTimer;
            private int                   _SyncPoint = 0;
            private readonly object       _Synclock = new object();

            public LingvisticsServerInstance( string url )
            {
                this.Url = url;
                this._ILingvisticsService = (ILingvisticsService) Activator.GetObject( typeof(ILingvisticsService), url );
                _StatusTimer = new SystemTimer()
                {
                    AutoReset = true,
                    Interval  = STATUS_PERIOD
                };
                _StatusTimer.Elapsed += (sender, e) => UpdateStatus();
                _StatusTimer.Start();
                ThreadPool.QueueUserWorkItem( state => UpdateStatus() );
            }
            public void ChangeLoadingStatus( double d )
            {
                if ( _Status != null )
                {
                    lock ( _Synclock )
                    {
                        if ( _Status != null )
                        {
                            _LoadingStatus += d / _Status.MaxProcessorCount;
                        }
                    }
                }
            }
            public double LoadingStatus
            {
                get { return (_LoadingStatus); }
            }
            public bool Active
            {
                get
                {
                    while ( _Status == null )
                    {
                        Thread.Sleep( 10 );
                    }
                    return (_Active);
                }
            }
            public void ResetActive()
            {
                lock ( _Synclock )
                {
                    _Active = false;
                }
            }

            private void UpdateStatus()
            {
                int sync = Interlocked.CompareExchange( ref _SyncPoint, 1, 0 );
                if ( sync == 0 )
                {
                    _StatusTimer.Stop();
                    try
                    {                        
                        lock ( _Synclock )
                        {
                            _Status = _ILingvisticsService.GetLoadingStatus();
                            _LoadingStatus = (double)(_Status.BusyProcessorCount + _Status.HeavyQueueLength +
                                             0.1 * _Status.LightQueueLength) / _Status.MaxProcessorCount;
                            _Active = true;
                        }
                    }
                    catch
                    {
                        lock ( _Synclock )
                        {
                            _Status = new ServerStatus();
                            _Active = false;
                        }
                    }
                    _StatusTimer.Start();
                    _SyncPoint = 0;
                }
            }

            public void Dispose()
            {
                _StatusTimer.Dispose();
            }
        }

        private readonly int _ServerTimeoutInSeconds;
        private readonly LingvisticsServerInstance[] _ServiceList;

        #region [.monitor-call-states.]
        /*
        private const int MAX_CALL_WAIT_IN_HOURS = 3;
        private const int MONITOR_INTERVAL_IN_MILLISECONDS = 1000 * 60 * 10; //10 min,
        private static readonly SystemTimer _CallStatesTimer;
        private static readonly ConcurrentDictionary< Thread, DateTime > _CallStates = new ConcurrentDictionary< Thread, DateTime >();
        
        private static void BeginCall()
        {
            _CallStates.TryAdd( Thread.CurrentThread, DateTime.Now );
        }
        private static void EndCall( Thread th = null )
        {
            var dt = default(DateTime);
            _CallStates.TryRemove( th ?? Thread.CurrentThread, out dt );
        }
        private static void MonitorCallStates()
        {
            _CallStatesTimer.Stop();
            foreach ( var p in _CallStates )
            {
                var ts = DateTime.Now - p.Value;
                if ( MAX_CALL_WAIT_IN_HOURS < ts.TotalHours )
                {
                    p.Key.Abort();
                    EndCall( p.Key );
                }
            }
            _CallStatesTimer.Start();
        }

        static LingvisticsServer()
        {
            _CallStatesTimer = new SystemTimer()
            {
                AutoReset = true,
                Interval  = MONITOR_INTERVAL_IN_MILLISECONDS,
            };
            _CallStatesTimer.Elapsed += (sender, e) => MonitorCallStates();
            _CallStatesTimer.Start();
        }
        */
        #endregion

        #region [.Specifying Timeout for remoting calls.]
        /*
        using System.Collections;
        using System.Runtime.Remoting.Channels;
        using System.Runtime.Remoting.Channels.Tcp;

        //http://blogs.msdn.com/b/anantd/archive/2004/04/13/112350.aspx
        private const int REMOTING_CALLS_TIMEOUT = 1000 * 60 * 60 * 3;

        static LingvisticsServer()
        {
            IDictionary ht = new Hashtable();
            ht[ "timeout" ] = 1; // REMOTING_CALLS_TIMEOUT;

            var channel = new TcpChannel( ht, null, null );
            ChannelServices.RegisterChannel( channel, false );
        }
        */
        #endregion

        public static LingvisticsServer Create()
        {
            return Create( DEFAULT_HOST );
        }
        public static LingvisticsServer Create( string connectString )
		{
            return Create( connectString, SERVER_TIMEOUT_IN_SECONDS );
		}
        public static LingvisticsServer Create( string connectString, int serverTimeoutInSeconds )
        {
            var urlList = connectString.Split( new[] { ';' }, StringSplitOptions.RemoveEmptyEntries )
                .Select( s => s.Trim() )
                .Distinct()
                .Select( s => {
                        //<NetProtocol>://<Host>:<Port>/<Uri>
                        var m = RegexUrl.Match( s );
                        if ( m.Success )
                        {
                            return (m.Groups[ "URL" ].Value);
                        }

                        //<Host>
                        var host = s;
                        var port = DEFAULT_PORT;

                        //<Host>:<Port>
                        m = RegexHostPort.Match( s );
                        if ( m.Success )
                        {
                            host = m.Groups[ "HOST" ].Value;
                            int.TryParse( m.Groups[ "PORT" ].Value, out port );
                        }
                        return (string.Format( "tcp://{0}:{1}/{2}", host, port, DEFAULT_URI ));

                        #region commented
                        /*---
                        //<Host>
                        var host = s;
                        var port = DEFAULT_PORT;

                        //<Host>:<Port>
                        var m = RegexHostPort.Match( s );
                        if ( m.Success )
                        {
                            host = m.Groups[ "HOST" ].Value;
                            int.TryParse( m.Groups[ "PORT" ].Value, out port );
                        }
                        //<NetProtocol>://<Host>:<Port>/<Uri>
                        else
                        {
                            m = RegexUrl.Match( s );
                            if ( m.Success )
                            {
                                return (m.Groups[ "URL" ].Value);
                            }
                        }
                        return (string.Format( "tcp://{0}:{1}/{2}", host, port, DEFAULT_URI ));
                        */
                        #endregion
                    }
                )
                .ToArray();
            return (new LingvisticsServer( urlList, serverTimeoutInSeconds ));
        }

        private LingvisticsServer( IEnumerable< string > urls, int serverTimeoutInSeconds )
        {
            _ServerTimeoutInSeconds = serverTimeoutInSeconds;
            _ServiceList = urls.Select( url => new LingvisticsServerInstance( url ) ).ToArray();
        }

        private TResult CallLinguisticFunction< TResult >( Func< ILingvisticsService, TResult > function, ProcessingType procType )
        {
            LingvisticsServerInstance service;
            lock ( _ServiceList )
            {
                var dtMax = DateTime.Now.AddSeconds( _ServerTimeoutInSeconds );
                for ( ; ; )
                {
                    service = _ServiceList.Where( t => t.Active )
                                          .OrderBy( t => t.LoadingStatus )
                                          .FirstOrDefault();
                    if ( service == null )
                    {
                        if ( DateTime.Now < dtMax )
                        {
                            Thread.Sleep( 1000 );
                            continue;
                        }
                        throw (new ApplicationException("Нет активных лингвистический сервисов: '" + string.Join(", ", _ServiceList.Select(t => t.Url).ToArray()) + '\''));
                    }
                    service.ChangeLoadingStatus( (procType == ProcessingType.Heavy) ? 1.0 : 0.1 );
                    break;
                }
            }

            try
            {
                //---BeginCall();
                var result = function( service._ILingvisticsService );
                //---EndCall();
                return (result);
            }
            //Рекурсивный вызов для другого сервера
            catch ( Exception ex )
            {
                if ( (ex is SocketException) || (ex is ApplicationException) )
                {
                    service.ResetActive();
                    return (CallLinguisticFunction< TResult >( function, procType ));
                }
                throw;
            }
            finally
            {
                service.ChangeLoadingStatus( (procType == ProcessingType.Heavy) ? -1.0 : -0.1 );
            }
        }

        #region [.ILingvisticServer.]
        public string[] GetAllWordForms( string word )
        {
            return CallLinguisticFunction< string[] >( f => f.GetAllWordForms( word ), ProcessingType.Light );
        }

        public Tuple< string[], string[] > GetAllWordFormsWithPartsOfSpeech( string word )
        {
            return CallLinguisticFunction< Tuple< string[], string[] > >( f => f.GetAllWordFormsWithPartsOfSpeech( word ), ProcessingType.Light );
        }

        public string GetNormalWordForm( string word )
        {
            return CallLinguisticFunction<string>( f => f.GetNormalWordForm( word ), ProcessingType.Light );
        }

        public string[] GetAllNormalWordForm( string word )
        {
            return CallLinguisticFunction< string[] >( f => f.GetAllNormalWordForm( word ), ProcessingType.Light );
        }

        public LingvisticsResult ProcessText( string                   text, 
                                              bool                     afterSpellChecking, 
                                              DateTime                 baseDate,
                                              LingvisticsResultOptions options              = LingvisticsResultOptions.All,
                                              SelectEntitiesMode       mode                 = SelectEntitiesMode.Full,
                                              bool                     generateAllSubthemes = false )
        {
            return CallLinguisticFunction< LingvisticsResult >( f => f.ProcessText( text, afterSpellChecking, baseDate, options, mode, generateAllSubthemes ), ProcessingType.Heavy );
        }
        public LingvisticsResult ProcessText( LingvisticsTextInput input )
        {
            return CallLinguisticFunction< LingvisticsResult >( f => f.ProcessText( input ), ProcessingType.Heavy );
        }

        public PTSResult ProcessPTS( string xml, bool buildSemanticNetwork, string language )
        {
            return CallLinguisticFunction< PTSResult >( f => f.ProcessPTS( xml, buildSemanticNetwork, language ), ProcessingType.Heavy );
        }

        public LingvisticsResult ProcessRDF( string rdf, LingvisticsResultOptions options, bool generateAllSubthemes = false )
        {
            return CallLinguisticFunction< LingvisticsResult >( f => f.ProcessRDF( rdf, options, generateAllSubthemes ), ProcessingType.Light );
        }
        public LingvisticsResult ProcessRDF( LingvisticsRDFInput input )
        {
            return CallLinguisticFunction< LingvisticsResult >( f => f.ProcessRDF( input ), ProcessingType.Light );
        }
        #endregion
    }
}

