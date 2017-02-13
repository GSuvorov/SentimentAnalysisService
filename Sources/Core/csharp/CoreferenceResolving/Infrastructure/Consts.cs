using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

using Linguistics.Core;
using Linguistics.Coreference;

namespace CoreferenceResolving
{
    /// <summary>
    /// 
    /// </summary>
    internal static class Consts
    {
        public const string SUBSENT_ELEMENT = "SUB_SENT";
        public const string ENTITY_ELEMENT  = "ENTITY";
        public const string TYPE_ATTRIBUTE  = "TYPE";
        public const string TYPE_SAVED_ATTRIBUTE = "TYPE-SAVED";
        public const string VALUE_ATTRIBUTE      = "VALUE";
        public const string VALUETHEME_ATTRIBUTE = "VALUE_THEME";        
        public const string PRE_ATTRIBUTE   = "PRE";
        public const string PROF_ATTRIBUTE  = "PROF";
        public const string LN_ATTRIBUTE    = "LN";
        public const string FN_ATTRIBUTE    = "FN";
        public const string MN_ATTRIBUTE    = "MN";
        public const string NAME_ATTRIBUTE  = "NAME";        
        public const string POST_ATTRIBUTE  = "POST";
        public const string PO_ATTRIBUTE    = "PO";
        public const string NOT_THEME_ATTRIBUTE    = "NOT_THEME";
        public const string COREF_MASTER_ATTRIBUTE = "COREF-MASTER";
        public const string COREF_SLAVE_ATTRIBUTE  = "COREF-SLAVE";

        /*public static IEnumerable< XElement > DescendantsSubsent( this XDocument xdoc )
        {
            return (xdoc.Descendants( Consts.SUBSENT_ELEMENT ));
        }
        public static IEnumerable< XElement > DescendantsEntity( this XElement xe )
        {
            return (xe.Descendants( Consts.ENTITY_ELEMENT ));
        }
        public static IEnumerable< XElement > ElementsEntity( this XElement xe )
        {
            return (xe.Elements( Consts.ENTITY_ELEMENT ));
        }*/
        public static IEnumerable< XElement > DescendantsEntity( this XElement element )
        {
            return (element.Descendants( Consts.ENTITY_ELEMENT ));
        }
        public static IEnumerable< XElement > AncestorsEntity( this XElement element )
        {
            return (element.Ancestors( Consts.ENTITY_ELEMENT ));
        }
        public static XElement GetAncestorSubsent( this XElement element )
        {
            return (element.Ancestors( Consts.SUBSENT_ELEMENT ).FirstOrDefault());
        }

