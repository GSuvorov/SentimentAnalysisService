using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;
using System.Xml;

using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// ReversIntoductory
    /// </summary>
    internal class ReversIntoductory
    {
        public ReversIntoductory( XElement reversIntoductoryNode )
        {
            var nestedPairs = (from rule in reversIntoductoryNode.GetMultiNodes( "NestedPairs/Rule" )
                               select new NestedPairsRuleRI( rule )
                              )
                              .ToArray();

            _RulesByOrder = nestedPairs.Cast< RuleBase >() 
                            .ToArray();
        }

        private RuleBase[] _RulesByOrder;
        public IEnumerable< RuleBase > RulesByOrder
        {
            get { return (_RulesByOrder); }
        }
    }

    /// <summary>
    /// ReversIntoductoryNestedPairsRule
    /// </summary>
    internal class NestedPairsRuleRI : NestedPairsRuleUoC
    {
        private class action : actionBase
        {
            private HashSet< TonalityType > _ConditionHashset = new HashSet< TonalityType >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a1SNT = condition.AttributeValue2Enum< TonalityType >( "a1" );

                    _ConditionHashset.Add( a1SNT );
                }
            }

            public TonalityType SNT
            {
                get;
                private set;
            }

            public bool Exists( TonalityType a1 )
            {
                return (_ConditionHashset.Contains( a1 ));
            }

            public override string ToString()
            {
                return (SNT.ToText() + " (" + _ConditionHashset.Count + ')');
            }
        }

        public NestedPairsRuleRI( XElement ruleNode )
            : base( ruleNode, null )
        {
            Patterns = (from p in ruleNode.GetMultiNodes( "pattern" )
                                          .RevealConfigNodes()
                        select new pattern( p, this )
                       )
                       .ToArray();

            Actions = (from a in ruleNode.GetMultiNodes( "action" )
                       select new action( a )
                      )
                      .ToArray();
        }

        private action[] Actions
        {
            get;
            set;
        }

        public override bool Process( XElement sent )
        {
            sent.ThrowIfNotSent();

            var result = false;
            foreach ( var pattern in Patterns )
            {
                #region [.condition #1.]
                var pairs =
                        from s1 in sent.ElementsSubsentence()
                        where ( s1.AttributeTypeAreEqual( pattern.SubsentType ) )

                        let s2 = s1.ElementsExceptT().FirstOrDefault( _ => _.IsElementSUBSENT() &&
                                                                           _.AttributeTypeAreEqual( pattern.NestedSubsentType ) )
                        where ( s2 != null && !s2.HasElementTypeInquiry() ) 

                        let inquiry1 = s1.ElementsEntity().FirstOrDefault( _ => _.IsTypeInquiry() )
                        where ( inquiry1 != null )

                        select new
                        {
                            SUBSENT1 = s1,
                            SUBSENT2 = s2,
                            INQUIRY1 = inquiry1,
                        };
                #endregion

                foreach ( var pair in pairs )
                {
                    #region [.condition #2.]
                    var a1SNT = pair.INQUIRY1.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut );
                    var a1FRT = pair.INQUIRY1.GetAttributeFRTFINAL();

                    #region [.#1.]
                    if ( pattern.SubsentBeginPhrases.Any() )
                    {
                        var subsentValuesArray = GetSubsentValuesArray( pair.SUBSENT1, pattern.MaxItemLengthInSubsentBeginPhrases );
                        if ( pattern.SubsentBeginPhrases.All
                                ( 
                                values => !subsentValuesArray.Take( values.Length ).SequenceEqual( values )
                                ) 
                           )
                        {
                            continue;
                        }
                    }
                    #endregion

                    #region [.#2.]
                    if ( pattern.NestedSubsentBeginPhrases.Any() )
                    {
                        var subsentValuesArray = GetSubsentValuesArray( pair.SUBSENT2, pattern.MaxItemLengthInNestedSubsentBeginPhrases );
                        if ( pattern.NestedSubsentBeginPhrases.All
                                ( 
                                values => !subsentValuesArray.Take( values.Length ).SequenceEqual( values )
                                ) 
                           )
                        {
                            continue;
                        }
                    }
                    #endregion
                    #endregion

                    MatchRuleDebugInfoOutput( pair.SUBSENT1 );

                    var localresult = false;
                    foreach ( var a in Actions )
                    {
                        if ( a.Exists( a1SNT ) )
                        {
                            #region [.match action.]
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;

                            base.SetOnSubsentSNTFINALnFRTFINAL( pair.SUBSENT1, a.SNT, a1FRT + 1 );
                            base.SetOnSubsentSNTFINALnFRTFINAL( pair.SUBSENT2, a.SNT, a1FRT + 1 );
                            break; 
                            #endregion
                        }
                    }

                    if ( !localresult )
                        NomatchActionDebugInfoOutput();
                }
            }
            return (result);
        }
    }
}



