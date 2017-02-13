using System;
using System.Configuration;
using System.IO;
using System.Web.UI;
using System.Xml.Linq;
using System.Xml.XPath;

namespace Test.TonalityMarking.Web
{
    /// <summary>
    /// 
    /// </summary>
    public partial class _Default : Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if ( Request.QueryString[ "CallAxaj" ] == "true" )
            {
                var inputText            = Request.Form[ "inputText"    ];
                var outputType           = Request.Form[ "outputType"   ];
                var objectAllocateMethod = Request.Form[ "objectAllocateMethod" ];
                var useInquiry           = Request.Form[ "useInquiry"   ];
                var inquiryText          = Request.Form[ "inquiryText"  ];
                var showSentWhenFrtMoreZero = Request.Form[ "showSentWhenFrtMoreZero" ];
                var showSentWhenFrtMoreOne  = Request.Form[ "showSentWhenFrtMoreOne"  ];
                var lingvistic_ProcessText_MethodName = Request.Form[ "Lingvistic_ProcessText_MethodName" ];

                Session[ "inputText"               ] = inputText;
                Session[ "outputType"              ] = outputType;
                Session[ "objectAllocateMethod"    ] = objectAllocateMethod;
                Session[ "useInquiry"              ] = useInquiry;
                Session[ "inquiryText"             ] = inquiryText;
                Session[ "showSentWhenFrtMoreZero" ] = showSentWhenFrtMoreZero;
                Session[ "showSentWhenFrtMoreOne"  ] = showSentWhenFrtMoreOne;
                Session[ "Lingvistic_ProcessText_MethodName" ] = lingvistic_ProcessText_MethodName;

                lock ( typeof(_Default) )
                {
                    File.WriteAllText( InputTextFileName   , inputText    );
                    File.WriteAllText( UseInquiryFileName  , useInquiry   );
                    File.WriteAllText( InquiryTextFileName , inquiryText  );
                    File.WriteAllText( OutputTypeFileName  , outputType   );
                    File.WriteAllText( ObjectAllocateMethodFileName   , objectAllocateMethod    );
                    File.WriteAllText( ShowSentWhenFrtMoreZeroFileName, showSentWhenFrtMoreZero );
                    File.WriteAllText( ShowSentWhenFrtMoreOneFileName , showSentWhenFrtMoreOne  );
                }
            }
            else
            {
                if ( File.Exists( InputTextFileName ) )
                    inputText.Text = File.ReadAllText( InputTextFileName );

                if ( File.Exists( InquiryTextFileName ) )
                    inquiryText.Text = File.ReadAllText( InquiryTextFileName );

                if ( File.Exists( UseInquiryFileName ) )
                {
                    var check = false;
                    if ( bool.TryParse( File.ReadAllText( UseInquiryFileName ), out check ) )
                        useInquiryHiddenField.Value = check.ToString().ToLower();
                }

                if ( File.Exists( OutputTypeFileName ) )
                    outputTypeDropDownList.SelectedValue = File.ReadAllText( OutputTypeFileName );

                if ( File.Exists( ObjectAllocateMethodFileName ) )
                    objectAllocateMethodDropDownList.SelectedValue = File.ReadAllText( ObjectAllocateMethodFileName );

                if ( File.Exists( ShowSentWhenFrtMoreZeroFileName ) )
                {
                    var check = false;
                    if ( bool.TryParse( File.ReadAllText( ShowSentWhenFrtMoreZeroFileName ), out check ) )
                        showSentWhenFrtMoreZeroHiddenField.Value = check.ToString().ToLower();
                }

                if ( File.Exists( ShowSentWhenFrtMoreOneFileName ) )
                {
                    var check = false;
                    if ( bool.TryParse( File.ReadAllText( ShowSentWhenFrtMoreOneFileName ), out check ) )
                        showSentWhenFrtMoreOneHiddenField.Value = check.ToString().ToLower();
                }
            }

            L2.ToolTip = ENDPOINT_URL;
        }

        protected string InputTextFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\inputText.txt"); }
        }
        protected string UseInquiryFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\useInquiry.txt"); }
        }
        protected string InquiryTextFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\inquiryText.txt"); }
        }
        protected string OutputTypeFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\outputType.txt"); }
        }
        protected string ObjectAllocateMethodFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\objectAllocateMethod.txt"); }
        }        
        protected string ShowSentWhenFrtMoreZeroFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\showSentWhenFrtMoreZero.txt"); }
        }
        protected string ShowSentWhenFrtMoreOneFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\showSentWhenFrtMoreOne.txt"); }
        }        

        protected string ENDPOINT_URL
        {
            get 
            {
                var epn = ConfigurationManager.AppSettings[ "ENDPOINT_CONFIGURATION_NAME" ];

                var path = Path.Combine( Server.MapPath("~/"), "Web.config" );
                using ( var sr = new StreamReader( path ) )
                {
                    var xd = XDocument.Load( sr );

                    var xe = xd.XPathSelectElement( "/configuration/system.serviceModel/client/endpoint[ @bindingConfiguration = '" + epn + "' ]" );
                    return (xe.Attribute( "address" ).Value);
                }
            }
        }
    }
}
