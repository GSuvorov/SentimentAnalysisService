using System;
using System.ServiceModel;
using System.ServiceProcess;

namespace TonalityMarking.WindowsServiceHost
{
    public partial class TonalityMarkingWindowsService : ServiceBase
    {
        public ServiceHost _ServiceHost = null;

        //.ctor
        public TonalityMarkingWindowsService()
        {
            //.0 - service name
            this.ServiceName = InstallService.SERVICE_NAME;            
        }     

        //.Start & Stop service
        protected override void OnStart(string[] args)
        {
            if ( _ServiceHost != null )
            {
                _ServiceHost.Close();
            }

            // Create a ServiceHost for the CalculatorService type and 
            // provide the base address.
            _ServiceHost = new ServiceHost( typeof(TonalityMarkingWcfService) );

            // Open the ServiceHostBase to create listeners and start 
            // listening for messages.
            _ServiceHost.Open();
        }
        protected override void OnStop()
        {
            if ( _ServiceHost != null )
            {
                _ServiceHost.Close();
                _ServiceHost = null;
            }           
        }       
    }
}
