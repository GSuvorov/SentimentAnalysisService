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
using Digest;

namespace Test.Digest.Web
{
    public partial class Test : System.Web.UI.Page
    {
        //protected void Page_Load(object sender, EventArgs e)
        //{
        //}
        protected void runTestButton_Click(object sender, EventArgs e)
        {
            #region [.set controls state.]
            runTestButton        .Visible = false;
            testFileUpload1      .Visible = false;
            testFileUpload2      .Visible = false;
            showOnlyErrorCheckBox.Visible = false;
            againHyperLink1      .Visible = true;
            againHyperLink2      .Visible = true;
            //repeatLinkButton1    .Visible = true;
            //repeatLinkButton2    .Visible = true;
            backHyperLink2       .Visible = true;
            #endregion

            #region [.read text-file.]
            var testXmlFile = new TestXmlFile( TestFileContent );
            #endregion

            #region [.test.]
            var errorCount = 0;
            foreach ( var rule in testXmlFile.TestRules )
            {
                foreach ( var sent in rule.TestSents )
                {
                    try
                    {
                        var result = GetDigestOutputResult( sent );

                        #region [check SNTFINAL.]
                        var subsentNodes = result.OutputXml.ToXDocument().XPathSelectElements( "//SUB_SENT" ).ToArray(); // || "TEXT/SENT/SUB_SENT"
                        if ( sent.SNTFINALs.Count != subsentNodes.Length )
                        {
                            throw (new TestException("Число подпредложений в строке не равно числу атрибутов-@SNTFINAL в тестовом файле."));
                        }
                        for ( var i = 0; i < sent.SNTFINALs.Count; i++ )
                        {
                            var snt1 = subsentNodes[ i ].GetAttributeSNTFINAL();
                            var snt2 = sent.SNTFINALs[ i ];
                            if ( snt1 != snt2 )
                            {
                                throw (new TestException("Подпредложение "          + subsentNodes[ i ].Value.ToHtmlBold().InSingleQuotes() + 
                                                         " получило @SNTFINAL="     + snt1.ToString()        .ToHtmlBold().InSingleQuotes() + 
                                                         ", должно быть @SNTFINAL=" + snt2.ToString()        .ToHtmlBold().InSingleQuotes() ));
                            }
                        }
                        #endregion

                        #region [.check operate-rule-order.]
                        if ( result.OperateRulesNames != null )
                        {
                            var indexofStartSearch = 0;
                            foreach ( var id in rule.Ids )
                            {
                                var name = result.OperateRulesNames
                                                 .Skip( indexofStartSearch )
                                                 .FirstOrDefault( n => n.StartsWith( id, StringComparison.InvariantCultureIgnoreCase ) );
                                if ( name != null )
                                {
                                    var indexofStartSearchNew = indexofStartSearch + 
                                                                result.OperateRulesNames.Skip( indexofStartSearch ).ToList().IndexOf( name );
                                        //result.OperateRulesNames.IndexOf( name );

                                    for ( int i = indexofStartSearch; i < indexofStartSearchNew; i++ )
                                    {
                                        var value = result.OperateRulesNames[ i ];
                                        if ( 3 <= value.Length && 
                                             100 <= value.Substring( 0, 3 ).TryConvert2Int().GetValueOrDefault() )
                                        {
                                            //found who's don't shoul be found!!!!!
                                            throw (new TestException("Сработало правило " + value.InSingleQuotes().ToHtmlBold() + ", которое не должно было сработать: " + result.OperateRulesNames.ToHtml() ));
                                        }
                                    }

                                    indexofStartSearch = indexofStartSearchNew + 1;
                                }
                                else
                                {
                                    //not found!!!!!!!
                                    throw (new TestException("Правило " + id.InSingleQuotes().ToHtmlBold() + " не найдено среди сработавших правил: " + result.OperateRulesNames.ToHtml() ));
                                }
                            }
                        }
                        #endregion

                        if ( showOnlyErrorCheckBox.Checked )
                            continue;

                        resultDiv.Controls.AddDiv4OkMessage( rule, sent );
                    }
                    catch ( Exception ex )
                    {
                        #region
                        errorCount++;

                        resultDiv.Controls.AddDiv4ExceptionMessage( ex, rule, sent );

                        if ( ex is CommunicationException ) //EndpointNotFoundException)
                        {
                            resultDiv.Controls.AddDiv( "....выполниение теста прервано...." );
                            return;
                        }
                        #endregion
                    }
                    resultDiv.Controls.AddHr();
                }
            }

            resultDiv.Controls.AddDiv4FinalMessage( testXmlFile, errorCount );
            #endregion
        }

