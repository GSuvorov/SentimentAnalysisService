using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.ServiceModel;
using System.Threading;
using System.Threading.Tasks;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;

using TextMining.Core;
using TonalityMarking;
using TextMining.Service;

namespace Test.TonalityMarking.Web
{
    /// <summary>
    /// 
    /// </summary>
    public partial class Test : Page
    {
        protected string Lingvistic_ProcessText_MethodNameFileName
        {
            get { return (Server.MapPath("~/App_Data") + "\\Lingvistic_ProcessText_MethodName.txt"); }
        }
        protected void Page_Load(object sender, EventArgs e)
        {
        }
        protected void runTestButton_Click(object sender, EventArgs e)
        {
            #region [.set controls state.]
            runTestButton        .Visible = false;
            testFileUpload1      .Visible = false;
            testFileUpload2      .Visible = false;
            showOnlyErrorCheckBox.Visible = false;
            againHyperLink1      .Visible = true;
            againHyperLink2      .Visible = true;
            backHyperLink2       .Visible = true;
            //repeatLinkButton1    .Visible = true;
            //repeatLinkButton2    .Visible = true;
            #endregion

            #region [.read text-file.]
            var testXmlFile = new TestXmlFile( TestFileContent );
            #endregion

            #region [.test.]
            var sw = Stopwatch.StartNew();

            #region [.parallel.]
            var errorCount = 0;
            var wasCommunicationException = 0;
            Parallel.ForEach( testXmlFile.TestRules, rule => 
                {
                    if ( wasCommunicationException.IsTrue() )
                        return;

                    foreach ( var sent in rule.TestSents )
                    {
                        try
                        {
                            var result = GetTonalityOutputResult( sent );

                            var xdoc = result.OutputXml.ToXDocument();

                            #region [check OT.]
                            if ( sent.OTs != null && sent.OTs.Any() )
                            {
                                var inquiryinitialNodes = xdoc.XPathSelectElements( "//*[ @INQUIRYINITIAL ]" ).ToArray();
                                if ( sent.OTs.Count != inquiryinitialNodes.Length /*result.AuthorTonalityMarkingTuple.Objects.Count*/ )
                                {
                                    throw (new TestException("Число выделенных объектов не равно числу объектов в атрибуте-@OT в тестовом файле."));
                                }

                                for ( var i = 0; i < sent.OTs.Count; i++ )
                                {
                                    var ot1     = sent.OTs[ i ];
                                    var inquiry = inquiryinitialNodes[ i ];
                                    var ot2     = inquiry.GetVALUETHEMEorVALUE();

                                    var isi = ot1.ToInquiriesSynonymsInfo();
                                    var xot = Algorithms.GetObjectBySynonyms( inquiry, isi );
                                    if ( xot == null )
                                    {
                                        var subsent = inquiry.GetAncestorSubsent();
                                        var subsentText = (subsent != null) ? subsent.Value : "???";

                                        throw (new TestException("В подпредложении "      + subsentText.ToHtmlBold().InSingleQuotes() + 
                                                                 " определился объект "   + ot2        .ToHtmlBold().InSingleQuotes() +
                                                                 ", должен определиться " + ot1        .ToHtmlBold().InSingleQuotes()));
                                    }
                                }
                            }
                            #endregion

                            #region [check SNTFINAL.]
                            var subsentNodes = xdoc.XPathSelectElements( "//SUB_SENT" ).ToArray(); // || "TEXT/SENT/SUB_SENT"
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

                            rule.HtmlControls.Add( Extensions4Test.CreateDiv4OkMessage( rule, sent ) );
                        }
                        catch ( Exception ex )
                        {
                            #region
                            Interlocked.Increment( ref errorCount );

                            rule.HtmlControls.Add( Extensions4Test.CreateDiv4ExceptionMessage( ex, rule, sent ) );

                            if ( ex is CommunicationException ) //EndpointNotFoundException)
                            {
                                Interlocked.CompareExchange( ref wasCommunicationException, 1, 0 );
                                return;
                            }
                            #endregion
                        }
                    }
                }
            );

            //output result to html
            foreach ( var rule in testXmlFile.TestRules )
            {
                foreach ( var control in rule.HtmlControls )
                {
                    resultDiv.Controls.Add( control );
                    resultDiv.Controls.AddHr();
                }                
            }
            if ( wasCommunicationException.IsTrue() )
            {
                resultDiv.Controls.AddDiv( "....выполниение теста прервано => [CommunicationException]...." );
                resultDiv.Controls.AddHr();
            }
            #endregion

            #region [.consecutively.]
            /*
            var errorCount = 0;
            foreach ( var rule in testXmlFile.TestRules )
            {
                foreach ( var sent in rule.TestSents )
                {
                    try
                    {
                        var result = GetTonalityOutputResult( sent, lptm );

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
            //*/
            #endregion

            sw.Stop();
            resultDiv.Controls.AddDiv4FinalMessage( testXmlFile, errorCount, sw );
            #endregion
        }

