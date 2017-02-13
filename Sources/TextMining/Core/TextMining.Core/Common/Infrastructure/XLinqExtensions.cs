using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;

namespace TextMining.Core
{
    /// <summary>
    /// 
    /// </summary>
    public static partial class XLinqExtensions
    {
        #region [.Consts-class.]
        public static class Consts
        {
        public const string VALUE_ATTRIBUTE      = "VALUE";
        public const string PRE_ATTRIBUTE        = "PRE";
        public const string PROF_ATTRIBUTE       = "PROF";
        public const string ADJ_ATTRIBUTE        = "ADJ";
        public const string SNT_ATTRIBUTE        = "SNT";
        public const string FRT_ATTRIBUTE        = "FRT";
        public const string TYPE_ATTRIBUTE       = "TYPE";
        public const string TYPE_SAVED_ATTRIBUTE = "TYPE-SAVED";
        public const string ROLE_ATTRIBUTE       = "ROLE";
        public const string ID_ATTRIBUTE         = "ID";
        public const string LINK_ATTRIBUTE       = "LINK";
        public const string POS_ATTRIBUTE        = "POS";
        public const string ISSUBJECT_ATTRIBUTE            = "ISSUBJECT";
        public const string ISOBJECT_ATTRIBUTE             = "ISOBJECT";
        public const string INDIRECTSPEECH_BEGIN_ATTRIBUTE = "INDIRECTSPEECH-BEGIN";
        public const string INDIRECTSPEECH_END_ATTRIBUTE   = "INDIRECTSPEECH-END";
        public const string DIRECTSPEECH_BEGIN_ATTRIBUTE   = "DIRECTSPEECH-BEGIN";
        public const string DIRECTSPEECH_END_ATTRIBUTE     = "DIRECTSPEECH-END";
        public const string LN_ATTRIBUTE       = "LN";
        public const string FN_ATTRIBUTE       = "FN";
        public const string MN_ATTRIBUTE       = "MN";
        public const string MI_ATTRIBUTE       = "MI";
        public const string FULLNAME_ATTRIBUTE = "FULLNAME";        

        public const string INQUIRYINITIAL_ATTRIBUTE   = "INQUIRYINITIAL";
        public const string SNTFINAL_ATTRIBUTE         = "SNTFINAL";
        public const string FRTFINAL_ATTRIBUTE         = "FRTFINAL";
        public const string PROCESSASDEFAULT_ATTRIBUTE = "PROCESSASDEFAULT";
        public const string NOTPROCESS_ATTRIBUTE       = "NOTPROCESS";
        public const string FIRST_ITEM_IN_COLLOCATION_ID_ATTRIBUTE        = "FIRST-ITEM-IN-COLLOCATION-ID";
        public const string ITS_FIRST_ENTITY_WITH_ROLE_SUBJ_ATTRIBUTE     = "ITS-FIRST-ENTITY-WITH-ROLE-SUBJ";
        public const string ITS_FIRST_ENTITY_WITH_TYPE_PRONOUN_ATTRIBUTE  = "ITS-FIRST-ENTITY-WITH-TYPE-PRONOUN";
        public const string ITS_FIRST_VERB_ENTITY_WITH_ROLE_OBJ_ATTRIBUTE = "ITS-FIRST-VERB-ENTITY-WITH-ROLE-OBJ";

        public const string U_ELEMENT            = "U";
        public const string T_ELEMENT            = "T";
        public const string ENTITY_ELEMENT       = "ENTITY";
        public const string ENTITIES_ELEMENT     = "ENTITIES";
        public const string VERB_ELEMENT         = "VERB";
        public const string CET_ELEMENT          = "CET";        
        public const string SENT_ELEMENT         = "SENT"; 
        public const string SUBSENT_ELEMENT      = "SUB_SENT";        
        public const string SKIP_SENT_ELEMENT    = "SKIP_" + SENT_ELEMENT;
        public const string FUSKINGSKIP_PREAMBLE = "X_";
        public const string TRUE                 = "TRUE";
        public const string NOT_THEME_ATTRIBUTE   = "NOT_THEME";
        public const string VALUE_THEME_ATTRIBUTE = "VALUE_THEME";
        }
        #endregion
        
