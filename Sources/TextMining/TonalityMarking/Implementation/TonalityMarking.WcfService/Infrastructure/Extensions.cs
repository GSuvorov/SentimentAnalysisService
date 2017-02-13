using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;

using log4net;
using TextMining.Core;

namespace TonalityMarking
{    
    using Consts = TextMining.Core.XLinqExtensions.Consts;

    /// <summary>
    /// 
    /// </summary>
    internal static partial class Extensions
    {
        private static string GetNoLongest( this string value, int maxLength )
        {
            if ( value.IsEmptyOrNull() || (value.Length <= maxLength) )
                return (value);

            if ( maxLength <= 3 ) maxLength += 3;
            return (value.Substring( 0, Math.Min(value.Length,  maxLength - 3) ) + "...");                
        }
        public static string Get4Log( this string value )
        {
            return (value.GetNoLongest( 100 ).InSingleQuote());
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
        public static T[] ToArrayEx< T >( this List< T > sequence )
        {
            if ( sequence.IsNotNull() )
                return (sequence.ToArray());
            return (null);
        }

        public static int ToInt32( this string value )
        {
            return (int.Parse( value ));
        }

        private static Int64 _TotalLogCount = 0;
#if DEBUG
        private static readonly object _SynRoot = new object();
#endif
        public static void InfoEx( this ILog log, string methodName, string clientRemoteAddress, TimeSpan linguisticElapsed, TimeSpan tonalityElapsed, InputParamsBase inputParams )
        {
            var value = Interlocked.Increment( ref _TotalLogCount ) + "]. " +
                        methodName + " => \r\n\t ClientRemoteAddress: "     + clientRemoteAddress +
                        "\r\n\t Linguistic processing time: "               + linguisticElapsed   +
                        "\r\n\t Tonality   processing time: "               + tonalityElapsed     +
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
        public static void InfoEx( this ILog log, string methodName, string clientRemoteAddress, TimeSpan tonalityElapsed, InputParamsBase inputParams )
        {
            var value = Interlocked.Increment( ref _TotalLogCount ) + "]. " +
                        methodName + " => \r\n\t ClientRemoteAddress: "     + clientRemoteAddress +
                        "\r\n\t Tonality processing time: "                 + tonalityElapsed     +
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
        public static void ErrorEx( this ILog log, string methodName, Exception ex, string clientRemoteAddress, InputParamsBase inputParams, XContainer xcontainer )
        {            
            var value = ("{0}]. ")                           .FormatEx( Interlocked.Increment( ref _TotalLogCount ) ) +
                        ("{0} => '{1}': '{2}'")              .FormatEx( methodName, ex.GetType().Name, ex.Message ) + 
                        ("\r\n\t ClientRemoteAddress: {0}")  .FormatEx( clientRemoteAddress          ) + 
                        ("\r\n\t Input text (length: {0}): ").FormatEx( inputParams.InputText.Length ) + 
                        inputParams.InputText.InSingleQuote() + 
                        ((xcontainer != null) ? ("\r\n\t XML: [\r\n{0}\r\n]").FormatEx( xcontainer.ToString() ) : string.Empty);

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
    }

    /// <summary>
    /// 
    /// </summary>
    internal static partial class XLinqExtensions
    {
        private const string FIRST_QUEUE_SUBSENT_ATTRIBUTE           = "FQS";
        private const string CREATED_BY_PARENTTWOCHILDRULE_ATTRIBUTE = "CBPTCR";

        public static void SetAttributeFIRSTQUEUESUBSENT( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            subsent.SetAttribute( FIRST_QUEUE_SUBSENT_ATTRIBUTE, bool.TrueString.ToUpper() );
        }
        public static bool HasAttributeFIRSTQUEUESUBSENT( this XElement subsent )
        {
            return (subsent.GetAttribute( FIRST_QUEUE_SUBSENT_ATTRIBUTE ) != null);
        }

        public static void SetAttributeCREATEDBYPARENTTWOCHILDRULE( this XElement element )
        {
            element.SetAttribute( CREATED_BY_PARENTTWOCHILDRULE_ATTRIBUTE, bool.TrueString.ToUpper() );
        }
        public static bool HasAttributeCREATEDBYPARENTTWOCHILDRULE( this XElement element )
        {
            return (element.GetAttribute( CREATED_BY_PARENTTWOCHILDRULE_ATTRIBUTE ) != null);
        }

        private static void SetAttributeSNTnFRT4SelfAndElements2Neutral( this XElement element )
        {
            element.SetAttributeSNTnFRT4SelfAndElements( TonalityType.nNeut );
        }
        private static void SetAttributeSNTnFRT4SelfAndElements( this XElement element, TonalityType? type )
        {
            if ( !element.IsElementT() && !element.IsElementUorCET() )
            {
                element.SetAttributeSNT( type.GetValueOrDefault( TonalityType.nNeut ) );
                element.SetAttributeFRT( 0                                            );//type.ToFRT() );

                foreach ( var child in element.ElementsExceptT() )
                {
                    child.SetAttributeSNTnFRT4SelfAndElements( type );
                }
            }
        }
        public static XElement CreateAndAddBeforeEntity4Collocation( this XElement element, Collocations collocationsType, int weight, RoleAttributeValue? role )
        {
            var entity = new XElement( Consts.ENTITY_ELEMENT );

            entity.SetAttributeTYPE( TypeAttributeValue.Collocation );
            entity.SetAttributeROLE( role                           );
            entity.SetAttributeSNT ( collocationsType               );
            entity.SetAttributeFRT ( weight                         );

            element.AddBeforeSelf( entity );

            return (entity);
        }
        public static XElement CreateAndAddBeforeVerb4Collocation( this XElement element, Collocations collocationsType, int weight )
        {
            var entity = new XElement( Consts.VERB_ELEMENT );

            entity.SetAttributeTYPE( TypeAttributeValue.Collocation );
            entity.SetAttributeSNT ( collocationsType               );
            entity.SetAttributeFRT ( weight                         );

            element.AddBeforeSelf( entity );

            return (entity);
        }
        private static void SetAttributeFIRSTITEMINCOLLOCATIONID( this XElement element, int? value )
        {
            if ( value.HasValue )
            {
                element.SetAttribute( Consts.FIRST_ITEM_IN_COLLOCATION_ID_ATTRIBUTE, value.Value.ToString() );
            }
        }
        public static void MoveIn2CollocationNode( this XElement entityElement, IList< CollocationProcessingItem > chainOfProcessingItems )
        {
            if ( !entityElement.IsElementENTITY() && !entityElement.IsElementVERB() )
                throw (new ArgumentException("!entityElement.IsElementENTITY() && !entityElement.IsElementVERB()"));

            if ( !chainOfProcessingItems.Any() )
                throw (new ArgumentException("!chainOfProcessingItems.Any()"));

            var first         = chainOfProcessingItems.First().XElement;
            var firstId       = first.GetAttributeID();
            var paretnOfFirst = first.Parent;

            foreach ( var processingItemInChain in chainOfProcessingItems )
            {
                processingItemInChain.XElement.SetAttributeSNTnFRT4SelfAndElements2Neutral();
                if ( paretnOfFirst == processingItemInChain.XElement.Parent )
                {
                processingItemInChain.XElement.Remove();
                entityElement.Add( processingItemInChain.XElement );
                }
                else //?!?!? if ( first != processingItemInChain.XElement.Parent )
                {
                processingItemInChain.XElement.SetAttributeFIRSTITEMINCOLLOCATIONID( firstId );
                }
            }
        }
        private static XElement CreateEmptyU()
        {
            var u = new XElement( Consts.U_ELEMENT );
            u.Add( new XCData( string.Empty ) );
            return (u);
        }
        /*private static bool IsElementBeforeSelfAreU( this XElement element )
        {
            var previous = element.ElementsBeforeSelf().LastOrDefault();
            return ((previous != null) && previous.IsElementU());
        }
        private static bool IsElementAfterSelfAreU( this XElement element )
        {
            var next = element.ElementsAfterSelf().FirstOrDefault();
            return ((next != null) && next.IsElementU());
        }*/
        public static void AddBeforeAndAfterEmptyU( this XElement element )
        {
            if ( element.IsElementU() )
                return;
            
            if ( element.Parent.IsNull() )
                return;

            if ( element.Parent.IsElementSUBSENT()  )
            {
                var previous = element.ElementsBeforeSelf().LastOrDefault();
                if ( (previous != null) && !previous.IsElementU() )
                //---if ( !element.IsElementBeforeSelfAreU() )
                {
                    element.AddBeforeSelf( CreateEmptyU() );
                }

                var next = element.ElementsAfterSelf().FirstOrDefault();
                if ( (next != null) && !next.IsElementU() )
                //---if ( !element.IsElementAfterSelfAreU() )
                {
                    element.AddAfterSelf( CreateEmptyU() );
                }
            } 
            else if ( element.HasAncestorHomogeneous() )
            {
                var previous = element.ElementsBeforeSelf().LastOrDefault();
                if ( (previous != null) && !previous.IsElementU() )
                {
                    element.AddBeforeSelf( CreateEmptyU() );
                }
            }
        }
        public static XElement CreateEntitiesFromSubsent( this XElement subsent )
        {
            subsent.ThrowIfNotSubsent();

            var ID   = subsent.Attribute( Consts.ID_ATTRIBUTE   );
            var TYPE = subsent.Attribute( Consts.TYPE_ATTRIBUTE );
            var SNT  = subsent.Attribute( Consts.SNT_ATTRIBUTE  );
            var FRT  = subsent.Attribute( Consts.FRT_ATTRIBUTE  );

            ID  .ThrowIfNull( Consts.ID_ATTRIBUTE   );
            TYPE.ThrowIfNull( Consts.TYPE_ATTRIBUTE );            
            if ( SNT == null ) SNT = new XAttribute( Consts.SNT_ATTRIBUTE, TonalityType.nNeut.ToText() );
            if ( FRT == null ) FRT = new XAttribute( Consts.FRT_ATTRIBUTE, 0 );

            ID   = new XAttribute( Consts.ID_ATTRIBUTE  , ID  .Value );
            TYPE = new XAttribute( Consts.TYPE_ATTRIBUTE, TYPE.Value );
            SNT  = new XAttribute( Consts.SNT_ATTRIBUTE , SNT .Value );
            FRT  = new XAttribute( Consts.FRT_ATTRIBUTE , FRT .Value );

            var newEntities = new XElement( Consts.ENTITIES_ELEMENT, ID, TYPE, SNT, FRT );

            return (newEntities);
        }
        public static XElement CreateEntitiesFromSubsentPartial( this XElement subsent, TonalityType snt, int frt )
        {
            subsent.ThrowIfNotSubsent();

            var ID   = subsent.Attribute( Consts.ID_ATTRIBUTE   );
            var TYPE = subsent.Attribute( Consts.TYPE_ATTRIBUTE );

            ID  .ThrowIfNull( Consts.ID_ATTRIBUTE   );
            TYPE.ThrowIfNull( Consts.TYPE_ATTRIBUTE );            

            ID   = new XAttribute( Consts.ID_ATTRIBUTE  , ID  .Value );
            TYPE = new XAttribute( Consts.TYPE_ATTRIBUTE, TYPE.Value );

            var newEntities = new XElement( Consts.ENTITIES_ELEMENT, ID, TYPE );
            newEntities.SetAttributeSNT( snt );
            newEntities.SetAttributeFRT( frt );

            return (newEntities);
        }
        public static void AddAfterSelfWithProvision4Collocation( this XElement entity, XElement fakeEntities )
        {            
            if ( !entity.IsElementENTITY() )
                throw (new ArgumentException("!entity.IsElementENTITY()"));
            if ( !fakeEntities.IsElementENTITIES() )
                throw (new ArgumentException("!fakeEntities.IsElementENTITIES()"));

            var oneLevelAncestor = (from ancestor in entity.AncestorsAndSelf()
                                    where ancestor.IsElementCET() ||
                                          ancestor.IsElementENTITIES() ||
                                          ancestor.IsElementENTITY() ||
                                          ancestor.IsElementVERB()
                                    where ( (ancestor.Parent != null) && ancestor.Parent.IsElementSUBSENT() )
                                    select ancestor
                                   ).FirstOrDefault();
            if ( oneLevelAncestor != null )
            {
                entity = oneLevelAncestor;
            }

            entity.AddAfterSelf( fakeEntities );

            #region previous. commented
            /*var collocationAncestors = from ancestor in entity.Ancestors()
                                       where ancestor.IsElementCET() ||
                                             ancestor.IsElementENTITIES() ||
                                             ancestor.IsElementENTITY() ||
                                             ancestor.IsElementVERB()
                                       where ( ancestor.GetAttributeTYPE() == TypeAttributeValue.Collocation )
                                       select ancestor;
            var collocationAncestor = collocationAncestors.FirstOrDefault();
            if ( collocationAncestor != null )
            {
                entity = collocationAncestor;
            }

            entity.AddAfterSelf( fakeEntities );*/
            #endregion
        }
        public static void CreateFakeEntityAfterSelf( this XElement cet, XElement entityBound, bool preventSetInquiryType = true )
        {
            if ( !cet.IsElementCET() )
                throw (new ArgumentException("!cet.IsElementCET()"));
            if ( !entityBound.IsElementENTITY() )
                throw (new ArgumentException("!entityBound.IsElementENTITY()"));

            var type = entityBound.GetAttributeTYPE().GetValueOrDefault( TypeAttributeValue.Unknown ); 
            if ( preventSetInquiryType && (type == TypeAttributeValue.Inquiry) )
            {
                type = TypeAttributeValue.Unknown;
            }

            var entity = new XElement( Consts.ENTITY_ELEMENT );
            entity.SetAttributeROLE( cet.GetAttributeROLE()        );
            entity.SetAttributeTYPE( type                          );
            entity.SetAttributeSNT ( entityBound.GetAttributeSNT() );
            entity.SetAttributeFRT ( entityBound.GetAttributeFRT() );

            cet.AddAfterSelf( CreateEmptyU(), entity );
        }
        public static void CreateFakeEntityBeforeSelf( this XElement element, XElement entityBound )
        {
            if ( !entityBound.IsElementENTITY() )
                throw (new ArgumentException("!entityBound.IsElementENTITY()"));

            var entity = new XElement( Consts.ENTITY_ELEMENT );
            entity.SetAttributeROLE( entityBound.GetAttributeROLE()                                                 );
            entity.SetAttributeTYPE( entityBound.GetAttributeTYPE().GetValueOrDefault( TypeAttributeValue.Unknown ) );
            entity.SetAttributeSNT ( entityBound.GetAttributeSNT()                                                  );
            entity.SetAttributeFRT ( entityBound.GetAttributeFRT()                                                  );

            if ( element.IsElementU() )
            {
                if ( element.ElementsBeforeSelf().Any() )
                    element.AddBeforeSelf( CreateEmptyU(), entity );
                else
                    element.AddBeforeSelf( entity );
            }
            else
                element.AddBeforeSelf( entity, CreateEmptyU() );
        }

        #region [.XPath's.]
        private static string Xpath4Sents( this Language language )
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
        private static string Xpath4Collocation( this Language language )
        {
            string COLLOCATION_XPATH = "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//ENTITY[ @TYPE != '" + TypeAttributeValue.Collocation + "' ] | " +
                                       "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//VERB | " +
                                       "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//U | " +
                                       "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//CET";
            switch ( language.LanguageType )
            {
                case LanguageType.RU:
                    return (COLLOCATION_XPATH.FormatEx( LanguageType.RU.ToString() ));

                default:
                    throw (new ArgumentException(language.LanguageType.ToString()));
            }
        }
        private static string Xpath4ExplosiveCollocation( this Language language )
        {
            string EXPLOSIVE_COLLOCATION_XPATH =
                "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//ENTITY[ @FIRST-ITEM-IN-COLLOCATION-ID and not(ancestor::*[ @FIRST-ITEM-IN-COLLOCATION-ID ]) ] | " +
                "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//ENTITIES[ @FIRST-ITEM-IN-COLLOCATION-ID and not(ancestor::*[ @FIRST-ITEM-IN-COLLOCATION-ID ]) ] | " +
                "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//VERB[ @FIRST-ITEM-IN-COLLOCATION-ID and not(ancestor::*[ @FIRST-ITEM-IN-COLLOCATION-ID ]) ]";

            switch ( language.LanguageType )
            {
                case LanguageType.RU:
                    return (EXPLOSIVE_COLLOCATION_XPATH.FormatEx( LanguageType.RU.ToString() ));

                default:
                    throw (new ArgumentException(language.LanguageType.ToString()));
            }
        }
        private static string Xpath4LexemesVerb( this Language language )
        {
            string LEXEMES_VERB_XPATH = 
                "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//VERB[ @TYPE != '" + TypeAttributeValue.Collocation + "' and " +
                                                          "not(@FIRST-ITEM-IN-COLLOCATION-ID) and " +
                                                          //"(parent::SUB_SENT or parent::VERB) and " + 
                                                          "not(ancestor::ENTITY[ @TYPE='" + TypeAttributeValue.Collocation + "' ]) ]";

            switch ( language.LanguageType )
            {
                case LanguageType.RU:
                    return (LEXEMES_VERB_XPATH.FormatEx( LanguageType.RU.ToString() ));

                default:
                    throw (new ArgumentException(language.LanguageType.ToString()));
            }
        }
        private static string Xpath4LexemesEntity( this Language language )
        {
            string LEXEMES_ENTITY_XPATH = 
                "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//ENTITY[" + //@TYPE != '" + TypeAttributeValue.Inquiry + "' and " + 
                                                            "(not(@SNT) or @SNT = '" + TonalityType.nNeut + "') and " +
                                                            "@TYPE != '" + TypeAttributeValue.Collocation + "' and " +
                                                            "not(@FIRST-ITEM-IN-COLLOCATION-ID) and " +
                                                            "not(ancestor::ENTITY[ @TYPE = '" + TypeAttributeValue.Collocation + "']) ]";

            switch ( language.LanguageType )
            {
                case LanguageType.RU:
                    return (LEXEMES_ENTITY_XPATH.FormatEx( LanguageType.RU.ToString() ));

                default:
                    throw (new ArgumentException(language.LanguageType.ToString()));
            }
        }
        private static string Xpath4AllLevelSubsentences(this Language language)
        {
            const string ALL_LEVEL_SUBSENTS_XPATH = "TEXT/SENT[ @LANG='{0}' ]//SUB_SENT";

            switch ( language.LanguageType )
            {
                case LanguageType.RU:
                    return (ALL_LEVEL_SUBSENTS_XPATH.FormatEx(LanguageType.RU.ToString()));

                default:
                    throw (new ArgumentException(language.LanguageType.ToString()));
            }
        }
        private static string Xpath4NERLexemesEntity( this Language language )
        {
            string NER_LEXEMES_ENTITY_XPATH = 
"TEXT/SENT[ @LANG='{0}' ]//SUB_SENT//ENTITY[".FormatEx( LanguageType.RU.ToString() ) +
                                            "(not(@SNT) or @SNT = '{0}') and ".FormatEx( TonalityType.nNeut ) +
                                            "not(@FIRST-ITEM-IN-COLLOCATION-ID) and " +
                                            "not(ancestor::ENTITY[ @TYPE='{0}' ]) and ".FormatEx( TypeAttributeValue.Collocation ) + 
                                            "((@PN and (@TYPE='{0}' or @TYPE='{1}' or @TYPE='{2}' or @TYPE-SAVED='{0}' or @TYPE-SAVED='{1}' or @TYPE-SAVED='{2}')) or (@TYPE='{3}' or @TYPE-SAVED='{3}')) ]"
                                             .FormatEx( TypeAttributeValue.PersonName, 
                                                        TypeAttributeValue.PersonInfo, 
                                                        TypeAttributeValue.PersonNoInfo, 
                                                        TypeAttributeValue.OrgName );

            switch ( language.LanguageType )
            {
                case LanguageType.RU:
                    return (NER_LEXEMES_ENTITY_XPATH);

                default:
                    throw (new ArgumentException(language.LanguageType.ToString()));
            }
        }
        /*private static string Xpath4TopLevelSubsentences( this Language language )
        {
            const string TOP_LEVEL_SUBSENTS_XPATH = "TEXT/SENT[ @LANG='{0}' ]/SUB_SENT";
            switch ( language.LanguageType )
            {
                case LanguageType.RU:
                    return (TOP_LEVEL_SUBSENTS_XPATH.FormatEx( LanguageType.RU.ToString() ));

                default:
                    throw (new ArgumentException(language.LanguageType.ToString()));
            }
        }*/
        #endregion

        public static IEnumerable< XElement > GetCollocations( this XDocument xdocument, Language language )
        {
            return (xdocument.XPathSelectElements( language.Xpath4Collocation() ));
        }
        public static IEnumerable< XElement > GetExplosiveCollocations( this XDocument xdocument, Language language )
        {
            return (xdocument.XPathSelectElements( language.Xpath4ExplosiveCollocation() ));
        }
        public static IEnumerable< XElement > GetLexemesVerb( this XDocument xdocument, Language language )
        {
            return (xdocument.XPathSelectElements( language.Xpath4LexemesVerb() ));
        }
        public static IEnumerable< XElement > GetLexemesEntity( this XDocument xdocument, Language language )
        {
            return (xdocument.XPathSelectElements( language.Xpath4LexemesEntity() ));
        }        
        public static IEnumerable< XElement > GetAllProcessingElementsInSubsent( this XElement subsent )
        {
            const string ALL_PROCESSING_ELEMENTS_IN_SUBSENT_XPATH = "U | ENTITY | ENTITIES | VERB | CET";

            //---subsent.ThrowIfNotSubsent();

            return (subsent.XPathSelectElements( ALL_PROCESSING_ELEMENTS_IN_SUBSENT_XPATH ));
        }
        public static IEnumerable< XElement > GetSents( this XDocument xd, Language language )
        {
            return (xd.XPathSelectElements( language.Xpath4Sents() ));
        }
        public static IEnumerable< XElement > GetAllLevelInquiriesWithSNTFINAL( this XDocument xd, Language language )
        {
            return (xd.XPathSelectElements( language.Xpath4AllLevelSubsentences() + "//*[ @TYPE = 'Inquiry' and @SNTFINAL ]"));
        }
        public static IEnumerable< XElement > GetAllLevelSubsentences( this XDocument xd, Language language )
        {
            return (xd.XPathSelectElements( language.Xpath4AllLevelSubsentences() ));
        }
        public static IEnumerable< XElement > GetNERLexemesEntity( this XDocument xdocument, Language language )
        {
            return (xdocument.XPathSelectElements( language.Xpath4NERLexemesEntity() ));
        }
        /*public static IEnumerable< XElement > GetTopLevelSubsentences( this XDocument xd, Language language )
        {
            return (xd.XPathSelectElements( language.Xpath4TopLevelSubsentences() ));
        }
        public static IEnumerable< XElement > GetAllLevelInitialInquiries( this XDocument xd, Language language )
        {
            return (xd.XPathSelectElements( language.Xpath4AllLevelSubsentences() + "//*[ @INQUIRYINITIAL = 'TRUE' ]"));
        }*/
        ////////////////////////////////////////////////////////////////////
                  
        public static void RenameSentence4Unprocessing( this XElement sentence )
        {
            if ( !sentence.IsElementSENT() )
                throw (new ArgumentException("!subsentence.IsElementSENT()"));

            sentence.Name = Consts.SKIP_SENT_ELEMENT;
        }
        private static void RenameUnprocessingSentenceBack( this XElement sentence )
        {
            if ( !sentence.IsElement_SKIP_SENT() )
                throw (new ArgumentException("!subsentence.IsElementSKIPSENT()"));

            sentence.Name = Consts.SENT_ELEMENT;
        }
        public static void RenameUnprocessingSentenceBack( this XDocument document, Language language )
        {
            var skipSubsentences = document.XPathSelectElements( language.Xpath4Sents()
                                    .Replace( '/' + Consts.SENT_ELEMENT, '/' + Consts.SKIP_SENT_ELEMENT ) ).ToArray();

            foreach ( var skipSubsentence in skipSubsentences )
            {
                skipSubsentence.RenameUnprocessingSentenceBack();
            }
        }
        public static void DeleteUnprocessingSentence( this XDocument document, Language language )
        {
            var skipSubsentences = document.XPathSelectElements( language.Xpath4Sents()
                                    .Replace( '/' + Consts.SENT_ELEMENT, '/' + Consts.SKIP_SENT_ELEMENT ) ).ToArray();

            foreach ( var skipSubsentence in skipSubsentences )
            {
                skipSubsentence.Remove();
            }
        }

        public static int GetPositionInParent( this XElement element )
        {
            if ( element.Parent == null )
                return (-1);

            return (element.Parent.Elements( element.Name ).ToList().IndexOf( element ) + 1);
        }

        public static void CopySNTnFRT2FINAL( this XElement element )
        {
            var snt = element.Attribute( Consts.SNT_ATTRIBUTE );           
            if ( snt != null )
            {
                element.SetAttributeSNTFINAL( snt.Value.ToEnum< TonalityType >() );
                //snt.Remove();
            }
            else
            {
                element.SetAttributeSNT     ( TonalityType.nNeut );
                element.SetAttributeSNTFINAL( TonalityType.nNeut );
            }

            var frt = element.Attribute( Consts.FRT_ATTRIBUTE );
            if ( frt != null )
            {
                element.SetAttributeFRTFINAL( frt.Value.TryConvert2Int().Value );
                //frt.Remove();
            }
            else
            {
                element.SetAttributeFRT     ( 0 );
                element.SetAttributeFRTFINAL( 0 );
            }
        }
        public static void DeleteAllSNTFINALnFRTFINAL( this XElement element )
        {
            var sntfrtFinals = element.GetAttributeNodes( ".//@" + Consts.SNTFINAL_ATTRIBUTE + " | " + 
                                                          ".//@" + Consts.FRTFINAL_ATTRIBUTE );
            foreach ( var sntfrtFinal in sntfrtFinals )
            {
                sntfrtFinal.Remove();
            }
        }

        public static void MoveIn2ThisNodesRange( this XElement parentNode, XElement startRangeNode, XElement endRangeNode )
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

        public static bool IsSentEndsWithQuestionMark( this XElement sent )
        {sent.ThrowIfNotSent();

            /*если предложении оканчивается на вопросительный знак (?), 
              вопросительный знак с точками (?.), 
              с запятыми (?,), 
              с двоеточиями (?:),
              точкой с запятой (?;) 
              и со звездочками (?*) – то все это вопросительные предложения.
            */
            var last = sent.Descendants().LastOrDefault();
            return (last.IsNotNull() ? last.Value.Trim( ' ', '\r', '\n', '\t', 
                                                        '.', ',', ':', ';', '*' ).EndsWith( "?" ) : false);

            #region
            //var lastSubsent = sent.ElementsSubsentence().LastOrDefault();

            //var last = (lastSubsent ?? sent).Elements().LastOrDefault();
            //if ( last.IsNull() )
            //    return (false);

            //return (last.Value.TrimWhiteSpaces().EndsWith( "?" )); 
            #endregion
        }

        public static XElement TryGetSingleElement( this XElement element )
        {
            var i = 0;
            var first = default(XElement);
            foreach ( var e in element.Elements() )
            {
                switch ( ++i )
                {
                    case 1: first = e; break;
                    default:  return (null);
                }
            }
            return (first);
        }

        public static readonly string HOMOGENEOUS_VALUE = TypeAttributeValue.Homogeneous.ToString();
        public static readonly string INQUIRY_VALUE     = TypeAttributeValue.Inquiry    .ToString();        
        private static bool HasSelfOrAncestorHomogeneous( this XElement element )
        {           
            foreach ( var a in element.AncestorsAndSelf() )
            {
                if ( (a.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE       ) == HOMOGENEOUS_VALUE) ||
                     (a.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_SAVED_ATTRIBUTE ) == HOMOGENEOUS_VALUE)                                 
                   )
                {
                    return (true);
                }
            }
            return (false);
        }
        public static bool HasSelfOrAncestorHomogeneous( this XElement element, bool isInSubsentScope )
        {
            if ( isInSubsentScope )    
            {
                return (element.HasSelfOrAncestorHomogeneous());
            }
            return (false);
        }
        private static bool HasSelfOrAncestorHomogeneous( this List< FlatChainRuleProcessingItem > itemsChain )
        {
            foreach ( var it in itemsChain )
            {
                if ( it.XElement.HasSelfOrAncestorHomogeneous() )
                    return (true);
            }
            return (false);
        }
        public static bool HasSelfOrAncestorHomogeneous( this List< FlatChainRuleProcessingItem > itemsChain, bool isInSubsentScope )
        {
            if ( isInSubsentScope )    
            {
                return (itemsChain.HasSelfOrAncestorHomogeneous());
            }
            return (false);
        }

        private static bool HasAncestorHomogeneous( this XElement element )
        {           
            foreach ( var a in element.Ancestors() )
            {
                if ( (a.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE       ) == HOMOGENEOUS_VALUE) ||
                     (a.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_SAVED_ATTRIBUTE ) == HOMOGENEOUS_VALUE)                                 
                   )
                {
                    return (true);
                }
            }
            return (false);
        }
        public static bool HasAncestorHomogeneous( this XElement element, bool isInSubsentScope )
        {
            if ( isInSubsentScope )    
            {
                return (element.HasAncestorHomogeneous());
            }
            return (false);
        }
        private static bool HasAncestorHomogeneous( this List< FlatChainRuleProcessingItem > itemsChain )
        {
            foreach ( var it in itemsChain )
            {
                if ( it.XElement.HasAncestorHomogeneous() )
                    return (true);
            }
            return (false);
        }
        public static bool HasAncestorHomogeneous( this List< FlatChainRuleProcessingItem > itemsChain, bool isInSubsentScope )
        {
            if ( isInSubsentScope )    
            {
                return (itemsChain.HasAncestorHomogeneous());
            }
            return (false);
        }

        public static XElement TryGetFirstINQUIRYINITIALfromDescendant( this XElement element )
        {
            foreach ( var d in element.Descendants() )
            {
                if ( d.IsINQUIRYINITIAL() )
                    return (d);
            }
            return (null);
        }        

        /*public static Nouns GetSNT4ProcessLexemesEntity( this Nouns snt, XElement element, TypeAttributeValue? type )
        {
            /*
            При выборе объекта тональности: если ОМ имеет одушевленность, 
            то значение его тональности по умолчанию nPPos, если слово не присутствует в тональных словарях. 
            Значение одушевленности смотреть MI=”***A*” – четвертая буква в MI==А.
            То есть, проверили по словарям, если ОМ SNT=nNeut & MI(4)==A   => SNT=nPPos                                
            * /
            if ( snt == Nouns.nNeut )
            {
                if ( (type == TypeAttributeValue.PersonName) 
                        ||
                     (element.IsTypeInquiry() /*node.IsINQUIRYINITIAL()* / &&
                      element.MIhasCharAtPosition( 'A', 4 ))
                   )
                {
                    snt = Nouns.nPPos;
                }
            }
            return (snt);
        }
        */
    }
}