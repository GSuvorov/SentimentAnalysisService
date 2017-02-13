using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

using TextMining.Core;

namespace TonalityMarking
{
    internal class Sentences
    {
        public Sentences( XElement sentencesNode )
        {            
            OneInSubsentsRules = (from rule in sentencesNode.GetMultiNodes( "OneInSubsents/Rule" )
                                  select new OneInSubsentsRule( rule )
                                 )
                                 .ToArray(); 

            PairsInSubsentsRules = (from rule in sentencesNode.GetMultiNodes( "PairsInSubsents/Rule" )
                                    select new PairsInSubsentsRule( rule )
                                   )
                                   .ToArray();

            ThreeInSubsentsRules = (from rule in sentencesNode.GetMultiNodes( "ThreeInSubsents/Rule" )
                                    select new ThreeInSubsentsRule( rule )
                                   )
                                   .ToArray();

            FourInSubsentsRules = (from rule in sentencesNode.GetMultiNodes( "FourInSubsents/Rule" )
                                   select new FourInSubsentsRule( rule )
                                  )
                                  .ToArray();            

            _RulesByOrder = FourInSubsentsRules.Cast< RuleBase >()
                           .Concat
                           (
                           ThreeInSubsentsRules.Cast< RuleBase >()
                           )
                           .Concat
                           (
                           PairsInSubsentsRules.Cast< RuleBase >()
                           )
                           .Concat
                           (
                           OneInSubsentsRules.Cast< RuleBase >() 
                           )
                           .ToArray();



            var subsentsNoNoun_One = (from rule in sentencesNode.GetMultiNodes("SubsentsNoNoun/One/Rule")
                                      select new SubsentsNoNoun_One( rule )
                                     )
                                     .ToArray();
          var subsentsNoNoun_Pairs = (from rule in sentencesNode.GetMultiNodes("SubsentsNoNoun/Pairs/Rule")
                                      select new SubsentsNoNoun_Pairs( rule )
                                     )
                                     .ToArray();
            _SubsentsNoNoun = subsentsNoNoun_Pairs.Cast< RuleBase >()
                             .Concat
                             (
                             subsentsNoNoun_One.Cast< RuleBase >()
                             )
                             .ToArray();

            #region [.commented. previous.]
            /*
            var subsentsNoNounNodes = sentencesNode.GetMultiNodes( "SubsentsNoNoun/Rule" );
            if ( subsentsNoNounNodes.Count() != 1 )
                throw (new ArgumentException("SubsentsNoNoun-rules more than one"));
            SubsentsNoNoun = new SubsentsNoNoun( subsentsNoNounNodes.First() );           
            */
            #endregion
        }

        public OneInSubsentsRule[] OneInSubsentsRules
        {
            get;
            private set;
        }
        public PairsInSubsentsRule[] PairsInSubsentsRules
        {
            get;
            private set;
        }
        public ThreeInSubsentsRule[] ThreeInSubsentsRules
        {
            get;
            private set;
        }
        public FourInSubsentsRule[] FourInSubsentsRules
        {
            get;
            private set;
        }        

        private RuleBase[] _RulesByOrder;
        public IEnumerable< RuleBase > RulesByOrder
        {
            get { return (_RulesByOrder); }
        }

        private RuleBase[] _SubsentsNoNoun;
        public IEnumerable< RuleBase > SubsentsNoNoun
        {
            get { return (_SubsentsNoNoun); }
        }
    }
}