        public static XDocument ToXDocument( this string text )
        {
            if ( string.IsNullOrEmpty( text ) )
                throw (new ArgumentNullException());

            using ( var sr  = new StringReader( text ) )
            using ( var xtr = new XmlTextReader( sr ) { Namespaces = false } )
            {                
                return (XDocument.Load( xtr ));
            }
        }
        public static XDocument ToXDocument( this XmlNode node )
        {
            using ( var sr  = new StringReader( node.OuterXml ) )
            using ( var xtr = new XmlTextReader( sr ) { Namespaces = false } )
            {                
                return (XDocument.Load( xtr ));
            }
        }
        public static XDocument ToXDocument( this XElement element )
        {            
            return (element.ToString().ToXDocument());
        }
        /*public static XmlDocument ToXmlDocument(this XDocument xd)
        {
            return (xd.ToString().ToXmlDocument());
        }
        public static XDocument ToXDocument(this XmlDocument xdoc)
        {
            using (var sr = new StringReader(xdoc.OuterXml))
            using (var xtr = new XmlTextReader(sr) { Namespaces = false })
            {
                return (XDocument.Load(xtr));
            }
        }*/

        ////////////////////////////////////////////////////////////////////
        public static string GetAttribute( this XElement element, string name )
        {
            var a = element.Attribute( name );
            if ( a != null )
                return (a.Value);

            return (null);
        }
        public static void SetAttribute( this XElement element, string name, string value )
        {
            var a = element.Attribute( name );
            if ( a == null )
                element.Add( new XAttribute( name, value ) );
            else   
                a.Value = value; 
        }

        public static bool IsElementU       ( this XElement element )
        {
            return (element.Name.LocalName == Consts.U_ELEMENT);
        }
        public static bool IsElementT       ( this XElement element )
        {
            return (element.Name.LocalName == Consts.T_ELEMENT);
        }
        public static bool IsElementCET     ( this XElement element )
        {
            return (element.Name.LocalName == Consts.CET_ELEMENT);
        }
        public static bool IsElementUorCET  ( this XElement element )
        {
            return (element.IsElementU() || element.IsElementCET());
        }
        public static bool IsElementENTITY  ( this XElement element )
        {
            return (element.Name.LocalName == Consts.ENTITY_ELEMENT);
        }
        public static bool IsElementVERB    ( this XElement element )
        {
            return (element.Name.LocalName == Consts.VERB_ELEMENT);
        }
        public static bool IsElementENTITIES( this XElement element )
        {
            return (element.Name.LocalName == Consts.ENTITIES_ELEMENT);
        }
        public static bool IsElementSENT    ( this XElement element )
        {
            return (element.Name.LocalName == Consts.SENT_ELEMENT);
        }
        public static bool IsElementSUBSENT ( this XElement element )
        {
            return (element.Name.LocalName == Consts.SUBSENT_ELEMENT);
        }
        public static bool IsElement_SKIP_SENT( this XElement element )
        {
            return (element.Name.LocalName == Consts.SKIP_SENT_ELEMENT);
        }
        public static string GetCDataTextFromElementU( this XElement element )
        {
            if ( !element.IsElementU() )
                throw (new ArgumentException("This is not 'U' element"));
            
            return (((XText) element.FirstNode).Value.TrimEndSignOfPunctuation());
        }
        public static bool IsTypeInquiry( this XElement element )
        {
            //return (element.GetAttributeTYPE() == TypeAttributeValue.Inquiry);

            return (element.GetAttribute( Consts.TYPE_ATTRIBUTE ).TryConvert2Enum< TypeAttributeValue >() == TypeAttributeValue.Inquiry);
        }
        public static bool IsTypeInquirySelfOrAncestors( this XElement element )
        {
            return (element.AncestorsAndSelf().Any( e => e.IsTypeInquiry() ));
        }
        public static bool IsINQUIRYINITIAL( this XElement element )
        { 
            return (element.GetAttribute( Consts.INQUIRYINITIAL_ATTRIBUTE ) == Consts.TRUE );
        }
        public static bool IsSNTFINALPositive( this XElement element )
        {
            var sntFinal = element.GetAttribute( Consts.SNTFINAL_ATTRIBUTE );
            if ( sntFinal != null )
                return (sntFinal.ToLower().IndexOf("pos") != -1);
            return (false);

            /*return (element.HasAttributeWithAnyValue( SNTFINAL_ATTRIBUTE, 
                 TonalityType.nPos.ToString(),
                 TonalityType.nPPos.ToString(),
                 TonalityType.nAPos.ToString() ));*/
        }
        public static bool IsSNTFINALNegative( this XElement element )
        {
            var sntFinal = element.GetAttribute( Consts.SNTFINAL_ATTRIBUTE );
            if ( sntFinal != null )
                return (sntFinal.ToLower().IndexOf("neg") != -1);
            return (false);
        }

