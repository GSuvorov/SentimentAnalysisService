using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Configuration;
using System.IO;
using System.Linq;
using System.ServiceModel;
using System.Web;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;
using OpinionMining;
using TextMining.Core;

namespace Test.OpinionMining.Web
{
    public partial class Test : Page
    {
        private const string RULE_CATEGORY_KEY = "ruleCategory";
        private const string SHOWONLYERROR_KEY = "showOnlyError";
        private const string NAVIGATE_URL      = "~/Test.aspx?" + RULE_CATEGORY_KEY + "={0}&" + SHOWONLYERROR_KEY + "={1}";

        protected void Page_Load(object sender, EventArgs e)
        {
            if ( !IsPostBack )
            {
                var ruleCategory = Request[ RULE_CATEGORY_KEY ];
                if ( !string.IsNullOrWhiteSpace( ruleCategory ) )
                    ruleCategoryDropDownList.SelectedValue = ruleCategory;

                var showOnlyError = Request[ SHOWONLYERROR_KEY ];
                if ( !string.IsNullOrWhiteSpace( showOnlyError ) )
                    showOnlyErrorCheckBox.Checked = bool.Parse( showOnlyError );
            }
        }

        protected void runTestButton_Click(object sender, EventArgs e)
        {
            #region [.set controls state.]
            runTestButton        .Visible = false;
            testFileUpload1      .Visible = false;
            testFileUpload2      .Visible = false;
            showOnlyErrorCheckBox.Visible = false;
            ruleCategoryDropDownList
                                 .Visible = false;
            againHyperLink1      .Visible = true; againHyperLink1.NavigateUrl = NAVIGATE_URL.FormatEx( RuleCategory.ToString(), ShowOnlyError );
            againHyperLink2      .Visible = true; againHyperLink2.NavigateUrl = NAVIGATE_URL.FormatEx( RuleCategory.ToString(), ShowOnlyError );
            //repeatLinkButton1    .Visible = true;
            //repeatLinkButton2    .Visible = true;
            backHyperLink2       .Visible = true;
            #endregion

            #region [.read text-file.]
            var testXmlFile = new TestXmlFile( TestFileContent, RuleCategory );
            #endregion

            resultDiv.Controls.Add( new Label() { Text = ruleCategoryDropDownList.SelectedItem.Text.InBrackets() } );
            resultDiv.Controls.AddHr();

            #region [.test.]
            var errorCount = 0;
            foreach ( var rule in testXmlFile.TestRules )
            {
                try
                {
                    var result = GetOpinionMiningOutputResult( rule );

                    #region [.check.]
                    if ( result.OperateRulesNames == null )
                    {
                        throw (new TestException(rule.Text.ToHtmlBold().InSingleQuotes() +
                                                 "<br/> - не сработало ни одно правило" +
                                                 ", должно " + rule.Id.ToHtmlBold()));
                    }
                    else if ( result.OperateRulesNames.Count != 1 )
                    {
                        throw (new TestException(rule.Text.ToHtmlBold().InSingleQuotes() +
                                                 "<br/> - должно &nbsp;&nbsp;&nbsp; " + rule.Id.ToHtmlBold().InSingleQuotes() +
                                                 "<br/>,  сработало " + result.OperateRulesNames.Count + " правил(а): " + result.OperateRulesNames.ToHtml()));
                    }
                    else if ( !rule.Id.Equals( result.OperateRulesNames.First(), StringComparison.InvariantCultureIgnoreCase ) )
                    {
                        throw (new TestException(rule.Text.ToHtmlBold().InSingleQuotes() +
                                                 "<br/> - должно &nbsp;&nbsp;&nbsp; " + rule.Id.ToHtmlBold() +
                                                 "<br/>,  сработало " + result.OperateRulesNames.First().ToString().ToHtmlBold()));
                    }
                    #endregion

                    if ( ShowOnlyError )
                        continue;

                    resultDiv.Controls.AddDiv4OkMessage( rule );
                }
                catch ( Exception ex )
                {
                    #region
                    errorCount++;

                    resultDiv.Controls.AddDiv4ExceptionMessage( ex, rule );

                    if ( ex is CommunicationException )
                    {
                        resultDiv.Controls.AddDiv( "....выполниение теста прервано...." );
                        return;
                    }
                    #endregion
                }
                resultDiv.Controls.AddHr();
            }

            resultDiv.Controls.AddDiv4FinalMessage( testXmlFile, errorCount );
            #endregion
        }

