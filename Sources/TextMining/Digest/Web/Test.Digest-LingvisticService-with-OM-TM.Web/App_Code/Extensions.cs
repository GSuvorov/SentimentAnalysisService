using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Web;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Xsl;

using OpinionMining;
using TextMining.Core;

namespace Test
{
    internal static class Extensions
    {
        private const string XSLT_FILENAME_FINALTONALITY = "FinalTonality.Digest.test.xslt";
        private const string XSLT_FILENAME_XML_CUSTOM    = "Xml.Digest.xslt";

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

        private static string MakeFinalTonalityXslt( this XElement sentence, HttpServerUtility server )
        {
            return (XSLT_FILENAME_FINALTONALITY.MakeXslt( sentence.ToString(), server ));
        }
        private static string MakeXslt( this string xsltFilename, ref string text, HttpServerUtility server )
        {
            var xslt = new XslCompiledTransform( false );

            xslt.Load( server.MapPath( "~/App_Data/" + xsltFilename ) );

            var xdoc = new XmlDocument(); xdoc.LoadXml( text );

            using ( var ms = new MemoryStream() )
            {
                xslt.Transform( xdoc.CreateNavigator(), null /*xsltArg*/, ms );

                return (Encoding.UTF8.GetString( ms.GetBuffer() ).Remove( 0, 1 ).ToString());
            }
        }
        private static string MakeXslt( this string xsltFilename, string text, HttpServerUtility server )
        {
            return (xsltFilename.MakeXslt( ref text, server ));
        }
        private static string MakeXslt( this string text, OutputTypeEnum outputType, HttpServerUtility server )
        {
            switch ( outputType )
            {
                case OutputTypeEnum.Xml_Custom:
                case OutputTypeEnum.Table:
                case OutputTypeEnum.Table_And_Xml_Custom:
                case OutputTypeEnum.Table_And_Xml_Custom_BySent:
                    return (XSLT_FILENAME_XML_CUSTOM.MakeXslt( ref text, server ));

                default:
                    throw (new ArgumentException(outputType.ToString()));
            }            
        }

        private static string ToHtml( this SubjectEssence subject )
        {
            return (subject.IsAuthor ? "<span style='color: silver;'>{0}</span>" : "{0}").FormatEx( subject.ToString() );
        }
        private static string ToHtml( this ObjectEssence @object )
        {
            return ((@object != null) ? (@object + ((@object.IsSubjectIndeed) ? "<span style='color: silver;'>&nbsp;(субъект-как-объект)</span>" : string.Empty)) : "-");
        }

        private const string ANYTHING_ISNT_PRESENT = "<span style='color: maroon; font-size: X-Small;'>[Ничего нет.]</span>";
        private const string TABLE_START           = "<table border='1' style='font-size: X-Small;'><tr><th>#</th><th>SUBJECT</th><th>OBJECT</th><th>SENTENCE</th></tr>";
        private const string TABLE_END             = "</table>";
        private const string TABLE_ROW_FORMAT      = "<tr valign='top'><td>{0}</td><td>&nbsp;{1}</td><td>&nbsp;{2}</td><td style='padding: 3px;'>{3}</td></tr>";

        public static string ToHtml< T >( this OutputResultTupleBase< T > result, OutputTypeEnum outputType, HttpServerUtility server )
            where T : OpinionMiningTuple
        {
            return (result.OutputXml.MakeXslt( outputType, server ));
        }
        public static string ToHtml2< T >( this OutputResultTupleBase< T > result, OutputTypeEnum outputType, HttpServerUtility server )
            where T : OpinionMiningTuple
        {
            if ( !result.Tuples.Any() )
                return (ANYTHING_ISNT_PRESENT);

            var sbGlobal = new StringBuilder();
            var number = 0;
            foreach ( var tuple in result.Tuples )
            {
                //1.
                var sb = new StringBuilder( TABLE_START );
                sb.AppendFormat
                (
                TABLE_ROW_FORMAT, 
                (++number),
                tuple.Subject.ToHtml(), 
                tuple.Object .ToHtml(),
                tuple.GetSentence().MakeFinalTonalityXslt( server )
                );
                sb.Append( TABLE_END );

                //2.
                sb.Append( tuple.GetSentence().ToString().MakeXslt( outputType, server ) );

                //3.
                sbGlobal.Append( sb.ToString() );
            }            

            return (sbGlobal.ToString());
        }
        public static string ToHtml( this IEnumerable< OpinionMiningTuple > opinionMiningTuples, HttpServerUtility server )
        {
            if ( !opinionMiningTuples.Any() )
                return (ANYTHING_ISNT_PRESENT);

            var sb = new StringBuilder( TABLE_START );
            var number = 0;
            foreach ( var tuple in opinionMiningTuples )
            {
                sb.AppendFormat
                (
                TABLE_ROW_FORMAT,
                (++number),
                tuple.Subject.ToHtml(), 
                tuple.Object .ToHtml(),
                tuple.GetSentence().MakeFinalTonalityXslt( server ) 
                );
            }
            sb.Append( TABLE_END );

            return (sb.ToString());
        }
    }
}