        public static string GetAttributeVALUE( this XElement element )
        {
            return (element.GetAttribute( Consts.VALUE_ATTRIBUTE ));
        }
        public static string GetAttributeVALUETHEME( this XElement element )
        {
            return (element.GetAttribute( Consts.VALUE_THEME_ATTRIBUTE ));
        }
        public static string GetAttributePRE ( this XElement element )
        {
            return (element.GetAttribute( Consts.PRE_ATTRIBUTE ));
        }
        public static string GetAttributePROF( this XElement element )
        {
            return (element.GetAttribute( Consts.PROF_ATTRIBUTE ));
        }
        public static string GetAttributeADJ( this XElement element )
        {
            return (element.GetAttribute( Consts.ADJ_ATTRIBUTE ));
        }
        public static RoleAttributeValue? GetAttributeROLE( this XElement element, string attributeName )
        {
            //return (Extensions.TryConvert2T < RoleAttributeValue >( element.GetAttribute( attributeName ) ));

            var v = element.GetAttribute( attributeName );
            if ( v == null )
                return (null);
            return (v.ToEnum< RoleAttributeValue >());
        }
        public static RoleAttributeValue? GetAttributeROLE( this XElement element )
        {
            return (element.GetAttributeROLE( Consts.ROLE_ATTRIBUTE ));
        }
        public static TypeAttributeValue? GetAttributeTYPE( this XElement element, string attributeName )
        {
            //return (Extensions.TryConvert2T < TypeAttributeValue >( element.GetAttribute( attributeName ) ));

            var v = element.GetAttribute( attributeName );
            if ( v == null )
                return (null);
            return (v.ToEnum< TypeAttributeValue >());
        }
        public static TypeAttributeValue? GetAttributeTYPE( this XElement element )
        {
            return (element.GetAttributeTYPE( Consts.TYPE_ATTRIBUTE ));
        }
        public static TypeAttributeValue? GetAttributeTYPESAVED( this XElement element )
        {
            return (element.GetAttributeTYPE( Consts.TYPE_SAVED_ATTRIBUTE ));
        }
        public static TypeAttributeValue? GetAttributeTYPESAVEDorTYPE( this XElement element )
        {
            return (element.GetAttributeTYPESAVED() ?? element.GetAttributeTYPE());
        }
        public static SubsentenceType GetAttributeSUBSENTTYPE( this XElement element )
        {
            return (element.AttributeValue2Enum< SubsentenceType >( Consts.TYPE_ATTRIBUTE ));

            //var v = element.GetAttribute( TYPE_ATTRIBUTE );
            //if ( v == null )
            //    return (null);
            //return (v.ToEnum< SubsentenceType >());
        }        
        public static int? GetAttributeID( this XElement element )
        {
            return (element.GetAttribute( Consts.ID_ATTRIBUTE ).TryConvert2Int());
        }
        public static int? GetAttributeLINK( this XElement element )
        {
            return (element.GetAttribute( Consts.LINK_ATTRIBUTE ).TryConvert2Int());
        }
        public static string GetAttributeLN( this XElement element )
        {
            return (element.GetAttribute( Consts.LN_ATTRIBUTE ));
        }
        public static string GetAttributeFN( this XElement element )
        {
            return (element.GetAttribute( Consts.FN_ATTRIBUTE ));
        }
        public static string GetAttributeMN( this XElement element )
        {
            return (element.GetAttribute( Consts.MN_ATTRIBUTE ));
        }
        public static string GetAttributeMI( this XElement element )
        {
            return (element.GetAttribute( Consts.MI_ATTRIBUTE ));
        }
        public static string GetAttributeFULLNAME( this XElement element )
        {
            return (element.GetAttribute( Consts.FULLNAME_ATTRIBUTE ));
        }
        public static string GetAttributeFIRST_ITEM_IN_COLLOCATION_ID( this XElement element )
        {
            return (element.GetAttribute( Consts.FIRST_ITEM_IN_COLLOCATION_ID_ATTRIBUTE ));
        }
        public static int? GetAttributePOS( this XElement element )
        {
            return (element.GetAttribute( Consts.POS_ATTRIBUTE ).TryConvert2Int());
        }
        
