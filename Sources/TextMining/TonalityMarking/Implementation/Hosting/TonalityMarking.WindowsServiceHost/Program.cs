using System;
using System.ComponentModel;
using System.Configuration.Install;
using System.Reflection;
using System.Security.Policy;
using System.ServiceProcess;
using System.Text;

namespace TonalityMarking.WindowsServiceHost
{
    static class Program
    {
        static void Main()
        {
            Environment.CurrentDirectory = Assembly.GetExecutingAssembly().Location.Substring( 0, Assembly.GetExecutingAssembly().Location.LastIndexOf('\\') );

            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
            //CServiceLog.WriteInfo("Точка входа (::Main()) службы\'" + InstallService.SERVICE_NAME + "'.");

            #region [.INSTALL.]
            string[] argv = Environment.GetCommandLineArgs();         
	        if ( argv.Length >= 2 )
	        {
		        if ( argv[ 1 ][ 0 ] == '/' )
			        argv[ 1 ] = argv[ 1 ].Replace('/', '-');

		        if ( string.Compare( argv[ 1 ], "-Install", true ) == 0 )
		        {			        
			        var _args  = new string[ argv.Length - 1 ];

			        // Set args[0] with the full path to the assembly,
			        var asm = Assembly.GetExecutingAssembly();
			        _args[ 0 ] = asm.Location;

                    Array.Copy( argv, 2, _args, 1, _args.Length - 1 );
			        var appDomain = AppDomain.CreateDomain( "execDom" );
			        var path = typeof(object).Assembly.Location;
			        var sb = new StringBuilder(path.Substring(0, path.LastIndexOf('\\')));
			        sb.Append( "\\InstallUtil.exe" );
                    appDomain.ExecuteAssembly( sb.ToString(), new Evidence(), _args );

                    //CServiceLog.WriteInfo("Service successfully Install/Uninstall.");
		        }
                else
                {
                    Console.WriteLine("Usage '[<fullpath>]\\TonalityMarking.WindowsServiceHost.exe' -Install [-u]");
                }
	        }
            else
            #endregion
            #region [.RUN.]
            {
                ServiceBase.Run( new TonalityMarkingWindowsService() );
            }
            #endregion

            //CServiceLog.WriteWarning("Точка выхода (::Main()) службы\'" + InstallService.SERVICE_NAME + "'.");
            //CServiceLog.WriteEndIndent();            
        }

        //.Unhandled AppDomain Exception
        static public void CurrentDomain_UnhandledException( object sender, UnhandledExceptionEventArgs e )
        {
            //CServiceLog.WriteError( e.ExceptionObject as Exception, "AppDomain::CurrentDomain::UnhandledException()" );
        }
    }
    //-------------------------------------------------------------------------------------------//

    [RunInstaller(true)]
	public class InstallService : Installer
    {
        public static readonly string DISPLAY_SERVICE_NAME = "Tonality-marking Service";
        public static readonly string SERVICE_NAME         = typeof(TonalityMarkingWindowsService).Name;

		private ServiceInstaller		_SInst;
		private ServiceProcessInstaller _SPInst;
        	
		/*protected override void OnAfterInstall(System.Collections.IDictionary savedState) 
		{	
			
		}
		protected override void OnAfterUninstall(System.Collections.IDictionary savedState) 
		{
			
		}*/

		public InstallService()
        {
			_SInst			   = new ServiceInstaller();
			_SPInst			   = new ServiceProcessInstaller();
            _SPInst.Account    = ServiceAccount.LocalSystem;
            _SInst.DisplayName = DISPLAY_SERVICE_NAME;
            _SInst.ServiceName = SERVICE_NAME; 
            _SInst.StartType   = ServiceStartMode.Automatic;
            this.Installers.Add( _SPInst );
            this.Installers.Add( _SInst  );
        }

        public override void Uninstall(System.Collections.IDictionary savedState)
        {
            using ( var _sc = new ServiceController( SERVICE_NAME, Environment.MachineName ) )
            {
                if ( _sc.CanStop )
                {
                    _sc.Stop();
                }
                _sc.Close();
            }
			base.Uninstall( savedState );
        }
    }
    //-------------------------------------------------------------------------------------------//
}