using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Web;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Xsl;

using TonalityMarking;

using OffsetsProcessor                 = global::OffsetsBases.TonalityMarking.OffsetsProcessor;
using OffsetsString2XDocumentConverter = global::OffsetsBases.TonalityMarking.OffsetsString2XDocumentConverter;

namespace Offsets.TonalityMarking.Web
{
    internal static class Extensions
    {
        private const string XSLT_FILENAME_XML_CUSTOM        = "Xml.Digest.Offsets.xslt";
        private const string XSLT_FILENAME_OFFSETS_FLAT      = "TonalityMarking.Offsets.Flat.xslt";
        private const string XSLT_FILENAME_OFFSETS_HIERARCHY = "TonalityMarking.Offsets.Hierarchy.xslt";

        public const string HR = "<hr style='margin-left: 10px; margin-right: 10px;' />";

        public static string ToText( this byte[] bytes )
        {
            return (Encoding.UTF8.GetString( bytes ));
        }
        public static byte[] ToBytes( this string text )
        {
            return (Encoding.UTF8.GetBytes( text ));
        }
        public static List< string > ToTextList( this string text )
        {
            return (text.Split( new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries ).ToList());
        }
        public static List< byte[] > ToBytesList( this string text )
        {
            return (new[] { text.ToBytes() }.ToList());
        }
        public static string InBrackets( this string text )
        {
            if ( string.IsNullOrEmpty( text ) )
                return (text);
            return ('(' + text + ')');
        }
        public static string InSingleQuotes( this string text )
        {
            if ( string.IsNullOrEmpty( text ) )
                return (text);
            return ('\'' + text + '\'');
        }
        public static string GetNoLongest( this string value, int maxLength )
        {
            if ( string.IsNullOrEmpty( value ) || (value.Length < maxLength) )
                return (value);

            if ( maxLength <= 3 ) maxLength += 3;
            return (value.Substring( 0, Math.Min(value.Length,  maxLength - 3) ) + "...");                
        }

        public static T? TryConvert2T< T >( this string value ) where T : struct
        {
            if ( (value != null) && Enum.IsDefined( typeof(T), value ) )
            {
                return ((T) Enum.Parse( typeof(T), value ));
            }
            return (null);
        }
        public static int? TryConvert2Int( this string value )
        {
            int v;
            if ( int.TryParse( value, out v ) )
                return (v);
            return (null);
        }

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
        public static string GetAttribute( this XElement element, string name )
        {
            var a = element.Attribute( name );
            if ( a != null )
                return (a.Value);

            return (null);
        }

        public static List< int > GetOffsets( this string text, List< string > inquiriesSynonyms )
        {
            text = text.ToLower();

            var offsets = new List< int >();

            foreach ( var inquirySynonym in inquiriesSynonyms.Where( _ => !string.IsNullOrEmpty( _ ) ) )
            {
                var index = text.IndexOf( inquirySynonym.ToLower() );
                index = (index != -1 ? index : 0);
                if ( !offsets.Contains( index ) )
                {
                    offsets.Add( index );
                }
            }

            return (offsets);
        }        

        /*private static string MakeFinalTonalityXslt( this XElement sentence, HttpServerUtility server )
        {
            return (XSLT_FILENAME_FINALTONALITY.MakeXslt( sentence.ToString(), server ));
        }*/
        /*private static string MakeOffsetsXslt( this XElement sentence, HttpServerUtility server )
        {
            return (XSLT_FILENAME_OFFSETS.MakeXslt( sentence.ToString(), server ));
        }

        private static string MakeOffsetsXslt( this XDocument xdoc, HttpServerUtility server )
        {
            return (XSLT_FILENAME_OFFSETS.MakeXslt( xdoc.ToString(), server ));
        }*/
        private static string MakeXslt( this string xsltFilename, XDocument xdoc, HttpServerUtility server )
        {
            var xslt = new XslCompiledTransform( false );

            xslt.Load( server.MapPath( "~/App_Data/" + xsltFilename ) );

            using ( var ms = new MemoryStream() )
            {
                xslt.Transform( xdoc.CreateReader(), null, ms );

                return (Encoding.UTF8.GetString( ms.GetBuffer() ).Remove( 0, 1 ).ToString());
            }
        }
        private static string MakeXslt( this string xsltFilename, string text, HttpServerUtility server )
        {
            var xslt = new XslCompiledTransform( false );

            xslt.Load( server.MapPath( "~/App_Data/" + xsltFilename ) );

            var xdoc = new XmlDocument(); xdoc.LoadXml( text );

            using ( var ms = new MemoryStream() )
            {
                xslt.Transform( xdoc.CreateNavigator(), null, ms );

                return (Encoding.UTF8.GetString( ms.GetBuffer() ).Remove( 0, 1 ).ToString());
            }
        }