        public static void SetAttributeROLE( this XElement element, RoleAttributeValue? role )
        {
            element.SetAttribute( Consts.ROLE_ATTRIBUTE, role.GetValueOrDefault( RoleAttributeValue.Other ).ToString() );
        }
        public static void SetAttributeTYPE( this XElement element, TypeAttributeValue type )
        {
            //if ( !element.IsElementU() )
            //if ( element.HasAttribute( Consts.TYPE_ATTRIBUTE ) )
            {
                if ( type == TypeAttributeValue.Inquiry )
                {
                    if ( !element.HasAttribute( Consts.TYPE_SAVED_ATTRIBUTE ) )
                    {
                        var typesaved = element.GetAttributeTYPE();
                        if ( typesaved.HasValue && typesaved.Value.IsLinguisticsCompability()  )
                        {
                            element.SetAttributeTYPESAVED( typesaved.Value );
                        }
                    }
                }
            
                element.SetAttribute( Consts.TYPE_ATTRIBUTE, type.ToString() );
            }
        }
        public static void SetAttributeTYPESAVED( this XElement element, TypeAttributeValue type )
        {
            element.SetAttribute( Consts.TYPE_SAVED_ATTRIBUTE, type.ToString() );
        }        
        public static void SetAttributeTYPE( this XElement element, SubsentenceType type )
        {
            element.SetAttribute( Consts.TYPE_ATTRIBUTE, type.ToString() );
        }
        public static void SetAttributeID( this XElement element, int value )
        {
            element.SetAttribute( Consts.ID_ATTRIBUTE, value.ToString() );
        }
        public static void SetAttributeITSFIRSTENTITYWITHROLESUBJ( this XElement element )
        {
            element.SetAttribute( Consts.ITS_FIRST_ENTITY_WITH_ROLE_SUBJ_ATTRIBUTE, bool.TrueString.ToUpper() );
        }
        public static void SetAttributeITSFIRSTENTITYWITHTYPEPRONOUN( this XElement element )
        {
            element.SetAttribute( Consts.ITS_FIRST_ENTITY_WITH_TYPE_PRONOUN_ATTRIBUTE, bool.TrueString.ToUpper() );
        }        
        public static void SetAttributeITSFIRSTVERBENTITYWITHROLEOBJ( this XElement element )
        {
            element.SetAttribute( Consts.ITS_FIRST_VERB_ENTITY_WITH_ROLE_OBJ_ATTRIBUTE, bool.TrueString.ToUpper() );
        }                

