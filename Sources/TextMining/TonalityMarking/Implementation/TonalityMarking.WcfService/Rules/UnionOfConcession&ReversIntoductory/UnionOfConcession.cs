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
    /// UnionOfConcession
    /// </summary>
    internal class UnionOfConcession
    {
        public UnionOfConcession( XElement unionOfConcessionNode )
        {
            var pairs = (from rule in unionOfConcessionNode.GetMultiNodes( "Pairs/Rule" )
                         select new PairsRuleUoC( rule )
                        )
                        .ToArray();

            var pairs2 = (from rule in unionOfConcessionNode.GetMultiNodes( "Pairs2/Rule" )
                          select new Pairs2RuleUoC( rule )
                         )
                         .ToArray();

            var nestedPairs = (from rule in unionOfConcessionNode.GetMultiNodes( "NestedPairs/Rule" )
                               select new NestedPairsRuleUoC( rule )
                              )
                              .ToArray();

            _RulesByOrder = pairs.Cast< RuleBase >()
                           .Concat
                           ( 
                            pairs2.Cast< RuleBase >() 
                           )
                           .Concat
                           ( 
                            nestedPairs.Cast< RuleBase >() 
                           )
                           .ToArray();
        }

        private RuleBase[] _RulesByOrder;
        public IEnumerable< RuleBase > RulesByOrder
        {
            get { return (_RulesByOrder); }
        }
    }

    /// <summary>
    /// UnionOfConcessionRuleBase
    /// </summary>
    internal abstract class RuleBaseUoC : RuleBase
    {
        protected RuleBaseUoC( XElement ruleNode )
            : base( ruleNode )
        {
        }

        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.None); }
        }

        protected string[] GetSubsentValuesArray( XElement subsent, int maxTakeCount = int.MaxValue )
        {
            subsent.ThrowIfNotSubsent();

            return
                (from e in subsent.ElementsExceptT()
                 where !e.IsElementSUBSENT()
                 let value = e.GetAttributeVALUE()
                 select (value.IsEmptyOrNull()  ? e.Value : value)
                         .ToLowerEx()
                         .SplitBySpace()
                         .Select( _ => _.TrimEndSignOfPunctuation().TrimWhiteSpaces() )
                         .Where ( _ => !_.IsEmptyOrNull() )
                )
                .SelectMany( _ => _ )
                .Take( maxTakeCount )
                .ToArray();
        }
        protected void SetOnSubsentSNTFINAL( XElement subsent, TonalityType sntfinal )
        {
            subsent.ThrowIfNotSubsent();


            subsent.SetAttributeSNTFINAL( sntfinal );

            foreach ( var element in from e in subsent.ElementsExceptT()
                                     where !e.IsElementSUBSENT()
                                     from d in e.DescendantsAndSelf()
                                     where d.HasAttributeSNTFINAL()
                                     select d )
            {
                element.SetAttributeSNTFINAL( sntfinal );
            }
        }
        protected void SetOnSubsentSNTFINALnFRTFINAL( XElement subsent, TonalityType sntfinal, int frtfinal )
        {
            subsent.ThrowIfNotSubsent();


            subsent.SetAttributeSNTFINAL( sntfinal );
            subsent.SetAttributeFRTFINAL( frtfinal );

            foreach ( var element in from e in subsent.ElementsExceptT()
                                     where !e.IsElementSUBSENT()
                                     from d in e.DescendantsAndSelf()
                                     where d.HasAttributeSNTFINAL()
                                     select d )
            {
                element.SetAttributeSNTFINAL( sntfinal );
                element.SetAttributeFRTFINAL( frtfinal );
            }
        }

        internal List< string[] > GetPatternValuesArray( string values )
        {
            return
                (   !values.IsEmptyOrNull()
                    ? values.ToLowerEx().Split( new[] { "||" }, StringSplitOptions.RemoveEmptyEntries )
                                  .Select( value => value.TrimWhiteSpaces().SplitBySpace() )
                                  .Select( _1 => _1.Select( _2 => _2.TrimWhiteSpaces() )
                                                   .Where( _3 => !_3.IsEmptyOrNull() )
                                                   .ToArray() )
                                  .ToList()
                    : new List< string[] >()
                );
        }
    }

    /// <summary>
    /// UnionOfConcessionPairsRule
    /// </summary>
    internal class PairsRuleUoC : RuleBaseUoC
    {
        protected class action : actionBase
        {
            private HashSet< Pair< TonalityType, TonalityType > > _ConditionHashset = new HashSet< Pair< TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT1 = actionNode.AttributeValue2Enum< TonalityType >( "SNT1" );
                SNT2 = actionNode.AttributeValue2Enum< TonalityType >( "SNT2" );

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a1SNT = condition.AttributeValue2Enum< TonalityType >( "a1" );
                    var a2SNT = condition.AttributeValue2Enum< TonalityType >( "a2" );

                    _ConditionHashset.Add( new Pair< TonalityType, TonalityType >( a1SNT, a2SNT ) );
                }
            }

            public TonalityType SNT1
            {
                get;
                private set;
            }
            public TonalityType SNT2
            {
                get;
                private set;
            }

            public bool Exists( TonalityType a1, TonalityType a2 )
            {
                return (_ConditionHashset.Contains( new Pair< TonalityType, TonalityType >( a1, a2 ) ));
            }

            public override string ToString()
            {
                return (SNT1.ToText() +  " - " + SNT2.ToText() +  " (" + _ConditionHashset.Count + ')');
            }
        }
        protected class pattern
        {
            public pattern( XElement patternNode, PairsRuleUoC ownerRule )
            {
                var childs = patternNode.Elements().ToArray();
                if ( childs.Length != 2 )
                    throw (new ArgumentException("childs.Length != 2 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                #region [.SUBSENT #1.]
                if ( !childs[ 0 ].IsElementSUBSENT() )
                    throw (new ArgumentException("!childs[ 0 ].IsElementSUBSENT() (rule " + ownerRule.Id.InSingleQuote() + ')'));
                if ( childs[ 0 ].HasElements )
                    throw (new ArgumentException("childs[ 0 ].HasElements (rule " + ownerRule.Id.InSingleQuote() + ')')); 

                Subsent1Type         = childs[ 0 ].GetAttributeSUBSENTTYPE();
                Subsent1BeginPhrases = ownerRule.GetPatternValuesArray( childs[ 0 ].Value.TrimWhiteSpaces() );
                #endregion

                #region [.SUBSENT #2.]
                if ( !childs[ 1 ].IsElementSUBSENT() )
                    throw (new ArgumentException("!childs[ 1 ].IsElementSUBSENT() (rule " + ownerRule.Id.InSingleQuote() + ')'));
                if ( childs[ 1 ].HasElements )
                    throw (new ArgumentException("childs[ 1 ].HasElements (rule " + ownerRule.Id.InSingleQuote() + ')')); 

                Subsent2Type         = childs[ 1 ].GetAttributeSUBSENTTYPE();
                Subsent2BeginPhrases = ownerRule.GetPatternValuesArray( childs[ 1 ].Value.TrimWhiteSpaces() );
                #endregion
            }

            public SubsentenceType Subsent1Type
            {
                get;
                private set;
            }
            public List< string[] > Subsent1BeginPhrases
            {
                get;
                private set;
            }
            private int? _MaxItemLengthInSubsent1BeginPhrases;
            public int MaxItemLengthInSubsent1BeginPhrases
            {
                get 
                {
                    if ( !_MaxItemLengthInSubsent1BeginPhrases.HasValue )
                    {
                        _MaxItemLengthInSubsent1BeginPhrases = Subsent1BeginPhrases.Max( _ => _.Length );
                    }
                    return (_MaxItemLengthInSubsent1BeginPhrases.Value); 
                }
            }

            public SubsentenceType Subsent2Type
            {
                get;
                private set;
            }
            public List< string[] > Subsent2BeginPhrases
            {
                get;
                private set;
            }
            private int? _MaxItemLengthInSubsent2BeginPhrases;
            public int MaxItemLengthInSubsent2BeginPhrases
            {
                get 
                {
                    if ( !_MaxItemLengthInSubsent2BeginPhrases.HasValue )
                    {
                        _MaxItemLengthInSubsent2BeginPhrases = Subsent2BeginPhrases.Max( _ => _.Length );
                    }
                    return (_MaxItemLengthInSubsent2BeginPhrases.Value); 
                }
            }

            public override string ToString()
            {
                return ("{SUB_SENT[@TYPE='" + Subsent1Type + "']:{" + Subsent1BeginPhrases.SelectMany( _ => _ ).JoinBy(" || ")  + "} - SUB_SENT[@TYPE='" + Subsent2Type + "']:{" + Subsent2BeginPhrases.SelectMany( _ => _ ).JoinBy(" || ") + "}}");
            }
        }

        public PairsRuleUoC( XElement ruleNode )
            : base( ruleNode )
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

        protected action[] Actions
        {
            get;
            set;
        }
        protected pattern[] Patterns
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
                        where ( s1.AttributeTypeAreEqual( pattern.Subsent1Type ) )

                        let s2 = s1.ElementsAfterSelfExceptT().FirstOrDefault( _ => _.IsElementSUBSENT() )
                        where ( (s2 != null) && 
                                 s2.AttributeTypeAreEqual( pattern.Subsent2Type ) ) 

                        let inquiry1 = s1.ElementsEntity()/*DescendantsAndSelfEntity()*/.FirstOrDefault( _ => _.IsTypeInquiry() )                        
                        where ( inquiry1 != null )
                        let inquiry2 = s2.ElementsEntity()/*DescendantsAndSelfEntity()*/.FirstOrDefault( _ => _.IsTypeInquiry() )
                        where ( inquiry2 != null )

                        select new
                        {
                            SUBSENT1 = s1,
                            SUBSENT2 = s2,
                            INQUIRY1 = inquiry1,
                            INQUIRY2 = inquiry2,
                        };
                #endregion

                foreach ( var pair in pairs )
                {
                    #region [.condition #2.]
                    var a1SNT = pair.INQUIRY1.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut );
                    var a2SNT = pair.INQUIRY2.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut );

                    if ( (a1SNT == TonalityType.nNeut) && (a2SNT == TonalityType.nNeut) )
                        continue;

                    #region [.#1.]
                    if ( pattern.Subsent1BeginPhrases.Any() )
                    {
                        var subsentValuesArray = GetSubsentValuesArray( pair.SUBSENT1, pattern.MaxItemLengthInSubsent1BeginPhrases );
                        if ( pattern.Subsent1BeginPhrases.All
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
                    if ( pattern.Subsent2BeginPhrases.Any() )
                    {
                        var subsentValuesArray = GetSubsentValuesArray( pair.SUBSENT2, pattern.MaxItemLengthInSubsent2BeginPhrases );
                        if ( pattern.Subsent2BeginPhrases.All
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
                        if ( a.Exists( a1SNT, a2SNT ) )
                        {
                            #region [.match action.]
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;

                            base.SetOnSubsentSNTFINAL( pair.SUBSENT1, a.SNT1 );
                            base.SetOnSubsentSNTFINAL( pair.SUBSENT2, a.SNT2 );
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

    /// <summary>
    /// 
    /// </summary>
    internal class Pairs2RuleUoC : PairsRuleUoC
    {
        public Pairs2RuleUoC( XElement ruleNode )
            : base( ruleNode )
        {
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
                        where ( s1.AttributeTypeAreEqual( pattern.Subsent1Type ) )

                        let s2 = s1.ElementsAfterSelfExceptT().FirstOrDefault( _ => _.IsElementSUBSENT() )
                        where ( (s2 != null) && 
                                 s2.AttributeTypeAreEqual( pattern.Subsent2Type ) ) 

                        let inquiry1 = s1.ElementsEntity()/*DescendantsAndSelfEntity()*/.FirstOrDefault( _ => _.IsTypeInquiry() )                        
                        where ( inquiry1 != null )
                        let inquiry2 = s2.ElementsEntity()/*DescendantsAndSelfEntity()*/.FirstOrDefault( _ => _.IsTypeInquiry() )
                        where ( inquiry2 != null )

                        select new
                        {
                            SUBSENT1 = s1,
                            SUBSENT2 = s2,
                            INQUIRY1 = inquiry1,
                            INQUIRY2 = inquiry2,
                        };
                #endregion

                foreach ( var pair in pairs )
                {
                    #region [.condition #2.]
                    var a1SNT = pair.INQUIRY1.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut );
                    var a2SNT = pair.INQUIRY2.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut );

                    if ( (a1SNT == TonalityType.nNeut) && (a2SNT == TonalityType.nNeut) )
                        continue;

                    #region [.#1.]
                    if ( pattern.Subsent1BeginPhrases.Any() )
                    {
                        var subsentValuesArray = GetSubsentValuesArray( pair.SUBSENT1 /*, pattern.MaxItemLengthInSubsent1BeginPhrases*/ );
                        if ( pattern.Subsent1BeginPhrases.All
                                ( 
                                values => !subsentValuesArray.SearchForSubsequence( values )
                                ) 
                           )
                        {
                            continue;
                        }
                    }
                    #endregion

                    #region [.#2.]
                    if ( pattern.Subsent2BeginPhrases.Any() )
                    {
                        var subsentValuesArray = GetSubsentValuesArray( pair.SUBSENT2, pattern.MaxItemLengthInSubsent2BeginPhrases );
                        if ( pattern.Subsent2BeginPhrases.All
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
                        if ( a.Exists( a1SNT, a2SNT ) )
                        {
                            #region [.match action.]
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;

                            base.SetOnSubsentSNTFINAL( pair.SUBSENT1, a.SNT1 );
                            base.SetOnSubsentSNTFINAL( pair.SUBSENT2, a.SNT2 );
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

    /// <summary>
    /// UnionOfConcessionNestedPairsRule
    /// </summary>
    internal class NestedPairsRuleUoC : RuleBaseUoC
    {
        private class action : actionBase
        {
            private HashSet< Pair< TonalityType, TonalityType > > _ConditionHashset = new HashSet< Pair< TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT1 = actionNode.AttributeValue2Enum< TonalityType >( "SNT1" );
                SNT2 = actionNode.AttributeValue2Enum< TonalityType >( "SNT2" );

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a1SNT = condition.AttributeValue2Enum< TonalityType >( "a1" );
                    var a2SNT = condition.AttributeValue2Enum< TonalityType >( "a2" );

                    _ConditionHashset.Add( new Pair< TonalityType, TonalityType >( a1SNT, a2SNT ) );
                }
            }

            public TonalityType SNT1
            {
                get;
                private set;
            }
            public TonalityType SNT2
            {
                get;
                private set;
            }

            public bool Exists( TonalityType a1, TonalityType a2 )
            {
                return (_ConditionHashset.Contains( new Pair< TonalityType, TonalityType >( a1, a2 ) ));
            }

            public override string ToString()
            {
                return (SNT1.ToText() +  " - " + SNT2.ToText() +  " (" + _ConditionHashset.Count + ')');
            }
        }
        protected class pattern
        {
            public pattern( XElement patternNode, NestedPairsRuleUoC ownerRule )
            {
                if ( patternNode.Elements().Count() != 1 )
                    throw (new ArgumentException("patternNode.Elements().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                #region [.SUBSENT.]
                var subsent = patternNode.Elements().First();
                if ( !subsent.IsElementSUBSENT() )
                    throw (new ArgumentException("!subsent.IsElementSUBSENT() (rule " + ownerRule.Id.InSingleQuote() + ')'));

                SubsentType = subsent.GetAttributeSUBSENTTYPE();
                var values = subsent.Nodes().Where( _ => _.NodeType == XmlNodeType.Text ).Select( _ => _.ToString() );                
                SubsentBeginPhrases = ownerRule.GetPatternValuesArray( values.JoinBy( " || " ).TrimWhiteSpaces() );
                #endregion

                #region [.NESTED SUBSENT.]
                if ( subsent.Elements().Count() != 1 )
                    throw (new ArgumentException("subsent.Elements().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                var nestedSubsent = subsent.Elements().First();
                if ( !nestedSubsent.IsElementSUBSENT() )
                    throw (new ArgumentException("nestedSubsent.IsElementSUBSENT() (rule " + ownerRule.Id.InSingleQuote() + ')'));
                if ( nestedSubsent.HasElements )
                    throw (new ArgumentException("nestedSubsent.HasElements (rule " + ownerRule.Id.InSingleQuote() + ')'));

                NestedSubsentType         = nestedSubsent.GetAttributeSUBSENTTYPE();
                NestedSubsentBeginPhrases = ownerRule.GetPatternValuesArray( nestedSubsent.Value.TrimWhiteSpaces() );
                #endregion
            }

            public SubsentenceType SubsentType
            {
                get;
                private set;
            }
            public List< string[] > SubsentBeginPhrases
            {
                get;
                private set;
            }
            private int? _MaxItemLengthInSubsentBeginPhrases;
            public int MaxItemLengthInSubsentBeginPhrases
            {
                get 
                {
                    if ( !_MaxItemLengthInSubsentBeginPhrases.HasValue )
                    {
                        _MaxItemLengthInSubsentBeginPhrases = SubsentBeginPhrases.Max( _ => _.Length );
                    }
                    return (_MaxItemLengthInSubsentBeginPhrases.Value); 
                }
            }

            public SubsentenceType NestedSubsentType
            {
                get;
                private set;
            }
            public List< string[] > NestedSubsentBeginPhrases
            {
                get;
                private set;
            }
            private int? _MaxItemLengthInNestedSubsentBeginPhrases;
            public int MaxItemLengthInNestedSubsentBeginPhrases
            {
                get 
                {
                    if ( !_MaxItemLengthInNestedSubsentBeginPhrases.HasValue )
                    {
                        _MaxItemLengthInNestedSubsentBeginPhrases = NestedSubsentBeginPhrases.Max( _ => _.Length );
                    }
                    return (_MaxItemLengthInNestedSubsentBeginPhrases.Value); 
                }
            }

            public override string ToString()
            {
                return ("{SUB_SENT[@TYPE='" + SubsentType + "']:{" + SubsentBeginPhrases.SelectMany( _ => _ ).JoinBy(" || ")  + "} - [NESTED] SUB_SENT[@TYPE='" + NestedSubsentType + "']:{" + NestedSubsentBeginPhrases.SelectMany( _ => _ ).JoinBy(" || ") + "}}");
            }
        }

        public NestedPairsRuleUoC( XElement ruleNode )
            : base( ruleNode )
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
        protected NestedPairsRuleUoC( XElement ruleNode, object dymmy4DoNotProcessAction )
            : base( ruleNode )
        {
            Patterns = (from p in ruleNode.GetMultiNodes( "pattern" )
                                                .RevealConfigNodes() 
                        select new pattern( p, this )
                       )
                       .ToArray();
        }

        private action[] Actions
        {
            get;
            set;
        }
        protected pattern[] Patterns
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
                        where ( s2 != null) 

                        let inquiry1 = s1.ElementsEntity().FirstOrDefault( _ => _.IsTypeInquiry() )
                        let inquiry2 = s2.ElementsEntity().FirstOrDefault( _ => _.IsTypeInquiry() )
                        where ( inquiry2 != null )

                        select new
                        {
                            SUBSENT1 = s1,
                            SUBSENT2 = s2,
                            a1SNT = (inquiry1 != null) ? inquiry1.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut ) : TonalityType.nNeut,
                            a2SNT = inquiry2.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut ),
                        };
                        #region commented. (temporary?) (22.01.2013)
                        /*
                        let inquiry1 = s1.ElementsEntity().FirstOrDefault( _ => _.IsTypeInquiry() )
                        where ( inquiry1 != null )
                        let inquiry2 = s2.ElementsEntity().FirstOrDefault( _ => _.IsTypeInquiry() )
                        where ( inquiry2 != null )

                        select new
                        {
                            SUBSENT1 = s1,
                            SUBSENT2 = s2,
                            INQUIRY1 = inquiry1,
                            INQUIRY2 = inquiry2,
                        };
                        */
                        #endregion
                #endregion

                foreach ( var pair in pairs )
                {
                    #region [.condition #2.]
                    #region commented. (temporary?) (22.01.2013)
                    /*
                    var a1SNT = pair.INQUIRY1.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut );
                    var a2SNT = pair.INQUIRY2.TryGetAttributeSNTFINAL().GetValueOrDefault( TonalityType.nNeut );

                    if ( (a1SNT == TonalityType.nNeut) && (a2SNT == TonalityType.nNeut) )
                        continue;
                    */
                    #endregion

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
                        if ( a.Exists( pair.a1SNT, pair.a2SNT ) )
                        {
                            #region [.match action.]
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;

                            base.SetOnSubsentSNTFINAL( pair.SUBSENT1, a.SNT1 );
                            base.SetOnSubsentSNTFINAL( pair.SUBSENT2, a.SNT2 );
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



