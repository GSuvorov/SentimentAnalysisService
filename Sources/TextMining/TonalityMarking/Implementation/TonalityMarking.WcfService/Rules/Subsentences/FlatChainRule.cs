using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.Xml.XPath;
using TextMining.Core;

namespace TonalityMarking
{
    /*internal static class FlatChainPatternXpathCreater
    {
        private static string GetXpathCondition( ProcessingItem processingItem )
        {
            var sb = new StringBuilder();
            switch ( processingItem.TypeOfXmlElement )
            {
                case TypeOfXmlElement.U:                            
                    var split = processingItem.XmlNode.GetAttribute("split");
                    var matchStrings = split.IsEmptyOrNull() 
                        ? new[] { processingItem.CDataText4ElementU }
                        : processingItem.CDataText4ElementU.Split( new[] { split }, StringSplitOptions.RemoveEmptyEntries );

                    matchStrings = (from ms in matchStrings
                                    let _ = ms.Trim().Replace('\'', '"')
                                    where !_.IsEmptyOrNull()
                                    select _
                                   ).ToArray();
                    if ( matchStrings.Length < 1 )
                        throw (new ArgumentException("matchStrings.Length < 1"));

                    if (1 < matchStrings.Length) sb.Append('(');
                    for ( int i = 0; i < matchStrings.Length; i++ )
                    {
                        sb.Append("contains(text(), '" + matchStrings[ i ] + "')");
                        if ( i < matchStrings.Length - 1 ) sb.Append(" or ");
                    }
                    if (1 < matchStrings.Length) sb.Append(')');
                break;

                default:
                    var role = processingItem.XmlNode.GetAttributeROLE();
                    var type = processingItem.XmlNode.GetAttributeTYPE();

                    if ( role.HasValue )
                    {
                        sb.Append("@ROLE = '" + role.Value.ToString().Replace('\'', '"') + "'");                                
                    }
                    if ( type.HasValue )
                    {
                        sb.Append("@TYPE = '" + type.Value.ToString().Replace('\'', '"') + "'");                                
                    }
                break;
            }
            return (sb.ToString());
        }

        public static string Parse( XmlNode patternNode )
        {
            var nodes = patternNode.GetMultiNodes( "./*" ).ToArray();
            if ( nodes.Length < 2 )
                throw (new ArgumentException("nodes.Length < 2"));

            var processingItem = new ProcessingItem( nodes[ 0 ] );
            var sb = new StringBuilder( processingItem.XmlNode.Name + '[' + GetXpathCondition( processingItem ) );
            for ( int i = 1; i < nodes.Length; i++ )
            {
                processingItem = new ProcessingItem( nodes[ i ] );

                sb.Append(" and (following-sibling::*)[1][ local-name()='" +
                            processingItem.XmlNode.Name + "' and " +
                            GetXpathCondition(processingItem) + ']');
            }
            sb.Append( ']' );

            return (sb.ToString());
        }
    }*/

    /// <summary>
    /// 
    /// </summary>
    internal class FlatChainRule : RuleBase
    {
        private enum determine_attribute
        {
            firsttype,
            secondtype,
            thirdtype,
        }
        private static class extensions
        {
            public static int ToIndex( determine_attribute value )
            {
                switch ( value )    
                {
                    case determine_attribute.firsttype:  return (0);
                    case determine_attribute.secondtype: return (1);
                    case determine_attribute.thirdtype:  return (2);
                    default:
                        throw (new ArgumentException(value.ToString()));
                }
            }
        }

