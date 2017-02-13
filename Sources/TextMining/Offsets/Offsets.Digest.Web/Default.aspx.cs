using System;
using System.IO;

namespace Offsets.Digest.Web
{
    public partial class _Default : PageBase
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if ( this.IsCallAxaj )
            {
                this.SaveRequestParams2Property();             

                lock ( typeof(_Default) )
                {
                    File.WriteAllText( InputTextFileName             , this.InputText                         );
                    File.WriteAllText( UseInquiryFileName            , this.UseInquiry            .ToString() );
                    File.WriteAllText( InquiryTextFileName           , this.InquiryText                       );
                    File.WriteAllText( ExecuteTonalityMarkingFileName, this.ExecuteTonalityMarking.ToString() );
                    File.WriteAllText( OutputTypeFileName            , this.OutputType            .ToString() );
                    File.WriteAllText( OffsetCreationXmlTypeFileName , this.OffsetCreationXmlType .ToString() );
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

                if ( File.Exists( ExecuteTonalityMarkingFileName ) )
                {
                    var check = false;
                    if ( bool.TryParse( File.ReadAllText( ExecuteTonalityMarkingFileName ), out check ) )
                        executeTonalityMarkingHiddenField.Value = check.ToString().ToLower();
                }

                if ( File.Exists( OutputTypeFileName ) )
                    outputTypeDropDownList.SelectedValue = File.ReadAllText( OutputTypeFileName );

                if ( File.Exists( OffsetCreationXmlTypeFileName ) )
                    offsetCreationXmlTypeDropDownList.SelectedValue = File.ReadAllText( OffsetCreationXmlTypeFileName );
            }
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
        protected string ExecuteTonalityMarkingFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\executeTonalityMarking.txt"); }
        }
        protected string OutputTypeFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\outputType.txt"); }
        }
        protected string OffsetCreationXmlTypeFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\offsetCreationXmlType.txt"); }
        }   
    }
}
