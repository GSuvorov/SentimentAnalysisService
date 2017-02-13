using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;
using log4net;
using TextMining.Core;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using Consts = TextMining.Core.XLinqExtensions.Consts;
    using TextMining.Service;

    internal static partial class Extensions
    {
        private static string GetNoLongest( this string value, int maxLength )
        {
            if ( value.IsEmptyOrNull() || (value.Length < maxLength) )
                return (value);

            if ( maxLength <= 3 ) maxLength += 3;
            return (value.Substring( 0, Math.Min(value.Length,  maxLength - 3) ) + "...");                
        }
        public static string Get4Log( this string value )
        {
            return (value.GetNoLongest( 100 ).InSingleQuote());
        }

        private static Int64 _TotalLogCount = 0;
#if DEBUG
        private static readonly object _SynRoot = new object();
#endif
        public static void InfoEx( this ILog log, string methodName, string clientRemoteAddress, TimeSpan linguisticElapsed, TimeSpan opinionMiningElapsed, InputParamsBase inputParams )
        {
            var value = Interlocked.Increment( ref _TotalLogCount ) + "]. " +
                        methodName + " => \r\n\t ClientRemoteAddress: " + clientRemoteAddress +
                      "\r\n\t Linguistic     processing time: "         + linguisticElapsed +
                      "\r\n\t Opinion-mining processing time: "         + opinionMiningElapsed +
                      "\r\n\t Input text (length: {0}): ".FormatEx( inputParams.InputText.Length ) +
                      inputParams.InputText.Get4Log();

            log.Info( value ); 

        #if DEBUG
            lock ( _SynRoot )
            {
                var fc = Console.ForegroundColor; Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine( Environment.NewLine + value );                
                Console.ForegroundColor = fc;
                Console.WriteLine( RuleBase.GetDebugInfoOutput() );
            }
        #endif
        }
        public static void ErrorEx( this ILog log, string methodName, Exception ex, string clientRemoteAddress, InputParamsBase inputParams, XDocument xdocument )
        {            
            var value = ("{0}]. ")                           .FormatEx( Interlocked.Increment( ref _TotalLogCount ) ) +
                        ("{0} => '{1}': '{2}'")              .FormatEx( methodName, ex.GetType().Name, ex.Message ) + 
                        ("\r\n\t ClientRemoteAddress: {0}")  .FormatEx( clientRemoteAddress          ) + 
                        ("\r\n\t Input text (length: {0}): ").FormatEx( inputParams.InputText.Length ) + 
                        inputParams.InputText.InSingleQuote() + 
                        ((xdocument != null) ? ("\r\n\t XML: [\r\n{0}\r\n]").FormatEx( xdocument.ToString() ) : string.Empty);

            log.Error( value, ex );

        #if DEBUG
            lock ( _SynRoot )
            {
                var fc = Console.ForegroundColor; Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine( Environment.NewLine + value + Environment.NewLine + Environment.NewLine + ex.GetType().Name + " => " + ex.ToString() );
                Console.ForegroundColor = fc;
                Console.WriteLine( RuleBase.GetDebugInfoOutput() );
            }
        #endif
        }

        public static int LengthEx( this string text )
        {
            return (text.IsEmptyOrNull() ? 0 : text.Length);
        }
        public static int MaxEx< T >( this ICollection< T > collection, Func< T, int > selector )
        {
            if ( (collection != null) && (0 < collection.Count) )
            {
                return (collection.Max( selector ));
            }
            return (0);
        }
        public static void AddEx< T >( this List< T > list, T item )
        {
            list.ThrowIfNull("list");

            if ( !item.IsNull() )
            {
                list.Add( item );
            }
        }
        public static void AddRangeEx< T >( this List< T > list, IEnumerable< T > collection )
        {
            list.ThrowIfNull("list");

            if ( !collection.IsNull() )
            {
                list.AddRange( collection );
            }
        }
        public static void AddRangeTo< T >( this IEnumerable< T > collection, List< T > list )
        {
            list.ThrowIfNull("list");

            if ( !collection.IsNull() )
            {
                list.AddRange( collection );
            }
        }
        public static void ForEachEx< T >( this IEnumerable< T > sequence, Action< T > action )
        {
            if ( sequence != null )
            {
                foreach ( var _ in sequence )
                {
                    action( _ );
                }
            }
        }
        public static bool AnyEx< T >( this IEnumerable< T > sequence )
        {
            return ((sequence == null) ? false : sequence.Any());
        }
        public static bool AnyEx< T >( this List< T > sequence )
        {
            return ((sequence == null) ? false : (0 < sequence.Count));
        }
        public static T[] ToArrayEx< T >( this List< T > sequence )
        {
            if ( sequence.IsNotNull() )
                return (sequence.ToArray());
            return (null);
        }
        public static int LengthWithoutSpace( this string text )
        {
            return (new StringBuilder( text )
                      .Replace( " " , string.Empty )
                      .Replace( "\r", string.Empty )
                      .Replace( "\n", string.Empty )
                      .Length 
                   );
        }

        public static void Add( this List< SubjectEssence > subjectEssenceList,
                                List< SubjectAndAnaphoriaEntityTuple > rigthEntity,
                                XElement verb )
        {
            if ( rigthEntity != null )
            {
                foreach ( var e in rigthEntity )
                {
                    subjectEssenceList.Add( new SubjectEssence( e, verb ) );
                }
            }
        }
        public static void Add( this List< SubjectEssence > subjectEssenceList,
                                List< SubjectAndAnaphoriaEntityTuple > leftEntity,
                                List< SubjectAndAnaphoriaEntityTuple > rigthEntity,
                                XElement verb )
        {
            if ( leftEntity != null )
            {
                foreach ( var e in leftEntity )
                {
                    subjectEssenceList.Add( new SubjectEssence( e, verb ) );
                }
            }

            if ( rigthEntity != null )
            {
                foreach ( var e in rigthEntity )
                {
                    subjectEssenceList.Add( new SubjectEssence( e, verb ) );
                }
            }
        }
        public static void Add( this List< SubjectEssence > subjectEssenceList,
                                List< SubjectAndAnaphoriaEntityTuple > leftEntity,
                                SubjectAndAnaphoriaEntityTuple[]       rigthEntity,
                                string verbValue )
        {
            if ( leftEntity != null )
            {
                foreach ( var e in leftEntity )
                {
                    subjectEssenceList.Add( new SubjectEssence( e, verbValue ) );
                }
            }

            if ( rigthEntity != null )
            {
                foreach ( var e in rigthEntity )
                {
                    subjectEssenceList.Add( new SubjectEssence( e, verbValue ) );
                }
            }
        }
        public static void Add( this List< SubjectEssence > subjectEssenceList,
                                List< SubjectAndAnaphoriaEntityTuple > leftEntity,
                                List< SubjectAndAnaphoriaEntityTuple > rigthEntity1,
                                List< SubjectAndAnaphoriaEntityTuple > rigthEntity2,
                                XElement verb )
        {
            subjectEssenceList.Add( leftEntity, rigthEntity1, verb );

            if ( rigthEntity2 != null )
            {
                foreach ( var e in rigthEntity2 )
                {
                    subjectEssenceList.Add( new SubjectEssence( e, verb ) );
                }
            }            
        }

        public static XElement FirstEntity( this SubjectData subjectData )
        {
            return (subjectData.SubjectEssences.First().Entity);
        }
        /*public static XElement LastEntity( this SubjectData subjectData )
        {
            return (subjectData.SubjectEssences.Last().Entity);
        }
        public static XElement LastVerb( this SubjectData subjectData )
        {
            return (subjectData.SubjectEssences.Last().Verb);
        }*/

        #region commented. CompactOutputXmlFormatConverter
        /*
        private static XElement ToCompactFormat( this XElement sent )
        {
            return (Config.CompactOutputXmlFormatConverter.ConvertSent( sent ));            
        }
        public static IEnumerable< OpinionMiningTuple > ConvertSent2OutputFormatIfNeed( this IEnumerable< OpinionMiningTuple > opinionMiningTuple, InputParamsBase inputParams )
        {
            if ( inputParams.OutputXmlFormat == OutputXmlFormat.Compact )
            {
                return (from t in opinionMiningTuple 
                        select new OpinionMiningTuple( t, t.GetSentence().ToCompactFormat().ToString() ) 
                       );
            }

            return (opinionMiningTuple);
        }
        */
        #endregion
    }

    internal static partial class XLinqExtensions
    {
        private const string OM_PEAMBLE = "OM";

        private static string Xpath4Sentences( this Language language )
        {
            const string SENTS_XPATH = "TEXT/SENT[ @LANG='{0}' ]";

            switch ( language.LanguageType )
            {
                case LanguageType.RU:
                    return (SENTS_XPATH.FormatEx( LanguageType.RU.ToString() ));

                default:
                    throw (new ArgumentException(language.LanguageType.ToString()));
            }
        }
        public static IEnumerable< XElement > GetSentences( this XDocument xd, Language language )
        {
            return (xd.XPathSelectElements( language.Xpath4Sentences() ));
        }

        public static int GetPositionInParent( this XElement element )
        {
            if ( element.Parent == null )
                return (-1);

            return (element.Parent.Elements( element.Name ).ToList().IndexOf( element ) + 1);
        }

        public static bool IsElementVERBtypeVERB( this XElement element )
        {
            return (element.IsElementVERB() && (element.GetAttributeTYPE() == TypeAttributeValue.Verb));
        }
        public static bool IsElementVERBtypeDEEPR( this XElement element )
        {
            return (element.IsElementVERB() && (element.GetAttributeTYPE() == TypeAttributeValue.Deepr));
        }

        public static string GetAttributeISSUBJECT( this XElement element )
        {
            return (element.GetAttribute( Consts.ISSUBJECT_ATTRIBUTE ));
        }
        public static string GetAttributeISOBJECT( this XElement element )
        {
            return (element.GetAttribute( Consts.ISOBJECT_ATTRIBUTE ));
        }
        public static int GetAttributeIDForce( this XElement element )
        {
            var id = element.GetAttributeID();
            if ( !id.HasValue ) throw (new ArgumentException("Element no has valid @id-attribute: " + element.ToString().InSingleQuote()));
            return (id.Value);
        }       
        public static string GetAttributeINDIRECTSPEECH_BEGIN( this XElement element )
        {
            return (element.GetAttribute( Consts.INDIRECTSPEECH_BEGIN_ATTRIBUTE ));
        }
        public static string GetAttributeDIRECTSPEECH_BEGIN( this XElement element )
        {
            return (element.GetAttribute( Consts.DIRECTSPEECH_BEGIN_ATTRIBUTE ));
        }

        public static void SetAttributeISSUBJECT( this XElement element, int value )
        {
            element.SetAttribute( Consts.ISSUBJECT_ATTRIBUTE, OM_PEAMBLE + value );
        }
        public static void SetAttributeISSUBJECTforAllEntity( this SubjectData subjectData, int value )
        {
            foreach ( var st in subjectData.SubjectEssences )
            {
                st.Entity.SetAttributeISSUBJECT( value );
            }
        }
        public static void SetAttributeISOBJECT( this XElement element, int value )
        {
            element.SetAttribute( Consts.ISOBJECT_ATTRIBUTE, OM_PEAMBLE + value );
        }
        private static void SetAttributeINDIRECTSPEECH_BEGIN( this XElement element, int directAndIndirectSpeechGlobalNumber )
        {
            element.SetAttribute( Consts.INDIRECTSPEECH_BEGIN_ATTRIBUTE, OM_PEAMBLE + directAndIndirectSpeechGlobalNumber );
        }
        private static void SetAttributeINDIRECTSPEECH_END  ( this XElement element, int directAndIndirectSpeechGlobalNumber )
        {
            element.SetAttribute( Consts.INDIRECTSPEECH_END_ATTRIBUTE, OM_PEAMBLE + directAndIndirectSpeechGlobalNumber );
        }
        private static void SetAttributeDIRECTSPEECH_BEGIN  ( this XElement element, int directAndIndirectSpeechGlobalNumber )
        {
            element.SetAttribute( Consts.DIRECTSPEECH_BEGIN_ATTRIBUTE, OM_PEAMBLE + directAndIndirectSpeechGlobalNumber );
        }
        private static void SetAttributeDIRECTSPEECH_END    ( this XElement element, int directAndIndirectSpeechGlobalNumber )
        {
            element.SetAttribute( Consts.DIRECTSPEECH_END_ATTRIBUTE, OM_PEAMBLE + directAndIndirectSpeechGlobalNumber );
        }

        /*public static XElement CreateSubsentIndirectSpeech( this int directAndIndirectSpeechGlobalNumber )
        {
            var subsentIS = new XElement( Consts.SUBSENT_ELEMENT );
            //subsentIS.SetAttributeTYPE( SubsentenceType.IndirectSpeech );
            subsentIS.SetAttributeTYPE( SubsentenceType.Default );
            subsentIS.SetAttributeISINDIRECTSPEECH();
            subsentIS.SetAttributeIndirectSpeechID( directAndIndirectSpeechGlobalNumber );
            return (subsentIS);
        }
        public static XElement CreateSubsentDirectSpeech( this int directAndIndirectSpeechGlobalNumber )
        {
            var subsentDS = new XElement( Consts.SUBSENT_ELEMENT );
            //subsentIS.SetAttributeTYPE( SubsentenceType.IndirectSpeech );
            subsentDS.SetAttributeTYPE( SubsentenceType.Default );
            subsentDS.SetAttributeISDIRECTSPEECH();
            subsentDS.SetAttributeIndirectSpeechID( directAndIndirectSpeechGlobalNumber );
            return (subsentDS);
        }
        */        
        /*public static XElement RemoveIndirectSpeechSubsents( this XElement sent )
        {sent.ThrowIfNotSent();

            var indirectSpeechSubsents = (from subsent in sent.DescendantsSubsentence()
                                          where subsent.GetAttributeISINDIRECTSPEECH().IsNotNull()
                                          select subsent
                                         )
                                         .ToArray();

            foreach ( var subsent in indirectSpeechSubsents )
            {
                var nodes = subsent.Nodes().ToArray();
                subsent.RemoveAll();
                subsent.AddAfterSelf( nodes );
                subsent.Remove();
            }

            return (sent);
        }*/
        /*public static void MoveIn2ThisNodesRange( this XElement parentNode, XElement startRangeNode, XElement endRangeNode )
        {
            var movedItems = (new[] { startRangeNode }.Concat( startRangeNode.NodesAfterSelf() )).ToArray();
            foreach ( var item in movedItems )
            {
                item.Remove();
                parentNode.Add( item );

                if ( item == endRangeNode )
                    break;
            }
        }
        public static void MoveIn2ThisAllNodes( this XElement newParentElement, IEnumerable< XNode > movedNodes )
        {
            foreach ( var node in movedNodes.ToArray() )
            {
                node.Remove();
                newParentElement.Add( node );
            }
        }*/
        public static XElement RemoveDirectAndIndirectSpeechBeginEndAttributes( this XElement sent )
        {sent.ThrowIfNotSent();

            var ra = from e in sent.Descendants()
                     let indirectspeech_begin = e.Attribute( Consts.INDIRECTSPEECH_BEGIN_ATTRIBUTE )
                     let indirectspeech_end   = e.Attribute( Consts.INDIRECTSPEECH_END_ATTRIBUTE )
                     let directspeech_begin   = e.Attribute( Consts.DIRECTSPEECH_BEGIN_ATTRIBUTE )
                     let directspeech_end     = e.Attribute( Consts.DIRECTSPEECH_END_ATTRIBUTE )
                     where ( indirectspeech_begin.IsNotNull() || 
                             indirectspeech_end  .IsNotNull() || 
                             directspeech_begin  .IsNotNull() || 
                             directspeech_end    .IsNotNull() )
                     select new 
                     {
                         INDIRECTSPEECH_BEGIN = indirectspeech_begin,
                         INDIRECTSPEECH_END   = indirectspeech_end,
                         DIRECTSPEECH_BEGIN   = directspeech_begin,
                         DIRECTSPEECH_END     = directspeech_end,
                     };

            foreach ( var a in ra )
            {
                if ( a.INDIRECTSPEECH_BEGIN.IsNotNull() ) a.INDIRECTSPEECH_BEGIN.Remove();
                if ( a.INDIRECTSPEECH_END  .IsNotNull() ) a.INDIRECTSPEECH_END  .Remove();
                if ( a.DIRECTSPEECH_BEGIN  .IsNotNull() ) a.DIRECTSPEECH_BEGIN  .Remove();
                if ( a.DIRECTSPEECH_END    .IsNotNull() ) a.DIRECTSPEECH_END    .Remove();
            }

            return (sent);
        }
        public static XElement Copy( this XElement xelement )
        {
            return (new XElement( xelement ));
        }

        public static string GetAttributeVALUEorContent( this XElement element )
        {
            var v = element.GetAttributeVALUE();
            return (v.IsEmptyOrNull() ? element.Value : v);
        }
        public static bool IsVALUEStartsWith( this XElement element, IEnumerable< string > strings )
        {
            var v = element.GetAttributeVALUEorContent().ToLowerEx().TrimWhiteSpaces();
            return (strings.Any( s => v.StartsWith( s ) ));
        }
        public static bool IsVALUEContains( this XElement element, IEnumerable< string > strings )
        {
            var v = element.GetAttributeVALUEorContent().ToLowerEx().TrimWhiteSpaces().SplitBySpace();
            return (v.Intersect( strings ).Any());
        }         
        public static bool IsDescendantsOrSelfPREAttributeContains( this XElement element, IEnumerable< string > strings )
        {
            return 
            (
                element.DescendantsAndSelf().Any( e => 
                    { 
                        var pre = e.GetAttributePRE();
                        if ( !pre.IsEmptyOrNull() )
                            return (pre.ToLowerEx().SplitBySpace().Intersect( strings ).Any());
                        return (false);
                    }
                )
            );
        }
        public static bool IsVALUEContains_or_IsDescendantsOrSelfPREAttributeContains( this XElement element, IEnumerable< string > strings )
        {
            return ( element.IsVALUEContains( strings ) ||
                     element.IsDescendantsOrSelfPREAttributeContains( strings ) );
        }

        public static IEnumerable< XElement > ElementsAfterSelfCET( this XElement element )
        {
            return (element.ElementsAfterSelf().Where( _ => _.IsElementCET() ));
        }       
        public static IEnumerable< XElement > ElementsBeforeSelfCET( this XElement element )
        {
            return (element.ElementsBeforeSelf().Where( _ => _.IsElementCET() ));
        }       
        public static IEnumerable< XElement > DescendantsAndSelfCet( this XElement element )
        {
            return (element.DescendantsAndSelf( Consts.CET_ELEMENT ));
        }

        public static XElement GetNotEmptyElementAfterSelf( this XElement element )
        {
            return (element.ElementsAfterSelfExceptT().FirstOrDefault( _ => !_.Value.TrimWhiteSpaces().IsEmptyOrNull() ));
        }

        public static bool IsQuote( this XElement element )
        {
            if ( element.IsElementCET() && (element.GetAttributeTYPE() == TypeAttributeValue.Quote) )
                return (true);

            //if ( element.IsElementU() )
            //{
            //    var u = element.GetCDataTextFromElementU().TrimWhiteSpaces();
            //    return (u.StartsWith( "\"" ) || u.EndsWith( "\"" ) );
            //}
            return (false);
        }

        public static IEnumerable< XElement > BeforeAndSelfSubsents( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            return (subsent.GetAncestorSent().DescendantsSubsentence().TakeWhile( s => s != subsent )
                                                                      .Concat( subsent.RepeatOnce() )
                   );
        }
        public static IEnumerable< XElement > BeforeSubsents( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            return (subsent.GetAncestorSent().DescendantsSubsentence().TakeWhile( s => s != subsent )
                   );
        }
        public static IEnumerable< XElement > AfterAndSelfSubsents( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            return (subsent.GetAncestorSent().DescendantsSubsentence().SkipWhile( s => s != subsent ));
        }
        public static IEnumerable< XElement > AfterSubsents( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            return (subsent.GetAncestorSent().DescendantsSubsentence().SkipWhile( s => s != subsent )
                                                                      .SkipWhile( s => s == subsent ) );
        }

        private static XElement GetMostLastElement( this IEnumerable< XElement > subsents )
        {
            var mostLast = subsents.First().DescendantsExceptT().LastOrDefault();
            foreach ( var s in subsents.Skip( 1 ) )
            {
                var last = s.DescendantsExceptT().LastOrDefault();
                if ( mostLast.IsNull() )
                    mostLast = last;
                else if ( last.IsNotNull() && mostLast.IsBefore( last ) )
                    mostLast = last;

            }
            //possible fusking-linguistica-tricks!
            if ( mostLast.IsNull() )
                throw (new InvalidOperationException("WTF: 'mostLast' IS NULL (FUSKING-LINGUISTICA)!"));

            return (mostLast);
        }

        public static void SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( this IEnumerable< XElement > subsents
                                                                          , int directAndIndirectSpeechGlobalNumber
                                                                          , XElement breakSearchElement = null )
        {
            if ( subsents.AnyEx() )
            {
                subsents.ForEachEx( _ => _.ThrowIfNotSubsent() );

                subsents.First().SetAttributeINDIRECTSPEECH_BEGIN( directAndIndirectSpeechGlobalNumber );
                if ( breakSearchElement.IsNull() )
                {
                    subsents.GetMostLastElement().SetAttributeINDIRECTSPEECH_END( directAndIndirectSpeechGlobalNumber );
                }
                else
                {
                    foreach ( var subsent in subsents )
                    {
                        var e0 = subsent.ElementsExceptT().ToArray();
                        var e1 = e0.TakeWhile( _ => _ != breakSearchElement ).ToArray();

                        if ( e0.Length != e1.Length && e1.Any() )
                        {
                            e1.Last().SetAttributeINDIRECTSPEECH_END( directAndIndirectSpeechGlobalNumber );
                        }
                    }
                }
            }
        }
        public static void SetAttributeINDIRECTSPEECH_BEGINEND( this IEnumerable< XElement > elements, int directAndIndirectSpeechGlobalNumber )
        {
            if ( elements.AnyEx() )
            {
                elements.First().SetAttributeINDIRECTSPEECH_BEGIN( directAndIndirectSpeechGlobalNumber );
                elements.Last() .SetAttributeINDIRECTSPEECH_END  ( directAndIndirectSpeechGlobalNumber );
            }
        }
        public static void SetAttributeDIRECTSPEECH_BEGINEND_4Subsents( this IEnumerable< XElement > subsents, int directAndIndirectSpeechGlobalNumber )
        {
            if ( subsents.AnyEx() )
            {
                subsents.ForEachEx( _ => _.ThrowIfNotSubsent() );

                subsents.First().SetAttributeDIRECTSPEECH_BEGIN( directAndIndirectSpeechGlobalNumber ); 
                subsents.GetMostLastElement().SetAttributeDIRECTSPEECH_END( directAndIndirectSpeechGlobalNumber );
            }
        }
        public static void SetAttributeDIRECTSPEECH_BEGINEND( this IEnumerable< XElement > elements, int directAndIndirectSpeechGlobalNumber )
        {
            if ( elements.AnyEx() )
            {
                elements.First().SetAttributeDIRECTSPEECH_BEGIN( directAndIndirectSpeechGlobalNumber );
                elements.Last() .SetAttributeDIRECTSPEECH_END  ( directAndIndirectSpeechGlobalNumber );
            }
        }
    }
}