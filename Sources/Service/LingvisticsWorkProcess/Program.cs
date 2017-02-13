using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Ipc;
using System.Threading;
using System.Threading.Tasks;

namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
    internal static class Program
    {
        private static LingvisticsWorkProcessor _Processor;

        private static void Main( string[] args )
        {
//System.Diagnostics.Debugger.Break();
            //Thread.Sleep( 60000 );

            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;

            var process = Process.GetCurrentProcess();

            #region [.watch when the parent process 'LingvisticsService' unexpected end.]
            if ( 0 < args.Length )
            {
                var aa = args[ 0 ].Split( new[] { '=' }, StringSplitOptions.RemoveEmptyEntries );
                int lingvisticsServicePID;
                if ( aa.Length == 2 &&
                     string.Compare( aa[ 0 ], "LingvisticsServicePID" ) == 0 &&
                     int.TryParse( aa[ 1 ], out lingvisticsServicePID ) )
                {
                    try
                    {
                        var lingvisticsServiceProcess = Process.GetProcessById( lingvisticsServicePID );
                        if ( lingvisticsServiceProcess != null )
                        {
                            Task.Factory.StartNew( () =>
                            {
                                try
                                {
                                    if ( lingvisticsServiceProcess.WaitForExit( Timeout.Infinite ) )
                                    {
                                        Environment.Exit( 666 ); //"the parent process 'LingvisticsService' unexpected end"
                                    }
                                }
                                catch ( Exception ex )
                                {
                                    Debug.WriteLine( ex );
                                }
                            } );
                        }
                    }
                    catch ( Exception ex )
                    {
                        Debug.WriteLine( ex );
                    }
                }
            } 
            #endregion

            //Регистрация remote интерфейса
            var uri = Path.GetFileNameWithoutExtension( process.MainModule.FileName ).ToLower();
            var channel = new IpcChannel( string.Format( "LP:{0}", process.Id ) );
            ChannelServices.RegisterChannel( channel, false );
            RemotingConfiguration.CustomErrorsEnabled( false );
            RemotingConfiguration.CustomErrorsMode = CustomErrorsModes.Off;
            RemotingConfiguration.RegisterWellKnownServiceType( typeof(LingvisticsWorkProcessor), uri, WellKnownObjectMode.Singleton );

            //Инициализация лингвистики
            _Processor = new LingvisticsWorkProcessor();
        }

        private static void CurrentDomain_UnhandledException( object sender, UnhandledExceptionEventArgs e )
        {
            LOG.GetLogger().ErrorFormat( "ProcID={0}\n{1}", Process.GetCurrentProcess().Id, e.ExceptionObject );
        }
    }
}
