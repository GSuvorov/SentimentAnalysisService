using System;
using System.Runtime.Serialization;

using TextMining.Core;

namespace Digest
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class DigestInputParams : InputParamsBase
    {
        public DigestInputParams( string inputText, InputTextFormat inputTextFormat )
            : base( inputText, inputTextFormat )
        {
            ExecuteTonalityMarking = true;
            ObjectAllocateMethod   = ObjectAllocateMethod.FirstVerbEntityWithRoleObj;
        }

        [DataMember]
        public bool ExecuteTonalityMarking
        {
            get;
            set;
        }
    }
}
