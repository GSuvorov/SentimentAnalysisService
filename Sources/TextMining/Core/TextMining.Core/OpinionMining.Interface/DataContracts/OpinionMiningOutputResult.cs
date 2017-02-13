using System;
using System.Collections.Generic;
using System.Runtime.Serialization;

using TextMining.Core;

namespace OpinionMining
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class OpinionMiningOutputResult : OutputResultTupleBase< OpinionMiningTuple >
    {
        public OpinionMiningOutputResult
            (
            IEnumerable< OpinionMiningTuple > tuples,
            IEnumerable< string >             operateRulesNames = null
            )
            : base( tuples, operateRulesNames )
        {
        }
    }
}