        protected TonalityMarkingOutputResult GetTonalityOutputResult( TestSent sent )
        {
            using ( var client = new TonalityMarkingWcfClient( ENDPOINT_CONFIGURATION_NAME ) )
            {
                var inputParams = new TonalityMarkingInputParams( sent.Text, InputTextFormat.PlainText );
                if ( sent.OTs != null && sent.OTs.Any() )
                {
                    inputParams.InquiriesSynonyms = sent.OTs.ToList();
                }
                var result = client.ExecuteTonalityMarking( inputParams );

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
    
    /// <summary>
    /// 
    /// </summary>
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
            controls.Add( CreateDiv4ExceptionMessage( ex, rule, sent ) );
        }
        public static void AddDiv4OkMessage( this ControlCollection controls, TestRule rule, TestSent sent )
        {
            controls.AddDiv( rule.ToString() + '.' + sent.ToString() + " - OK" );
        }
        public static void AddDiv4FinalMessage( this ControlCollection controls, TestXmlFile testXmlFile, int errorCount, Stopwatch sw )
        {
            controls.AddDiv( "<br/>&nbsp;&nbsp;&nbsp;&nbsp;(Всего: " + testXmlFile.TestRules.Sum( _ => _.TestSents.Count ) + " штук, &nbsp;" +
                            ((errorCount == 0) ? "Ошибок НЕТ" : "Ошибок: " + errorCount + " штук") + ", &nbsp;Время: " + 
                            sw.Elapsed );
        }
        public static void AddDiv( this ControlCollection controls, string text )
        {
            controls.Add( CreateDiv( text ) );
        }
        public static void AddHr( this ControlCollection controls )
        {
            controls.Add( CreateHr() );
        }

        public static HtmlGenericControl CreateDiv4ExceptionMessage( Exception ex, TestRule rule, TestSent sent )
        {
            var div = new HtmlGenericControl("div") { InnerHtml = rule.ToString() + '.' + sent.ToString() + " - " + (ex is TestException ? ex.Message : ex.ToString().GetNoLongest( 250 ).ToHtmlBold()) };
            div.Style[ HtmlTextWriterStyle.Color ] = "red";

            return (div);
        }
        public static HtmlGenericControl CreateDiv4OkMessage( TestRule rule, TestSent sent )
        {
            return (CreateDiv( rule.ToString() + '.' + sent.ToString() + " - OK" ));
        }
        public static HtmlGenericControl CreateDiv( string text )
        {
            return (new HtmlGenericControl("div") { InnerHtml = text });
        }
        public static HtmlGenericControl CreateHr()
        {
            return (new HtmlGenericControl("hr"));
        }

        public static bool IsTrue( this int value )
        {
            return (value != 0);
        }
        public static bool IsFalse( this int value )
        {
            return (value == 0);
        }

        private static readonly string HOMOGENEOUS_VALUE = TypeAttributeValue.Homogeneous.ToString();
        public static bool IsHomogeneous( this XElement e )
        {
            return ((e.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE       ) == HOMOGENEOUS_VALUE) ||
                    (e.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_SAVED_ATTRIBUTE ) == HOMOGENEOUS_VALUE)
                   );
        }

