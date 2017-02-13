using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;

using TextMining.Core;
using TextMining.Service;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    internal class ParentChildRule : RuleBase
    {
        /// <summary>
        /// 
        /// </summary>
        private enum determine_attribute
        {
            parent,
            child,
        }

        /// <summary>
        /// 
        /// </summary>
        private class action : actionBase
        {
            private HashSet< Pair< TonalityType, TonalityType > > _ConditionHashset = new HashSet< Pair< TonalityType, TonalityType > >();

            public action( XElement actionNode ) : base( actionNode )
            {
                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );

                ROLE = actionNode.GetAttributeROLE("setattribute_ROLE");
                if ( !ROLE.HasValue ) {
                    var role = actionNode.GetAttribute( "determineattribute_ROLE" );
                    if ( !role.IsEmptyOrNull() ) {
                        determineattribute_ROLE = role.ToEnum< determine_attribute >();
                    }
                }
                TYPE = actionNode.GetAttributeTYPE("setattribute_TYPE");
                if ( !TYPE.HasValue ) {
                    var type = actionNode.GetAttribute( "determineattribute_TYPE" );
                    if ( !type.IsEmptyOrNull() ) {
                        determineattribute_TYPE = type.ToEnum< determine_attribute >();
                    }
                }

                foreach ( var condition in actionNode.GetMultiNodes( "condition" )
                                                     .RevealConfigNodes() 
                        )
                {
                    var parentSNT = condition.AttributeValue2Enum< TonalityType >( "Parent" );
                    var childSNT  = condition.AttributeValue2Enum< TonalityType >( "Child"  );

                    _ConditionHashset.Add( new Pair< TonalityType, TonalityType >( parentSNT, childSNT ) );
                }
            }

            public TonalityType SNT
            {
                get;
                private set;
            }
            public RoleAttributeValue? ROLE
            {
                get;
                private set;
            }
            public determine_attribute? determineattribute_ROLE
            {
                get;
                private set;
            }
            public TypeAttributeValue? TYPE
            {
                get;
                private set;
            }
            public determine_attribute? determineattribute_TYPE
            {
                get;
                private set;
            }

            public bool Exists( TonalityType parent, TonalityType child )
            {
                return (_ConditionHashset.Contains( new Pair< TonalityType, TonalityType >( parent, child ) ));
            }
        }
        /// <summary>
        /// 
        /// </summary>
        private class pattern
        {
            public pattern( XElement patternNode, ParentChildRule ownerRule )
            {
                if ( patternNode.Elements().Count() != 1 )
                    throw (new ArgumentException("patternNode.Elements().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                var parent = patternNode.Elements().First();
                if ( parent.Attributes().Count() != 1 )
                    throw (new ArgumentException("parent.Attributes().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                ParentName = parent.Name.LocalName;
                var attr = parent.FirstAttribute;
                ParentAttributeName = attr.Name.LocalName;
                switch ( ParentAttributeName )
                {
                    case "TYPE":
                        ParentAttributeValue = attr.Value.ToEnum< TypeAttributeValue >().ToString();
                    break;

                    case "ROLE":
                        ParentAttributeValue = attr.Value.ToEnum< RoleAttributeValue >().ToString();
                    break;

                    case "PROF":
                        ParentAttributeValue = attr.Value;
                    break;

                    default:
                        throw (new ArgumentException("parent.FirstAttribute.Name is " + attr.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }

                if ( parent.Elements().Count() != 1 )
                    throw (new ArgumentException("parent.Elements().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                var child = parent.Elements().First();
                if (child.Attributes().Count() != 1)
                    throw (new ArgumentException("first.Attributes().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));
                ChildName = child.Name.LocalName;
                attr = child.FirstAttribute;
                ChildAttributeName = attr.Name.LocalName;
                if ( ChildAttributeName == "TYPE" ) {
                    ChildAttributeValue = attr.Value.ToEnum< TypeAttributeValue >().ToString();
                } else if ( ChildAttributeName == "ROLE" ) {
                    ChildAttributeValue = attr.Value.ToEnum< RoleAttributeValue >().ToString();
                } else {
                    throw (new ArgumentException("child.FirstAttribute.Name is " + attr.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }
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

            public string ChildName
            {
                get;
                private set;
            }
            public string ChildAttributeName
            {
                get;
                private set;
            }
            public string ChildAttributeValue
            {
                get;
                private set;
            }

            public override string ToString()
            {
                return (ParentName + "[@" + ParentAttributeName + "='" + ParentAttributeValue + "'] : " +
                        ChildName  + "[@" + ChildAttributeName  + "='" + ChildAttributeValue + "']");
            }
        }

        public ParentChildRule( XElement ruleNode )
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

            ParentName_ChildName_Tuples = (from pt in Patterns
                                           select new Tuple< string, string >( pt.ParentName, pt.ChildName )
                                          )
                                          .Distinct()
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
        private Tuple< string, string >[] ParentName_ChildName_Tuples
        {
            get;
            set;
        }

        public override bool Process( XElement subsent )
        {
            var isSubsent = subsent.IsElementSUBSENT();
            //---subsent.ThrowIfNotSubsent();

            var result = false;
            foreach ( var tuple in ParentName_ChildName_Tuples )
            {
                #region [.condition.]
                var pairs = from parent in subsent.Descendants( tuple.Item1/*ParentName*/ )

                            where (parent.GetAttributeTYPE() != TypeAttributeValue.Collocation)
                                    &&
                                  (!parent.HasAttributeCREATEDBYPARENTTWOCHILDRULE())
                                    &&
                                  (!parent.GetAncestorSubsent().HasAttributeFIRSTQUEUESUBSENT())
                                    &&
                                  (!parent.HasSelfOrAncestorHomogeneous( isSubsent ))

                            let child = parent.ElementsExceptT()
                                              .SkipWhile( _e => _e.IsElementVERB() && !_e.ElementsExceptT().Any() )
                                              .FirstOrDefault()
                            where ( (child != null) && (child.Name == tuple.Item2/*ChildName*/) )

                            select new
                            {
                                PARENT = parent,
                                CHILD  = child,
                            };
                #endregion

                foreach ( var pair in pairs.Reverse() /*VERY IMPORTANT - in inverse-backward order*/ )
                {
                    foreach ( var pt in Patterns )
                    {
                        #region [.try match.]
                        if ( !pair.PARENT.HasAttributeWithValueIgnoreCase( pt.ParentAttributeName, pt.ParentAttributeValue ) )
                            continue;

                        if ( !pair.CHILD.HasAttributeWithValue( pt.ChildAttributeName, pt.ChildAttributeValue ) )
                            continue;

                        MatchRuleDebugInfoOutput( pair.PARENT );

                        var childSNT  = pair.CHILD .GetAttributeSNT();
                        var parentSNT = pair.PARENT.GetAttributeSNT();

                        var localresult = false;
                        foreach ( var a in Actions )
                        {
                            if ( a.Exists( parentSNT, childSNT ) )
                            {
                                #region [.match action.]
                                MatchActionDebugInfoOutput( a );
                                localresult = true;

                                result = true;

                                var frt = pair.PARENT.GetAttributeFRT() + pair.CHILD.GetAttributeFRT() + a.FRT_toAdd;
                                if (frt < 0) frt = 0;

                                pair.PARENT.SetAttributeSNT( a.SNT );
                                pair.PARENT.SetAttributeFRT( frt   );
                            
                                #region [.set ROLE.]
                                if ( a.ROLE.HasValue )
                                {
                                    pair.PARENT.SetAttributeROLE( a.ROLE.Value );
                                }
                                else
                                {
                                    if ( a.determineattribute_ROLE.HasValue )
                                    {
                                        switch ( a.determineattribute_ROLE.Value )
                                        {
                                            case determine_attribute.child:
                                                pair.PARENT.SetAttributeROLE( pair.CHILD.GetAttributeROLE() );
                                                break;
                                            case determine_attribute.parent: break;
                                            default:
                                                throw (new ArgumentException(a.determineattribute_ROLE.ToString() + " (rule " + this.Id.InSingleQuote() + ')'));
                                        }
                                    }
                                } 
                                #endregion

                                #region [.set TYPE.]
                                var childType = pair.CHILD.GetAttributeTYPE();
                                if ( childType == TypeAttributeValue.Inquiry )
                                {
                                    pair.PARENT.SetAttributeTYPE( TypeAttributeValue.Inquiry );
                                }
                                else if ( a.TYPE.HasValue )
                                {
                                    pair.PARENT.SetAttributeTYPE( a.TYPE.Value );
                                }
                                else
                                {
                                    if ( a.determineattribute_TYPE.HasValue )
                                    {
                                        switch ( a.determineattribute_TYPE.Value )
                                        {
                                            case determine_attribute.child:
                                                if ( !childType.HasValue )
                                                    throw (new ArgumentException("Child item no has @TYPE-attrubute"));
                                                pair.PARENT.SetAttributeTYPE( childType.Value );
                                                break;
                                            case determine_attribute.parent: break;
                                            default:
                                                throw (new ArgumentException(a.determineattribute_TYPE.ToString() + " (rule " + this.Id.InSingleQuote() + ')'));
                                        }
                                    }
                                } 
                                #endregion
                                break;
                                #endregion
                            }
                        }
                        if ( localresult )
                            break;

                        NomatchActionDebugInfoOutput();
                        #endregion
                    }
                }
            }
            return (result);
        }
        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.None); }
        }
    }
}