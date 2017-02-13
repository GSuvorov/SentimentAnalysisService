using System;
using System.Linq;
using System.Web.UI;

using TextMining.Core;
using TonalityMarking;
using OffsetsProcessor = global::OffsetsBases.TonalityMarking.OffsetsProcessor;

namespace Offsets.TonalityMarking.Web
{
    public partial class Result : PageBase
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            var result = default(TonalityMarkingOutputResult);
            using ( var client = new TonalityMarkingWcfClient( ENDPOINT_CONFIGURATION_NAME ) )
            {
                var inputParams = new TonalityMarkingInputParams( this.InputText )
                {
                    InquiriesSynonyms = UseInquiry ? InquiryText.ToTextList() : null,
                };

                result = client.ExecuteTonalityMarking( inputParams );
            }

            var originalText = this.InputText;
            var offsetsProcessor = new OffsetsProcessor( /*this.InputText*/ );

            //var viewInWindowText = (this.ViewInWindow ? (this.InputText + Extensions.HR) : string.Empty);

            resultHtmlDiv.InnerHtml = result.InquiryAllocateStatus.ToHtml( this.UseInquiry, this.InquiryText ) +
                                      result.AuthorTonalityMarkingTuple.ToHtml() +
                                      Extensions.HR + 
                                      ((this.OutputType == OutputTypeEnum.Xml_Custom && this.ViewInWindow) ? (this.InputText + "<hr/>") : string.Empty) +
                                      result.ToHtml( OutputType, Server, offsetsProcessor, originalText, OffsetCreationXmlType );
        }
    }
}
