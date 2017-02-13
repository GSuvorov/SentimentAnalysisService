using System.ComponentModel;
using System.Configuration.Install;
using System.ServiceProcess;

namespace Lingvistics
{
    [RunInstaller(true)]
    public partial class LingvisticsServiceInstaller : Installer
    {
#if WITH_OM_TM
        private const string SERVICE_NAME = "LingvisticsServer-with-OM-TM";
#else
        private const string SERVICE_NAME = "LingvisticsServer";
#endif
        public LingvisticsServiceInstaller()
        {
            InitializeComponent();

            var processInstaller = new ServiceProcessInstaller() { Account  = ServiceAccount.LocalSystem };
            var serviceInstaller = new ServiceInstaller() 
            {
                StartType = ServiceStartMode.Automatic,

                ServiceName = SERVICE_NAME,
                DisplayName = SERVICE_NAME,
                Description = SERVICE_NAME,
            };

            Installers.Add( serviceInstaller );
            Installers.Add( processInstaller );
        }
    }
}