        public static string GetAttribute( this XElement element, string name )
        {
            var a = element.Attribute( name );
            if ( a != null )
                return (a.Value);

            return (null);
        }
        public static string GetAttributeVALUE( this XElement element )
        {
            return (element.GetAttribute( Consts.VALUE_ATTRIBUTE ));
        }
        public static string GetAttributeVALUETHEME( this XElement element )
        {
            return (element.GetAttribute( Consts.VALUETHEME_ATTRIBUTE ));
        }
        public static string GetAttributePRE ( this XElement element )
        {
            return (element.GetAttribute( Consts.PRE_ATTRIBUTE ));
        }
        public static string GetAttributePROF( this XElement element )
        {
            return (element.GetAttribute( Consts.PROF_ATTRIBUTE ));
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
        public static string GetAttributePOST( this XElement element )
        {
            return (element.GetAttribute( Consts.POST_ATTRIBUTE ));
        }
        public static string GetAttributePO( this XElement element )
        {
            return (element.GetAttribute( Consts.PO_ATTRIBUTE ));
        }
        public static SubSentenceType GetAttributeSUBSENTTYPE( this XElement element )
        {
            return (element.AttributeValue2Enum< SubSentenceType >( Consts.TYPE_ATTRIBUTE ));
        }
        public static EntityType? GetAttributeTYPE ( this XElement element )
        {
            var v = element.GetAttribute( Consts.TYPE_ATTRIBUTE );
            if ( v == null )
                return (null);

            var type = default(EntityType);
            if ( Enum.TryParse( v, out type ) )
                return (type);
            return (null);
        }
        public static EntityType  GetAttributeTYPE2( this XElement element )
        {
            return (element.AttributeValue2Enum< EntityType >( Consts.TYPE_ATTRIBUTE ));
        }
        public static EntityType? GetAttributeTYPESAVED( this XElement element )
        {
            var v = element.GetAttribute( Consts.TYPE_SAVED_ATTRIBUTE );
            if ( v == null )
                return (null);

            var type = default(EntityType);
            if ( Enum.TryParse( v, out type ) )
                return (type);
            return (null);
        }
        public static EntityType? GetAttributeTYPESAVEDorTYPE( this XElement element )
        {
            return (element.GetAttributeTYPESAVED() ?? element.GetAttributeTYPE());
        }
        public static string GetAttributeNAME( this XElement element )
        {
            return (element.GetAttribute( Consts.NAME_ATTRIBUTE ));
        }

        public static T AttributeValue2Enum< T >( this XElement element, string attributeName )
        {
            var a = element.Attribute( attributeName );
            if ( a == null )
                throw (new InvalidOperationException( (attributeName.StartsWith("@") ? attributeName.Substring( 1 ) : attributeName).InSingleQuote() + " is not found in " + element.FullXPath().InSingleQuote()) );

            return ((T) Enum.Parse( typeof( T ), a.Value ));
        }
        private static string FullXPath( this XElement element )
        {
            var sb = new StringBuilder();
            while ( element != null )
            {
                sb.Insert( 0, element.Name + ((sb.Length != 0) ? "/" : string.Empty) );

                element = element.Parent;
            }
            return (sb.ToString());
        }
        private static bool HasAttribute( this XElement xelement, string attributeName )
        {
            if ( xelement == null )
                return (false);

            var attribute = xelement.Attribute( attributeName );

            return (attribute != null);
        }
        public static bool HasAttributeNOTTHEME( this XElement node )
        {
            return (node.HasAttribute( Consts.NOT_THEME_ATTRIBUTE ));
        }

        public static bool IsEmptyOrNull( this string text )
        {
            return (string.IsNullOrEmpty( text ));    
        }
        public static string InSingleQuote( this string text )
        {
            return ('\'' + text + '\'');
        }

        public static CoreferenceGroupType? ToGroupType ( this EntityType type )
        {
            switch ( type )
            {
                case EntityType.PersonName:
                    return (CoreferenceGroupType.PersonNameGroup);

                case EntityType.PersonInfo:
                    return (CoreferenceGroupType.PersonInfoGroup);

                case EntityType.PersonNoInfo:
                    return (CoreferenceGroupType.PersonNoInfoGroup);

                case EntityType.OrgName:
                case EntityType.OrgNoInfo:
                    return (CoreferenceGroupType.OrgGroup);

                case EntityType.PlaceName:
                case EntityType.PlaceNoInfo:
                case EntityType.PseudoGeo:
                    return (CoreferenceGroupType.PlaceGroup);

                case EntityType.OrgAbbr:
                    return (CoreferenceGroupType.OrgAbbrGroup);

                case EntityType.Event:
                    return (CoreferenceGroupType.EventGroup);

                case EntityType.Product:
                    return (CoreferenceGroupType.ProductGroup);
            }
            return (null);
        }
        public static CoreferenceGroupType  ToGroupType2( this EntityType type )
        {
            var groupType = type.ToGroupType();
            if ( !groupType.HasValue )
                throw (new ArgumentException(type.ToString()));

            return (groupType.Value);
        }

        public static bool has_FN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.FN) == essence.ExistsAttributesEnum.FN);
        }
        public static bool has_LN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.LN) == essence.ExistsAttributesEnum.LN);
        }
        public static bool has_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.MN) == essence.ExistsAttributesEnum.MN);
        }
        public static bool has_FN_and_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.FN) == essence.ExistsAttributesEnum.FN &&
                    (existsAttributes & essence.ExistsAttributesEnum.MN) == essence.ExistsAttributesEnum.MN);
        }
        public static bool has_FN_and_LN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.FN) == essence.ExistsAttributesEnum.FN &&
                    (existsAttributes & essence.ExistsAttributesEnum.LN) == essence.ExistsAttributesEnum.LN);
        }
        public static bool has_FN_and_LN_and_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.FN) == essence.ExistsAttributesEnum.FN &&
                    (existsAttributes & essence.ExistsAttributesEnum.LN) == essence.ExistsAttributesEnum.LN &&
                    (existsAttributes & essence.ExistsAttributesEnum.MN) == essence.ExistsAttributesEnum.MN);
        }
        
        public static bool hasNot_FN_and_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.FN) != essence.ExistsAttributesEnum.FN &&
                    (existsAttributes & essence.ExistsAttributesEnum.MN) != essence.ExistsAttributesEnum.MN);
        }
        public static bool hasNot_LN_and_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.LN) != essence.ExistsAttributesEnum.LN &&
                    (existsAttributes & essence.ExistsAttributesEnum.MN) != essence.ExistsAttributesEnum.MN);
        }
        public static bool hasNot_FN_and_LN_and_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.FN) != essence.ExistsAttributesEnum.FN && 
                    (existsAttributes & essence.ExistsAttributesEnum.LN) != essence.ExistsAttributesEnum.LN &&
                    (existsAttributes & essence.ExistsAttributesEnum.MN) != essence.ExistsAttributesEnum.MN);
        }
        /*public static bool has_FN_or_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.FN) == essence.ExistsAttributesEnum.FN ||
                    (existsAttributes & essence.ExistsAttributesEnum.MN) == essence.ExistsAttributesEnum.MN);
        }
        public static bool has_LN_or_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.LN) == essence.ExistsAttributesEnum.LN ||
                    (existsAttributes & essence.ExistsAttributesEnum.MN) == essence.ExistsAttributesEnum.MN);
        }
        public static bool has_FN_or_LN_or_MN( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.FN) == essence.ExistsAttributesEnum.FN || 
                    (existsAttributes & essence.ExistsAttributesEnum.LN) == essence.ExistsAttributesEnum.LN ||
                    (existsAttributes & essence.ExistsAttributesEnum.MN) == essence.ExistsAttributesEnum.MN);
        }*/
        public static bool has_VALUE( this essence.ExistsAttributesEnum existsAttributes )
        {
            return ((existsAttributes & essence.ExistsAttributesEnum.VALUE) == essence.ExistsAttributesEnum.VALUE);
        }

        public static bool has_FN( this essence.InitialsEnum initials )
        {
            return ((initials & essence.InitialsEnum.FN) == essence.InitialsEnum.FN);
        }
        public static bool has_FN_and_MN( this essence.InitialsEnum initials )
        {
            return ((initials & essence.InitialsEnum.FN) == essence.InitialsEnum.FN &&
                    (initials & essence.InitialsEnum.MN) == essence.InitialsEnum.MN);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public static class GlobalConsts
    {
        public const string COREF_MASTER_ATTRIBUTE = Consts.COREF_MASTER_ATTRIBUTE;
        public const string COREF_SLAVE_ATTRIBUTE  = Consts.COREF_SLAVE_ATTRIBUTE;
    }
}
