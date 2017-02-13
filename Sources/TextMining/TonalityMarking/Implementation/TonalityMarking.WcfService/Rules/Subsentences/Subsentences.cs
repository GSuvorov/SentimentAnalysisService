using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

using TextMining.Core;

namespace TonalityMarking
{
    internal class Subsentences
    {
        public Subsentences( XElement subsentsNode )
        {
            ParentChildRules = (from rule in subsentsNode.GetMultiNodes( "ParentChild/Rule" )
                                select new ParentChildRule( rule )
                               )
                               .ToArray();

            ParentChildRules2 = (from rule in subsentsNode.GetMultiNodes( "ParentChild-2/Rule" )
                                 select new ParentChildRule2( rule )
                                )
                                .ToArray();
            
            ParentTwoChildRules = (from rule in subsentsNode.GetMultiNodes( "ParentTwoChild/Rule" )
                                   select new ParentTwoChildRule( rule )
                                  )
                                  .ToArray();

            ParentTwoChildRules2 = (from rule in subsentsNode.GetMultiNodes( "ParentTwoChild-2/Rule" )
                                    select new ParentTwoChildRule2( rule )
                                   )
                                   .ToArray();

            FlatChainRules = (from rule in subsentsNode.GetMultiNodes( "FlatChains/Rule" )
                              select new FlatChainRule( rule )
                             )
                             .ToArray();
        }

        public ParentChildRule[]     ParentChildRules
        {
            get;
            private set;
        }
        public ParentChildRule2[]    ParentChildRules2
        {
            get;
            private set;
        }
        public ParentTwoChildRule[]  ParentTwoChildRules
        {
            get;
            private set;
        }
        public ParentTwoChildRule2[] ParentTwoChildRules2
        {
            get;
            private set;
        }
        public FlatChainRule[]       FlatChainRules
        {
            get;
            private set;
        }

        public IEnumerable< RuleBase > AllRules
        {
            get
            {
                return 
                (
                    FlatChainRules.Cast< RuleBase >()
                    .Concat
                    ( 
                    ParentChildRules.Cast< RuleBase >() 
                    )
                    .Concat
                    ( 
                    ParentChildRules2.Cast< RuleBase >() 
                    )
                    .Concat
                    ( 
                    ParentTwoChildRules.Cast< RuleBase >() 
                    )
                    .Concat
                    ( 
                    ParentTwoChildRules2.Cast< RuleBase >() 
                    )
                );
            }
        }
    }
}



