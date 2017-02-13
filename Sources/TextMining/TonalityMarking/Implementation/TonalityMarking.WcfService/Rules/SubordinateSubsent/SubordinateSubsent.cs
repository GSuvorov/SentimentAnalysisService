using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

using TextMining.Core;

namespace TonalityMarking
{
    internal class SubordinateSubsent
    {
        public SubordinateSubsent( XElement subordinateSubsentNode )
        {       
            OneRules = (from rule in subordinateSubsentNode.GetMultiNodes( "One/Rule" )
                        select new OneRule( rule )
                       )
                       .ToArray();
  
            PairsRules = (from rule in subordinateSubsentNode.GetMultiNodes( "Pairs/Rule" )
                          select new PairsRule( rule )
                         )
                         .ToArray();

            ThreeRules = (from rule in subordinateSubsentNode.GetMultiNodes( "Three/Rule" )
                          select new ThreeRule( rule )
                         )
                         .ToArray();           

            _RulesByOrder = ThreeRules.Cast< RuleBase >()
                           .Concat
                           (
                           PairsRules.Cast< RuleBase >()
                           )
                           .Concat
                           (
                           OneRules.Cast< RuleBase >()
                           )
                           .ToArray();
        }

        public OneRule[] OneRules
        {
            get;
            private set;
        }
        public PairsRule[] PairsRules
        {
            get;
            private set;
        }
        public ThreeRule[] ThreeRules
        {
            get;
            private set;
        }

        private RuleBase[] _RulesByOrder;
        public IEnumerable< RuleBase > RulesByOrder
        {
            get { return (_RulesByOrder); }
        }
    }
}
