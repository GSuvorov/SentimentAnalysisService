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

using OpinionMining;
using TextMining.Core;

namespace Test.OpinionMining.Web
{
    public partial class Result : PageBase
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            var result = default(OpinionMiningOutputResult);
            using ( var client = new OpinionMiningWcfClient( ENDPOINT_CONFIGURATION_NAME ) )
            {
                var inputParams = new OpinionMiningInputParams( this.InputText, InputTextFormat.PlainText )
                {
                    InquiriesSynonyms    = UseInquiry ? InquiryText.ToTextList() : null,
                    ObjectAllocateMethod = this.ObjectAllocateMethod,
                };

                result = client.ExecuteOpinionMining( inputParams );
            }

            var viewInWindowText = (this.ViewInWindow ? (this.InputText + Extensions.HR) : string.Empty);

            switch ( this.OutputType )
            {
                case OutputTypeEnum.Xml:
                    resultXmlTable.Visible = true;
                    resultXmlDiv  .Visible = true; //this.ViewInWindow;
                    resultHtmlDiv .Visible = false;

                    resultXmlDiv.Style[ HtmlTextWriterStyle.Height ] =
                        (this.ViewInWindow ? (result.Tuples.Any() ? 200 : 100) : (result.Tuples.Any() ? 100 : 25)).ToString() + "px";
                    resultXmlDiv.InnerHtml = viewInWindowText +                                             
                                             result.Tuples.ToHtml( Server );

                    this.CurrentOpinionMiningOutputResult = result;
                break;

                default:
                    resultXmlTable.Visible = false;
                    resultHtmlDiv .Visible = true;

                    switch ( this.OutputType )
                    {
                        case OutputTypeEnum.Xml_Custom:
                            resultHtmlDiv.InnerHtml = viewInWindowText +
                                                      result.ToHtml( OutputType, Server );
                        break;

                        case OutputTypeEnum.Table:
                            resultHtmlDiv.InnerHtml = viewInWindowText +
                                                      result.Tuples.ToHtml( Server );
                        break;

                        case OutputTypeEnum.Table_And_Xml_Custom:
                            resultHtmlDiv.InnerHtml = viewInWindowText +
                                                      result.Tuples.ToHtml( Server ) + 
                                                      Extensions.HR + 
                                                      result.ToHtml( OutputType, Server );
                        break;

                        case OutputTypeEnum.Table_And_Xml_Custom_BySent:
                            resultHtmlDiv.InnerHtml = viewInWindowText +
                                                      result.ToHtml2( OutputType, Server );
                        break;

                        default:
                            throw (new ArgumentException(this.OutputType.ToString()));
                    }
                break;
            }
        }
    }
}
