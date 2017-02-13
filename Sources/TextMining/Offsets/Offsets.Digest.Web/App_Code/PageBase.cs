using System;
using System.Configuration;
using System.Web;
using System.Web.UI;

using TextMining.Core;
using Digest;
using OpinionMining;

namespace Offsets.Digest.Web
{
    public abstract class PageBase : Page
    {
        protected static string ENDPOINT_CONFIGURATION_NAME
        {
            get { return (ConfigurationManager.AppSettings[ "ENDPOINT_CONFIGURATION_NAME" ]); }
        }

        private const string OUTPUTTYPE             = "outputType";
        private const string INPUTTEXT              = "inputText";
        private const string USEINQUIRY             = "useInquiry";
        private const string INQUIRYTEXT            = "inquiryText";
        private const string EXECUTETONALITYMARKING = "executeTonalityMarking";
        private const string VIEWINWINDOW           = "viewInWindow";
        //private const string RULECATEGORY           = "ruleCategory";
        private const string OFFSETCREATIONXMLTYPE  = "offsetCreationXmlType";

        protected OutputTypeEnum OutputType
        {
            get
            {
                try
                {
                    var outputType = Request.Form[ OUTPUTTYPE ] ?? Convert.ToString( Session[ OUTPUTTYPE ] );

                    return ((OutputTypeEnum) Enum.Parse( typeof( OutputTypeEnum ), outputType ));
                }
                catch
                {
                    return (OutputTypeEnum.Table);
                }
            }
            set
            {
                Session_outputType = value.ToString();
            }
        }
        protected bool           ViewInWindow
        {
            get 
            {
                try
                {
                    var viewInWindow = Request.Form[ VIEWINWINDOW ] ?? 
                                       Request.QueryString[ VIEWINWINDOW ] ?? 
                                       Convert.ToString( Session[ VIEWINWINDOW ] );

                    if (viewInWindow == "on")
                        return (true);

                    return (bool.Parse( viewInWindow ));
                }
                catch
                {
                    return (true);
                }
            }
            set
            {
                Session[VIEWINWINDOW] = value;
            }
        }
        protected string         InputText
        {
            get
            {
                var text = Request.Form[ INPUTTEXT ] ?? Convert.ToString( Session[ INPUTTEXT ] );

                if ( string.IsNullOrEmpty( text ) )
                    throw (new ArgumentNullException());

                text = text.Trim();
                if ( string.IsNullOrEmpty( text ) )
                    throw (new ArgumentNullException());

                return (text);
            }
            set
            {
                Session_inputText = value;
            }
        }
        protected string         InputText4Title
        {
            get { return ((InputText.Length > 50) ? (InputText.Substring( 0, 47 ) + "...") : InputText); }
        }
        protected bool           UseInquiry
        {
            get
            {
                try
                {
                    var useInquiry = Request.Form[ USEINQUIRY ] ?? 
                                     Request.QueryString[ USEINQUIRY ] ?? 
                                     Convert.ToString( Session[ USEINQUIRY ] );

                    if (useInquiry == "on")
                        return (true);

                    return (bool.Parse( useInquiry ));
                }
                catch
                {
                    return (!string.IsNullOrEmpty( InquiryText ));
                }
            }
            set
            {
                Session_useInquiry = value.ToString();
            }
        }
        protected string         InquiryText
        {
            get
            {
                var inquiryText = Request.Form[ INQUIRYTEXT ] ?? 
                                  Request.QueryString[ INQUIRYTEXT ] ?? 
                                  Convert.ToString( Session[ INQUIRYTEXT ] );

                return (inquiryText);
            }
            set
            {
                Session_inquiryText = value;
            }
        }
        protected bool           ExecuteTonalityMarking
        {
            get
            {
                try
                {
                    var executeTonalityMarking = Request.Form[EXECUTETONALITYMARKING] ?? 
                                                 Request.QueryString[ EXECUTETONALITYMARKING ] ?? 
                                                 Convert.ToString( Session[ EXECUTETONALITYMARKING ] );
                    if ( executeTonalityMarking == "on" ) 
                        return (true);

                    return (bool.Parse( executeTonalityMarking ));
                }
                catch
                {
                    return (!string.IsNullOrEmpty( InquiryText ));
                }
            }
            set
            {
                Session_executeTonalityMarking = value.ToString();
            }
        }
        /*protected RuleCategory   RuleCategory
        {
            get
            {
                try
                {
                    var ruleCategory = Request.Form[ RULECATEGORY ] ?? Convert.ToString( Session[ RULECATEGORY ] );

                    return ((RuleCategory) Enum.Parse( typeof( RuleCategory ), ruleCategory ));
                }
                catch
                {
                    return (RuleCategory.All);
                }
            }
            set
            {
                Session_ruleCategory = value.ToString();
            }
        }*/
        protected OffsetCreationXmlTypeEnum OffsetCreationXmlType
        {
            get
            {
                try
                {
                    var offsetCreationXmlType = Request.Form[ OFFSETCREATIONXMLTYPE ] ?? Convert.ToString( Session[ OFFSETCREATIONXMLTYPE ] );

                    return ((OffsetCreationXmlTypeEnum) Enum.Parse( typeof( OffsetCreationXmlTypeEnum ), offsetCreationXmlType ));
                }
                catch
                {
                    return (OffsetCreationXmlTypeEnum.Flat);
                }
            }
            set
            {
                Session_offsetCreationXmlType = value.ToString();
            }
        }

