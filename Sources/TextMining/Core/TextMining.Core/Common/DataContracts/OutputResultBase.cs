using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Serialization;

namespace TextMining.Core
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public abstract class OutputResultBase
    {
        public OutputResultBase( IEnumerable< string > operateRulesNames = null )
        {
            if ( operateRulesNames != null )
            {
                OperateRulesNames = operateRulesNames.ToList().AsReadOnly();
            }
        }

        [DataMember]
        public ReadOnlyCollection< string > OperateRulesNames
        {
            get;
            private set;
        }
    }
}