        public static string ToHtml( this InquiryAllocateStatus status, bool useInquiery, string inquiryText )
        {
            var text = "<span style='color: silver; font-size: X-Small;'>[{0}]</span>";
            var useInquieryText = (useInquiery ? ("('" + inquiryText + "')") : "[не задавался]");

            switch ( status )
            {
                case InquiryAllocateStatus.Allocated:
                    return (string.Format( text, "Объект мониторинга: " + useInquieryText + " найден." ));
                case InquiryAllocateStatus.NotAllocated:
                    return (string.Format(text, "Объект мониторинга: " + useInquieryText + " <span style='color: maroon;'>не найден.</span>"));
                default:
                    throw (new ArgumentException(status.ToString()));
            }
        }
        public static string ToHtml( this TonalityMarkingTuple tonalityInfo )
        {
            var text = "<span style='color: silver; font-size: X-Small;'>{0}</span>";

            var sb = new StringBuilder();
            //sb.AppendFormat( text, ",&nbsp;Негатив (" + tonalityInfo.Negative.ToTextDigit() + "): " + tonalityInfo.Negative.ToText() );
            //sb.AppendFormat( text, ",&nbsp;Позитив (" + tonalityInfo.Positive.ToTextDigit() + "): " + tonalityInfo.Positive.ToText() );

            sb.AppendFormat( text, ",&nbsp;Негатив: " + tonalityInfo.Negative.ToTextDigit() );
            sb.AppendFormat( text, ",&nbsp;Позитив: " + tonalityInfo.Positive.ToTextDigit() );

            return (sb.ToString());
        }
        private static string ToTextDigit( this double? value )
        {
            if ( !value.HasValue )
                return ("-");
            
            return (value.Value.ToString("N2"));
        }

        public static string ToHtml
            ( 
            this TonalityMarkingOutputResult result,
            OutputTypeEnum                   outputType,
            HttpServerUtility                server,
            OffsetsProcessor                 offsetsProcessor,
            string                           originalText,
            OffsetCreationXmlTypeEnum        offsetCreationXmlType
            )
        {
            var xdoc = result.OutputXml.ToXDocument(); 

            var offsetsString = offsetsProcessor.CreateOffsetsString( xdoc.Root );
            //var offsetsXml    = offsetsProcessor.CreateOffsetsString( xdoc.Root, global::OffsetsBases.TextFormat.Xml );

            var xdoc2 = default(XDocument);
            var xsltFilename = default(string);
            switch ( offsetCreationXmlType )
            {
                case OffsetCreationXmlTypeEnum.Flat:
                    xdoc2 = OffsetsString2XDocumentConverter.ConvertToFlat( originalText, offsetsString );

                    #region 
		            switch ( outputType )
                    {
                        case OutputTypeEnum.Xml_Custom:
                            xsltFilename = XSLT_FILENAME_XML_CUSTOM;
                            break;
                        case OutputTypeEnum.Html_FinalTonality:
                            xsltFilename = XSLT_FILENAME_OFFSETS_FLAT; 
                            break;
                        default:
                            throw (new ArgumentException(outputType.ToString()));
                    } 
	                #endregion
                break;

                case OffsetCreationXmlTypeEnum.Hierarchy:
                    xdoc2 = OffsetsString2XDocumentConverter.ConvertToHierarchy( originalText, offsetsString );

                    #region
                    switch ( outputType )
                    {
                        case OutputTypeEnum.Xml_Custom:
                            xsltFilename = XSLT_FILENAME_XML_CUSTOM;
                            break;
                        case OutputTypeEnum.Html_FinalTonality:
                            xsltFilename = XSLT_FILENAME_OFFSETS_HIERARCHY; 
                            break;
                        default:
                            throw (new ArgumentException(outputType.ToString()));
                    } 
	                #endregion
                break;

                default:
                        throw (new ArgumentException(offsetCreationXmlType.ToString()));
            }

            var html = xsltFilename.MakeXslt( xdoc2, server );
            return (html);
        }        
    }
}