        protected DigestOutputResult CurrentDigestOutputResult
        {
            get { return ((DigestOutputResult) Session[ "DigestOutputResult" ]); }
            set 
            {
                Session.Remove( "DigestOutputResult" );
                if ( value != null )
                    Session[ "DigestOutputResult" ] = value; 
            }
        }
        protected OpinionMiningOutputResult CurrentOpinionMiningOutputResult
        {
            get { return ((OpinionMiningOutputResult) Session[ "OpinionMiningOutputResult" ]); }
            set 
            {
                Session.Remove( "OpinionMiningOutputResult" );
                if ( value != null )
                    Session[ "OpinionMiningOutputResult" ] = value; 
            }
        }

        protected bool IsCallAxaj
        {
            get { return (Request.QueryString[ "CallAxaj" ] == "true"); }
        }
        protected void SaveRequestParams2Property()
        {
            Session_inputText              = Request_inputText;
            Session_outputType             = Request_outputType;
            Session_useInquiry             = Request_useInquiry;
            Session_inquiryText            = Request_inquiryText;
            Session_executeTonalityMarking = Request_executeTonalityMarking;
            Session_offsetCreationXmlType  = Request_offsetCreationXmlType;
        }

        private string Request_inputText
        {
            get { return (Request.Form[INPUTTEXT]); }
        }
        private string Request_outputType
        {
            get { return (Request.Form[OUTPUTTYPE]); }
        }
        private string Request_useInquiry
        {
            get { return (Request.Form[USEINQUIRY]); }
        }
        private string Request_inquiryText
        {
            get { return (Request.Form[INQUIRYTEXT]); }
        }
        private string Request_executeTonalityMarking
        {
            get { return (Request.Form[ EXECUTETONALITYMARKING ]); }
        }
        private string Request_offsetCreationXmlType
        {
            get { return (Request.Form[ OFFSETCREATIONXMLTYPE ]); }
        }

        private string Session_inputText
        {
            set { Session[ INPUTTEXT ] = value; }
        }
        private string Session_outputType
        {
            set { Session[OUTPUTTYPE] = value; }
        }
        private string Session_useInquiry
        {
            set { Session[USEINQUIRY] = value; }
        }
        private string Session_inquiryText
        {
            set { Session[INQUIRYTEXT] = value; }
        }
        private string Session_executeTonalityMarking
        {
            set { Session[EXECUTETONALITYMARKING] = value; }
        }
        /*private string Session_ruleCategory
        {
            set { Session[ RULECATEGORY ] = value; }
        }*/
        private string Session_offsetCreationXmlType
        {
            set { Session[ OFFSETCREATIONXMLTYPE ] = value; }
        }

        protected void XmlToResponse< T >( OutputResultTupleBase< T > result ) where T : OpinionMiningTuple
        {
            if ( result == null ) throw (new ArgumentNullException("result"));

            Response.Cache.SetCacheability( HttpCacheability.NoCache );
            Response.ContentType = "text/xml";
            Response.Write( result.OutputXml );
            Response.Flush();
            Response.End();
        }
    }
}