        protected DigestOutputResult GetDigestOutputResult( TestSent sent )
        {
            using ( var client = new DigestWcfClient( ENDPOINT_CONFIGURATION_NAME ) )
            {
                var inputParams = new DigestInputParams( sent.Text, InputTextFormat.PlainText ) 
                            {
                                ExecuteTonalityMarking = true,
                            };
                var result = client.ExecuteDigest( inputParams );

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
    }

    //----------------------------------------------------------------//
    internal static class Extensions4Test
    {
        public static SNTFINAL GetAttributeSNTFINAL( this XElement node )
        {
            var nodeWithSNTFINAL = node.XPathSelectElement("./self::*[@SNTFINAL] | ./child::*[@SNTFINAL and local-name() != 'SUB_SENT']"); /*"./descendant-or-self::*[@SNTFINAL]"*/
            if ( nodeWithSNTFINAL == null )
            {
                var nodeWithSNT = node.XPathSelectElement("./self::*[@SNT]" );
                if ( nodeWithSNT == null )
                {
                    return (SNTFINAL.nNeut);
                }

                return (nodeWithSNT.GetAttribute( "SNT" )
                                   .TryConvert2T< SNTFINAL >()
                                   .GetValueOrDefault( SNTFINAL.nNeut ));
            }

            return (nodeWithSNTFINAL.GetAttribute( "SNTFINAL" )
                                    .TryConvert2T< SNTFINAL >()
                                    .GetValueOrDefault( SNTFINAL.nNeut ));
        }
        public static string ToHtmlBold( this string text )
        {
            return ("<span style='font-weight: bold;'>" + text + "</span>");   
        }
        public static string ToHtml( this ReadOnlyCollection< string > operateRulesNames )
        {
            return ("<br/>&nbsp;&nbsp;&nbsp;&nbsp;" + string.Join("<br/>&nbsp;&nbsp;&nbsp;&nbsp;", operateRulesNames.ToArray()).ToHtmlBold());
        }

        public static void AddDiv4ExceptionMessage( this ControlCollection controls, Exception ex, TestRule rule, TestSent sent )
        {
            var div = new HtmlGenericControl("div") { InnerHtml = rule.ToString() + '.' + sent.ToString() + " - " + (ex is TestException ? ex.Message : ex.ToString().GetNoLongest( 250 ).ToHtmlBold()) };
            div.Style[ HtmlTextWriterStyle.Color ] = "red";

            controls.Add( div );
        }
        public static void AddDiv4OkMessage(this ControlCollection controls, TestRule rule, TestSent sent)
        {
            controls.AddDiv( rule.ToString() + '.' + sent.ToString() + " - OK" );
        }
        public static void AddDiv4FinalMessage( this ControlCollection controls, TestXmlFile testXmlFile, int errorCount )
        {
            controls.AddDiv( "<br/>&nbsp;&nbsp;&nbsp;&nbsp;(Всего: " + testXmlFile.TestRules.Sum( _ => _.TestSents.Count ) + " штук, &nbsp;" + ((errorCount == 0) ? "Ошибок НЕТ)" : "Ошибок: " + errorCount + " штук)") );
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

    public enum SNTFINAL
    {
        nNeg,
        nPos,
        nNeut,
    }

    public class TestRule
    {
        public TestRule( XmlNode ruleNode )
        {
            Ids = new ReadOnlyCollection< string >
                (                  
                 (from id in ruleNode.Attributes[ "id" ].Value.Split( new[] { ',' }, StringSplitOptions.RemoveEmptyEntries )
                  select id.Trim()
                 ).ToList()
                ); 

            TestSents = new ReadOnlyCollection< TestSent >
                (
                 (from sentNode in ruleNode.SelectNodes( "sent" ).Cast< XmlNode >()
                  select new TestSent( sentNode )
                 ).ToList()
                );
        }

        public ReadOnlyCollection< string > Ids
        {
            get;
            private set;
        }

        public ReadOnlyCollection< TestSent > TestSents
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return ("[Rule " + string.Join( ",", Ids ) + ']');
        }
    }

    public class TestSent
    {
        public TestSent( XmlNode sentNode )
        {
            Number = sentNode.Attributes[ "n" ].Value;

            SNTFINALs = new ReadOnlyCollection< SNTFINAL >
                (
                 (from sntfinal in sentNode.Attributes[ "SNTFINAL" ].Value.Split( new[] { ',' }, StringSplitOptions.RemoveEmptyEntries )
                  select (SNTFINAL) Enum.Parse( typeof(SNTFINAL), sntfinal )
                 ).ToList()
                );

            Text = sentNode.InnerText;
        }

        public string Number
        {
            get;
            private set;
        }
        public ReadOnlyCollection< SNTFINAL > SNTFINALs
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
            return ("[sent " + Number + "]. " + Text);
        }
    }

    public class TestXmlFile
    {
        public TestXmlFile( Stream testFileContent )
        {
            var xdoc = new XmlDocument();
            using ( var xtr = new XmlTextReader( testFileContent ) )
            {
                xtr.Read();
                xdoc.LoadXml( xtr.ReadOuterXml() );
            }

            TestRules = new ReadOnlyCollection< TestRule >
                (
                 (from ruleNode in xdoc.SelectNodes( "TestSentenses/Rule" ).Cast< XmlNode >()
                  select new TestRule( ruleNode )
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