        protected OpinionMiningOutputResult GetOpinionMiningOutputResult( TestRule rule )
        {
            using ( var client = new OpinionMiningWcfClient( ENDPOINT_CONFIGURATION_NAME ) )
            {
                var inputParams = new OpinionMiningInputParams( rule.Text, InputTextFormat.PlainText );
                var result = client.ExecuteOpinionMining( inputParams );

                return (result);
            }
        }
        protected string ENDPOINT_CONFIGURATION_NAME
        {
            get { return (ConfigurationManager.AppSettings[ "ENDPOINT_CONFIGURATION_NAME" ]); }
        }
        protected Stream TestFileContent
        {
            get { return (testFileUpload1.HasFile ? testFileUpload1.FileContent : (testFileUpload2.HasFile ? testFileUpload2.FileContent : null)); }
        }
        private bool ShowOnlyError
        {
            get { return (showOnlyErrorCheckBox.Checked); }
        }
        private RuleCategory RuleCategory
        {
            get { return ((RuleCategory) Enum.Parse( typeof(RuleCategory), ruleCategoryDropDownList.SelectedValue )); }
        }
    }

    //----------------------------------------------------------------//
    internal static class Extensions4Test
    {
        public static string ToHtmlBold( this string text )
        {
            return ("<span style='font-weight: bold;'>" + text + "</span>");   
        }
        public static string ToHtml( this ReadOnlyCollection< string > operateRulesNames )
        {
            return ("<br/>&nbsp;&nbsp;&nbsp;&nbsp;" + string.Join("<br/>&nbsp;&nbsp;&nbsp;&nbsp;", operateRulesNames.ToArray()).ToHtmlBold());
        }

        public static void AddDiv4ExceptionMessage( this ControlCollection controls, Exception ex, TestRule rule )
        {
            var div = new HtmlGenericControl("div") { InnerHtml = rule.ToString() + " - " + (ex is TestException ? ex.Message : ex.ToString().GetNoLongest( 250 ).ToHtmlBold()) };
            div.Style[ HtmlTextWriterStyle.Color ] = "red";

            controls.Add( div );
        }
        public static void AddDiv4OkMessage( this ControlCollection controls, TestRule rule )
        {
            controls.AddDiv( rule.ToString() + " - OK" );
        }
        public static void AddDiv4FinalMessage( this ControlCollection controls, TestXmlFile testXmlFile, int errorCount )
        {
            controls.AddDiv( "<br/>&nbsp;&nbsp;&nbsp;&nbsp;(Всего: " + testXmlFile.TestRules.Count + " штук, &nbsp;" + ((errorCount == 0) ? "Ошибок НЕТ)" : "Ошибок: " + errorCount + " штук)") );
        }
        public static void AddDiv( this ControlCollection controls, string text )
        {
            controls.Add( new HtmlGenericControl("div") { InnerHtml = text } );
        }
        public static void AddHr( this ControlCollection controls )
        {
            controls.Add( new HtmlGenericControl("hr") );
        }
    }

    public class TestException : Exception
    {
        public TestException(string message) : base(message) { }
    }

    public class TestRule
    {
        public TestRule( string id, string text )
        {
            id  .ThrowIfEmptyOrNull("id"  );
            text.ThrowIfEmptyOrNull("text");

            Id   = id;
            Text = text;
        }

        public string Id
        {
            get;
            private set;
        }

        public string Text
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return ("[Rule: " + Id /*+ " - " + Text*/ + ']');
        }
    }

    public class TestXmlFile
    {
        public TestXmlFile( Stream testFileContent, RuleCategory ruleCategory )
        {
            var xdoc = new XmlDocument();
            using ( var xtr = new XmlTextReader( testFileContent ) )
            {
                xtr.Read();
                xdoc.LoadXml( xtr.ReadOuterXml() );
            }

            var xpath = default(string);
            switch ( ruleCategory )
            {
                case RuleCategory.All:              xpath = "//example";                 break;
                case RuleCategory.IndirectSpeech:   xpath = "//IndirectSpeech//example"; break;
                case RuleCategory.DirectSpeech:     xpath = "//DirectSpeech//example";   break;
                default: throw (new ArgumentException(ruleCategory.ToString()));
            }

            TestRules = new ReadOnlyCollection< TestRule >
                (
                 (from ruleNode in xdoc.SelectNodes( xpath ).Cast< XmlNode >()
                  let text = ruleNode.InnerText
                  where !text.IsEmptyOrNull()
                  let pattern_id = ruleNode.SelectSingleNode( "./ancestor::pattern/@id" )
                  where pattern_id.IsNotNull()
                  let id = pattern_id.Value
                  where !id.IsEmptyOrNull()
                  select new TestRule( id, text )
                 ).ToList()
                );
        }

        public ReadOnlyCollection< TestRule > TestRules
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return ("Rules: " + TestRules.Count);
        }
    }
}