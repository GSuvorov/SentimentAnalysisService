using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;
using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    internal class Inquiry
    {
        public Inquiry( XElement inquiryNode )
        {
            PairsInSubsentsInqueryRules = (from rule in inquiryNode.GetMultiNodes("PairsInSubsents/Rule")
                                           select new PairsInSubsentsInqueryRule( rule )
                                          )
                                          .ToArray();

            ThreeInSubsentsInqueryRules = (from rule in inquiryNode.GetMultiNodes("ThreeInSubsents/Rule")
                                           select new ThreeInSubsentsInqueryRule( rule )
                                          )
                                          .ToArray();

            ThreeInSubsentsExInqueryRules = (from rule in inquiryNode.GetMultiNodes("ThreeInSubsents-Ex/Rule")
                                             select new ThreeInSubsentsExInqueryRule( rule )
                                            )
                                            .ToArray();

            FourInSubsentsInqueryRules = (from rule in inquiryNode.GetMultiNodes("FourInSubsents/Rule")
                                          select new FourInSubsentsInqueryRule( rule )
                                         )
                                         .ToArray();
            
            FiveInSubsentsInqueryRules = (from rule in inquiryNode.GetMultiNodes("FiveInSubsents/Rule")
                                          select new FiveInSubsentsInqueryRule( rule )
                                         )
                                         .ToArray();

            _RulesByOrder = FiveInSubsentsInqueryRules.Cast< InqueryRule >()
                           .Concat
                           (
                           FourInSubsentsInqueryRules.Cast< InqueryRule >()
                           )
                           .Concat
                           ( 
                           ThreeInSubsentsInqueryRules.Cast< InqueryRule >() 
                           )
                           .Concat
                           ( 
                           ThreeInSubsentsExInqueryRules.Cast< InqueryRule >() 
                           )
                           .Concat
                           ( 
                           PairsInSubsentsInqueryRules.Cast< InqueryRule >() 
                           )
                           .ToArray();
        }

        public PairsInSubsentsInqueryRule[]   PairsInSubsentsInqueryRules
        {
            get;
            private set;
        }
        public ThreeInSubsentsInqueryRule[]   ThreeInSubsentsInqueryRules
        {
            get;
            private set;
        }
        public ThreeInSubsentsExInqueryRule[] ThreeInSubsentsExInqueryRules
        {
            get;
            private set;
        }
        public FourInSubsentsInqueryRule[]    FourInSubsentsInqueryRules
        {
            get;
            private set;
        }
        public FiveInSubsentsInqueryRule[]    FiveInSubsentsInqueryRules
        {
            get;
            private set;
        }        

        private InqueryRule[] _RulesByOrder;
        public IEnumerable< InqueryRule > RulesByOrder
        {
            get { return (_RulesByOrder); }
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal abstract class InqueryRule : RuleBase
    {
        public InqueryRule( XElement ruleNode )
            : base( ruleNode )
        {
        }

        protected static void SetSNTFINAL( TonalityType finalSNT, params XElement[] elements )
        {
            var INQUIRY = elements.GetInquiryElement();
            if ( INQUIRY == null )
                throw (new InvalidOperationException("No item with @TYPE='Inquiry' found"));

            //Set @SNTFINAL for all elements
#if MAX_FRTFINAL
            var finalFRT = elements.Max( e => e.GetAttributeFRT() );
#else
            var finalFRT = elements.Sum( e => e.GetAttributeFRT() );
#endif
            foreach ( var element in elements )
            {
                element.SetAttributeSNTFINAL( finalSNT );
                element.SetAttributeFRTFINAL( finalFRT );
            }

            //Put @SNTFINAL on linked Subsentense
            var ids = from element in elements
                           from entities in element.DescendantsAndSelfEntities()
                           let  id = entities.GetAttributeID()
                           where id.HasValue
                           select id.Value;
            foreach ( var id in ids.Distinct() )
            {
                var linkedSubsent = elements[ 0 ].FindSubsentById( id );
                if ( linkedSubsent != null )
                {
                    linkedSubsent.SetAttributeSNTFINAL( finalSNT );
                    linkedSubsent.SetAttributeFRTFINAL( finalFRT );
                }
            }
            #region [.commented. previous.]
            /*var ENTITIES = elements.GetEntitiesElement();
            if ( ENTITIES != null )
            {
                var ID = ENTITIES.GetAttributeID();
                if ( ID.HasValue )
                {
                    var linkedSubsent = elements[ 0 ].FindSubsentenceById( ID.Value );
                    if ( linkedSubsent != null )
                    {
                        linkedSubsent.SetAttributeSNTFINAL( finalSNT );
                    }
                }
            }*/
            #endregion
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal static class Extension4Inquiry
    {
        private static readonly string[] NeutTonalityTypes = new[] { TonalityType.nNeut    .ToText(),
                                                                     TonalityType.vNeut    .ToText(), 
                                                                     TonalityType.adjNeut  .ToText(),
                                                                     TonalityType.advNeut  .ToText(),
                                                                     TonalityType.collNeut .ToText(),
                                                                     TonalityType.collvNeut.ToText(),                                                                     
                                                                   };

        private static bool HasSNTAttributeWithNotNeut( this XElement element )
        {
            var a = element.Attribute( "SNT" );
            if ( a == null )
                return (false);
            return (!NeutTonalityTypes.Contains( a.Value ));
        }
        public static XElement GetNextSiblingWithSkipSNTNeuts( this XElement element, 
                                string name, 
                                RoleAttributeValue? role, 
                                TypeAttributeValue? type,
                                bool allowWithSNTNeut = false
                               )
        {
            foreach ( var nextSibling in element.ElementsAfterSelfExceptT() )
            {
                if ( nextSibling.Name.LocalName == name )
                {
                    if ( ((allowWithSNTNeut || nextSibling.HasSNTAttributeWithNotNeut()) ||
                          nextSibling.IsTypeInquiry()) &&
                         (!role.HasValue ||
                           nextSibling.HasRoleAttributeValue( role.Value )) &&
                         (!type.HasValue ||
                           nextSibling.HasTypeAttributeValue( type.Value )) )
                    {
                        return (nextSibling);
                    }
                }
                else if ( nextSibling.IsElementSUBSENT() )
                {
                    continue;
                }
                else if ( nextSibling.HasSNTAttributeWithNotNeut() )
                {
                    return (null);
                }
            }
            return (null);
        }
        public static XElement GetNextSiblingWithSkipSNTNeuts( this XElement element, 
                                string              name, 
                                RoleAttributeValue? role, 
                                TypeAttributeValue? type,
                                string              nameNext,
                                RoleAttributeValue? roleNext,
                                TypeAttributeValue? typeNext 
                               )
        {
            var next = element.GetNextSiblingWithSkipSNTNeuts( name, role, type );
            if ( next != null )
            {
                var nextNext = next.GetNextSiblingWithSkipSNTNeuts( nameNext, roleNext, typeNext, true );
                if ( nextNext != null )
                    return (next);
            }
            else
            {
                next = element.GetNextSiblingWithSkipSNTNeuts( name, role, type, true );
                if ( next != null )
                {
                    var nextNext = next.GetNextSiblingWithSkipSNTNeuts( nameNext, roleNext, typeNext, false );
                    if ( nextNext != null )
                        return (next);
                }
            }
            return (null);
        }

        public static bool HasRoleAttributeValue( this XElement element, RoleAttributeValue role )
        {
            return (element.HasAttributeWithValue( "ROLE", role.ToString() ) );
        }
        public static bool HasTypeAttributeValue( this XElement element, TypeAttributeValue type )
        {
            return (element.HasAttributeWithValue( "TYPE", type.ToString() ) );
        }
        public static bool HasTypeAttributeManyValue( this XElement element, params TypeAttributeValue[] type )
        {
            return (element.HasAttributeWithAnyValue( "TYPE", type.Select( _ => _.ToString() ).ToArray() ));
        }

        public static XElement GetInquiryElement( this XElement[] elements )
        {
            return (elements.FirstOrDefault( _ => _.IsTypeInquiry() ));
        }
        public static XElement GetEntitiesElement( this XElement[] elements )
        {
            return (elements.FirstOrDefault( _ => _.IsElementENTITIES() ));
        }

        public static string[] GetMI4Pattern( this XElement element )
        {
            var mi = element.GetAttributeMI();
            var MI = (mi != null) ? mi.SplitBy( '|' ).Select( _ => _.TrimWhiteSpaces() )
                                                     .Where( _ => !_.IsEmptyOrNull() )
                                                     .Distinct()
                                                     .ToArray() : null;
            return (MI);
        }
        public static bool SuccessByMI( this XElement e, string[] mi_array )
        {
            if ( mi_array == null )
                return (true);

            var mi = e.GetAttributeMI();
            if (mi == null)
                return (false);
            
            var result = mi.SplitByComma().Intersect( mi_array );
            return (result.Any());
        }
    }

    /// <summary>
    /// PairsInSubsents
    /// </summary>
    internal class PairsInSubsentsInqueryRule : InqueryRule
    {
        private class action : actionBase
        {
            private HashSet< Pair< TonalityType, TonalityType > > _ConditionHashset = new HashSet< Pair< TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a1SNT = condition.AttributeValue2Enum< TonalityType >( "a1" );
                    var a2SNT = condition.AttributeValue2Enum< TonalityType >( "a2" );

                    _ConditionHashset.Add( new Pair< TonalityType, TonalityType >( a1SNT, a2SNT ) );
                }
            }

            public TonalityType SNT
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
                return (SNT.ToText() + " (" + _ConditionHashset.Count + ')');
            }
        }
        private class pattern
        {
            public pattern( XElement patternNode, PairsInSubsentsInqueryRule ownerRule )
            {
                var childs = patternNode.Elements().ToArray();
                if ( childs.Length != 2 )
                    throw (new ArgumentException("childs.Length != 2 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                FirstName = childs[ 0 ].Name.LocalName;
                FirstROLE = childs[ 0 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                FirstTYPE = childs[ 0 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();
                FirstMI   = childs[ 0 ].GetMI4Pattern();

                SecondName = childs[ 1 ].Name.LocalName;
                SecondROLE = childs[ 1 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                SecondTYPE = childs[ 1 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();
                SecondMI   = childs[ 1 ].GetMI4Pattern();

                //check
                if ( FirstTYPE  != TypeAttributeValue.Inquiry && 
                     SecondTYPE != TypeAttributeValue.Inquiry )
                {
                    throw (new ArgumentException("No item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
                if ( FirstTYPE  == TypeAttributeValue.Inquiry &&
                     SecondTYPE == TypeAttributeValue.Inquiry )
                {
                    throw (new ArgumentException("More than one item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            }

            public string FirstName
            {
                get;
                private set;
            }
            public RoleAttributeValue? FirstROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? FirstTYPE
            {
                get;
                private set;
            }
            public string[] FirstMI
            {
                get;
                private set;
            }

            public string SecondName
            {
                get;
                private set;
            }
            public RoleAttributeValue? SecondROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? SecondTYPE
            {
                get;
                private set;
            }
            public string[] SecondMI
            {
                get;
                private set;
            }

            public override string ToString()
            {
                return ('{' + FirstName + "-" + SecondName + '}');
            }
        }

        public PairsInSubsentsInqueryRule( XElement ruleNode )
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

        private action[] Actions
        {
            get;
            set;
        }
        private pattern[] Patterns
        {
            get;
            set;
        }

        public override bool Process( XElement subsentElement )
        {
            subsentElement.ThrowIfNotSubsent();

            var result = false;
            foreach ( var pattern in Patterns )
            {
                #region [.condition.]
                var pairs = 
                        from first in subsentElement.Elements( pattern.FirstName )
                        where ( !pattern.FirstROLE.HasValue ||                              
                                 first.HasRoleAttributeValue( pattern.FirstROLE.Value ) ) &&
                              ( !pattern.FirstTYPE.HasValue ||
                                 first.HasTypeAttributeValue( pattern.FirstTYPE.Value ) )

                        let second = first.GetNextSiblingWithSkipSNTNeuts( pattern.SecondName, pattern.SecondROLE, pattern.SecondTYPE )
                        where ( second != null ) 

                        where ( first .SuccessByMI( pattern.FirstMI  ) &&
                                second.SuccessByMI( pattern.SecondMI ) 
                              )

                        select new
                        {
                            FIRST  = first,
                            SECOND = second,
                        };
                #endregion

                var pair = pairs.FirstOrDefault();
                if ( pair != null )
                {
                    MatchRuleDebugInfoOutput( pair.FIRST );

                    var a1SNT = pair.FIRST .GetAttributeSNT();
                    var a2SNT = pair.SECOND.GetAttributeSNT();

                    var localresult = false;
                    foreach ( var a in Actions )
                    {
                        if ( a.Exists( a1SNT, a2SNT ) )
                        {
                            #region
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;

                            SetSNTFINAL
                                ( 
                                a.SNT, 
                                pair.FIRST, 
                                pair.SECOND 
                                );
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
        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.None); }
        }
    }

    /// <summary>
    /// ThreeInSubsents
    /// </summary>
    internal class ThreeInSubsentsInqueryRule : InqueryRule
    {
        private class action : actionBase
        {
            private HashSet< Tuple3< TonalityType, TonalityType, TonalityType > > _ConditionHashset = new HashSet< Tuple3< TonalityType, TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a1SNT = condition.AttributeValue2Enum< TonalityType >( "a1" );
                    var a2SNT = condition.AttributeValue2Enum< TonalityType >( "a2" );
                    var a3SNT = condition.AttributeValue2Enum< TonalityType >( "a3" );

                    _ConditionHashset.Add( new Tuple3< TonalityType, TonalityType, TonalityType >( a1SNT, a2SNT, a3SNT ) );
                }
            }

            public TonalityType SNT
            {
                get;
                private set;
            }

            public bool Exists( TonalityType a1, TonalityType a2, TonalityType a3 )
            {
                return (_ConditionHashset.Contains( new Tuple3< TonalityType, TonalityType, TonalityType >( a1, a2, a3 ) ));
            }

            public override string ToString()
            {
                return (SNT.ToText() + " (" + _ConditionHashset.Count + ')');
            }
        }
        private class pattern
        {
            public pattern( XElement patternNode, ThreeInSubsentsInqueryRule ownerRule )
            {
                var childs = patternNode.Elements().ToArray();
                if ( childs.Length != 3 )
                    throw (new ArgumentException("childs.Length != 3 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                FirstName = childs[ 0 ].Name.LocalName;
                FirstROLE = childs[ 0 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                FirstTYPE = childs[ 0 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();
                FirstMI   = childs[ 0 ].GetMI4Pattern();

                SecondName = childs[ 1 ].Name.LocalName;
                SecondROLE = childs[ 1 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                SecondTYPE = childs[ 1 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();
                SecondMI   = childs[ 1 ].GetMI4Pattern();

                ThreeName = childs[ 2 ].Name.LocalName;
                ThreeROLE = childs[ 2 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                ThreeTYPE = childs[ 2 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();
                ThreeMI   = childs[ 2 ].GetMI4Pattern();

                //check
                if ( FirstTYPE  != TypeAttributeValue.Inquiry && 
                     SecondTYPE != TypeAttributeValue.Inquiry && 
                     ThreeTYPE  != TypeAttributeValue.Inquiry )
                {
                    throw (new ArgumentException("No item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
                if ( 
                    ((FirstTYPE == TypeAttributeValue.Inquiry) && (SecondTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry)) 
                        ||
                    ((SecondTYPE == TypeAttributeValue.Inquiry) && (FirstTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry)) 
                        ||
                    ((ThreeTYPE == TypeAttributeValue.Inquiry) && (FirstTYPE == TypeAttributeValue.Inquiry || SecondTYPE == TypeAttributeValue.Inquiry))
                   )
                {
                    throw (new ArgumentException("More than one item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            }

            public string FirstName
            {
                get;
                private set;
            }
            public RoleAttributeValue? FirstROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? FirstTYPE
            {
                get;
                private set;
            }
            public string[] FirstMI
            {
                get;
                private set;
            }

            public string SecondName
            {
                get;
                private set;
            }
            public RoleAttributeValue? SecondROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? SecondTYPE
            {
                get;
                private set;
            }
            public string[] SecondMI
            {
                get;
                private set;
            }

            public string ThreeName
            {
                get;
                private set;
            }
            public RoleAttributeValue? ThreeROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? ThreeTYPE
            {
                get;
                private set;
            }
            public string[] ThreeMI
            {
                get;
                private set;
            }

            public override string ToString()
            {
                return ('{' + FirstName + "-" + SecondName + "-" + ThreeName + '}');
            }
        }

        public ThreeInSubsentsInqueryRule( XElement ruleNode )
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

        private action[] Actions
        {
            get;
            set;
        }
        private pattern[] Patterns
        {
            get;
            set;
        }

        public override bool Process( XElement subsentElement )
        {
            subsentElement.ThrowIfNotSubsent();

            var result = false;
            foreach ( var pattern in Patterns )
            {
                #region [.condition.]
                var tuples = 
                        from first in subsentElement.Elements( pattern.FirstName )
                        where ( !pattern.FirstROLE.HasValue ||                              
                                 first.HasRoleAttributeValue( pattern.FirstROLE.Value ) ) &&
                              ( !pattern.FirstTYPE.HasValue ||
                                 first.HasTypeAttributeValue( pattern.FirstTYPE.Value ) )

                        let second = first.GetNextSiblingWithSkipSNTNeuts( pattern.SecondName, pattern.SecondROLE, pattern.SecondTYPE,
                                                                           pattern.ThreeName , pattern.ThreeROLE , pattern.ThreeTYPE )
                        where ( second != null ) 

                        let three = second.GetNextSiblingWithSkipSNTNeuts( pattern.ThreeName, pattern.ThreeROLE, pattern.ThreeTYPE )
                        where ( (three != null) &&
                                first .SuccessByMI( pattern.FirstMI  ) &&
                                second.SuccessByMI( pattern.SecondMI ) &&
                                three .SuccessByMI( pattern.ThreeMI  )
                              )

                        select new
                        {
                            FIRST  = first,
                            SECOND = second,
                            THREE  = three,
                        };
                #endregion

                var tuple = tuples.FirstOrDefault();
                if ( tuple != null )
                //foreach ( var tuple in tuples.ToArray() )
                {
                    MatchRuleDebugInfoOutput( tuple.FIRST );

                    var a1SNT = tuple.FIRST .GetAttributeSNT();
                    var a2SNT = tuple.SECOND.GetAttributeSNT();
                    var a3SNT = tuple.THREE .GetAttributeSNT();

                    var localresult = false;
                    foreach ( var a in Actions )
                    {
                        if ( a.Exists( a1SNT, a2SNT, a3SNT ) )
                        {
                            #region
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;

                            SetSNTFINAL
                                (
                                a.SNT,
                                tuple.FIRST,
                                tuple.SECOND,
                                tuple.THREE 
                                );

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
        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.None); }
        }
    }

    /// <summary>
    /// ThreeInSubsents-Ex
    /// </summary>
    internal class ThreeInSubsentsExInqueryRule : InqueryRule
    {
        private class action : actionBase
        {
            private HashSet< Pair< TonalityType, TonalityType > > _ConditionHashset = new HashSet< Pair< TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a1SNT = condition.AttributeValue2Enum< TonalityType >( "a1" );
                    //var a2SNT = condition.AttributeValue2Enum< TonalityType >( "a2" );
                    var a3SNT = condition.AttributeValue2Enum< TonalityType >( "a3" );

                    _ConditionHashset.Add( new Pair< TonalityType, TonalityType >( a1SNT, a3SNT ) );
                }
            }

            public TonalityType SNT
            {
                get;
                private set;
            }

            public bool Exists( TonalityType a1, TonalityType a3 )
            {
                return (_ConditionHashset.Contains( new Pair< TonalityType, TonalityType >( a1, a3 ) ));
            }

            public override string ToString()
            {
                return (SNT.ToText() + " (" + _ConditionHashset.Count + ')');
            }
        }
        private class pattern
        {
            public pattern( XElement patternNode, ThreeInSubsentsExInqueryRule ownerRule )
            {
                var childs = patternNode.Elements().ToArray();
                if ( childs.Length != 3 )
                    throw (new ArgumentException("childs.Length != 3 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                FirstName = childs[ 0 ].Name.LocalName;
                FirstROLE = childs[ 0 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                FirstTYPE = childs[ 0 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                SecondName = childs[ 1 ].Name.LocalName;
                SecondROLE = childs[ 1 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                SecondTYPE = childs[ 1 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                ThreeName = childs[ 2 ].Name.LocalName;
                ThreeROLE = childs[ 2 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                ThreeTYPE = childs[ 2 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                //check
                if ( FirstTYPE  != TypeAttributeValue.Inquiry && 
                     SecondTYPE != TypeAttributeValue.Inquiry && 
                     ThreeTYPE  != TypeAttributeValue.Inquiry )
                {
                    throw (new ArgumentException("No item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
                if ( 
                    ((FirstTYPE == TypeAttributeValue.Inquiry) && (SecondTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry)) 
                        ||
                    ((SecondTYPE == TypeAttributeValue.Inquiry) && (FirstTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry)) 
                        ||
                    ((ThreeTYPE == TypeAttributeValue.Inquiry) && (FirstTYPE == TypeAttributeValue.Inquiry || SecondTYPE == TypeAttributeValue.Inquiry))
                   )
                {
                    throw (new ArgumentException("More than one item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            }

            public string FirstName
            {
                get;
                private set;
            }
            public RoleAttributeValue? FirstROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? FirstTYPE
            {
                get;
                private set;
            }

            public string SecondName
            {
                get;
                private set;
            }
            public RoleAttributeValue? SecondROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? SecondTYPE
            {
                get;
                private set;
            }

            public string ThreeName
            {
                get;
                private set;
            }
            public RoleAttributeValue? ThreeROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? ThreeTYPE
            {
                get;
                private set;
            }

            public override string ToString()
            {
                return ('{' + FirstName + "-" + SecondName + "-" + ThreeName + '}');
            }
        }

        public ThreeInSubsentsExInqueryRule( XElement ruleNode )
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

        private action[] Actions
        {
            get;
            set;
        }
        private pattern[] Patterns
        {
            get;
            set;
        }

        public override bool Process( XElement subsentElement )
        {
            subsentElement.ThrowIfNotSubsent();

            var result = false;
            foreach ( var pattern in Patterns )
            {
                #region [.condition.]
                var tuples = 
                        from first in subsentElement.Elements( pattern.FirstName )
                        where ( !pattern.FirstROLE.HasValue ||                              
                                 first.HasRoleAttributeValue( pattern.FirstROLE.Value ) ) &&
                              ( !pattern.FirstTYPE.HasValue ||
                                 first.HasTypeAttributeValue( pattern.FirstTYPE.Value ) )

                        let second = first.GetNextSiblingWithSkipSNTNeuts( pattern.SecondName, pattern.SecondROLE, pattern.SecondTYPE,
                                                                           pattern.ThreeName , pattern.ThreeROLE , pattern.ThreeTYPE )
                        where ( second != null ) 

                        let three = second.GetNextSiblingWithSkipSNTNeuts( pattern.ThreeName, pattern.ThreeROLE, pattern.ThreeTYPE )
                        where ( three != null ) 

                        select new
                        {
                            FIRST  = first,
                            SECOND = second,
                            THREE  = three,
                        };
                #endregion

                var tuple = tuples.FirstOrDefault();
                if ( tuple != null )
                //foreach ( var tuple in tuples.ToArray() )
                {
                    MatchRuleDebugInfoOutput( tuple.FIRST );

                    var a1SNT = tuple.FIRST .GetAttributeSNT();
                    //var a2SNT = tuple.SECOND.GetAttributeSNT();
                    var a3SNT = tuple.THREE .GetAttributeSNT();

                    var localresult = false;
                    foreach ( var a in Actions )
                    {
                        if ( a.Exists( a1SNT, a3SNT ) )
                        {
                            #region
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;

                            SetSNTFINAL
                                (
                                a.SNT,
                                tuple.FIRST,
                                tuple.SECOND,
                                tuple.THREE 
                                );

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
        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.None); }
        }
    }

    /// <summary>
    /// FourInSubsents
    /// </summary>
    internal class FourInSubsentsInqueryRule : InqueryRule
    {
        private class action : actionBase
        {
            private HashSet< Tuple4< TonalityType, TonalityType, TonalityType, TonalityType > > _ConditionHashset = new HashSet< Tuple4< TonalityType, TonalityType, TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a1SNT = condition.AttributeValue2Enum< TonalityType >( "a1" );
                    var a2SNT = condition.AttributeValue2Enum< TonalityType >( "a2" );
                    var a3SNT = condition.AttributeValue2Enum< TonalityType >( "a3" );
                    var a4SNT = condition.AttributeValue2Enum< TonalityType >( "a4" );

                    _ConditionHashset.Add( new Tuple4< TonalityType, TonalityType, TonalityType, TonalityType >( a1SNT, a2SNT, a3SNT, a4SNT ) );
                }
            }

            public TonalityType SNT
            {
                get;
                private set;
            }

            public bool Exists( TonalityType a1, TonalityType a2, TonalityType a3, TonalityType a4 )
            {
                return (_ConditionHashset.Contains( new Tuple4< TonalityType, TonalityType, TonalityType, TonalityType >( a1, a2, a3, a4 ) ));
            }

            public override string ToString()
            {
                return (SNT.ToText() + " (" + _ConditionHashset.Count + ')');
            }
        }
        private class pattern
        {
            public pattern( XElement patternNode, FourInSubsentsInqueryRule ownerRule )
            {
                var childs = patternNode.Elements().ToArray();
                if ( childs.Length != 4 )
                    throw (new ArgumentException("childs.Length != 4 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                FirstName = childs[ 0 ].Name.LocalName;
                FirstROLE = childs[ 0 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                FirstTYPE = childs[ 0 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                SecondName = childs[ 1 ].Name.LocalName;
                SecondROLE = childs[ 1 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                SecondTYPE = childs[ 1 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                ThreeName = childs[ 2 ].Name.LocalName;
                ThreeROLE = childs[ 2 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                ThreeTYPE = childs[ 2 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                FourName = childs[ 3 ].Name.LocalName;
                FourROLE = childs[ 3 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                FourTYPE = childs[ 3 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                //check
                if ( FirstTYPE  != TypeAttributeValue.Inquiry && 
                     SecondTYPE != TypeAttributeValue.Inquiry && 
                     ThreeTYPE  != TypeAttributeValue.Inquiry && 
                     FourTYPE   != TypeAttributeValue.Inquiry )
                {
                    throw (new ArgumentException("No item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
                if ( 
                    ((FirstTYPE == TypeAttributeValue.Inquiry)  && (SecondTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry || FourTYPE == TypeAttributeValue.Inquiry)) 
                        ||
                    ((SecondTYPE == TypeAttributeValue.Inquiry) && (FirstTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry || FourTYPE == TypeAttributeValue.Inquiry)) 
                        ||
                    ((ThreeTYPE == TypeAttributeValue.Inquiry)  && (FirstTYPE == TypeAttributeValue.Inquiry || SecondTYPE == TypeAttributeValue.Inquiry || FourTYPE == TypeAttributeValue.Inquiry))
                        ||
                    ((FourTYPE == TypeAttributeValue.Inquiry)  && (FirstTYPE == TypeAttributeValue.Inquiry || SecondTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry))
                   )
                {
                    throw (new ArgumentException("More than one item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            }

            public string FirstName
            {
                get;
                private set;
            }
            public RoleAttributeValue? FirstROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? FirstTYPE
            {
                get;
                private set;
            }

            public string SecondName
            {
                get;
                private set;
            }
            public RoleAttributeValue? SecondROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? SecondTYPE
            {
                get;
                private set;
            }

            public string ThreeName
            {
                get;
                private set;
            }
            public RoleAttributeValue? ThreeROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? ThreeTYPE
            {
                get;
                private set;
            }

            public string FourName
            {
                get;
                private set;
            }
            public RoleAttributeValue? FourROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? FourTYPE
            {
                get;
                private set;
            }

            public override string ToString()
            {
                return ('{' + FirstName + "-" + SecondName + "-" + ThreeName + "-" + FourName + '}');
            }
        }

        public FourInSubsentsInqueryRule( XElement ruleNode )
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

        private action[] Actions
        {
            get;
            set;
        }
        private pattern[] Patterns
        {
            get;
            set;
        }

        public override bool Process( XElement subsentElement )
        {
            subsentElement.ThrowIfNotSubsent();

            var result = false;
            foreach ( var pattern in Patterns )
            {
                #region [.condition.]
                var tuples = 
                        from first in subsentElement.Elements( pattern.FirstName )
                        where ( !pattern.FirstROLE.HasValue ||                              
                                 first.HasRoleAttributeValue( pattern.FirstROLE.Value ) ) &&
                              ( !pattern.FirstTYPE.HasValue ||
                                 first.HasTypeAttributeValue( pattern.FirstTYPE.Value ) )

                        let second = first.GetNextSiblingWithSkipSNTNeuts( pattern.SecondName, pattern.SecondROLE, pattern.SecondTYPE,
                                                                           pattern.ThreeName , pattern.ThreeROLE , pattern.ThreeTYPE )
                        where ( second != null ) 

                        let three = second.GetNextSiblingWithSkipSNTNeuts( pattern.ThreeName, pattern.ThreeROLE, pattern.ThreeTYPE,
                                                                           pattern.FourName , pattern.FourROLE , pattern.FourTYPE )
                        where ( three != null )

                        let four = three.GetNextSiblingWithSkipSNTNeuts( pattern.FourName, pattern.FourROLE, pattern.FourTYPE )
                        where ( four != null ) 

                        select new
                        {
                            FIRST  = first,
                            SECOND = second,
                            THREE  = three,
                            FOUR   = four,
                        };
                #endregion

                var tuple = tuples.FirstOrDefault();
                if ( tuple != null )
                //foreach ( var tuple in tuples.ToArray() )
                {
                    MatchRuleDebugInfoOutput( tuple.FIRST );

                    var a1SNT = tuple.FIRST .GetAttributeSNT();
                    var a2SNT = tuple.SECOND.GetAttributeSNT();
                    var a3SNT = tuple.THREE .GetAttributeSNT();
                    var a4SNT = tuple.FOUR  .GetAttributeSNT();

                    var localresult = false;
                    foreach ( var a in Actions )
                    {
                        if ( a.Exists( a1SNT, a2SNT, a3SNT, a4SNT ) )
                        {
                            #region
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;
                            
                            SetSNTFINAL
                                (
                                a.SNT,
                                tuple.FIRST,
                                tuple.SECOND,
                                tuple.THREE,
                                tuple.FOUR 
                                );

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
        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.None); }
        }
    }

    /// <summary>
    /// FiveInSubsents
    /// </summary>
    internal class FiveInSubsentsInqueryRule : InqueryRule
    {
        private class action : actionBase
        {
            private HashSet< Pair< TonalityType, TonalityType > > _ConditionHashset = new HashSet< Pair< TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );
                CreateNewElementType = actionNode.AttributeValue2Enum< TypeOfXmlElement >( "createnewelement" );

                TYPE = actionNode.GetAttributeTYPE( "setattribute_TYPE" );

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a4SNT = condition.AttributeValue2Enum< TonalityType >( "a4" );
                    var a5SNT = condition.AttributeValue2Enum< TonalityType >( "a5" );

                    _ConditionHashset.Add( new Pair< TonalityType, TonalityType >( a4SNT, a5SNT ) );
                }
            }

            public TypeOfXmlElement CreateNewElementType
            {
                get;
                private set;
            }
            public TonalityType SNT
            {
                get;
                private set;
            }
            public TypeAttributeValue? TYPE
            {
                get;
                private set;
            }

            public bool Exists( TonalityType a4, TonalityType a5 )
            {
                return (_ConditionHashset.Contains( new Pair< TonalityType, TonalityType >( a4, a5 ) ));
            }

            public override string ToString()
            {
                return (SNT.ToText() + " (" + _ConditionHashset.Count + ')');
            }
        }
        private class pattern
        {
            public pattern( XElement patternNode, FiveInSubsentsInqueryRule ownerRule )
            {
                var childs = patternNode.Elements().ToArray();
                if ( childs.Length != 5 )
                    throw (new ArgumentException("childs.Length != 5 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                FirstName = childs[ 0 ].Name.LocalName;
                FirstROLE = childs[ 0 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                FirstTYPE = childs[ 0 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                SecondName = childs[ 1 ].Name.LocalName;
                SecondROLE = childs[ 1 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                SecondTYPE = childs[ 1 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                ThreeName = childs[ 2 ].Name.LocalName;
                ThreeROLE = childs[ 2 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                ThreeTYPE = childs[ 2 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                FourName = childs[ 3 ].Name.LocalName;
                FourROLE = childs[ 3 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                FourTYPE = childs[ 3 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                FiveName = childs[ 4 ].Name.LocalName;
                FiveROLE = childs[ 4 ].GetAttribute( "ROLE" ).TryConvert2Enum< RoleAttributeValue >();
                FiveTYPE = childs[ 4 ].GetAttribute( "TYPE" ).TryConvert2Enum< TypeAttributeValue >();

                //check
                if ( FirstTYPE  != TypeAttributeValue.Inquiry && 
                     SecondTYPE != TypeAttributeValue.Inquiry && 
                     ThreeTYPE  != TypeAttributeValue.Inquiry && 
                     FourTYPE   != TypeAttributeValue.Inquiry && 
                     FiveTYPE   != TypeAttributeValue.Inquiry )
                {
                    throw (new ArgumentException("No item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
                if ( 
                    ((FirstTYPE == TypeAttributeValue.Inquiry)  && (SecondTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry || FourTYPE == TypeAttributeValue.Inquiry || FiveTYPE == TypeAttributeValue.Inquiry)) 
                        ||
                    ((SecondTYPE == TypeAttributeValue.Inquiry) && (FirstTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry || FourTYPE == TypeAttributeValue.Inquiry || FiveTYPE == TypeAttributeValue.Inquiry)) 
                        ||
                    ((ThreeTYPE == TypeAttributeValue.Inquiry)  && (FirstTYPE == TypeAttributeValue.Inquiry || SecondTYPE == TypeAttributeValue.Inquiry || FourTYPE == TypeAttributeValue.Inquiry || FiveTYPE == TypeAttributeValue.Inquiry))
                        ||
                    ((FourTYPE == TypeAttributeValue.Inquiry)  && (FirstTYPE == TypeAttributeValue.Inquiry || SecondTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry || FiveTYPE == TypeAttributeValue.Inquiry))
                        ||
                    ((FiveTYPE == TypeAttributeValue.Inquiry)  && (FirstTYPE == TypeAttributeValue.Inquiry || SecondTYPE == TypeAttributeValue.Inquiry || ThreeTYPE == TypeAttributeValue.Inquiry || FourTYPE == TypeAttributeValue.Inquiry))
                   )
                {
                    throw (new ArgumentException("More than one item with @TYPE='Inquiry' (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            }

            public string FirstName
            {
                get;
                private set;
            }
            public RoleAttributeValue? FirstROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? FirstTYPE
            {
                get;
                private set;
            }

            public string SecondName
            {
                get;
                private set;
            }
            public RoleAttributeValue? SecondROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? SecondTYPE
            {
                get;
                private set;
            }

            public string ThreeName
            {
                get;
                private set;
            }
            public RoleAttributeValue? ThreeROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? ThreeTYPE
            {
                get;
                private set;
            }

            public string FourName
            {
                get;
                private set;
            }
            public RoleAttributeValue? FourROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? FourTYPE
            {
                get;
                private set;
            }

            public string FiveName
            {
                get;
                private set;
            }
            public RoleAttributeValue? FiveROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? FiveTYPE
            {
                get;
                private set;
            }

            public override string ToString()
            {
                return ('{' + FirstName + '-' + SecondName + '-' + ThreeName + '-' + FourName + '-' + FiveName + '}');
            }
        }

        public FiveInSubsentsInqueryRule( XElement ruleNode )
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

        private action[] Actions
        {
            get;
            set;
        }
        private pattern[] Patterns
        {
            get;
            set;
        }

        public override bool Process( XElement subsentElement )
        {
            subsentElement.ThrowIfNotSubsent();

            //var result = false;
            foreach ( var pattern in Patterns )
            {
                #region [.condition.]
                var tuples = 
                        from first in subsentElement.Elements( pattern.FirstName )
                        where ( !pattern.FirstROLE.HasValue ||                              
                                 first.HasRoleAttributeValue( pattern.FirstROLE.Value ) ) &&
                              ( !pattern.FirstTYPE.HasValue ||
                                 first.HasTypeAttributeValue( pattern.FirstTYPE.Value ) )

                        let second = first.GetNextSiblingWithSkipSNTNeuts( pattern.SecondName, pattern.SecondROLE, pattern.SecondTYPE,
                                                                           pattern.ThreeName , pattern.ThreeROLE , pattern.ThreeTYPE )
                        where ( second != null )

                        let three = second.GetNextSiblingWithSkipSNTNeuts( pattern.ThreeName, pattern.ThreeROLE, pattern.ThreeTYPE,
                                                                           pattern.FourName , pattern.FourROLE , pattern.FourTYPE )
                        where ( three != null )

                        let four = three.GetNextSiblingWithSkipSNTNeuts( pattern.FourName, pattern.FourROLE, pattern.FourTYPE,
                                                                         pattern.FiveName, pattern.FiveROLE, pattern.FiveTYPE )
                        where ( four != null )

                        let five = four.GetNextSiblingWithSkipSNTNeuts( pattern.FiveName, pattern.FiveROLE, pattern.FiveTYPE )
                        where ( five != null ) 

                        select new
                        {
                            //FIRST  = first,
                            //SECOND = second,
                            //THREE  = three,
                            FOUR   = four,
                            FIVE   = five,
                        };
                #endregion

                var tuple = tuples.FirstOrDefault();
                if ( tuple != null )
                //foreach ( var tuple in tuples.ToArray() )
                {
                    MatchRuleDebugInfoOutput( tuple.FOUR );

                    var a4SNT = tuple.FOUR.GetAttributeSNT();
                    var a5SNT = tuple.FIVE.GetAttributeSNT();

                    var localresult = false;
                    foreach ( var a in Actions )
                    {
                        if ( a.Exists( a4SNT, a5SNT ) )
                        {
                            #region
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            //result = true;
                            
                            var newNode = new XElement( a.CreateNewElementType.ToString() );
                            newNode.SetAttributeSNT( a.SNT );
                            newNode.SetAttributeFRT( tuple.FOUR.GetAttributeFRT() +
                                                     tuple.FIVE.GetAttributeFRT() );
                            if (a.TYPE.HasValue) newNode.SetAttributeTYPE( a.TYPE.Value );

                            tuple.FOUR.AddBeforeSelf( newNode );
                            newNode.MoveIn2ThisNodesRange( tuple.FOUR, tuple.FIVE );
                            newNode.AddBeforeAndAfterEmptyU();
                            break;
                            #endregion
                        }
                    }

                    if ( !localresult )
                        NomatchActionDebugInfoOutput();
                }
            }
            //!Need always returned FALSE!
            return (false); //return (result);
        }
        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.None); }
        }
    }
}



