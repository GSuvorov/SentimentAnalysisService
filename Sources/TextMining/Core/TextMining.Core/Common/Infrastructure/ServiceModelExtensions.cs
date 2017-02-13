using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Security.Tokens;

namespace TextMining.Core
{
    /// <summary>
    /// 
    /// </summary>
    public static partial class Extensions
    {    
        public static Binding AdjustBinding( this Binding binding )
        {
            var customBinding  = new CustomBinding( binding );
            var bindingElement = customBinding.Elements.Find< SecurityBindingElement >();
            if ( bindingElement == null ) { return (binding); }            
            bindingElement.LocalServiceSettings.MaxPendingSessions      = 100000;
            bindingElement.LocalServiceSettings.MaxStatefulNegotiations = 100000;
            bindingElement.LocalServiceSettings.MaxClockSkew = TimeSpan.FromHours( 2 );
            bindingElement.LocalClientSettings .MaxClockSkew = TimeSpan.FromHours( 2 );

            //
            // Check if secure conversation is enabled
            //
            SecurityTokenParameters tokenParameters = ((SymmetricSecurityBindingElement) bindingElement).ProtectionTokenParameters;
            var sct = tokenParameters as SecureConversationSecurityTokenParameters;
            if ( sct != null )
            {                
                bindingElement = sct.BootstrapSecurityBindingElement;                
                bindingElement.LocalServiceSettings.MaxPendingSessions      = 100000;
                bindingElement.LocalServiceSettings.MaxStatefulNegotiations = 100000;
                bindingElement.LocalServiceSettings.MaxClockSkew = TimeSpan.FromHours( 2 );
                bindingElement.LocalClientSettings .MaxClockSkew = TimeSpan.FromHours( 2 );
            }

            return (customBinding);
        }
        public static void AdjustBinding4AllEndpoints( this OperationContext operationContext )
        {
            if ( operationContext != null )
            {
                foreach ( var endpoint in operationContext.Host.Description.Endpoints )
                {
                    endpoint.Binding = endpoint.Binding.AdjustBinding();
                }
            }
        }
    }
}
