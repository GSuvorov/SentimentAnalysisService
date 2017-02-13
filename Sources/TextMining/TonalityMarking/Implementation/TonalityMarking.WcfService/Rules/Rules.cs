using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

using TextMining.Core;

namespace TonalityMarking
{
    internal class Rules
    {
        public Rules( XElement rootRulesNode, 
                      XElement rootRulesOrderNode, 
                      XElement rootRulesOrderParticipialNode, 
                      XElement rootRulesOrderHomogenousNode 
                    )
        {
            Lexemes = new Lexemes();

            var subsentsNode = rootRulesNode.GetSingleNode( "Subsentences" );
            Subsentences = new Subsentences( subsentsNode );

            var sentencesNode = rootRulesNode.GetSingleNode( "Sentences" );
            Sentences = new Sentences( sentencesNode );

            var subordinateSubsentNode = rootRulesNode.GetSingleNode( "SubordinateSubsent" );
            SubordinateSubsent = new SubordinateSubsent( subordinateSubsentNode );

            var inquiryNode = rootRulesNode.GetSingleNode( "Inquiry" );
            Inquiry = new Inquiry( inquiryNode );

            UnionOfConcessionAndReversIntoductory = new UnionOfConcessionAndReversIntoductory( rootRulesNode );

            #region [.Rules by order.]
            var rulesIdsByOrder = rootRulesOrderNode.GetAttributeNodes( "Rule/@id" )
                                    .Select( _ => _.Value )
                                    .ToArray();

            _SubsentencesRulesByOrder = (from ruleId in rulesIdsByOrder
                                         select Subsentences.AllRules
                                                .ToArray()
                                                .FirstOrDefault( rule => rule.Id == ruleId )
                                        )
                                        .ToArray();

            if ( _SubsentencesRulesByOrder.Any( rule => rule.IsNull() ) )
            {
                var notexists_rulesId = (from ruleId in rulesIdsByOrder
                                         let r = Subsentences.AllRules
                                                 .FirstOrDefault( rule => rule.Id == ruleId )
                                         where ( r == null )
                                         select ruleId
                                        )
                                        .ToArray();                
                throw (new ArgumentException("In rules-order file exists rules with names, which not exists in main-rules file: " + string.Join( ", ", notexists_rulesId ).InSingleQuote()));
            }
            #endregion

            #region [.Rules by order 4 participial (Subsents, only 4 types => Participle, DanglingParticiple, Subordinate & some-Default).]
            var rulesIdsByOrderParticipial = rootRulesOrderParticipialNode.GetAttributeNodes( "Rule/@id" )
                                                .Select( _ => _.Value )
                                                .ToArray();

            _SubsentencesRulesByOrder4Participial = (from ruleId in rulesIdsByOrderParticipial
                                                     select Subsentences.AllRules
                                                            .ToArray()
                                                            .FirstOrDefault( rule => rule.Id == ruleId )
                                                    )
                                                    .ToArray();

            if ( _SubsentencesRulesByOrder4Participial.Any( rule => rule.IsNull() ) )
                throw (new ArgumentException("In rules-order-participial file exists rules with names, which not exists in main-rules file."));
            #endregion

            #region [.Rules by order 4 homogenous-entity.]
            var rulesIdsByOrderHomogenous = rootRulesOrderHomogenousNode.GetAttributeNodes( "Rule/@id" )
                                                .Select( _ => _.Value )
                                                .ToArray();

            _SubsentencesRulesByOrder4Homogenous = (from ruleId in rulesIdsByOrderHomogenous
                                                     select Subsentences.AllRules
                                                            .ToArray()
                                                            .FirstOrDefault( rule => rule.Id == ruleId )
                                                    )
                                                    .ToArray();

            if ( _SubsentencesRulesByOrder4Homogenous.Any( rule => rule.IsNull() ) )
                throw (new ArgumentException("In rules-order-homogenous file exists rules with names, which not exists in main-rules file."));
            #endregion
        }

        public Lexemes Lexemes
        {
            get;
            private set;
        }
        public Subsentences Subsentences
        {
            get;
            private set;
        }
        public Sentences Sentences
        {
            get;
            private set;
        }
        public SubordinateSubsent SubordinateSubsent
        {
            get;
            private set;
        }

        public Inquiry Inquiry
        {
            get;
            private set;
        }
        public UnionOfConcessionAndReversIntoductory UnionOfConcessionAndReversIntoductory
        {
            get;
            private set;
        }

        private RuleBase[] _SubsentencesRulesByOrder;
        public IEnumerable< RuleBase > SubsentencesRulesByOrder
        {
            get { return (_SubsentencesRulesByOrder); }
        }

        private RuleBase[] _SubsentencesRulesByOrder4Participial;
        public IEnumerable< RuleBase > SubsentencesRulesByOrder4Participial
        {
            get { return (_SubsentencesRulesByOrder4Participial); }
        }

        private RuleBase[] _SubsentencesRulesByOrder4Homogenous;
        public IEnumerable< RuleBase > SubsentencesRulesByOrder4Homogenous
        {
            get { return (_SubsentencesRulesByOrder4Homogenous); }
        }
    }
}



