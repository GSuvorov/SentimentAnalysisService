using System;
using System.Diagnostics;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Security.Tokens;

using TonalityMarking.TonalityMarkingWcfService;

namespace TonalityMarking
{
    public class TonalityMarkingWcfClient : ITonalityMarkingWcfClient
    {
        #region [.ctor().]
        private TonalityMarkingClient _TonalityMarkingClient;

        public TonalityMarkingWcfClient()
        {
            _TonalityMarkingClient = new TonalityMarkingClient();

            _TonalityMarkingClient.Endpoint.Binding = AdjustBinding( _TonalityMarkingClient.Endpoint.Binding );
        }
        public TonalityMarkingWcfClient( string endpointConfigurationName )
        {
            _TonalityMarkingClient = new TonalityMarkingClient( endpointConfigurationName );

            _TonalityMarkingClient.Endpoint.Binding = AdjustBinding( _TonalityMarkingClient.Endpoint.Binding );
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

        #region [.ITonalityMarking.]
        public TonalityMarkingOutputResult ExecuteTonalityMarking( TonalityMarkingInputParams inputParams )
        {
            return (_TonalityMarkingClient.ExecuteTonalityMarking( inputParams ));
        }

        /*public TonalityMarkingOutputResult ExecuteTonalityMarking4Automat( TonalityMarkingInputParams4Automat inputParams )
        {
            return (_TonalityMarkingClient.ExecuteTonalityMarking4Automat( inputParams ));
        }*/

        public TonalityMarkingOutputResult4Blogs ExecuteTonalityMarking4Blogs( TonalityMarkingInputParams4Blogs inputParams )
        {
            return (_TonalityMarkingClient.ExecuteTonalityMarking4Blogs( inputParams ));
        }

        public SocviewProcessingData ExecuteTonalityMarking4Socview( SocviewProcessingData socviewData )
        {
            return (_TonalityMarkingClient.ExecuteTonalityMarking4Socview( socviewData ));
        }
        #endregion

        #region [.IDisposable.]
        public void Dispose()
        {
            if ( _TonalityMarkingClient != null )
            {
                try
                {
                    if ( _TonalityMarkingClient.State == CommunicationState.Faulted )
                        _TonalityMarkingClient.Abort();
                    else
                        _TonalityMarkingClient.Close();
                    _TonalityMarkingClient = null;
                }
                catch ( Exception ex )
                {
                    _TonalityMarkingClient.Abort();
                    _TonalityMarkingClient = null;
                    Debug.WriteLine( ex );    
                }
            }
        }
        #endregion
    }
}
