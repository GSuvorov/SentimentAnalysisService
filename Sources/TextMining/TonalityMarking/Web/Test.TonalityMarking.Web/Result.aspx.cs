using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Xsl;

using TonalityMarking;
using TextMining.Core;

namespace Test.TonalityMarking.Web
{
    /// <summary>
    /// 
    /// </summary>
    public partial class Result : Page
    {
        private enum OutputTypeEnum
        {
            Xml,
            Xml_Custom,
            Html_FinalTonality,
            Html_FinalTonalityDividedSentence,
            Html_ToplevelTonality,
            Html_ToplevelTonalityDividedSentence,
            Html_BackcolorAllTonality,
        }

        protected void Page_Load(object sender, EventArgs e)
        {
            var result = default(TonalityMarkingOutputResult);
            using ( var client = new TonalityMarkingWcfClient( ENDPOINT_CONFIGURATION_NAME ) )
            {
                var inputParams = new TonalityMarkingInputParams( this.InputText )
                {
                    InquiriesSynonyms    = this.UseInquiry ? this.InquiryText.ToTextList() : null,
                    ObjectAllocateMethod = this.ObjectAllocateMethod,
                };

                result = client.ExecuteTonalityMarking( inputParams );
            }

            #region [.show-sent-when-frt-more-zero|one.]
            if ( ShowSentWhenFrtMoreZero || ShowSentWhenFrtMoreOne )
            {
                const string SENT     = "SENT";
                const string FRTFINAL = "FRTFINAL";

                var frt_threshold = ShowSentWhenFrtMoreOne ? 1 : 0;

                var xdoc = result.GetOutputXDocument();
                //1.
                var sents = (from sent in xdoc.Descendants( SENT )
                             let x = sent.Descendants().FirstOrDefault( d => d.HasAttribute( FRTFINAL ) && (frt_threshold < d.GetAttributeFRTFINAL()) )
                             where x.IsNull()
                             select sent
                            )
                            .ToArray();
                foreach ( var s in sents )
                {
                    s.Remove();
                }

                //2.
                var dd = (from sent in xdoc.Descendants( SENT )
                          let _d = sent.Descendants().FirstOrDefault( d => d.HasAttribute( FRTFINAL ) && (frt_threshold < d.GetAttributeFRTFINAL()) )
                          where _d.IsNotNull()
                          select _d
                         )
                         .ToArray();
                foreach ( var d in dd )
                {
                    var u = new XElement( FRTFINAL )
                    {
                        Value = " [FRTFINAL: " + d.GetAttributeFRTFINAL() + "] ",
                    };
                    d.AddAfterSelf( u );
                }

                result = new TonalityMarkingOutputResult
                             (
                                xdoc,
                                //result.OutputXmlFormat,
                                result.InquiryAllocateStatus,
                                result.AuthorTonalityMarkingTuple.Positive,
                                result.AuthorTonalityMarkingTuple.Negative,
                                result.AuthorTonalityMarkingTuple.Objects.ToList(),
                                result.OperateRulesNames
                             );
            }            
            #endregion

            #region [.out-put.]
            switch ( this.OutputType )
            {
                case OutputTypeEnum.Xml:
                    resultXmlTable.Visible = true;
                    resultXmlDiv  .Visible = true; //this.ViewInWindow;
                    resultHtmlDiv .Visible = false;

                    resultXmlDiv.Style[ HtmlTextWriterStyle.Height ] = (this.ViewInWindow ? 100 : 25).ToString() + "px";
                    resultXmlDiv.InnerHtml = result.InquiryAllocateStatus.ToHtml( this.UseInquiry, this.InquiryText ) +
                                             result.AuthorTonalityMarkingTuple.ToHtml() +
                                             Extensions.HR + 
                                             (this.ViewInWindow ? this.InputText : string.Empty);

                    this.TonalityMarkingOutputResult = result;
                break;

                default:
                    resultXmlTable.Visible = false;
                    resultHtmlDiv .Visible = true;

                    resultHtmlDiv.InnerHtml = result.InquiryAllocateStatus.ToHtml( this.UseInquiry, this.InquiryText ) +
                                              result.AuthorTonalityMarkingTuple.ToHtml() +
                                              Extensions.HR + 
                                              ((this.OutputType == OutputTypeEnum.Xml_Custom && this.ViewInWindow) ? (this.InputText + "<hr/>") : string.Empty) +
                                              ToHtml( result, OutputType );
                break;
            }            
            #endregion
        }

        private string ToHtml( TonalityMarkingOutputResult result, OutputTypeEnum outputType )
        {
            var xdoc = new XmlDocument(); 
            xdoc.LoadXml( result.OutputXml ); //.ToText() );

            var xslt = new XslCompiledTransform( false );

            var xsltFilename = default(string);
            switch ( outputType )
            {
                case OutputTypeEnum.Xml_Custom:
                    xsltFilename = "Xml.xslt";
                    break;
                case OutputTypeEnum.Html_ToplevelTonality:
                    xsltFilename = "ToplevelTonality.xslt"; 
                    break;
                case OutputTypeEnum.Html_ToplevelTonalityDividedSentence:
                    xsltFilename = "ToplevelTonalityDividedSentence.xslt";
                    break;
                case OutputTypeEnum.Html_FinalTonality:
                    xsltFilename = "FinalTonality.xslt"; 
                    break;
                case OutputTypeEnum.Html_FinalTonalityDividedSentence:
                    xsltFilename = "FinalTonalityDividedSentence.xslt";
                    break;
                case OutputTypeEnum.Html_BackcolorAllTonality:
                    xsltFilename = "BackcolorAllTonality.xslt"; 
                    break;
                default:
                    throw (new ArgumentException(outputType.ToString()));
            }

            xslt.Load( Server.MapPath( "~/App_Data/" + xsltFilename ) );

            using ( var ms = new MemoryStream() )
            {
                xslt.Transform( xdoc.CreateNavigator(), null /*xsltArg*/, ms );

                return (Encoding.UTF8.GetString( ms.GetBuffer() ).Remove( 0, 1 ).ToString());
            }
        }

