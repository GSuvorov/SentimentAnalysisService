using System;
using System.Linq;
using System.Web.UI;

using Digest;
using TextMining.Core;

namespace Test.Digest.Web
{
    public partial class Result : PageBase
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            var result = default(DigestOutputResult);
            using ( var client = new DigestWcfClient( ENDPOINT_CONFIGURATION_NAME ) )
            {
                var inputParams = new DigestInputParams( this.InputText, InputTextFormat.PlainText )
                {
                    ExecuteTonalityMarking = ExecuteTonalityMarking,
                    InquiriesSynonyms      = UseInquiry ? InquiryText.ToTextList() : null,
                    ObjectAllocateMethod   = this.ObjectAllocateMethod,
                };

                result = client.ExecuteDigest( inputParams );
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

                    this.CurrentDigestOutputResult = result;
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

        #region commented
        /*private void ToXml( ref string xml )
        {
            Response.Cache.SetCacheability( HttpCacheability.NoCache );
            Response.ContentType = "text/xml";
            Response.Write( xml );
            Response.Flush();
            Response.End();
        }*/
        #endregion
    }
}
