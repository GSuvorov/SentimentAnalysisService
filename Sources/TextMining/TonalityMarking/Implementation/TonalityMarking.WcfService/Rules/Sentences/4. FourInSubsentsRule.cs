using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
using TextMining.Core;

namespace TonalityMarking
{
    internal class FourInSubsentsRule : RuleBase
    {
        protected class action : actionBase
        {
            private HashSet< Pair< TonalityType, TonalityType > > _ConditionHashset = new HashSet< Pair< TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );
                CreateNewElementType = actionNode.AttributeValue2Enum< TypeOfXmlElement >( "createnewelement" );

                TYPE = actionNode.GetAttributeTYPE( "setattribute_TYPE" );
                //if ( !TYPE.HasValue ) {
                //    var type = actionNode.GetAttribute( "determineattribute_TYPE" );
                //    if ( !type.IsEmptyOrNull() ) {
                //        determineattribute_TYPE = type.ToEnum< determine_attribute >();
                //    }
                //}

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes()
                        )
                {
                    var a3SNT = condition.AttributeValue2Enum< TonalityType >( "a3" );
                    var a4SNT = condition.AttributeValue2Enum< TonalityType >( "a4" );

                    _ConditionHashset.Add( new Pair< TonalityType, TonalityType >( a3SNT, a4SNT ) );
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

            public bool Exists( TonalityType a3, TonalityType a4 )
            {
                return (_ConditionHashset.Contains( new Pair< TonalityType, TonalityType >( a3, a4 ) ));
            }

            public override string ToString()
            {
                return (SNT.ToText() + " (" + _ConditionHashset.Count + ')');
            }
        }
        protected class pattern
        {
            public pattern( XElement patternNode, RuleBase ownerRule )
            {
                if ( patternNode.Elements().Count() != 1 )
                    throw (new ArgumentException("patternNode.Elements().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));

            #region [.Parent.]
                var parent = patternNode.Elements().First();
                ParentName = parent.Name.LocalName;

                switch ( parent.Attributes().Count() )
                {
                    case 0: break;
                    case 1:
                        var a = parent.FirstAttribute;
                        ParentAttributeName = a.Name.LocalName;
                        switch ( ParentAttributeName )
                        {
                            case "TYPE": ParentAttributeValue = a.Value.ToEnum< TypeAttributeValue >().ToString(); break;
                            case "ROLE": ParentAttributeValue = a.Value.ToEnum< RoleAttributeValue >().ToString(); break;
                            default:
                                throw (new ArgumentException("parent.FirstAttribute.Name is " + a.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                        }
                        break;
                    default:
                        throw (new ArgumentException("1 < parent.Attributes().Count() (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            #endregion

                if ( parent.Elements().Count() != 4 )
                    throw (new ArgumentException("parent.Elements().Count() != 4 (rule " + ownerRule.Id.InSingleQuote() + ')'));

            #region [.First-child.]
                var first = parent.Elements().First();
                FirstName = first.Name.LocalName;

                switch ( first.Attributes().Count() )
                {
                    case 0: break;
                    case 1: 
                        var a = first.FirstAttribute;
                        FirstAttributeName = a.Name.LocalName;
                        switch ( FirstAttributeName )
                        {
                            case "TYPE": FirstAttributeValue = a.Value.ToEnum< TypeAttributeValue >().ToString(); break;
                            case "ROLE": FirstAttributeValue = a.Value.ToEnum< RoleAttributeValue >().ToString(); break;
                            default:
                                throw (new ArgumentException("first.FirstAttribute.Name is " + a.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                        }
                        break;
                    default:
                        throw (new ArgumentException("1 < first.Attributes().Count() (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            #endregion

            #region [.Second-child.]
                var second = parent.Elements().Skip( 1 ).First();
                SecondName = second.Name.LocalName;

                switch ( second.Attributes().Count() )
                {
                    case 0: break;
                    case 1: 
                        var a = second.FirstAttribute;
                        SecondAttributeName = a.Name.LocalName;
                        switch ( SecondAttributeName )
                        {
                            case "TYPE": SecondAttributeValue = a.Value.ToEnum< TypeAttributeValue >().ToString(); break;
                            case "ROLE": SecondAttributeValue = a.Value.ToEnum< RoleAttributeValue >().ToString(); break;
                            default:
                                throw (new ArgumentException("second.FirstAttribute.Name is " + a.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                        }
                        break;
                    default:
                        throw (new ArgumentException("1 < second.Attributes().Count() (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            #endregion

            #region [.Three-child.]
                var three = parent.Elements().Skip( 2 ).First();
                ThreeName = three.Name.LocalName;

                switch ( three.Attributes().Count() )
                {
                    case 0: break;
                    case 1: 
                        var a = three.FirstAttribute;
                        ThreeAttributeName = a.Name.LocalName;
                        switch ( ThreeAttributeName )
                        {
                            case "TYPE": ThreeAttributeValue = a.Value.ToEnum< TypeAttributeValue >().ToString(); break;
                            case "ROLE": ThreeAttributeValue = a.Value.ToEnum< RoleAttributeValue >().ToString(); break;
                            default:
                                throw (new ArgumentException("three.FirstAttribute.Name is " + a.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                        }
                        break;
                    default:
                        throw (new ArgumentException("1 < three.Attributes().Count() (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            #endregion

            #region [.Four-child.]
                var four = parent.Elements().Skip( 3 ).First();
                FourName = four.Name.LocalName;

                switch ( four.Attributes().Count() )
                {
                    case 0: break;
                    case 1: 
                        var a = four.FirstAttribute;
                        FourAttributeName = a.Name.LocalName;
                        switch ( FourAttributeName )
                        {
                            case "TYPE": FourAttributeValue = a.Value.ToEnum< TypeAttributeValue >().ToString(); break;
                            case "ROLE": FourAttributeValue = a.Value.ToEnum< RoleAttributeValue >().ToString(); break;
                            default:
                                throw (new ArgumentException("four.FirstAttribute.Name is " + a.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                        }
                        break;
                    default:
                        throw (new ArgumentException("1 < four.Attributes().Count() (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
            #endregion
            }

            public string ParentName
            {
                get;
                private set;
            }
            public string ParentAttributeName
            {
                get;
                private set;
            }
            public string ParentAttributeValue
            {
                get;
                private set;
            }

            public string FirstName
            {
                get;
                private set;
            }
            public string FirstAttributeName
            {
                get;
                private set;
            }
            public string FirstAttributeValue
            {
                get;
                private set;
            }

            public string SecondName
            {
                get;
                private set;
            }
            public string SecondAttributeName
            {
                get;
                private set;
            }
            public string SecondAttributeValue
            {
                get;
                private set;
            }

            public string ThreeName
            {
                get;
                private set;
            }
            public string ThreeAttributeName
            {
                get;
                private set;
            }
            public string ThreeAttributeValue
            {
                get;
                private set;
            }

            public string FourName
            {
                get;
                private set;
            }
            public string FourAttributeName
            {
                get;
                private set;
            }
            public string FourAttributeValue
            {
                get;
                private set;
            }

            public override string ToString()
            {
                return (ParentName + ": {" + FirstName + "-" + SecondName + "-" + ThreeName + "-" + FourName + '}');
            }
        }

        public FourInSubsentsRule( XElement ruleNode )
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

        public override bool Process( XElement subsent )
        {
            subsent.ThrowIfNotSubsent();

            var result = false;
            foreach ( var pt in Patterns )
            {
                #region [.condition.]
                var parent = ((subsent.Name.LocalName == pt.ParentName) &&
                              (pt.ParentAttributeName.IsEmptyOrNull() ||
                               subsent.HasAttributeWithValue( pt.ParentAttributeName, pt.ParentAttributeValue )))
                             ? subsent : null;
                if ( parent == null )
                    continue;

                var first = parent.Elements( pt.FirstName )
                                    .Where( _ => pt.FirstAttributeName.IsEmptyOrNull() ||
                                                _.HasAttributeWithValue( pt.FirstAttributeName, pt.FirstAttributeValue ) )
                                    .FirstOrDefault();
                if ( first == null )
                    continue;

                var second = first.ElementsAfterSelf( pt.SecondName )
                                    .Where( _ => pt.SecondAttributeName.IsEmptyOrNull() ||
                                                _.HasAttributeWithValue( pt.SecondAttributeName, pt.SecondAttributeValue ) )
                                    .FirstOrDefault();
                if ( second == null )
                    continue;

                var three = second.ElementsAfterSelf( pt.ThreeName )
                                    .Where( _ => pt.ThreeAttributeName.IsEmptyOrNull() ||
                                                _.HasAttributeWithValue( pt.ThreeAttributeName, pt.ThreeAttributeValue ) )
                                    .FirstOrDefault();
                if ( three == null )
                    continue;

                var four = three.ElementsAfterSelf( pt.FourName )
                                    .Where( _ => pt.FourAttributeName.IsEmptyOrNull() ||
                                                _.HasAttributeWithValue( pt.FourAttributeName, pt.FourAttributeValue ) )
                                    .FirstOrDefault();
                if ( four == null )
                    continue;

                var tuple = new 
                {
                    //PARENT = parent,
                    //FIRST  = first,
                    //SECOND = second,
                    THREE = three,
                    FOUR  = four,
                };     
                #endregion

                #region [.process.]
                MatchRuleDebugInfoOutput( tuple.THREE );

                var threeSNT = tuple.THREE.Attribute("SNT");
                var fourSNT  = tuple.FOUR .Attribute("SNT");                    

                if ( threeSNT == null || fourSNT == null )
                    throw (new InvalidOperationException("Element no has 'SNT' attribute (rule " + this.Id.InSingleQuote() + ')'));

                var a3SNT = threeSNT.Value.ToEnum< TonalityType >();
                var a4SNT = fourSNT .Value.ToEnum< TonalityType >();

                var localresult = false;
                foreach ( var a in Actions )
                {
                    if ( a.Exists( a3SNT, a4SNT ) )
                    {
                        #region [.match action.]
                        MatchActionDebugInfoOutput( a );
                        localresult = true;

                        result = true;

                        var newNode = new XElement( a.CreateNewElementType.ToString() );
                        newNode.SetAttributeSNT( a.SNT );
                        newNode.SetAttributeFRT( tuple.THREE.GetAttributeFRT() +
                                                    tuple.FOUR .GetAttributeFRT() );
                        if (a.TYPE.HasValue) newNode.SetAttributeTYPE( a.TYPE.Value );

                        tuple.THREE.AddBeforeSelf( newNode );
                        newNode.MoveIn2ThisNodesRange( tuple.THREE, tuple.FOUR );
                        newNode.AddBeforeAndAfterEmptyU();
                        break;
                        #endregion
                    }
                }

                if ( !localresult )
                    NomatchActionDebugInfoOutput();

                if ( result )
                    break;
                #endregion
            }
            return (result);
        }
        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.RecycleOnSuccsess); }
        }
    }
}