        private class pattern
        {
            internal class item : ProcessingItem
            {
                public item( XElement node ) : base( node )
                {
                    //var split = default(string);
                    switch (this.TypeOfXmlElement)
                    {
                        case TypeOfXmlElement.U:
                            #region
                            var cdataText = (node.FirstNode != null)
                                           ? node.GetCDataTextFromElementU() /*<U>a [| b [| c]]</U>*/
                                           : string.Empty /*<U> </U>*/;

                            MatchStringsCETorU = (from ms in cdataText.ToLowerEx().SplitBySpace()
                                                  let _ = ms.Trim().Replace('\'', '"')
                                                  where !_.IsEmptyOrNull()
                                                  select _
                                                 ).ToArray();

                            #region [.commented. previous.]
                            //split = node.GetAttribute("split");

                            //var cdataText = (node.FirstNode != null)
                            //               ? node.GetCDataTextFromElementU() /*<U>a [| b [| c]]</U>*/
                            //               : string.Empty /*<U> </U>*/;

                            //MatchStringsCETorU = split.IsEmptyOrNull()
                            //    ? new[] { cdataText.ToLowerEx() }
                            //    : cdataText.ToLowerEx().Split( new[] { split }, StringSplitOptions.RemoveEmptyEntries );

                            //MatchStringsCETorU = (from ms in MatchStringsCETorU
                            //                      let _ = ms.Trim().Replace('\'', '"')
                            //                      where !_.IsEmptyOrNull()
                            //                      select _
                            //                     ).ToArray();
                            #endregion
                            break;
                            #endregion
                            

                        case TypeOfXmlElement.CET:
                            #region
                            TypeENTITYorVERB = node.GetAttributeTYPE();

                            var cetInnerText = node.Value;

                            MatchStringsCETorU = (from ms in cetInnerText.ToLowerEx().SplitBySpace()
                                                  let _ = ms.Trim().Replace('\'', '"')
                                                  where !_.IsEmptyOrNull()
                                                  select _
                                                 ).ToArray();

                            #region [.commented. previous.]
                            //split = node.GetAttribute("split");
                            //var cetInnerText = node.Value;

                            //MatchStringsCETorU = split.IsEmptyOrNull()
                            //    ? new[] { cetInnerText.ToLowerEx() }
                            //    : cetInnerText.ToLowerEx().Split( new[] { split }, StringSplitOptions.RemoveEmptyEntries );

                            //MatchStringsCETorU = (from ms in MatchStringsCETorU
                            //                      let _ = ms.Trim().Replace('\'', '"')
                            //                      where !_.IsEmptyOrNull()
                            //                      select _
                            //                     ).ToArray();
                            #endregion
                            break;
                            #endregion

                        case TypeOfXmlElement.ENTITY:
                        case TypeOfXmlElement.ENTITIES:
                        case TypeOfXmlElement.VERB:
                            #region
                            RoleENTITYorVERB = node.GetAttributeROLE();
                            TypeENTITYorVERB = node.GetAttributeTYPE();

                            var innerText = node.Value.Trim().ToLowerEx();
                            MatchStringENTITYorVERB = innerText.IsEmptyOrNull() ? null : innerText;
                            break;
                            #endregion

                        default:
                            throw (new ArgumentException(this.TypeOfXmlElement.ToString()));
                    }
                }

                private string[] MatchStringsCETorU
                {
                    get;
                    set;
                }
                private string MatchStringENTITYorVERB
                {
                    get;
                    set;
                }
                private RoleAttributeValue? RoleENTITYorVERB
                {
                    get;
                    set;
                }
                private TypeAttributeValue? TypeENTITYorVERB
                {
                    get;
                    set;
                }

                public bool IsEquality( FlatChainRuleProcessingItem processingItem )
                {
                    if ( this.TypeOfXmlElement != processingItem.TypeOfXmlElement )
                        return (false);

                    var result = default(bool);
                    switch ( this.TypeOfXmlElement )
                    {
                        case TypeOfXmlElement.U:
                            #region
                            if (MatchStringsCETorU.Length == 0)
                            {
                                return (processingItem.Uvalue.Length == 0);
                            }
                            return (processingItem.Uvalue.SearchForSubsequence( MatchStringsCETorU ));
                            #endregion
                        case TypeOfXmlElement.CET:
                            #region
                            result = true;
                            if (TypeENTITYorVERB.HasValue)
                                result &= (TypeENTITYorVERB == processingItem.TYPE);

                            if ( MatchStringsCETorU.Length == 0 )
                            {
                                return (result);// && processingItem.VALUE.Trim().IsEmptyOrNull()); 
                                                //processingItem.VALUE.SplitBySpace().Length == 0);
                            }
                            return (result && processingItem.CETvalue.SequenceEqual( MatchStringsCETorU ));
                            #endregion

                        default:
                            #region
                            result = true;
                            if (RoleENTITYorVERB.HasValue)
                                result &= (RoleENTITYorVERB == processingItem.Role);
                            if (TypeENTITYorVERB.HasValue)
                                result &= (TypeENTITYorVERB == processingItem.TYPE);
                            if ( MatchStringENTITYorVERB != null )
                                result &= (MatchStringENTITYorVERB == processingItem.VALUE);
                            return (result); 
                            #endregion
                    }
                }