        public static InquiriesSynonymsInfo ToInquiriesSynonymsInfo( this string value )
        {
            var ip = new TonalityMarkingInputParams( "DUMMY" )
            {
                InquiriesSynonyms = new[] { value }.ToList(),
            };

            var isi = new InquiriesSynonymsInfo( ip.InquiriesSynonyms );

            return (isi);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public class TestException : Exception
    {
        public TestException(string message) : base(message) { }
    }

    /// <summary>
    /// 
    /// </summary>
    public enum SNTFINAL
    {
        nNeg,
        nPos,
        nNeut,
    }

    /// <summary>
    /// 
    /// </summary>
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

            HtmlControls = new List< HtmlGenericControl >();
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

        public List< HtmlGenericControl > HtmlControls
        {
            get;
            private set;
        }
    }

    /// <summary>
    /// 
    /// </summary>
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

            var ot_attr = sentNode.Attributes[ "OT" ];
            if ( ot_attr != null )
            {
                OTs = new ReadOnlyCollection< string >
                    (
                     (from ot in ot_attr.Value.Split( new[] { ',' }, StringSplitOptions.RemoveEmptyEntries )
                      let _ot = ot.Trim()
                      where ( !string.IsNullOrEmpty( _ot ) )
                      select ot
                     ).ToList()
                    );
            }
            /*else
            {
                OTs = new ReadOnlyCollection< string >( new string[ 0 ].ToList() );
            }*/

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
        public ReadOnlyCollection< string > OTs
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

    /// <summary>
    /// 
    /// </summary>
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

    /// <summary>
    /// An orderable dynamic partitioner for lists
    /// </summary>
    /// <typeparam name="TSource"></typeparam>
    internal class OrderablePartitionerImpl< TSource > : OrderablePartitioner< TSource >
    {
        private readonly IList< TSource > _Input;

        public OrderablePartitionerImpl( IList< TSource > input ) : base( true, true, true )
        {
            _Input = input;
        }

        // Must override to return true.
        public override bool SupportsDynamicPartitions
        {
            get { return true; }
        }

        /*public override IEnumerable< TSource > GetDynamicPartitions()
        {
            return base.GetDynamicPartitions();
        }*/
        public override IEnumerable< KeyValuePair< long, TSource > > GetOrderableDynamicPartitions()
        {
            return (new DynamicPartitions( _Input ));
        }

        public override IList< IEnumerator< KeyValuePair< long, TSource > > > GetOrderablePartitions( int partitionCount )
        {
            throw new NotImplementedException();

            /*
            var dynamicPartitions = GetOrderableDynamicPartitions();
            var partitions = new IEnumerator< KeyValuePair< long, TSource > >[ partitionCount ];

            for ( int i = 0; i < partitionCount; i++ )
            {
                partitions[ i ] = dynamicPartitions.GetEnumerator();
            }
            return (partitions);
            */
        }
        /*public override IList< IEnumerator< TSource > > GetPartitions( int partitionCount )
        {
            return base.GetPartitions( partitionCount );
        }*/

        /// <summary>
        /// 
        /// </summary>
        private class DynamicPartitions : IEnumerable< KeyValuePair< long, TSource > >
        {
            private IList< TSource > _Input;
            private int              _Pos;

            internal DynamicPartitions( IList< TSource > input )
            {
                _Input = input;
                _Pos   = 0;
            }

            public IEnumerator< KeyValuePair< long, TSource > > GetEnumerator()
            {
                for ( ;; )
                {
                    lock ( this )
                    {
                        // Each task gets the next item in the list. The index is 
                        // incremented in a thread-safe manner to avoid races.
                        //---int elemIndex = Interlocked.Increment( ref _Pos ) - 1;
                        int elemIndex = ++_Pos - 1;

                        if (elemIndex >= _Input.Count)
                        {
                            yield break;
                        }

                        yield return (new KeyValuePair< long, TSource >( elemIndex, _Input[ elemIndex ] ));
                    }
                }
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return ((IEnumerable< KeyValuePair< long, TSource > >)this).GetEnumerator();
            }
        }
    }
}