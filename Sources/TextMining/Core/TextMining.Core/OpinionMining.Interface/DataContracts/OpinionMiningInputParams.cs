using System;
using System.Runtime.Serialization;

using TextMining.Core;

namespace OpinionMining
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class OpinionMiningInputParams : InputParamsBase
    {
        public OpinionMiningInputParams( string inputText, InputTextFormat inputTextFormat )
            : base( inputText, inputTextFormat )
        {
            ObjectAllocateMethod = ObjectAllocateMethod.FirstVerbEntityWithRoleObj;
        }
    }
}