        public static void ThrowIfNotSent( this XElement sent )
        {
            if ( !sent.IsElementSENT() )
                throw (new ArgumentException("!sent.IsElementSENT()"));
        }
        public static void ThrowIfNotSubsent( this XElement subsent )
        {
            if ( !subsent.IsElementSUBSENT() )
                throw (new ArgumentException("!subsent.IsElementSUBSENT()"));
        }
        public static void ThrowIfNotTypeParticipialOrDanglingParticiple( this XElement subsent )
        {
            if ( !subsent.AttributeTypeAreEqual( SubsentenceType.Participial, SubsentenceType.DanglingParticiple ) )
                throw (new ArgumentException("!subsent.AttributeTypeAreEqual( SubsentenceType.Participial, SubsentenceType.DanglingParticiple )"));
        }
        public static void ThrowIfTypeParticipialOrDanglingParticiple( this XElement subsent )
        {
            if ( subsent.AttributeTypeAreEqual( SubsentenceType.Participial, SubsentenceType.DanglingParticiple ) )
                throw (new ArgumentException("!subsent.AttributeTypeAreEqual( SubsentenceType.Participial, SubsentenceType.DanglingParticiple )"));
        }
        public static void ThrowIfNotTypeSubordinateDefault( this XElement subsent )
        {
            if ( !subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate, SubsentenceType.Default ) )
                throw (new ArgumentException("!subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate, SubsentenceType.Default )"));
        }
        public static void ThrowIfNotTypeSubordinate( this XElement subsent )
        {
            if ( !subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate ) )
                throw (new ArgumentException("!subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate )"));
        }
        public static void ThrowIfNotTypeDefault( this XElement subsent )
        {
            if ( !subsent.AttributeTypeAreEqual( SubsentenceType.Default ) )
                throw (new ArgumentException("!subsent.AttributeTypeAreEqual( SubsentenceType.Default )"));
        }
        public static void ThrowIfNotTypeDirectSpeech( this XElement subsent )
        {
            if ( !subsent.AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )
                throw (new ArgumentException("!subsent.AttributeTypeAreEqual( SubsentenceType.DirectSpeech )"));
        }

        //---------------------------------------------------------------------------//
        public static string FullXPath( this XElement element )
        {
            var sb = new StringBuilder();
            while ( element != null )
            {
                sb.Insert( 0, element.Name + ((sb.Length != 0) ? "/" : string.Empty) );

                element = element.Parent;
            }
            return (sb.ToString());
        }
        public static XElement GetSingleNode( this XElement element, string xpath )
        {
            if ( element.Name.LocalName == xpath )
                return (element);

            var resultNode = element.XPathSelectElement( xpath );
            if ( resultNode == null ) 
                throw (new InvalidOperationException( xpath.InSingleQuote() + " is not found in " + element.FullXPath().InSingleQuote()) );

            return (resultNode);
        }
        public static IEnumerable< XElement > GetMultiNodes( this XElement element, string xpath )
        {
            var resultNodes = element.XPathSelectElements( xpath );
            if ( resultNodes == null || !resultNodes.Any() ) 
                throw (new InvalidOperationException( xpath.InSingleQuote() + " is not found in " + element.FullXPath().InSingleQuote()) );

            return (resultNodes);
        }
        public static IEnumerable< XAttribute > GetAttributeNodes( this XElement element, string xpath )
        {
            var obj = element.XPathEvaluate( xpath );
            var attributeNodes = (obj != null) ? ((IEnumerable< object >) obj).Cast< XAttribute >() : null;

            if (attributeNodes == null || !attributeNodes.Any())
                throw (new InvalidOperationException( xpath.InSingleQuote() + " is not found in " + element.FullXPath().InSingleQuote()) );

            return (attributeNodes);
        }        
        public static T AttributeValue2Enum< T >( this XElement element, string attributeName )
        {
            var a = element.Attribute( attributeName );
            if ( a == null )
                throw (new InvalidOperationException( (attributeName.StartsWith("@") ? attributeName.Substring( 1 ) : attributeName).InSingleQuote() + " is not found in " + element.FullXPath().InSingleQuote()) );

            return ((T) Enum.Parse( typeof( T ), a.Value ));
        }        
        //---------------------------------------------------------------------------//
    
        public static IEnumerable< XElement > ElementsEntity( this XElement element )
        {
            return (element.Elements( Consts.ENTITY_ELEMENT ));
        }
        public static IEnumerable< XElement > ElementsCet( this XElement element )
        {
            return (element.Elements( Consts.CET_ELEMENT ));
        }
        public static IEnumerable< XElement > ElementsEntityOrCet( this XElement element )
        {
            return (element.Elements( Consts.ENTITY_ELEMENT )
                        .Concat
                        ( 
                        element.Elements( Consts.CET_ELEMENT ) 
                        )
                    );
        }
        private static IEnumerable< XElement > ElementsEntityOrVerbOrCet( this XElement element )
        {
            return (element.Elements( Consts.ENTITY_ELEMENT )
                        .Concat
                        ( 
                        element.Elements( Consts.VERB_ELEMENT ) 
                        )
                        .Concat
                        ( 
                        element.Elements( Consts.CET_ELEMENT ) 
                        )
                    );
        }
        private static IEnumerable< XElement > DescendantsAndSelfEntityOrCet( this XElement element )
        {
            return (element.DescendantsAndSelf( Consts.ENTITY_ELEMENT )
                        .Concat
                        ( 
                        element.DescendantsAndSelf( Consts.CET_ELEMENT ) 
                        )
                    );
        }
        public static IEnumerable< XElement > ElementsEntityOrEntities( this XElement element )
        {
            return (element.Elements( Consts.ENTITY_ELEMENT )
                        .Concat
                        ( 
                        element.Elements( Consts.ENTITIES_ELEMENT ) 
                        )
                    );
        }
        public static IEnumerable< XElement > ElementsSubsentence( this XElement element )
        {
            return (element.Elements( Consts.SUBSENT_ELEMENT ));
        }
        public static IEnumerable< XElement > ElementsVerb( this XElement element )
        {
            return (element.Elements( Consts.VERB_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsVerb( this XElement element )
        {
            return (element.Descendants( Consts.VERB_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsAndSelfVerb( this XElement element )
        {
            return (element.DescendantsAndSelf( Consts.VERB_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsEntity( this XElement element )
        {
            return (element.Descendants( Consts.ENTITY_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsAndSelfEntity( this XElement element )
        {
            return (element.DescendantsAndSelf( Consts.ENTITY_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsSubsentence( this XElement element )
        {
            return (element.Descendants( Consts.SUBSENT_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsEntities( this XElement element )
        {
            return (element.Descendants( Consts.ENTITIES_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsAndSelfEntities( this XElement element )
        {
            return (element.DescendantsAndSelf( Consts.ENTITIES_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsEntity( this XDocument xdoc )
        {
            return (xdoc.Descendants( Consts.ENTITY_ELEMENT ));
        }
        public static bool HasElementsExceptT( this XElement element )
        {
            return (element.ElementsExceptT().Any());
        }
        public static IEnumerable< XElement > ElementsExceptT( this XElement element )
        {
            return (element.Elements().Where( _ => !_.IsElementT() ));
        }
        public static IEnumerable< XElement > ElementsAfterSelfExceptT( this XElement element )
        {
            return (element.ElementsAfterSelf().Where( _ => !_.IsElementT() ));
        }        
        public static IEnumerable< XElement > DescendantsExceptT( this XElement element )
        {
            return (element.Descendants().Where( _ => !_.IsElementT() ));
        }
        public static IEnumerable< XElement > ElementsAfterSelfUorCET( this XElement element )
        {
            return (element.ElementsAfterSelf().Where( _ => _.IsElementUorCET() ));
        }        
        ////////////////////////////////////////////////////////////////////
       
        public static XElement GetElement( this XContainer container, string name )
        {
            return (container.Element( XName.Get( name ) ));
        }        
        public static IEnumerable< XElement > GetElements( this XElement element, string name )
        {
            return (element.Elements( XName.Get( name ) ));
        }
        public static IEnumerable< XElement > GetDescendants< T >( this IEnumerable< T > source, string name ) where T : XContainer
        {
            return (source.Descendants( XName.Get( name ) ));
        }
        public static IEnumerable< XElement > GetElementsAfterSelf( this XNode node, string name )
        {
            return (node.ElementsAfterSelf( XName.Get( name ) ));
        }

        public static bool HasAttribute( this XElement xelement, string attributeName )
        {
            if ( xelement == null )
                return (false);

            var attribute = xelement.Attribute( attributeName );

            return (attribute != null);
        }
        public static bool HasAttributeWithValue( this XElement xelement, string attributeName, string attributeValue )
        {
            if ( xelement == null )
                return (false);

            var attribute = xelement.Attribute( XName.Get( attributeName ) );

            return ( attribute != null && attribute.Value == attributeValue );
        }
        public static bool HasAttributeWithValueIgnoreCase( this XElement xelement, string attributeName, string attributeValue )
        {
            if ( xelement == null )
                return (false);

            var attribute = xelement.Attribute( XName.Get( attributeName ) );

            return ( attribute != null && string.Compare( attribute.Value, attributeValue, true ) == 0 );
        }
        public static bool HasAttributeWithAnyValue( this XElement xelement, string attributeName, params string[] attributeValues )
        {
            if ( attributeValues == null || attributeValues.Length == 0 )
                throw (new ArgumentNullException());

            if ( xelement == null )
                return (false);

            var attribute = xelement.Attribute( XName.Get( attributeName ) );

            return ( attribute != null && attributeValues.Any( _ => _ == attribute.Value ) );
        }
        public static bool HasTypeAttributeManyValue( this XElement element, params TypeAttributeValue[] types )
        {
            var type = element.GetAttributeTYPE();
            if ( type.HasValue )
                return (types.Contains( type.Value ));
            return (false);
        }
        public static bool HasRoleAttributeValue( this XElement element, RoleAttributeValue role )
        {
            return (element.HasAttributeWithValue( Consts.ROLE_ATTRIBUTE, role.ToString() ) );
        }
        public static bool HasTypeAttributeValue( this XElement element, TypeAttributeValue type )
        {
            return (element.HasAttributeWithValue( Consts.TYPE_ATTRIBUTE, type.ToString() ) );
        }
        public static int? TryAttributeValue2Int( this XElement xelement, string attributeName )
        {
            if ( xelement == null )
                return (null);

            var attribute = xelement.Attribute( XName.Get( attributeName ) );

            if ( attribute == null )
                return (null);

            int v;
            if ( int.TryParse( attribute.Value, out v ) )
                return (v);

            return (null);
        }

        public static bool AttributeTypeAreEqual( this XElement xelement, params SubsentenceType[] types )
        {
            if ( xelement == null )
                return (false);

            var type = xelement.GetAttributeSUBSENTTYPE();
            for ( int i = 0; i < types.Length; i++ )
            {
                if ( type == types[ i ] )
                    return (true);
            }
            return (false);

            //return (xelement.HasAttributeWithAnyValue( "TYPE", types.Select( _ => _.ToString() ).ToArray() ));
        }
        public static int? AttributeSubjValue(this XElement xelement)
        {
            return (xelement.TryAttributeValue2Int( "SUBJ" ));
        }
        public static IEnumerable< XElement > GetDescendantsSubsent< T >( this IEnumerable< T > source ) where T : XContainer
        {
            return (source.GetDescendants( Consts.SUBSENT_ELEMENT ));
        }
        public static XElement GetAncestorSubsent( this XElement element )
        {
            return (element.Ancestors( Consts.SUBSENT_ELEMENT ).FirstOrDefault());
        }
        public static IEnumerable< XElement > GetAncestorsSubsents( this XElement element )
        {
            return (element.Ancestors( Consts.SUBSENT_ELEMENT ));
        }
        public static XElement GetAncestorSent( this XElement element )
        {
            return (element.Ancestors( Consts.SENT_ELEMENT ).FirstOrDefault());
        }
        public static XElement GetLinkEntityOrCet(this XElement subsent)
        {subsent.ThrowIfNotSubsent();

            foreach ( var e in subsent.ElementsEntityOrVerbOrCet() )
            {
                var linkEntityOrCet = e.DescendantsAndSelfEntityOrCet()
                                       .FirstOrDefault( d => d.GetAttributeLINK().HasValue );
                if ( linkEntityOrCet.IsNotNull() )
                    return (linkEntityOrCet);
            }
            return (null);
        }

        public/*private*/ static XElement FindEntityById( this XDocument xd, int id )
        {
            return (xd.XPathSelectElement( "//ENTITY[ @ID = '" + id + "' ]" ));
        }
        /*public*/private static XElement FindEntityById( this XElement element, int id )
        {
            return (element.XPathSelectElement( ".//ENTITY[ @ID = '" + id + "' ]" ));
        }
        public static XElement FindEntityByIdNotInDirectSpeech( this XElement element, int id )
        {
            return (element.XPathSelectElement(".//ENTITY[ @ID = '" + id + "' and not(ancestor::*[ @TYPE='DirectSpeech' ]) ]"));

            /*return (element.DescendantsAndSelfEntity()
                           .FirstOrDefault( d => d.GetAttributeID() == id && 
                                                 d.Ancestors()
                                                  .All( a => a.GetAttributeTYPE2() != SubsentenceType.DirectSpeech ) 
                                          )
                   );
            */            
        }
        public static XElement FindEntityById_4Subsent( this XElement subsent, int id )
        {subsent.ThrowIfNotSubsent();

            var subjEntity = default(XElement);
            var ancestorSubsent = subsent.GetAncestorsSubsents()
                                         .FirstOrDefault( s => s.AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) );
            if ( ancestorSubsent.IsNotNull() )
            {
                subjEntity = ancestorSubsent.FindEntityById( id );
            }
            else
            {
                var sent = subsent.GetAncestorSent();
                if ( sent != null )
                     subjEntity = sent.FindEntityById( id );

                if ( subjEntity == null )
                {
                    subjEntity = subsent.Document.FindEntityById( id );

                    if ( subjEntity.IsNotNull() &&
                         subjEntity.Ancestors().Any( a => a.IsElement_SKIP_SENT() ) )
                    {
                        return (null);
                    }
                }
            }
            return (subjEntity);
        }
        public static XElement FindSubsentById( this XElement element, int id )
        {
            return (element.Ancestors( Consts.SENT_ELEMENT ).First().XPathSelectElement( ".//SUB_SENT[ @ID = '" + id + "' ]" ));
        }
        public static XElement FindElementById( this XElement element, int id )
        {
            return (element.XPathSelectElement( ".//*[ @ID = '" + id + "' ]" ));
        }

        public static void RenameCET2ENTITY( this XElement element )
        {
            if ( element.IsElementCET() )
                element.Name = Consts.ENTITY_ELEMENT;
        }

        public static bool HasElementTypeInquiry( this XElement node )
        {
            return (node.ElementsExceptT().Any( element => element.IsTypeInquiry() ));
        }
        public static bool HasAttributeSNTFINAL( this XElement node )
        {
            return (node.HasAttribute( Consts.SNTFINAL_ATTRIBUTE ));
        }


        public static void RemoveOtherSubjectAttributes( this XElement sent, XElement subject, Func< XElement, XElement, bool > subjectCompareFunction )
        {sent.ThrowIfNotSent();

            //remove other-value '@ISSUBJECT' & '@INDIRECTSPEECH_BEGIN-END' & '@DIRECTSPEECH_BEGIN-END'
            var issubject = subject.GetAttribute( Consts.ISSUBJECT_ATTRIBUTE );
            var xpath     = ".//*[ @{1} != '{0}' or @{2} != '{0}' or @{3} != '{0}' or @{4} != '{0}' or @{5} != '{0}' ]"
                            .FormatEx( issubject, Consts.ISSUBJECT_ATTRIBUTE,
                                                  Consts.INDIRECTSPEECH_BEGIN_ATTRIBUTE,
                                                  Consts.INDIRECTSPEECH_END_ATTRIBUTE,
                                                  Consts.DIRECTSPEECH_BEGIN_ATTRIBUTE,
                                                  Consts.DIRECTSPEECH_END_ATTRIBUTE );
            var elements = sent.XPathSelectElements( xpath );
            foreach ( var e in elements )
            {
                e.RemoveAttribute( Consts.ISSUBJECT_ATTRIBUTE );
                e.RemoveAttribute( Consts.INDIRECTSPEECH_BEGIN_ATTRIBUTE );
                e.RemoveAttribute( Consts.INDIRECTSPEECH_END_ATTRIBUTE );
                e.RemoveAttribute( Consts.DIRECTSPEECH_BEGIN_ATTRIBUTE );
                e.RemoveAttribute( Consts.DIRECTSPEECH_END_ATTRIBUTE );
            }

            //remove same-value '@ISSUBJECT'
            xpath     = ".//*[ @{1} = '{0}' ]"
                        .FormatEx( issubject, Consts.ISSUBJECT_ATTRIBUTE );
            elements = sent.XPathSelectElements( xpath );
            foreach ( var e in elements )
            {
                if ( !subjectCompareFunction( e, subject ) )
                    e.RemoveAttribute( Consts.ISSUBJECT_ATTRIBUTE );
            }
        }
        private static void RemoveAttribute( this XElement element, string attrname )
        {
            var issubject = element.Attribute( attrname );
            if ( issubject != null )
            {
                issubject.Remove();
            }
        }

        public static bool IsLinguisticsCompability( this TypeAttributeValue value )
        {
            switch ( value )
            {
                case TypeAttributeValue.Collocation:
                case TypeAttributeValue.Participial:
                case TypeAttributeValue.DanglingParticiple:
                case TypeAttributeValue.Subordinate:
                case TypeAttributeValue.Inquiry:
                    return (false);

                default:
                    return (true);
            }
        }

        public static string GetVALUE( this XElement element ) 
        {
            #region
            /*var prof = element.GetAttributePROF();            
            if ( !prof.IsEmptyOrNull() )
            {
                var pre = element.GetAttributePRE();
                if ( !pre.IsEmptyOrNull() ) 
                    return (pre + ' ' + prof);
                return (prof);
            }*/
            #endregion

            var v = element.GetAttributeVALUE();
            if ( !v.IsEmptyOrNull() )
                return (v);

            return (element.Value);
        }
        public static string GetVALUETHEMEorVALUE( this XElement element ) 
        {
            var v = element.GetAttribute( Consts.VALUE_THEME_ATTRIBUTE );
            if ( !v.IsEmptyOrNull() )
                return (v);                
            
            v = element.GetAttributeVALUE();
            if ( !v.IsEmptyOrNull() )
                return (v);

            return (element.Value);
        }
        public static bool MIhasCharAtPosition( this XElement element, char @char, int position )
        {
            var mi = element.GetAttributeMI();
            if ( mi != null )
            {
                var _MI =  mi.SplitByComma();
                for ( var i = 0; i < _MI.Length; i++ )
                {
                    var partof_mi = _MI[ i ];
                    if ( position <= partof_mi.Length )
                    {
                        if ( partof_mi[ position - 1 ] == @char )
                            return (true);
                    }
                }
            }
            return (false);
        }

        public static bool HasAttributeNOTTHEME( this XElement node )
        {
            return (node.HasAttribute( Consts.NOT_THEME_ATTRIBUTE ));
        }
        public static bool HasAttributeVALUE( this XElement node )
        {
            return (node.HasAttribute( Consts.VALUE_ATTRIBUTE ));
        }
        public static bool HasAttributeVALUETHEME( this XElement node )
        {
            return (node.HasAttribute( Consts.VALUE_THEME_ATTRIBUTE ));
        }        
    }
}