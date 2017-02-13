using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Security.Tokens;
using System.Text;

using OpinionMining;
using TonalityMarking;
using Digest.DigestWcfService;

namespace Digest
{
    /// <summary>
    /// 
    /// </summary>
    public class DigestWcfClient : IDigestWcfClient 
    {
    #region [.ctor().]
        private DigestClient _DigestClient;

        public DigestWcfClient()
        {
            _DigestClient = new DigestClient();

            _DigestClient.Endpoint.Binding = AdjustBinding( _DigestClient.Endpoint.Binding );
        }
        public DigestWcfClient( string endpointConfigurationName )
        {
            _DigestClient = new DigestClient( endpointConfigurationName );

            _DigestClient.Endpoint.Binding = AdjustBinding( _DigestClient.Endpoint.Binding );
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
            return (_DigestClient.ExecuteTonalityMarking( inputParams ));
        }

        /*public TonalityMarkingOutputResult ExecuteTonalityMarking4Automat( TonalityMarkingInputParams4Automat inputParams )
        {
            return (_DigestClient.ExecuteTonalityMarking4Automat( inputParams ));
        }*/

        public TonalityMarkingOutputResult4Blogs ExecuteTonalityMarking4Blogs( TonalityMarkingInputParams4Blogs inputParams )
        {
            return (_DigestClient.ExecuteTonalityMarking4Blogs( inputParams ));
        }

        public SocviewProcessingData ExecuteTonalityMarking4Socview( SocviewProcessingData socviewData )
        {
            return (_DigestClient.ExecuteTonalityMarking4Socview( socviewData ));
        }
    #endregion

    #region [.IOpinionMining.]
        public OpinionMiningOutputResult ExecuteOpinionMining( OpinionMiningInputParams inputParams )
        {
            return (_DigestClient.ExecuteOpinionMining( inputParams ));
        }

        /*public OpinionMiningOutputResult ExecuteOpinionMining4Automat( OpinionMiningInputParams4Automat inputParams )
        {
            return (_DigestClient.ExecuteOpinionMining4Automat( inputParams ));
        }*/
    #endregion

    #region IDigest Members
        public DigestOutputResult ExecuteDigest( DigestInputParams inputParams )
        {
            return (_DigestClient.ExecuteDigest( inputParams ));
        }

        public DigestOutputResult ExecuteDigestAsync( DigestInputParams inputParams ) 
        {
            return (_DigestClient.ExecuteDigestAsync( inputParams ));
        }
        public IAsyncResult BeginExecuteDigestAsync( DigestInputParams inputParams, AsyncCallback callback, object state )
        {
            throw new NotImplementedException();
        }
        public DigestOutputResult EndExecuteDigestAsync( IAsyncResult result )
        {
            throw new NotImplementedException();
        }

        /*public DigestOutputResult ExecuteDigest4Automat( DigestInputParams4Automat inputParams )
        {
            return (_DigestClient.ExecuteDigest4Automat( inputParams ));
        }*/
    #endregion

    #region [.IDisposable.]
        public void Dispose()
        {
            if ( _DigestClient != null )
            {
                try
                {
                    if ( _DigestClient.State == CommunicationState.Faulted )
                        _DigestClient.Abort();
                    else
                        _DigestClient.Close();
                    _DigestClient = null;
                }
                catch (Exception ex)
                {
                    _DigestClient.Abort();
                    _DigestClient = null;
                    System.Diagnostics.Debug.WriteLine( ex );    
                }
            }
        }
    #endregion

    #region [.method's.]
        public void OpenChannel()
        {
            _DigestClient.Open();
        }
    #endregion
    }
}
