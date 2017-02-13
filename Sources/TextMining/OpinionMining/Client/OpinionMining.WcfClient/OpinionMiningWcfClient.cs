using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Security.Tokens;
using System.Text;

using OpinionMining.OpinionMiningWcfService;

namespace OpinionMining
{
    public class OpinionMiningWcfClient : IOpinionMiningWcfClient 
    {
    #region [.ctor().]
        private OpinionMiningClient _OpinionMiningClient;

        public OpinionMiningWcfClient()
        {
            _OpinionMiningClient = new OpinionMiningClient();

            _OpinionMiningClient.Endpoint.Binding = AdjustBinding( _OpinionMiningClient.Endpoint.Binding );
        }
        public OpinionMiningWcfClient( string endpointConfigurationName )
        {
            _OpinionMiningClient = new OpinionMiningClient( endpointConfigurationName );

            _OpinionMiningClient.Endpoint.Binding = AdjustBinding( _OpinionMiningClient.Endpoint.Binding );
        }

        private static Binding AdjustBinding( Binding binding )
        {
            CustomBinding customBinding = new CustomBinding( binding );
            SecurityBindingElement bindingElement = customBinding.Elements.Find< SecurityBindingElement >();
            if ( bindingElement == null ) { return (binding); }            
            bindingElement.LocalServiceSettings.MaxPendingSessions      = 100000;
            bindingElement.LocalServiceSettings.MaxStatefulNegotiations = 100000;
            bindingElement.LocalServiceSettings.MaxClockSkew = TimeSpan.FromHours(2);
            bindingElement.LocalClientSettings .MaxClockSkew = TimeSpan.FromHours(2);

            //
            // Check if secure conversation is enabled
            //
            SecurityTokenParameters tokenParameters = ((SymmetricSecurityBindingElement) bindingElement).ProtectionTokenParameters;
            SecureConversationSecurityTokenParameters sct = tokenParameters as SecureConversationSecurityTokenParameters;
            if ( sct != null )
            {                
                bindingElement = sct.BootstrapSecurityBindingElement;                
                bindingElement.LocalServiceSettings.MaxPendingSessions      = 100000;
                bindingElement.LocalServiceSettings.MaxStatefulNegotiations = 100000;
                bindingElement.LocalServiceSettings.MaxClockSkew = TimeSpan.FromHours(2);
                bindingElement.LocalClientSettings .MaxClockSkew = TimeSpan.FromHours(2);
            }

            return (customBinding);
        }
    #endregion

    #region [.IOpinionMining.]
        public OpinionMiningOutputResult ExecuteOpinionMining( OpinionMiningInputParams inputParams )
        {
            return (_OpinionMiningClient.ExecuteOpinionMining( inputParams ));
        }

        /*public OpinionMiningOutputResult ExecuteOpinionMining4Automat( OpinionMiningInputParams4Automat inputParams )
        {
            return (_OpinionMiningClient.ExecuteOpinionMining4Automat( inputParams ));
        }*/
    #endregion

    #region [.IDisposable.]
        public void Dispose()
        {
            if ( _OpinionMiningClient != null )
            {
                try
                {
                    if ( _OpinionMiningClient.State == CommunicationState.Faulted )
                        _OpinionMiningClient.Abort();
                    else
                        _OpinionMiningClient.Close();
                    _OpinionMiningClient = null;
                }
                catch (Exception ex)
                {
                    _OpinionMiningClient.Abort();
                    _OpinionMiningClient = null;
                    System.Diagnostics.Debug.WriteLine( ex );    
                }
            }
        }
    #endregion
    }
}
