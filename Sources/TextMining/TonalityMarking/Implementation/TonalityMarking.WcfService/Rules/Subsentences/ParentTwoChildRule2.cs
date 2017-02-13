using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using TextMining.Core;

namespace TonalityMarking
{
    internal class ParentTwoChildRule2 : RuleBase
    {
        private enum determine_attribute
        {
            firstchild_type_or_role = 0,
            firstchildtype          = firstchild_type_or_role,
            firstchildrole          = firstchild_type_or_role,

            secondchild_type_or_role = 1,
            secondchildtype          = secondchild_type_or_role,
            secondchildrole          = secondchild_type_or_role,
        }
        private static class extensions
        {
            public static int ToIndex( determine_attribute value )
            {
                switch ( value )    
                {
                    case determine_attribute.firstchildtype:  return (0);
                    case determine_attribute.secondchildtype: return (1);
                    default:
                        throw (new ArgumentException(value.ToString()));
                }
            }
        }

        private class action : actionBase
        {
            private HashSet< string > _ConditionHashset = new HashSet< string >();

            public action( XElement actionNode, pattern[] patterns, ParentTwoChildRule2 ownerRule ) : base( actionNode )
            {
                OwnerRule = ownerRule;

                SNT = actionNode.AttributeValue2Enum< TonalityType >( "SNT" );
                CreateNewElementType = actionNode.AttributeValue2Enum< TypeOfXmlElement >( "createnewelement" );

                ROLE = actionNode.GetAttributeROLE( "setattribute_ROLE" );
                if ( !ROLE.HasValue ) {
                    var role = actionNode.GetAttribute( "determineattribute_ROLE" );
                    if ( !role.IsEmptyOrNull() ) {
                        determineattribute_ROLE = role.ToEnum< determine_attribute >();
                    }
                }
                TYPE = actionNode.GetAttributeTYPE( "setattribute_TYPE" );
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
                    var a_attributes = condition.GetAttributeNodes( "@*[ starts-with( local-name(), 'a' ) ]" ).ToArray();
                    if ( 2 < a_attributes.Length )
                        throw (new ArgumentException("@*[ starts-with( local-name(), 'a' ) ].Length > 2 (rule " + OwnerRule.Id.InSingleQuote() + ')'));

                    if ( a_attributes.Except( condition.Attributes() ).Any() )
                        throw (new ArgumentException("Action-element has not only a[1,2,3..] attributes (rule " + OwnerRule.Id.InSingleQuote() + ')'));

                    //for check
                    foreach ( var a in a_attributes )
                    {
                        var number = 0;
                        if ( !int.TryParse( a.Name.LocalName.Substring( 1 ), out number ) )
                            throw (new ArgumentException("Action-element has wrong name-format a[1,2,3..] attribute (rule " + OwnerRule.Id.InSingleQuote() + ')'));

                        var type = a.Value.TryConvert2Enum< TonalityType >();
                        if ( !type.HasValue )
                            throw (new ArgumentException("Action-element has wrong value in a[1,2,3..] attribute (rule " + OwnerRule.Id.InSingleQuote() + ')'));
                    }

                    
                    var a_type = from a in a_attributes
                                 let number = int.Parse( a.Name.LocalName.Substring( 1 ) )
                                 orderby number
                                 select new 
                                 { 
                                     index = number - 1,
                                     value = a.Value.ToEnum< TonalityType >()
                                 };

                    var sb = new StringBuilder();
                    foreach ( var a_ in a_type )
                    {
                        sb.Append( a_.value.ToText() + '|' );
                    }
                    _ConditionHashset.Add( sb.ToString() );

                    if ( Indexies4GettingSNT == null )
                    {
                        Indexies4GettingSNT = a_type.Select( _ => _.index ).ToArray();
                    }
                    else
                    {
                        if ( Indexies4GettingSNT.Except( a_type.Select( _ => _.index ) ).Any() )
                            throw (new ArgumentException("Action-element has condition-element with different a[1,2,3..] attributes sets (rule " + OwnerRule.Id.InSingleQuote() + ')'));
                    }

                    //check corresponding patterns & action @SNT-attribute indexies
                    foreach ( var pattern in patterns )
                    {
                        foreach ( var i in Indexies4GettingSNT )
                        {
                            if ( pattern.Items.Length <= i )
                                throw (new ArgumentException("a[1,2,3..] has index length more than pattern-item-count (rule " + OwnerRule.Id.InSingleQuote() + ')'));

                            var item = pattern.Items[ i ];
                            switch ( item.TypeOfXmlElement )
                            {
                                case TypeOfXmlElement.U:
                                case TypeOfXmlElement.CET:
                                    throw (new ArgumentException("U-element & CET-element can't have @SNT-attribute (rule " + OwnerRule.Id.InSingleQuote() + ')'));
                            }
                        }
                        

                        if ( determineattribute_ROLE.HasValue )
                        {
                            var index = extensions.ToIndex( determineattribute_ROLE.Value );
                            if ( pattern.Items.Length < index )
                                throw (new ArgumentException("@determineattribute_ROLE-attribute point to not exists element (rule " + OwnerRule.Id.InSingleQuote() + ')'));

                            switch ( pattern.Items[ index ].TypeOfXmlElement )
                            {
                                case TypeOfXmlElement.U:
                                case TypeOfXmlElement.CET:
                                    throw (new ArgumentException("@determineattribute_ROLE-attribute can't point on U-element | CET-element (rule " + OwnerRule.Id.InSingleQuote() + ')'));
                            }
                        }
                        if ( determineattribute_TYPE.HasValue )
                        {
                            var index = extensions.ToIndex( determineattribute_TYPE.Value );
                            if ( pattern.Items.Length < index )
                                throw (new ArgumentException("@determineattribute_TYPE-attribute point to not exists element (rule " + OwnerRule.Id.InSingleQuote() + ')'));

                            switch ( pattern.Items[ index ].TypeOfXmlElement )
                            {
                                case TypeOfXmlElement.U:
                                case TypeOfXmlElement.CET:
                                    throw (new ArgumentException("@determineattribute_TYPE-attribute can't point on U-element | CET-element (rule " + OwnerRule.Id.InSingleQuote() + ')'));
                            }
                        }
                        
                    }
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

            private int[] Indexies4GettingSNT
            {
                get;
                set;
            }
            private ParentTwoChildRule2 OwnerRule
            {
                get;
                set;
            }

            public bool Exists( XElement first, XElement second )
            {
                return (_ConditionHashset.Contains( GetUniqueCode( first, second ).ToString() ));
            }
            private bool Exists( StringBuilder UniqueCode )
            {
                return (_ConditionHashset.Contains( UniqueCode.ToString() ));
            }
            private StringBuilder GetUniqueCode( params XElement[] elements )
            {
                var tempStringBuilder = new StringBuilder( 0x100 );

                foreach ( var i in Indexies4GettingSNT )
                {
                    var element = elements[ i ];
                    if ( element.IsElementU() || element.IsElementCET() )
                        throw (new ArgumentException("U-element & CET-element can't have @SNT-attribute (rule " + OwnerRule.Id.InSingleQuote() + ')'));

                    tempStringBuilder.Append( element.GetAttributeSNT().ToText() + '|' );
                }

                return (tempStringBuilder);
            }
        }
        private class pattern
        {
            internal class item : ProcessingItem
            {
                public item( XElement node )
                    : base( node )
                {                
                }
            }

