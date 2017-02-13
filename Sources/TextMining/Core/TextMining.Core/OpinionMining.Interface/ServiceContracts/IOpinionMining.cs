using System.ServiceModel;
using System.ServiceModel.Web;

namespace OpinionMining
{   
    /// <summary>
    /// 
    /// </summary>
    [ServiceContract(Namespace = "OpinionMining")]
    public interface IOpinionMining
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="inputParams"></param>
        /// <returns></returns>
        [OperationContract]
        [WebInvoke]
        OpinionMiningOutputResult ExecuteOpinionMining( OpinionMiningInputParams inputParams );

    }
}
