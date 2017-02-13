using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Xml.Linq;

using log4net;
using OpinionMining;
using TextMining.Core;
using TonalityMarking;

namespace Digest
{
    /// <summary>
    /// 
    /// </summary>
    internal static class Extensions
    {  
        private static string GetNoLongest( ref string value, int maxLength )
        {
            if ( value.IsEmptyOrNull() || (value.Length < maxLength) )
                return (value);

            if ( maxLength <= 3 ) maxLength += 3;
            return (value.Substring( 0, Math.Min(value.Length,  maxLength - 3) ) + "...");                
        }
        public static string Get4Log( this string value )
        {
            return (GetNoLongest( ref value, 100 ).InSingleQuote());
        }

        private static Int64 _TotalLogCount = 0;
#if DEBUG
        private static readonly object _SynRoot = new object();
#endif
        public static void InfoEx( this ILog log, string methodName, string clientRemoteAddress, TimeSpan totalElapsed, InputParamsBase inputParams )
        {
            var value = Interlocked.Increment( ref _TotalLogCount ) + "]. " +
                        methodName + " => \r\n\t ClientRemoteAddress: " + clientRemoteAddress +
                      "\r\n\t Total processing time: "                  + totalElapsed +
                      "\r\n\t Input text (length: {0}): ".FormatEx( inputParams.InputText.Length ) +
                      inputParams.InputText.Get4Log();

            log.Info( value ); 

        #if DEBUG
            lock ( _SynRoot )
            {
                var fc = Console.ForegroundColor; Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine( Environment.NewLine + value );                
                Console.ForegroundColor = fc;
                //Console.WriteLine( RuleBase.GetDebugInfoOutput() );
            }
        #endif
        }
        public static void ErrorEx( this ILog log, string methodName, Exception ex, string clientRemoteAddress, InputParamsBase inputParams )
        {            
            var value = ("{0}]. ")                           .FormatEx( Interlocked.Increment( ref _TotalLogCount ) ) +
                        ("{0} => '{1}': '{2}'")              .FormatEx( methodName, ex.GetType().Name, ex.Message ) + 
                        ("\r\n\t ClientRemoteAddress: {0}")  .FormatEx( clientRemoteAddress          ) + 
                        ("\r\n\t Input text (length: {0}): ").FormatEx( inputParams.InputText.Length ) + 
                        inputParams.InputText.InSingleQuote();

            log.Error( value, ex );

        #if DEBUG
            lock ( _SynRoot )
            {
                var fc = Console.ForegroundColor; Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine( Environment.NewLine + value + Environment.NewLine + Environment.NewLine + ex.GetType().Name + " => " + ex.ToString() );
                Console.ForegroundColor = fc;
                //Console.WriteLine( RuleBase.GetDebugInfoOutput() );
            }
        #endif
        }

        public static OpinionMiningInputParams ToOpinionMiningInputParams( this DigestInputParams inputParams )
        {
            var omParams = new OpinionMiningInputParams( inputParams.InputText, inputParams.InputTextFormat )
                {
                    InquiriesSynonyms    = inputParams.InquiriesSynonyms,
                    //OutputXmlFormat      = OutputXmlFormat.Full, //!IMPORTANT! //inputParams.OutputXmlFormat,
                    LanguageType         = inputParams.LanguageType,
                    ThemeType            = inputParams.ThemeType,
                    ObjectAllocateMethod = inputParams.ObjectAllocateMethod,
                };                
            return (omParams);
        }        

        #region commented. CompactOutputXmlFormatConverter
        /*
        private static XElement Convert2OutputFormat( this XElement sent, InputParamsBase inputParams )
        {
            inputParams.ThrowIfNull("inputParams");
            sent.ThrowIfNull("sent");
            sent.ThrowIfNotSent();

            if ( inputParams.OutputXmlFormat == OutputXmlFormat.Compact )
            {
                sent = sent.ToCompactFormat();
            }

            return (sent);            
        }
        private static XElement ToCompactFormat( this XElement sent )
        {
            return (Config.CompactOutputXmlFormatConverter.ConvertSent( sent ));            
        }

        public static DigestTuple ToDigestTuple( this OpinionMiningTuple tuple, InputParamsBase inputParams )
        {
            var sentenceXml = default(string);
            if ( inputParams.OutputXmlFormat == OutputXmlFormat.Compact ) {
                sentenceXml = tuple.GetSentence().ToCompactFormat().ToString();
            } else {
                sentenceXml = tuple.SentenceText;
            }

            return (new DigestTuple( tuple, sentenceXml ));
        }
        */
        #endregion

        public static DigestTuple ToDigestTuple( this OpinionMiningTuple tuple )
        {
            return (new DigestTuple( tuple, tuple.SentenceText ));
        }
        public static DigestTuple ToDigestTuple( this OpinionMiningTuple tuple
                                                 , TonalityMarkingWcfService.OutputResult4DigestWcfService outputResult4DigestWcfService
                                                 , InputParamsBase inputParams )
        {
            var result = new DigestTuple
                             ( 
                             tuple,
                             outputResult4DigestWcfService.Sentence.ToString(),
                             //outputResult4DigestWcfService.Sentence.Convert2OutputFormat( inputParams ).ToString(),
                             outputResult4DigestWcfService.Positive,
                             outputResult4DigestWcfService.Negative 
                             );
            return (result);
        }
    }
}
