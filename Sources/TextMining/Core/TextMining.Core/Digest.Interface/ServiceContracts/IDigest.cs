using System;
using System.ServiceModel;
using System.ServiceModel.Web;

using TonalityMarking;
using OpinionMining;

namespace Digest
{   
    /// <summary>
    /// 
    /// </summary>
    [ServiceContract(Namespace = "Digest")]
    public interface IDigest : ITonalityMarking, IOpinionMining
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="inputParams"></param>
        /// <returns></returns>
        [OperationContract]
        [WebInvoke]
        DigestOutputResult ExecuteDigest( DigestInputParams inputParams );

        /// <summary>
        /// 
        /// </summary>
        /// <param name="inputParams"></param>
        /// <param name="callback"></param>
        /// <param name="state"></param>
        /// <returns></returns>
        [OperationContract(AsyncPattern = true)]
        IAsyncResult       BeginExecuteDigestAsync( DigestInputParams inputParams, AsyncCallback callback, object state );
        DigestOutputResult EndExecuteDigestAsync( IAsyncResult result );

    }
}