                public override string ToString()
                {
                    switch (TypeOfXmlElement)
                    {
                        case TypeOfXmlElement.U:
                            return (TypeOfXmlElement.ToString() + " [" + string.Join(" | ", MatchStringsCETorU) + ']');

                        default:
                            var sb = new StringBuilder();
                            if (RoleENTITYorVERB.HasValue)
                                sb.Append("role:" + RoleENTITYorVERB.Value.ToString());
                            if (TypeENTITYorVERB.HasValue)
                                sb.Append("type:" + TypeENTITYorVERB.Value.ToString());
                            return (TypeOfXmlElement.ToString() + ((sb.Length != 0) ? (" [" + sb.ToString() + ']') : string.Empty));
                    }
                }
            }

            public pattern( XElement patternNode, FlatChainRule ownerRule )
            {
                OwnerRule = ownerRule;

                var nodes = patternNode.GetMultiNodes("./*").ToArray();
                if (nodes.Length < 2)
                    throw (new ArgumentException("Pattern element contains less than 2 items (rule " + OwnerRule.Id.InSingleQuote() + ')'));

                Items = (from node in nodes
                         select new item( node )
                        ).ToArray();
            }

            internal item[] Items
            {
                get;
                private set;
            }
            private FlatChainRule OwnerRule
            {
                get;
                set;
            }
            
            /*public List< FlatChainRuleProcessingItem > TryGetItemsChain( FlatChainRuleProcessingItem processingItem,
                                                                         List< FlatChainRuleProcessingItem > tempResultList )
            {
                //var tempResultList = new List< FlatChainRuleProcessingItem >();
                foreach ( var item in Items )
                {
                    if ( processingItem == null )
                    {
                        tempResultList.Clear();
                        return (null);
                    }

                    if ( !item.IsEquality( processingItem ) )
                    {
                        tempResultList.Clear();
                        return (null);
                    }

                    tempResultList.Add( processingItem );
                    processingItem = processingItem.NextSiblingFlatChain;
                }

                if ( Items.Length != tempResultList.Count )
                {
                    tempResultList.Clear();
                    return (null);
                }

                return (tempResultList);
            }
            */
            public bool TryGetItemsChain( FlatChainRuleProcessingItem processingItem,
                                          List< FlatChainRuleProcessingItem > tempResultList )
            {
                foreach ( var item in Items )
                {
                    if ( processingItem == null )
                    {
                        tempResultList.Clear();
                        return (false);
                    }

                    if ( !item.IsEquality( processingItem ) )
                    {
                        tempResultList.Clear();
                        return (false);
                    }

                    tempResultList.Add( processingItem );
                    processingItem = processingItem.NextSiblingFlatChain;
                }

                if ( Items.Length != tempResultList.Count )
                {
                    tempResultList.Clear();
                    return (false);
                }

                return (true);
            }

            public override string ToString()
            {
                return (string.Join(", ", Items.Select(_ => _.ToString()).ToArray()));
            }
        }
        private class action : actionBase
        {
            private HashSet< string > _ConditionHashset = new HashSet< string >();

            public action( XElement actionNode, pattern[] patterns, FlatChainRule ownerRule ) : base( actionNode )
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
                    //var a_attributes = condition.GetMultiNodes( "@*[ starts-with( local-name(), 'a' ) ]" ).Cast< XAttribute >().ToArray();
                    var a_attributes = condition.GetAttributeNodes( "@*[ starts-with( local-name(), 'a' ) ]" ).ToArray();

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
            private FlatChainRule OwnerRule
            {
                get;
                set;
            }

            public bool Exists( List< FlatChainRuleProcessingItem > itemsChain )
            {
                return (_ConditionHashset.Contains( GetUniqueCode( itemsChain ).ToString() ));
            }
            private bool Exists( StringBuilder UniqueCode )
            {
                return (_ConditionHashset.Contains( UniqueCode.ToString() ));
            }
            private StringBuilder GetUniqueCode( List< FlatChainRuleProcessingItem > itemsChain )
            {
                var tempStringBuilder = new StringBuilder( 0x100 );

                foreach ( var i in Indexies4GettingSNT )
                {
                    var item = itemsChain[ i ];
                    switch ( item.TypeOfXmlElement )
                    {
                        case TypeOfXmlElement.U:
                        case TypeOfXmlElement.CET:
                            throw (new ArgumentException("U-element & CET-element can't have @SNT-attribute (rule " + OwnerRule.Id.InSingleQuote() + ')'));
                    }

                    tempStringBuilder.Append( item.XElement.GetAttributeSNT().ToText() + '|' );
                }

                return (tempStringBuilder);
            }
        }

