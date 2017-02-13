using System;
using System.Diagnostics;
using System.Linq;
using System.Web.UI;

using Lingvistics.Client;

namespace Test.Digest.Web
{
    /// <summary>
    /// 
    /// </summary>
    public partial class Result : PageBase
    {
        protected void Page_Load( object sender, EventArgs e )
        {
            var sw = Stopwatch.StartNew();
            var inputText = this.InputText;
            var lingvisticResult = _LingvisticsServer.Value.ProcessText
                                                (
                                                inputText, 
                                                false, 
                                                DateTime.Now,
                                                LingvisticsResultOptions.OpinionMiningWithTonality, 
                                                SelectEntitiesMode.Full,
                                                false
                                                );
            sw.Stop();

            var result = lingvisticResult.OpinionMiningWithTonalityResult;

            var viewInWindowText = (this.ViewInWindow ? (inputText + Extensions.HR) : string.Empty);
            var elapsedHtml = "<i style='font-size: 9pt;'>elapsed: " + sw.Elapsed + ", input-text-length: " + inputText.Length + "</i><hr style='margin: 3px 0 3px 0;' />";

            switch ( this.OutputType )
            {
                case OutputTypeEnum.Xml:
                    resultXmlTable.Visible = true;
                    resultXmlDiv  .Visible = true; //this.ViewInWindow;
                    resultHtmlDiv .Visible = false;

                    resultXmlDiv.Style[ HtmlTextWriterStyle.Height ] =
                        (this.ViewInWindow ? (result.Tuples.Any() ? 200 : 100) : (result.Tuples.Any() ? 100 : 25)).ToString() + "px";
                    resultXmlDiv.InnerHtml = elapsedHtml +
                                             viewInWindowText +                                             
                                             result.Tuples.ToHtml( Server );

                    this.CurrentDigestOutputResult = result;
                break;

                default:
                    resultXmlTable.Visible = false;
                    resultHtmlDiv .Visible = true;

                    switch ( this.OutputType )
                    {
                        case OutputTypeEnum.Xml_Custom:
                            resultHtmlDiv.InnerHtml = elapsedHtml +
                                                      viewInWindowText +
                                                      result.ToHtml( OutputType, Server );
                        break;

                        case OutputTypeEnum.Table:
                            resultHtmlDiv.InnerHtml = elapsedHtml +
                                                      viewInWindowText +
                                                      result.Tuples.ToHtml( Server );
                        break;

                        case OutputTypeEnum.Table_And_Xml_Custom:
                            resultHtmlDiv.InnerHtml = elapsedHtml +
                                                      viewInWindowText +
                                                      result.Tuples.ToHtml( Server ) + 
                                                      Extensions.HR + 
                                                      result.ToHtml( OutputType, Server );
                        break;

                        case OutputTypeEnum.Table_And_Xml_Custom_BySent:
                            resultHtmlDiv.InnerHtml = elapsedHtml +
                                                      viewInWindowText +
                                                      result.ToHtml2( OutputType, Server );
                        break;

                        default:
                            throw (new ArgumentException( this.OutputType.ToString() ));
                    }
                break;
            }
        }

        #region commented
        /*private void ToXml( string xml )
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
