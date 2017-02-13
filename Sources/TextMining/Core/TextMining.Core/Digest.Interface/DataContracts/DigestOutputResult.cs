using System;
using System.Collections.Generic;
using System.Runtime.Serialization;

using TextMining.Core;

namespace Digest
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class DigestOutputResult : OutputResultTupleBase< DigestTuple >
    {
        public DigestOutputResult
            (
            IEnumerable< DigestTuple > tuples,
            IEnumerable< string >      operateRulesNames = null
            )
            : base( tuples, operateRulesNames )
        {
        }
    }
}