            public pattern( XElement patternNode, ParentTwoChildRule2 ownerRule )
            {
                #region [.parent.]
                if ( patternNode.Elements().Count() != 1 )
                    throw (new ArgumentException("patternNode.Elements().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                var parent = patternNode.Elements().First();
                if ( parent.Attributes().Count() != 1 )
                    throw (new ArgumentException("parent.Attributes().Count() != 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));

                ParentName = parent.Name.LocalName;
                var attr = parent.FirstAttribute;
                ParentAttributeName = attr.Name.LocalName;
                if ( ParentAttributeName == "TYPE" ) {
                    ParentAttributeValue = attr.Value.ToEnum< TypeAttributeValue >().ToString();
                } else if ( ParentAttributeName == "ROLE" ) {
                    ParentAttributeValue = attr.Value.ToEnum< RoleAttributeValue >().ToString();
                } else {
                    throw (new ArgumentException("parent.FirstAttribute.Name is " + attr.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                }

                if ( parent.Elements().Count() != 2 )
                    throw (new ArgumentException("parent.Elements().Count() != 2 (rule " + ownerRule.Id.InSingleQuote() + ')'));
                #endregion

                #region [.first.]
                var first = parent.Elements().First();
                FirstName = first.Name.LocalName;
                if (1 < first.Attributes().Count())
                    throw (new ArgumentException("first.Attributes().Count() > 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));
                if (first.Attributes().Count() == 1)
                {
                    attr = first.FirstAttribute;
                    FirstAttributeName = attr.Name.LocalName;
                    if ( FirstAttributeName == "TYPE" ) {
                        FirstAttributeValue = attr.Value.ToEnum< TypeAttributeValue >().ToString();
                    } else if ( FirstAttributeName == "ROLE" ) {
                        FirstAttributeValue = attr.Value.ToEnum< RoleAttributeValue >().ToString();
                    } else {
                        throw (new ArgumentException("first.FirstAttribute.Name is " + attr.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                    }
                }

                var firstInnerText = first.Value.Trim();
                if ( !firstInnerText.IsEmptyOrNull() ) {
                    FirstMatchString = firstInnerText.ToLowerEx().Replace('\'', '"');
                }
                #endregion

                #region [.second.]
                var second = first.ElementsAfterSelf().First();
                SecondName = second.Name.LocalName;
                if (1 < second.Attributes().Count())
                    throw (new ArgumentException("second.Attributes().Count() > 1 (rule " + ownerRule.Id.InSingleQuote() + ')'));
                if (second.Attributes().Count() == 1)
                {
                    attr = second.FirstAttribute;
                    SecondAttributeName = attr.Name.LocalName;
                    if ( SecondAttributeName == "TYPE" ) {
                        SecondAttributeValue = attr.Value.ToEnum< TypeAttributeValue >().ToString();
                    } else if ( SecondAttributeName == "ROLE" ) {
                        SecondAttributeValue = attr.Value.ToEnum< RoleAttributeValue >().ToString();
                    } else {
                        throw (new ArgumentException("second.FirstAttribute.Name is " + attr.Name.LocalName.InSingleQuote() + " (rule " + ownerRule.Id.InSingleQuote() + ')'));
                    }
                }

                var secondInnerText = second.Value.Trim();
                if ( !secondInnerText.IsEmptyOrNull() ) {
                    SecondMatchString = secondInnerText.ToLowerEx().Replace('\'', '"');
                }
                #endregion

                #region [.first + second = items.]
                var nodes = parent.GetMultiNodes("./*").ToArray();
                if (nodes.Length != 2)
                    throw (new ArgumentException("Pattern element != 2 items (rule " + ownerRule.Id.InSingleQuote() + ')'));

                Items = (from node in nodes
                         select new item( node )
                        ).ToArray();                
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
            public string FirstMatchString
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
            public string SecondMatchString
            {
                get;
                private set;
            }

            internal item[] Items
            {
                get;
                private set;
            }

            public override string ToString()
            {
                return (ParentName + "[@" + ParentAttributeName + "='" + ParentAttributeValue + "'] : {" +
                        FirstName  + "[@" + FirstAttributeName  + "='" + FirstAttributeValue  + "'] - " + 
                        SecondName + "[@" + SecondAttributeName + "='" + SecondAttributeValue + "']}");
            }
        }

        public ParentTwoChildRule2( XElement ruleNode )
            : base( ruleNode )
        {
            Patterns = (from p in ruleNode.GetMultiNodes( "pattern" )
                                          .RevealConfigNodes()
                        select new pattern( p, this )
                       )
                       .ToArray();

            Actions = (from a in ruleNode.GetMultiNodes( "action" )
                       select new action( a, Patterns, this )
                      )
                      .ToArray();
        }

        private action[]  Actions
        {
            get;
            set;
        }
        private pattern[] Patterns
        {
            get;
            set;
        }

        private static XElement GetNextSiblingWithSkipEmptyU( XElement element, string name )
        {
            foreach ( var nextSibling in element.ElementsAfterSelfExceptT() )
            {
                #region
                /*if ( nextSibling.IsElementU() )
                {
                    if ( !nextSibling.GetCDataTextFromElementU().Trim().IsEmptyOrNull() )
                    {
                        return (null);
                    }
                }
                else*/
                #endregion
                if ( nextSibling.Name.LocalName == name )
                {
                    return (nextSibling);
                }
                else if ( !nextSibling.IsElementU() && (nextSibling.NodeType != XmlNodeType.CDATA) )
                {
                    return (null);
                }
            }
            return (null);
        }

        public override bool Process( XElement subsent )
        {
            var isSubsent = subsent.IsElementSUBSENT();
            //---subsent.ThrowIfNotSubsent();

            var result = false;
            foreach ( var pt in Patterns )
            {
                #region [.condition.]
                var pairs = from parent in subsent.Descendants( pt.ParentName )
                            where (parent.HasAttributeWithValue( pt.ParentAttributeName, pt.ParentAttributeValue ))
                                    &&
                                  (!parent.GetAncestorSubsent().HasAttributeFIRSTQUEUESUBSENT())
                                    &&
                                  (!parent.HasSelfOrAncestorHomogeneous( isSubsent ))

                            let first = parent.Elements( pt.FirstName )
                                              .FirstOrDefault( _child =>
                                                  (pt.FirstAttributeName.IsEmptyOrNull() || 
                                                   _child.HasAttributeWithValue( pt.FirstAttributeName, pt.FirstAttributeValue )) &&
                                                  (pt.FirstMatchString.IsEmptyOrNull() ||
                                                   _child.GetAttributeVALUE().ToLowerEx().Trim() == pt.FirstMatchString)
                                              )
                            where ( first != null )

                            let second = GetNextSiblingWithSkipEmptyU( first, pt.SecondName )
                            where ( second != null ) &&
                                  ( pt.SecondAttributeName.IsEmptyOrNull() ||
                                    second.HasAttributeWithValue( pt.SecondAttributeName, pt.SecondAttributeValue ) ) &&
                                  ( pt.SecondMatchString.IsEmptyOrNull() ||
                                    second.GetAttributeVALUE().ToLowerEx().Trim() == pt.SecondMatchString )
                            
                            select new
                            {
                                FIRST  = first,
                                SECOND = second,
                            };
                #endregion

                foreach ( var p in pairs.Reverse() /*VERY IMPORTANT - in inverse-backward order*/ )
                {
                    MatchRuleDebugInfoOutput( p.FIRST );

                    var localresult = false;
                    foreach ( var a in Actions )
                    {
                        if ( a.Exists( p.FIRST, p.SECOND ) )
                        {
                            #region
                            MatchActionDebugInfoOutput( a );
                            localresult = true;

                            result = true;


                            var role = a.ROLE.HasValue ? a.ROLE.Value :
                                            (a.determineattribute_ROLE.HasValue ? 
                                            (a.determineattribute_ROLE.Value == determine_attribute.firstchildtype) ? 
                                                p.FIRST.GetAttributeROLE() : p.SECOND.GetAttributeROLE() 
                                            : null);
                            var type = a.TYPE.HasValue ? a.TYPE.Value :
                                            (a.determineattribute_TYPE.HasValue ?
                                            (a.determineattribute_TYPE.Value == determine_attribute.firstchildtype) ?
                                                p.FIRST.GetAttributeTYPE() : p.SECOND.GetAttributeTYPE()
                                            : null);

                            var frt = p.FIRST.GetAttributeFRT() + p.SECOND.GetAttributeFRT() + a.FRT_toAdd;
                            if (frt < 0) frt = 0;

                            var newNode = new XElement( a.CreateNewElementType.ToString() );
                            newNode.SetAttributeSNT( a.SNT );
                            newNode.SetAttributeFRT( frt   );                            
                            if (type.HasValue) newNode.SetAttributeTYPE( type.Value );
                            if (role.HasValue) newNode.SetAttributeROLE( role.Value );
                            newNode.SetAttributeCREATEDBYPARENTTWOCHILDRULE();

                            p.FIRST.AddBeforeSelf( newNode );
                            newNode.MoveIn2ThisNodesRange( p.FIRST, p.SECOND );
                            newNode.AddBeforeAndAfterEmptyU();
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
}



