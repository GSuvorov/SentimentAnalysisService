using System;
using System.IO;

namespace Test.OpinionMining.Web
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
                    File.WriteAllText( InputTextFileName   , this.InputText               );
                    File.WriteAllText( OutputTypeFileName  , this.OutputType.ToString()   );
                    File.WriteAllText( UseInquiryFileName  , this.UseInquiry.ToString()   );
                    File.WriteAllText( InquiryTextFileName , this.InquiryText             );
                    File.WriteAllText( ObjectAllocateMethodFileName, this.ObjectAllocateMethod.ToString() );
                }
            }
            else
            {
                if ( File.Exists( InputTextFileName ) )
                    inputText.Text = File.ReadAllText( InputTextFileName );

                if ( File.Exists( OutputTypeFileName ) )
                    outputTypeDropDownList.SelectedValue = File.ReadAllText( OutputTypeFileName );

                if ( File.Exists( InquiryTextFileName ) )
                    inquiryText.Text = File.ReadAllText( InquiryTextFileName );

                if ( File.Exists( UseInquiryFileName ) )
                {
                    var check = false;
                    if ( bool.TryParse( File.ReadAllText( UseInquiryFileName ), out check ) )
                        useInquiryHiddenField.Value = check.ToString().ToLower();
                }

                if ( File.Exists( ObjectAllocateMethodFileName ) )
                    objectAllocateMethodDropDownList.SelectedValue = File.ReadAllText( ObjectAllocateMethodFileName );
            }

            L2.ToolTip = ENDPOINT_URL;
        }

        protected string InputTextFileName
        {
            //get { return (Server.MapPath("~/App_Data") + "\\" + Session.SessionID + ".inputText.txt"); }
            get { return (Server.MapPath("~/App_Data") + "\\inputText.txt"); }
        }
        protected string OutputTypeFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\outputType.txt"); }
        }        
        protected string UseInquiryFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\useInquiry.txt"); }
        }
        protected string InquiryTextFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\inquiryText.txt"); }
        }
        protected string RuleCategoryFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\ruleCategory.txt"); }
        }
        protected string ObjectAllocateMethodFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\objectAllocateMethod.txt"); }
        }
    }
}
