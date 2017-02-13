using System;
using System.Linq;
using System.Web.UI;

using TextMining.Core;
using Digest;
using OffsetsProcessor = global::OffsetsBases.Digest.OffsetsProcessor;

namespace Offsets.Digest.Web
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
                };

                result = client.ExecuteDigest( inputParams );
            }

            var originalText = this.InputText;
            var offsetsProcessor = new OffsetsProcessor( /*this.InputText*/ );

            var viewInWindowText = (this.ViewInWindow ? (this.InputText + Extensions.HR) : string.Empty);

            switch ( this.OutputType )
            {
                case OutputTypeEnum.Table:
                    resultHtmlDiv.InnerHtml = viewInWindowText +
                                              result.Tuples.ToHtmlTable( Server, offsetsProcessor, originalText, OffsetCreationXmlType );
                break;

                case OutputTypeEnum.Table_And_Xml_Custom_BySent:
                    resultHtmlDiv.InnerHtml = viewInWindowText +
                                              result.ToHtmlTableAndXmlCustomBySent( OutputType, Server, offsetsProcessor, originalText, OffsetCreationXmlType );
                break;

                default:
                    throw (new ArgumentException(this.OutputType.ToString()));
            }
        }
    }
}