        private OutputTypeEnum OutputType
        {
            get 
            {
                try
                {
                    var outputType = Request.Form[ "outputType" ] ?? Convert.ToString( Session[ "outputType" ] );

                    return ((OutputTypeEnum) Enum.Parse( typeof( OutputTypeEnum ), outputType ));
                }
                catch
                {
                    return (OutputTypeEnum.Xml);
                }
            }
        }
        private ObjectAllocateMethod ObjectAllocateMethod
        {
            get 
            {
                try
                {
                    var objectAllocateMethod = Request.Form[ "objectAllocateMethod" ] ?? Convert.ToString( Session[ "objectAllocateMethod" ] );

                    return ((ObjectAllocateMethod) Enum.Parse( typeof( ObjectAllocateMethod ), objectAllocateMethod ));
                }
                catch
                {
                    return (ObjectAllocateMethod.FirstVerbEntityWithRoleObj);
                }
            }
        }
        private bool           ViewInWindow
        {
           get 
            {
                try
                {
                    var viewInWindow = Request.Form[ "viewInWindow" ] ?? 
                                       Request.QueryString[ "viewInWindow" ] ?? 
                                       Convert.ToString( Session[ "viewInWindow" ] );

                    return (bool.Parse( viewInWindow ));
                }
                catch
                {
                    return (true);
                }
            }
        }
        protected string       InputText
        {
            get
            {
                var text = Request.Form[ "inputText" ] ?? Convert.ToString( Session[ "inputText" ] );

                if ( string.IsNullOrEmpty( text ) )
                    throw (new ArgumentNullException());

                text = text.Trim();
                if ( string.IsNullOrEmpty( text ) )
                    throw (new ArgumentNullException());

                return (text);
            }
        }
        protected string       InputText4Title
        {
            get { return ((InputText.Length > 50) ? (InputText.Substring( 0, 47 ) + "...") : InputText); }
        }
        private bool           UseInquiry
        {
            get
            {
                try
                {
                    var useInquiry = Request.Form[ "useInquiry" ] ?? 
                                       Request.QueryString[ "useInquiry" ] ?? 
                                       Convert.ToString( Session[ "useInquiry" ] );

                    return (bool.Parse( useInquiry ));
                }
                catch
                {
                    return (!string.IsNullOrEmpty( InquiryText ));
                }
            }
        }
        private string         InquiryText
        {
            get
            {
                var inquiryText = Request.Form[ "inquiryText" ] ?? 
                                  Request.QueryString[ "inquiryText" ] ?? 
                                  Convert.ToString( Session[ "inquiryText" ] );

                return (inquiryText);
            }
        }
        private bool           ShowSentWhenFrtMoreZero
        {
            get
            {
                try
                {
                    var showSentWhenFrtMoreZero = Request.Form["showSentWhenFrtMoreZero"] ?? 
                                                  Request.QueryString[ "showSentWhenFrtMoreZero" ] ?? 
                                                  Convert.ToString( Session[ "showSentWhenFrtMoreZero" ] );

                    return (bool.Parse( showSentWhenFrtMoreZero ));
                }
                catch
                {
                    return (false);
                }
            }
        }
        private bool           ShowSentWhenFrtMoreOne
        {
            get
            {
                try
                {
                    var showSentWhenFrtMoreOne = Request.Form[ "showSentWhenFrtMoreOne" ] ??
                                                 Request.QueryString[ "showSentWhenFrtMoreOne" ] ?? 
                                                 Convert.ToString( Session[ "showSentWhenFrtMoreOne" ] );

                    return (bool.Parse( showSentWhenFrtMoreOne ));
                }
                catch
                {
                    return (false);
                }
            }
        }

        private TonalityMarkingOutputResult TonalityMarkingOutputResult
        {
            get { return ((TonalityMarkingOutputResult) Session[ "TonalityMarkingOutputResult" ]); }
            set 
            {
                Session.Remove( "TonalityMarkingOutputResult" );
                if ( value != null )
                    Session[ "TonalityMarkingOutputResult" ] = value; 
            }
        }

        protected string ENDPOINT_CONFIGURATION_NAME
        {
            get { return (ConfigurationManager.AppSettings[ "ENDPOINT_CONFIGURATION_NAME" ]); }
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal static class Extensions
    {
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

        public const string HR = "<hr style='margin-left: 10px; margin-right: 10px;' />";

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
        private static string ToText( this double? value )
        {
            /*Диапазон значений определения негатива/позитива по тексту
                Если 0<=Frt<=1 – слабый негатив / позитив (было 0<=Frt<1)
                Если 1<Frt<1,5 – средний негатив / позитив (было 1<=Frt<1,5)
                Если 1,5<=Frt  – сильный негатив / позитив
            */

            if ( !value.HasValue )
                return ("отсутствует");
            
            if ( 0 <= value.Value && value.Value <= 1 )
                return ("слабый");
            if ( 1 < value.Value && value.Value < 1.5 )
                return ("средний");

            return ("сильный");
        }
        private static string ToTextDigit( this double? value )
        {
            if ( !value.HasValue )
                return ("-");
            
            return (value.Value.ToString("N2"));
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
    }
}
