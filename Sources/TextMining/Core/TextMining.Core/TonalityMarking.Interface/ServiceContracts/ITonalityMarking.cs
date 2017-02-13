using System.ServiceModel;
using System.ServiceModel.Web;

namespace TonalityMarking
{   
    /// <summary>
    /// 
    /// </summary>
    [ServiceContract(Namespace = "TonalityMarking")]
    public interface ITonalityMarking
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="inputParams"></param>
        /// <returns></returns>
        [OperationContract]
        [WebInvoke]
        TonalityMarkingOutputResult ExecuteTonalityMarking( TonalityMarkingInputParams inputParams );


        /// <summary>
        /// 
        /// </summary>
        /// <param name="inputParams"></param>
        /// <returns></returns>
        [OperationContract]
        [WebInvoke]
        TonalityMarkingOutputResult4Blogs ExecuteTonalityMarking4Blogs( TonalityMarkingInputParams4Blogs inputParams );

        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="inputParams"></param>
        /// <returns></returns>
        [OperationContract]
        [WebInvoke]
        SocviewProcessingData ExecuteTonalityMarking4Socview( SocviewProcessingData socviewData );
       
    }
}
