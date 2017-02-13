using System.ServiceModel;
using System.ServiceModel.Channels;

namespace TextMining.Service
{
    public static class WcfHelper
    {
        public static bool IsNotAllowedWCF
        {
            get { return (OperationContext.Current == null); }
        }
        public static bool IsAllowedWCF
        {
            get { return (OperationContext.Current != null); }
        }
        public static string ClientRemoteAddress
        {
            get
            {
                if ( IsNotAllowedWCF )
                    return ("[without WCF]");

                /*if ( OperationContext.Current.Channel.RemoteAddress != null )
                {
                    if ( OperationContext.Current.Channel.RemoteAddress.Uri != null )
                        return (OperationContext.Current.Channel.RemoteAddress.Uri.ToString());
                }*/

                var remoteEndpointProperty = OperationContext.Current.IncomingMessageProperties[ RemoteEndpointMessageProperty.Name ];
                /*if ( remoteEndpointProperty == null )
                    return (???);*/
                return ((RemoteEndpointMessageProperty) remoteEndpointProperty).Address;
            }
        }
    }
}




