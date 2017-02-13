using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Serialization;

using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class TonalityMarkingOutputResult4Blogs : OutputResultBase
    {
        public TonalityMarkingOutputResult4Blogs
            (
            IEnumerable< TonalityMarkingTuple4Blogs > tuples,
            IEnumerable< string > operateRulesNames = null
            )
            : base( operateRulesNames )
        {
            tuples.ThrowIfNull("tuples");

            Tuples = tuples.ToList().AsReadOnly();
        }

        [DataMember]
        public ReadOnlyCollection< TonalityMarkingTuple4Blogs > Tuples
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return ("Tuples: " + Tuples.Count.ToString());
        }
    }
}