        public FlatChainRule( XElement ruleNode )
            : base( ruleNode )
        {
            Patterns = (from p in ruleNode.GetMultiNodes("pattern")
                                          .RevealConfigNodes()
                        select new pattern( p, this )
                       )
                       .ToArray();

            Actions = (from a in ruleNode.GetMultiNodes("action")
                       select new action( a, Patterns, this )
                      )
                      .ToArray();
        }

        private pattern[] Patterns
        {
            get;
            set;
        }
        private action[] Actions
        {
            get;
            set;
        }

        /*private List< FlatChainRuleProcessingItem > TryGetItemsChain( FlatChainRuleProcessingItem processingItem )
        {
            var tempResultList = new List< FlatChainRuleProcessingItem >();
            foreach ( var pattern in Patterns )
            {
                var itemsChain = pattern.TryGetItemsChain( processingItem, tempResultList );
                if ( itemsChain != null )
                {
                    return (itemsChain);
                }
            }
            return (null);
        }*/

        private bool ProcessInternal( XElement subsent )
        {
            var isSubsent = subsent.IsElementSUBSENT();

            var itemsChain = new List< FlatChainRuleProcessingItem >();

            foreach ( XElement node in subsent.GetAllProcessingElementsInSubsent() )
            {
                var processingItem = new FlatChainRuleProcessingItem( node );

                #region [.TryGetItemsChain.]
                foreach ( var pattern in Patterns )
                {
                    if ( pattern.TryGetItemsChain( processingItem, itemsChain ) )
                    {
                        break;
                    }
                }
                if ( itemsChain.Count == 0 )
                {
                    continue;
                }
                if ( itemsChain.HasAncestorHomogeneous( isSubsent ) )
                {
                    continue;
                }
                #endregion

                MatchRuleDebugInfoOutput( subsent );

                foreach ( var a in Actions )
                {
                    if ( a.Exists( itemsChain ) )
                    {
                        #region
                        MatchActionDebugInfoOutput( a );

                        var role = a.ROLE.HasValue ? a.ROLE.Value :
                                                    (a.determineattribute_ROLE.HasValue
                                                     ? itemsChain[extensions.ToIndex(a.determineattribute_ROLE.Value)].Role : null);
                        var inquiryItem = itemsChain.FirstOrDefault( _ => _.TYPE == TypeAttributeValue.Inquiry );
                        var type = (inquiryItem != null) ? TypeAttributeValue.Inquiry :
                                     a.TYPE.HasValue
                                     ? a.TYPE.Value :
                                       (a.determineattribute_TYPE.HasValue
                                        ? itemsChain[extensions.ToIndex(a.determineattribute_TYPE.Value)].TYPE : null);

                        var frt = itemsChain.Sum( _ => _.XElement.GetAttributeFRT() ) + a.FRT_toAdd;
                        if ( frt < 0 ) frt = 0;

                        var newNode = new XElement( a.CreateNewElementType.ToString() );
                        newNode.SetAttributeSNT( a.SNT );
                        newNode.SetAttributeFRT( frt   );
                        if (type.HasValue) newNode.SetAttributeTYPE( type.Value );
                        if (role.HasValue) newNode.SetAttributeROLE( role.Value );

                        #region [.if some-one item in chain is ENTITIES => put him @ID on new-created-parent.]
                        if ( newNode.IsElementENTITIES() )
                        {
                            var entitiesItemsChain = (from item in itemsChain
                                                      where item.XElement.IsElementENTITIES()
                                                      select item.XElement
                                                     )
                                                     .FirstOrDefault();
                            if ( entitiesItemsChain != null )
                            {
                                newNode.SetAttributeID( entitiesItemsChain.GetAttributeID().Value );
                            }
                        }
                        #endregion

                        node.AddBeforeSelf( newNode );

                        foreach ( var itemInChain in itemsChain )
                        {
                            itemInChain.XElement.Remove();
                            newNode.Add( itemInChain.XElement );
                        }
                        newNode.AddBeforeAndAfterEmptyU();

                        return (true);
                        #endregion
                    }
                }

                NomatchActionDebugInfoOutput();
            }
            return (false);
        }
        public override bool Process( XElement subsent )
        {
            if ( ProcessInternal( subsent ) )
            {
                while ( ProcessInternal( subsent ) );

                return (true);
            }
            return (false);
        }
        public override RuleCycleType RuleCycle
        {
            get { return (RuleCycleType.RecycleOnSuccsess); }
        }
    }
}



