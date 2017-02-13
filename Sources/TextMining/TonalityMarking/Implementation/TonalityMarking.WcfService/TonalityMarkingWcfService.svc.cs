using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.ServiceModel;
using System.ServiceModel.Activation;
using System.Xml.Linq;

#if !WITH_INPROC_LINGVISTIC
using CoreferenceResolving;
#endif

using Linguistics.Coreference;
using TextMining.Core;
using TextMining.Service;
using TonalityMarking.Blogs;
using TonalityMarking.Socview;
using log4net;
[assembly: log4net.Config.XmlConfigurator(ConfigFile = "TonalityMarkingWcfService_log4net.xml", Watch = false)]

namespace TonalityMarking
{
    #region [.wcf-service attribute's.]
    // NOTE: If you change the class name "TonalityMarkingWcfService" here, you must also update the reference to "TonalityMarkingWcfService" in Web.config and in the associated .svc file.
    [AspNetCompatibilityRequirements(RequirementsMode = AspNetCompatibilityRequirementsMode.Allowed)]
    [ServiceBehavior
    (
               ConcurrencyMode = ConcurrencyMode.Multiple,
           InstanceContextMode = InstanceContextMode.Single, //.PerCall,
         MaxItemsInObjectGraph = int.MaxValue,
IncludeExceptionDetailInFaults = true
   //UseSynchronizationContext = false,
    )]
    #endregion
    public class TonalityMarkingWcfService : ITonalityMarking
    {
        #region [.Static field's & properties.]
        internal static readonly ILog LOG = LogManager.GetLogger( typeof(TonalityMarkingWcfService).Name );
        private static readonly ICoreferenceResolver _CoreferenceResolver;
        #endregion

        #region [.ctor().]
        static TonalityMarkingWcfService()
        {
            OperationContext.Current.AdjustBinding4AllEndpoints();

#if WITH_INPROC_LINGVISTIC
            Config.ExecuteConfigure();
#else
            _CoreferenceResolver = new CoreferenceResolver();
#endif

            #region [.check fusking linguistic-enum-types.]
            if ( Enum.GetValues( typeof(TypeAttributeValue) ).Length !=
                 Enum.GetValues( typeof(TypeAttributeValue) ).Cast< TypeAttributeValue >().Select( _ => (int)_ ).Distinct().Count() )
            {
                throw (new InvalidOperationException("[FUSKING-LINGUISTIC CHANGED 'EntityType'-ENUM]"));
            }
            if ( Enum.GetValues( typeof(RoleAttributeValue) ).Length !=
                 Enum.GetValues( typeof(RoleAttributeValue) ).Cast< RoleAttributeValue >().Select( _ => (int)_ ).Distinct().Count() )
            {
                throw (new InvalidOperationException("[FUSKING-LINGUISTIC CHANGED 'EntityRole'-ENUM]"));
            }
            #endregion
        }
        #endregion

        #region [.Set from outside Config-Section.]
        public static void SetConfigSection( XDocument section )
        {
            Config.SetConfigSection( section );
        }
        public static string ConfigSectionName
        {
            get { return (Config.ConfigSectionName); }
        }        
        #endregion

        #region [.ITonalityMarking.]
        public TonalityMarkingOutputResult ExecuteTonalityMarking( TonalityMarkingInputParams inputParams )
        {
            var xdocument = default(XDocument);
            try
            {
                #region [.check input.]
                inputParams.ThrowIfNull( "inputParams" );
                #endregion   

                #region [.Linguistic.]
                var linguisticStopwatch = Stopwatch.StartNew();
                switch ( inputParams.InputTextFormat )
                {
                    case InputTextFormat.PlainText:
                        xdocument = LinguisticsDecorator.ProcessText( inputParams );
                        if ( !xdocument.Root.HasElements ) throw (new ArgumentException("Linguistics returned empty xml."));
                    break;
                    case InputTextFormat.LinguisticXml:
                        xdocument = Algorithms.PreprocessRdf( inputParams.InputText );
                        if ( !xdocument.Root.HasElements ) throw (new ArgumentException("Input Linguistic-xml is empty."));
                    break;
                    default: 
                        throw (new ArgumentException(inputParams.InputTextFormat.ToString()));
                }                
                linguisticStopwatch.Stop();
                #endregion

                #region [.read coreference-info from Rdf.]
                var coreferenceInfo = _CoreferenceResolver.ReadFromRdf( xdocument.Root );
                #endregion

                #region [.Tonality-marking.]
                var tonalityStopwatch = Stopwatch.StartNew();
                
                var language = Config.ThemesManager[ inputParams.ThemeType ].LanguagesManager[ inputParams.LanguageType ];

                var allocResult = AllocateInquiry( xdocument, language, inputParams );

                ExecuteInternal( xdocument, language );

                if ( allocResult.Second )
                {
                    xdocument.RenameUnprocessingSentenceBack( language );
                }

                tonalityStopwatch.Stop();
                #endregion

                #region [.write in log.]
                LOG.InfoEx( MethodBase.GetCurrentMethod().Name, WcfHelper.ClientRemoteAddress, linguisticStopwatch.Elapsed, tonalityStopwatch.Elapsed, inputParams );
                #endregion

                #region [.result.]
                return (CreateTonalityOutputResult( xdocument, language, allocResult.First, inputParams.ObjectAllocateMethod, coreferenceInfo )); 
                #endregion
            }
            catch ( Exception ex )
            {
                #region [.write error in log & rethrow.]
                LOG.ErrorEx( MethodBase.GetCurrentMethod().Name, ex, WcfHelper.ClientRemoteAddress, inputParams, xdocument ); 
                throw; 
                #endregion
            }
        }


        //[.Blog's processing.]
        public TonalityMarkingOutputResult4Blogs ExecuteTonalityMarking4Blogs( TonalityMarkingInputParams4Blogs inputParams )
        {
            inputParams.ThrowIfNull("inputParams");

            return (BlogsProcessing.Processing( this, inputParams ));
        }

        //[.Socview processing.]
        public SocviewProcessingData ExecuteTonalityMarking4Socview( SocviewProcessingData socviewData )
        {
            socviewData.ThrowIfNull("socviewData");

            return (SocviewProcessing.Processing( this, socviewData ));
        }

        internal Tuple< XDocument, ICoreferenceInfo > ExecuteTonalityMarking4SocviewCallback( string plainText )
        {
            var inputParams = new TonalityMarkingInputParams( plainText );
            var xdocument = default(XDocument);
            try
            {
                #region [.check input.]
                plainText.ThrowIfEmptyOrNull("plainText");
                #endregion   

                #region [.Linguistic.]
                var linguisticStopwatch = Stopwatch.StartNew();
                switch ( inputParams.InputTextFormat )
                {
                    case InputTextFormat.PlainText:
                        xdocument = LinguisticsDecorator.ProcessText( inputParams );
                        if ( !xdocument.Root.HasElements ) throw (new ArgumentException("Linguistics returned empty xml."));
                    break;
                    case InputTextFormat.LinguisticXml:
                        xdocument = Algorithms.PreprocessRdf( inputParams.InputText );
                        if ( !xdocument.Root.HasElements ) throw (new ArgumentException("Input Linguistic-xml is empty."));
                    break;
                    default: 
                        throw (new ArgumentException(inputParams.InputTextFormat.ToString()));
                }                
                linguisticStopwatch.Stop();            
                #endregion

                #region [.read coreference-info from Rdf.]
                var coreferenceInfo = _CoreferenceResolver.ReadFromRdf( xdocument.Root );
                #endregion

                #region [.Tonality-marking.]
                var tonalityStopwatch = Stopwatch.StartNew();
                
                var language = Config.ThemesManager[ inputParams.ThemeType ].LanguagesManager[ inputParams.LanguageType ];

                var allocResult = AllocateInquiry( xdocument, language, inputParams );

                ExecuteInternal( xdocument, language );

                if ( allocResult.Second )
                {
                    xdocument.RenameUnprocessingSentenceBack( language );
                }

                tonalityStopwatch.Stop();
                #endregion

                #region [.write in log.]
                LOG.InfoEx( MethodBase.GetCurrentMethod().Name, WcfHelper.ClientRemoteAddress, linguisticStopwatch.Elapsed, tonalityStopwatch.Elapsed, inputParams );
                #endregion

                #region [.result.]
                return (Tuple.Create( xdocument, coreferenceInfo )); 
                #endregion
            }
            catch ( Exception ex )
            {
                #region [.write error in log & rethrow.]
                LOG.ErrorEx( MethodBase.GetCurrentMethod().Name, ex, WcfHelper.ClientRemoteAddress, inputParams, xdocument ); 
                throw; 
                #endregion
            }
        }
        #endregion

        #region [.4 use only from DIGEST-wcf-service. use as-inprocess in DIGEST-wcf-service.]
        /// <summary>
        /// 
        /// </summary>
        public sealed class OutputResult4DigestWcfService
        {
            public OutputResult4DigestWcfService( XElement sentence, double? positive, double? negative )
            {
                sentence.ThrowIfNotSent();

                Sentence  = sentence;
                Positive  = positive;
                Negative  = negative;
            }

            public XElement Sentence
            {
                get;
                private set;
            }
            public double?  Positive
            {
                get;
                private set;
            }
            public double?  Negative
            {
                get;
                private set;
            }
        }

        #region commented. CompactOutputXmlFormatConverter
        /*private class OutputXmlFormatFullTemporary : IDisposable
        {
            private InputParamsBase _InputParams;
            private OutputXmlFormat _OutputXmlFormat;

            public OutputXmlFormatFullTemporary( InputParamsBase inputParams )
            {
                _InputParams = inputParams;
                if ( _InputParams != null )
                {
                    _OutputXmlFormat = _InputParams.OutputXmlFormat;
                    _InputParams.OutputXmlFormat = OutputXmlFormat.Full;
                }
            }

                #region IDisposable
            public void Dispose()
            {
                if ( _InputParams != null )
                {
                    _InputParams.OutputXmlFormat = _OutputXmlFormat;
                    _InputParams = null;
                }
            }
                #endregion
        }*/
        #endregion

        public static OutputResult4DigestWcfService ExecuteTonalityMarking4DigestWcfService( XElement sent, InputParamsBase inputParams )
        {
            try
            {
                #region [.check input.]
                sent.ThrowIfNotSent();
                inputParams.ThrowIfNull( "inputParams" );
                #endregion               

                #region [.prepare data.]
                var sentenceText = new SentenceText( sent );
                #endregion

                #region [.Tonality-marking.]
                var tonalityStopwatch = Stopwatch.StartNew();
                
                var language = Config.ThemesManager[ inputParams.ThemeType ].LanguagesManager[ inputParams.LanguageType ];

                ExecuteInternal( sentenceText.XDocument, language );

                tonalityStopwatch.Stop();
                #endregion

                #region [.write in log.]
                LOG.InfoEx( MethodBase.GetCurrentMethod().Name, WcfHelper.ClientRemoteAddress, tonalityStopwatch.Elapsed, inputParams );
                #endregion

                #region [.result.]
                /*var result = CreateResult( sentenceText.XDocument, language, inputParams );*/
                var result = CreateResult4DigestWcfService( sentenceText.XDocument, language, inputParams );

                return (new OutputResult4DigestWcfService( SentenceText.GetSent( result.XDocument ), result.Positive, result.Negative ));
                #endregion
            }
            catch ( Exception ex )
            {
                #region [.write error in log & rethrow.]
                LOG.ErrorEx( MethodBase.GetCurrentMethod().Name, ex, WcfHelper.ClientRemoteAddress, inputParams, sent ); 
                throw; 
                #endregion
            }
        }

        private static IntermediateResult CreateResult4DigestWcfService( XDocument xdocument, Language language, InputParamsBase inputParams )
        {
            /*
            Если сила тональности предложения равна нулю (FRT=0), отдаем цифру 0.1
            Если тональность предложения равна единице (FRT=1), отдаем цифру 0.5
            Если больше единицы (1<FRT),  - отдаем 0.9              
            */
            #region [.positive & negative.]
            const double FRT_0      = 1; //0.1;
            const double FRT_1      = 2; //0.5;
            const double FRT_MORE_1 = 3; //0.9;

            double? positive = null;
            double? negative = null;

            var inquiriesSNTFinal = xdocument.GetAllLevelInquiriesWithSNTFINAL( language ).ToArray();
            if ( inquiriesSNTFinal.Any() )
            {
                #region [.negative.]
                var negative_frt_finals = (from inquiry in inquiriesSNTFinal
                                           where inquiry.IsSNTFINALNegative()
                                           let frt = inquiry.GetAttributeFRTFINAL()
                                           select frt
                                          ).ToArray();
                if ( negative_frt_finals.Length != 0 )
                {
                    var negative_frt_final = negative_frt_finals.Max();
                    switch ( negative_frt_final )
                    {
                        case 0: negative = FRT_0; break;
                        case 1: negative = FRT_1; break;
                        default: if ( 1 < negative_frt_final ) 
                                 negative = FRT_MORE_1; break;
                    }
                }                
                #endregion

                #region [.positive.]
                var positive_frt_finals = (from inquiry in inquiriesSNTFinal
                                           where inquiry.IsSNTFINALPositive()
                                           let frt = inquiry.GetAttributeFRTFINAL()
                                           select frt
                                          )
                                          .ToArray();
                if ( positive_frt_finals.Length != 0 )
                {
                    var positive_frt_final = positive_frt_finals.Max();
                    switch ( positive_frt_final )
                    {
                        case 0: positive = FRT_0; break;
                        case 1: positive = FRT_1; break;
                        default: if ( 1 < positive_frt_final ) 
                                 positive = FRT_MORE_1; break;
                    }   
                }
                #endregion
            }
            #endregion

            #region commented. CompactOutputXmlFormatConverter
            /*
            #region [.to-compact.]
            if ( inputParams.OutputXmlFormat == OutputXmlFormat.Compact )
            {
                xdocument = Config.CompactOutputXmlFormatConverter.Convert( xdocument );
            } 
            #endregion
            */
            #endregion

            return (new IntermediateResult( xdocument, positive, negative ));
        }
        #endregion

        #region [.4 use only from 'LingvisticsWorkProcess-with-OM-TM'. use as-inprocess in 'LingvisticsWorkProcess-with-OM-TM'.]
        public static TonalityMarkingOutputResult ExecuteTonalityMarkingInprocWithLinguisticService( 
            XDocument rdf, ICoreferenceInfo coreferenceInfo, ObjectAllocateMethod objectAllocateMethod, 
            TonalityMarkingInputParams4InProcess inputParams )            
        {
            #region [.check input.]
            inputParams.ThrowIfNull( "inputParams" );
            #endregion

            #region [.process.]
            var language = Config.ThemesManager[ inputParams.ThemeType ].LanguagesManager[ inputParams.LanguageType ];

            rdf = Algorithms.PreprocessRdf( rdf );

            var allocResult = AllocateInquiry( rdf, language, inputParams.InquiriesSynonyms, objectAllocateMethod );

            ExecuteInternal( rdf, language );

            if ( allocResult.Second )
            {
                rdf.RenameUnprocessingSentenceBack( language );
            }
            #endregion

            #region [.result.]
            return (CreateTonalityOutputResult( rdf, language, allocResult.First, objectAllocateMethod, coreferenceInfo ));
            #endregion
        }
        #endregion

        #region [.4 use only Alexey-fusking-playing. use as-inprocess in TM.Wpf2-application.]
        public TonalityMarkingOutputResult TMWpf2AlexeyFuskingPlaying( TonalityMarkingInputParams inputParams )
        {
            var xdocument = default(XDocument);

                #region [.check input.]
                inputParams.ThrowIfNull( "inputParams" );
                #endregion   

                #region [.Linguistic.]
                var linguisticStopwatch = Stopwatch.StartNew();
                switch ( inputParams.InputTextFormat )
                {
                    case InputTextFormat.PlainText:
                        xdocument = LinguisticsDecorator.ProcessText( inputParams );
                        if ( !xdocument.Root.HasElements ) throw (new ArgumentException("Linguistics returned empty xml."));
                    break;
                    case InputTextFormat.LinguisticXml:
                        xdocument = Algorithms.PreprocessRdf( inputParams.InputText );
                        if ( !xdocument.Root.HasElements ) throw (new ArgumentException("Input Linguistic-xml is empty."));
                    break;
                    default: 
                        throw (new ArgumentException(inputParams.InputTextFormat.ToString()));
                }                
                linguisticStopwatch.Stop();
                #endregion

                #region [.read coreference-info from Rdf.]
                var coreferenceInfo = _CoreferenceResolver.ReadFromRdf( xdocument.Root );
                #endregion

                #region [.Tonality-marking.]
                var tonalityStopwatch = Stopwatch.StartNew();
                
                var language = Config.ThemesManager[ inputParams.ThemeType ].LanguagesManager[ inputParams.LanguageType ];

                var allocResult = AllocateInquiry( xdocument, language, inputParams );

                /*ExecuteInternal( xdocument, language );*/
                ProcessCollocation( xdocument, language );
                ProcessLexemes    ( xdocument, language );

                if ( allocResult.Second )
                {
                    xdocument.RenameUnprocessingSentenceBack( language );
                }

                tonalityStopwatch.Stop();
                #endregion

                #region [.result.]
                return (CreateTonalityOutputResult( xdocument, language, allocResult.First, inputParams.ObjectAllocateMethod, coreferenceInfo )); 
                #endregion
        }
        #endregion


        #region [.Private method's & properties.]
        #region [.ExecuteInternal.]
        private static void ExecuteInternal( XDocument xdocument, Language language )
        {            
            ProcessCollocation( xdocument, language );
            ProcessLexemes    ( xdocument, language );

            xdocument.ExecRules_HomogenousEntity( language );

            //process first-queue subsentence, nested with each other: Participle & DanglingParticiple & Subordinate & anaphoria-Default
            ProcessFirstQueueSubsentence( xdocument, language );

            //Process second-queue subsentence, which all other
            ProcessSecondQueueSubsentence( xdocument, language );

            ProcessUnionOfConcessionAndReversIntoductory( xdocument, language );

            ProcessExplosiveCollocation( xdocument, language );
        }
        #endregion

        #region [.Allocate Inquiry.]
        private static Pair< InquiryAllocateStatus, bool > AllocateInquiry( 
            XDocument xdocument, Language language, TonalityMarkingInputParams inputParams )
        {
            return (AllocateInquiry( xdocument, language, inputParams.InquiriesSynonyms, inputParams.ObjectAllocateMethod ));
        }

        private static Pair< InquiryAllocateStatus, bool > AllocateInquiry( 
            XDocument xdocument, Language language, List< string > inquiriesSynonyms, ObjectAllocateMethod objectAllocateMethod )
        {
            var isi = new InquiriesSynonymsInfo( inquiriesSynonyms );
            var inquiryAllocateStatus = InquiryAllocateStatus.NotAllocated;
            var hasSkipSentences      = false;

            var sents = xdocument.GetSents( language ).ToArray();

            #region [.process all Sentences.]
            foreach ( var sent in sents )
            {
                #region [.for all SubSentences in Sentence.]
                var inquiryFoundInSent = Algorithms.AllocateInquiry4Sent( sent, objectAllocateMethod, isi );
                if ( inquiryFoundInSent )
                {
                    inquiryAllocateStatus = InquiryAllocateStatus.Allocated;
                }
                #endregion

                #region [.if inquiry not found - rename Sentence for unprocessing.]
                if ( !inquiryFoundInSent )
                {
                    sent.RenameSentence4Unprocessing();
                    hasSkipSentences = true;
                }
                #endregion
            }
            #endregion

            #region [.remark Homogeneous as INQUIRYINITIAL if has INQUIRYINITIAL as child.]
            RemarkHomogeneousAsINQUIRYINITIALIfNeed( xdocument );
            #endregion

            return (new Pair< InquiryAllocateStatus, bool >( inquiryAllocateStatus, hasSkipSentences )); 
        }

        private static void RemarkHomogeneousAsINQUIRYINITIALIfNeed( XDocument xdocument )
        {
            var tuples
                =
                from e in xdocument.DescendantsEntity()
                where ( (e.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE       ) == XLinqExtensions.INQUIRY_VALUE    ) &&
                        (e.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_SAVED_ATTRIBUTE ) == XLinqExtensions.HOMOGENEOUS_VALUE)
                    )
                let inquiryinitial = e.TryGetFirstINQUIRYINITIALfromDescendant()
                where ( inquiryinitial != null )
                select new 
                {
                    homogeneous    = e,
                    inquiryinitial = inquiryinitial,
                };

            foreach ( var t in tuples )
            {
                //1.
                t.homogeneous.MarkAsInquiryHomogeneous();

                //2.
                t.inquiryinitial.Attribute( TextMining.Core.XLinqExtensions.Consts.INQUIRYINITIAL_ATTRIBUTE ).Remove();

                //3.
                foreach ( var d in t.homogeneous.Descendants() )
                {
                    var type_saved_attr = d.Attribute( TextMining.Core.XLinqExtensions.Consts.TYPE_SAVED_ATTRIBUTE );
                    if ( type_saved_attr != null )
                    {
                        var type_attr = d.Attribute( TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE );
                        Debug.Assert( type_attr != null );

                        type_attr.Value = type_saved_attr.Value;
                        type_saved_attr.Remove();
                    }
                }
            }

            #region commented
            /*var tuples 
                =
                from e in xdocument.DescendantsEntity()
                where ( (e.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE       ) == XLinqExtensions.INQUIRY_VALUE    ) &&
                        (e.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_SAVED_ATTRIBUTE ) == XLinqExtensions.HOMOGENEOUS_VALUE) &&
                        (e.HasINQUIRYINITIALDescendant())
                    )
                let t = e.TryGetFirstINQUIRYINITIALandTYPESAVEDAttributesFromDescendants()
                where ( t != null )
                select 
                new
                {
                    homogeneous         = e,
                    inquiryinitial_attr = t.Item1,
                    type_saved_attr     = t.Item2,
                    type_attr           = t.Item3,
                };

            foreach ( var t in tuples )
            {
                //1.
                t.homogeneous.SetAttribute( TextMining.Core.XLinqExtensions.Consts.INQUIRYINITIAL_ATTRIBUTE, 
                                            TextMining.Core.XLinqExtensions.Consts.TRUE );

                //2.                
                t.type_attr.Value = t.type_saved_attr.Value;
                t.inquiryinitial_attr.Remove();
                t.type_saved_attr.Remove();
            }*/
            #endregion
        }
        #endregion

        #region [.Collocations.]
        /// <summary>
        /// Process collocations markup
        /// </summary>
        /// <param name="xdoc">Input xml-text</param>
        /// <param name="dictionaryManager">determined dictionary-kit</param>
        private static void ProcessCollocation( XDocument xdocument, Language language )
        {
            var nodes = xdocument.GetCollocations( language )
                                 .ToArray();

            var list = new List< MultiWordWeightDictionaryResult< Collocations > >();
            var skipItemCount = 0;
            foreach ( var node in nodes )
            {
                if ( 0 < skipItemCount )
                {
                    skipItemCount--;
                    continue;
                }

                var processingItem = new CollocationProcessingItem( node );

                if ( processingItem.FirstWordInChainOfWords.IsEmptyOrNull() )
                    continue;

                var collocationPairs = language.DictionaryManager.GetCollocationsLists
                        (
                        processingItem.FirstWordInChainOfWords, list
                        );

                if ( !collocationPairs.Any() )
                    continue;


                var orderedCollocationPairs = from collocationPair in collocationPairs                                             
                                              orderby collocationPair.StringArrayWeightList
                                                        .Max( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) descending
                                              select collocationPair;
                foreach ( var collocationPair in orderedCollocationPairs )
                {
                    var bingo = false;
                    foreach ( var stringArrayWeight in collocationPair.StringArrayWeightList.OrderByDescending( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) )
                    {
                        var chainOfWords4Compare = processingItem.GetChainOfWords4Compare( stringArrayWeight.StringArray.Length );

                        if ( !chainOfWords4Compare.IsNull() )
                        {
                            var chainOfWords           = chainOfWords4Compare.First;
                            var chainOfProcessingItems = chainOfWords4Compare.Second;

                            bingo = stringArrayWeight.StringArray.SequenceEqual( chainOfWords );
                            if ( bingo )
                            {
                                //---skipItemCount = chainOfProcessingItems.Count - 1;

                                var firstXElement = chainOfProcessingItems.First().XElement;
                                if ( firstXElement.IsTypeInquiry() )
                                {
                                    #region [.if-element-is-single-inquiry-mark-him-snt-frt.]
                                    var xe = chainOfProcessingItems
                                            .Where( e => e.TypeOfXmlElement == TypeOfXmlElement.ENTITY   ||
                                                         e.TypeOfXmlElement == TypeOfXmlElement.ENTITIES ||
                                                         e.TypeOfXmlElement == TypeOfXmlElement.VERB )
                                            .Select( e => e.XElement );
                                    if ( xe.All( e => e.IsTypeInquiry() ) /*|| 
                                         firstXElement.DescendantsAndSelf().Intersect( xe ).Count() == chainOfProcessingItems.Count*/ )
                                    {
                                        skipItemCount = chainOfProcessingItems.Count - 1;

                                        xe.ForEachEx( e => 
                                        {
                                            e.SetAttributeSNT( collocationPair.DictionaryType );
                                            e.SetAttributeFRT( stringArrayWeight.Weight       );
                                        });
                                    }    
                                    #endregion
                                }
                                else
                                {
                                    #region [.move-elements-into-collocation-element.]
                                    skipItemCount = chainOfProcessingItems.Count - 1;

                                    var entityOrVerbNode = ( collocationPair.DictionaryType.IsVerbCollocation() )
                                                           ? node.CreateAndAddBeforeVerb4Collocation
                                                               (
                                                               collocationPair.DictionaryType,
                                                               stringArrayWeight.Weight
                                                               )
                                                           : node.CreateAndAddBeforeEntity4Collocation
                                                               (
                                                               collocationPair.DictionaryType, 
                                                               stringArrayWeight.Weight,
                                                               chainOfProcessingItems.Last().Role 
                                                               );
                                    entityOrVerbNode.MoveIn2CollocationNode( chainOfProcessingItems );
                                    entityOrVerbNode.AddBeforeAndAfterEmptyU();                                                                    
                                    #endregion
                                }
                                break;
                            }  
                        }
                    }

                    if ( bingo )
                    {
                        break;
                    }
                }

            }
        }
        #endregion

        #region [.Lexemes.]
        /// <summary>
        /// 
        /// </summary>
        private static void ProcessLexemes( XDocument xdocument, Language language )
        {
            ProcessLexemesVerb( xdocument, language );            

            ProcessLexemesEntity( xdocument, language );

            ProcessNERLexemesEntity( xdocument, language );

            //ProcessVerbPassiveVoice    ( xdocument, language );
            ProcessVerbReflexivePronoun( xdocument, language );

            ProcessEntityTYPEMoney( xdocument, language );
        } 

        /// <summary>
        /// 
        /// </summary>
        private static void ProcessLexemesVerb( XDocument xdocument, Language language )
        {
            var nodes = xdocument.GetLexemesVerb( language );
            var list = new List< MultiWordWeightDictionaryResult< Verbs > >();

            #region [.ProcessLexemesVerb.]
            foreach ( XElement node in nodes )
            {
                var processingItem = new LexemeVerbProcessingItem( node );

                var verbPairs = language.DictionaryManager.GetVerbsLists
                        (
                        processingItem.VALUE, list
                        );
            
                var verbType = default(Verbs?);

                #region [.code.]
                if ( verbPairs.Any() )
                {
                    var wordsTillSentenceEnd = processingItem.GetWordsTillSentenceEnd();

                    var orderedVerbPairs = from verbPair in verbPairs
                                           orderby verbPair.StringArrayWeightList
                                                .Max( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) descending
                                           select verbPair;

                    foreach ( var verbPair in orderedVerbPairs )
                    {
                        #region [.code.]
                        var stringArrayWeight = verbPair.StringArrayWeightList
                                                    .FirstOrDefault
                                                    ( 
                                                    _stringArrayWeight => _stringArrayWeight.StringArray.SequenceEqual
                                                                            ( 
                                                                            wordsTillSentenceEnd.Take( _stringArrayWeight.StringArray.Length ) 
                                                                            ) 
                                                    ) ;

                        if ( stringArrayWeight != null )
                        {
                            verbType = verbPair.DictionaryType;

                            node.SetAttributeSNT( verbType );
                            node.SetAttributeFRT( stringArrayWeight.Weight );

                            break;
                        }
                        #endregion
                    }
                }
                #endregion

                if ( !verbType.HasValue )
                {
                    node.SetAttributeSNT( verbType );
                    node.SetAttributeFRT( 0        );
                }
            }
            #endregion

            #region [.ProcessVerbPassiveVoice.]
            /// <summary>
            /// Определение залога.
            /// Пассивный залог меняет сантимент некоторых классов глаголов. Он определяется по служебному атрибуту MI элемента VERB: 
            /// четвертая буква этого атрибута обозначает активный (А) или пассивный (Р) залог – VERB[@MI=”****P” ]. 
            /// После разметки коллокаций и глаголов (с учетом глагольного управления) определяется залог.  
            /// Если залог пассивный - MI=”****P” - , то у некоторых классов глаголов сантимент изменится на следующий:
            /// </summary>
            foreach ( XElement node in nodes )
            {
                if ( node.MIhasCharAtPosition( 'P', 6 ) )
                {
                    var SNT = node.GetAttributeSNT();
                    var result = language.Rules.Lexemes.VerbPassiveVoice.Check( SNT.ToText().ToEnum< Verbs >() );
                    if ( result.HasValue )
                    {
                        node.SetAttributeSNT( result );
                    }
                }
            }
            #endregion
        }
        /*/// <summary>
        /// Определение залога.
        /// Пассивный залог меняет сантимент некоторых классов глаголов. Он определяется по служебному атрибуту MI элемента VERB: 
        /// четвертая буква этого атрибута обозначает активный (А) или пассивный (Р) залог – VERB[@MI=”****P” ]. 
        /// После разметки коллокаций и глаголов (с учетом глагольного управления) определяется залог.  
        /// Если залог пассивный - MI=”****P” - , то у некоторых классов глаголов сантимент изменится на следующий:
        /// </summary>
        private void ProcessVerbPassiveVoice( XDocument xdocument, Language language )
        {
            var nodes = xdocument.GetLexemesVerb( language );

            foreach ( XElement node in nodes )
            {
                if ( node.MIhasCharAtPosition( 'P', 6 ) )
                {
                    var SNT = node.GetAttributeSNT();
                    var result = language.Rules.Lexemes.VerbPassiveVoice.Check( SNT.ToText().ToEnum< Verbs >() );
                    if ( result.HasValue )
                    {
                        node.SetAttributeSNT( result );
                    }
                }
            }
        }
        //*/

        /// <summary>
        /// 
        /// </summary>
        private static void ProcessNERLexemesEntity( XDocument xdocument, Language language )
        {
            //ЭТО НЕ НУЖНО!!!!!!!!!!
            /*
            var nodes = xdocument.GetNERLexemesEntity( language )
                                 .ToArray();

            foreach ( var node in nodes )
            {
                node.SetAttributeSNT( Nouns.nPPos );
                node.SetAttributeFRT( 0 );
            }
            */

            #region commented. while
            /*
            var list = new List< MultiWordWeightDictionaryResult< Nouns > >();
            var skipItemCount = 0;
            foreach ( var node in nodes )
            {
                if ( 0 < skipItemCount )
                {
                    skipItemCount--;
                    continue;
                }

                var processingItem = new CollocationProcessingItem( node );

                if ( processingItem.FirstWordInChainOfWords.IsEmptyOrNull() )
                    continue;

                var nerPairs = language.DictionaryManager.GetNERsLists
                        (
                        processingItem.FirstWordInChainOfWords, list
                        );

                if ( !nerPairs.Any() )
                    continue;


                var orderedNerPairs = from nerPair in nerPairs                                             
                                      orderby nerPair.StringArrayWeightList
                                              .Max( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) descending
                                      select nerPair;
                foreach ( var nerPair in orderedNerPairs )
                {
                    var bingo = false;
                    foreach ( var stringArrayWeight in nerPair.StringArrayWeightList.OrderByDescending( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) )
                    {
                        var chainOfWords4Compare = processingItem.GetChainOfWords4Compare( stringArrayWeight.StringArray.Length );

                        if ( !chainOfWords4Compare.IsNull() )
                        {
                            var chainOfWords           = chainOfWords4Compare.First;
                            var chainOfProcessingItems = chainOfWords4Compare.Second;

                            bingo = stringArrayWeight.StringArray.SequenceEqual( chainOfWords );
                            if ( bingo )
                            {
                                skipItemCount = chainOfProcessingItems.Count - 1;

                                chainOfProcessingItems.ForEachEx( e => 
                                {
                                    e.XElement.SetAttributeSNT( nerPair.DictionaryType   );
                                    e.XElement.SetAttributeFRT( stringArrayWeight.Weight );
                                });                              

                                break;
                            }  
                        }
                    }

                    if ( bingo )
                    {
                        break;
                    }
                }

            }
            */
            #endregion
        }

        /// <summary>
        /// 
        /// </summary>
        private static void ProcessLexemesEntity( XDocument xdocument, Language language )
        {
            var nodes = xdocument.GetLexemesEntity( language );      

            var verbSNT = default(Verbs?);

            foreach ( XElement node in nodes )
            {
                var processingItem = new LexemeEntityProcessingItem( node );

                var hasPREwithOneWordAndFirstChildVerb = false;
                var adjadjSNT = default(Adjectives?);
                var adjadjFRT = 0;

                if ( processingItem.AdverbsWord != null )
                {
                    #region [.Adverb.]
                    var adverbTypeAndWeight = language.DictionaryManager.ExistsInAdverbs( processingItem.AdverbsWord );

                    node.SetAttributeSNT( adverbTypeAndWeight.ToSNT( Adverbs.advNeut ) );
                    node.SetAttributeFRT( adverbTypeAndWeight.ToFRT() );
                    #endregion
                }                
                else //alwasy - if ( processingItem.NounWord != null )
                {
                    #region [.Adjective-Adjective.]
                    if ( processingItem.HasPREwithOneWord && 
                         processingItem.HasFirstChildVerb( out verbSNT, out adjadjFRT ) )
                    {
                        hasPREwithOneWordAndFirstChildVerb = true;

                        #region [.description.]
                        /*
                        Возникли трудности: в PRE  могут быть и прилагательные и причастия.
                            Вася самый хороший  и публикуемый министр.

                        Поэтому п.4 не делаем. Но проверяем в п.2, чтобы PRE имело одно значение, одно слово.
                        В противном случае работаем по обычному алгоритму, несмотря на наличие verb в  entity

                        Иначе не разобрать где причастие, где прилагательное. Ср:
                            Вася самый публикуемый, трудолюбивый и цитируемый министр.
                        -----------------------------------------------------------------------------------
                        Задача усложняется:
                        1) Размечаем все глаголы, в том числе и внутри entity 
                        2) Если встретилась сущность ENTITY@PRE (имеющая PRE), то смотреть дочерний элемент.
                        3) Если дочерний элемент VERB, то брать значение у VERB.
                        4) Если несколько дочерних элементов VERB, то для каждого значения атрибута ENTITY@PRE последовательно брать значение у VERB.
                        */
                        #endregion
                    }
                    else if ( processingItem.AdjectivesWords != null )
                    {
                        #region [.code.]
                        foreach ( var adj in processingItem.AdjectivesWords )
                        {
                            var adjTypeAndWeight = language.DictionaryManager.ExistsInAdjectives( adj );
                            if ( !adjadjSNT.HasValue )
                            {
                                adjadjSNT = adjTypeAndWeight.ToSNT( Adjectives.adjNeut );
                                adjadjFRT = adjTypeAndWeight.ToFRT();
                            }
                            else
                            {
                                var resultType = language.Rules.Lexemes.AdjectiveAdjective.Check
                                    (
                                    adjadjSNT.Value,
                                    adjTypeAndWeight.ToSNT( Adjectives.adjNeut )
                                    );

                                adjadjSNT  = resultType.GetValueOrDefault( Adjectives.adjNeut );
                                adjadjFRT += adjTypeAndWeight.ToFRT(); //resultType.ToFRT();
                            }
                        }
                        #endregion
                    }
                    #endregion

                    #region [.Adjective-Noun.]
                    switch ( processingItem.RealTYPE )
                    {
                        case TypeAttributeValue.Adjective:
                        case TypeAttributeValue.ShortAdjective:
                            #region [.code.]
                            if ( hasPREwithOneWordAndFirstChildVerb )
                            {
                                if ( verbSNT.HasValue )
                                    throw (new InvalidOperationException("Не могет такого быть - промежуток должен быть!"));
                                    //node.SetAttributeSNT( verbSNT.Value );
                                else
                                    node.SetAttributeSNT( Adjectives.adjNeut );
                            }
                            else
                            {
                                node.SetAttributeSNT( adjadjSNT.GetValueOrDefault( Adjectives.adjNeut ) );
                            }
                            #endregion
                        break;

                        default:
                            #region [.code.]
                            var nounType = GetSNT4NounWord( processingItem, language );
                            if ( hasPREwithOneWordAndFirstChildVerb ? verbSNT.HasValue : adjadjSNT.HasValue )
                            {
                                var adjNounType = default(Nouns?); 
                                if ( hasPREwithOneWordAndFirstChildVerb )
                                {
                                    adjNounType = language.Rules.Lexemes.ParticipleNoun.Check
                                        (
                                        verbSNT.Value, 
                                        nounType.GetValueOrDefault( Nouns.nNeut )
                                        );
                                }
                                else
                                {
                                    adjNounType = language.Rules.Lexemes.AdjectiveNoun.Check
                                        (
                                        adjadjSNT.Value, 
                                        nounType.GetValueOrDefault( Nouns.nNeut )
                                        );
                                }

                                /*
                                При выборе объекта тональности: если ОМ имеет одушевленность, 
                                то значение его тональности по умолчанию nPPos, если слово не присутствует в тональных словарях. 
                                Значение одушевленности смотреть MI=”***A*” – четвертая буква в MI==А.
                                То есть, проверили по словарям, если ОМ SNT=nNeut & MI(4)==A   => SNT=nPPos                                
                                */
                                var snt = adjNounType.GetValueOrDefault( Nouns.nNeut );
                                node.SetAttributeSNT( snt );
                            }
                            else
                            {
                                /*
                                При выборе объекта тональности: если ОМ имеет одушевленность, 
                                то значение его тональности по умолчанию nPPos, если слово не присутствует в тональных словарях. 
                                Значение одушевленности смотреть MI=”***A*” – четвертая буква в MI==А.
                                То есть, проверили по словарям, если ОМ SNT=nNeut & MI(4)==A   => SNT=nPPos                                
                                */
                                var snt = nounType.GetValueOrDefault( Nouns.nNeut );                               
                                node.SetAttributeSNT( snt );
                            }
                            #endregion
                            #region [.previous commneted.]
                            /*
                            var nounTypeAndWeight = !processingItem.NounWord.IsEmptyOrNull()
                                                    ? language.DictionaryManager.ExistsInNouns( processingItem.NounWord )
                                                    : null;
                            if ( hasPREwithOneWordAndFirstChildVerb ? verbSNT.HasValue : adjadjSNT.HasValue )
                            {
                                var adjNounType = default(Nouns?); 
                                if ( hasPREwithOneWordAndFirstChildVerb )
                                {
                                    adjNounType = language.Rules.Lexemes.ParticipleNoun.Check
                                        (
                                        verbSNT.Value, 
                                        nounTypeAndWeight.ToSNT( Nouns.nNeut )
                                        );
                                }
                                else
                                {
                                    adjNounType = language.Rules.Lexemes.AdjectiveNoun.Check
                                        (
                                        adjadjSNT.Value, 
                                        nounTypeAndWeight.ToSNT( Nouns.nNeut )
                                        );
                                }

                               
                                //При выборе объекта тональности: если ОМ имеет одушевленность, 
                                //то значение его тональности по умолчанию nPPos, если слово не присутствует в тональных словарях. 
                                //Значение одушевленности смотреть MI=”***A*” – четвертая буква в MI==А.
                                //То есть, проверили по словарям, если ОМ SNT=nNeut & MI(4)==A   => SNT=nPPos                                
                                
                                var snt = adjNounType.GetValueOrDefault( Nouns.nNeut )
                                                     .GetSNT4ProcessLexemesEntity( node, processingItem.RealTYPE );
                                node.SetAttributeSNT( snt );
                            }
                            else
                            {
                                
                                //При выборе объекта тональности: если ОМ имеет одушевленность, 
                                //то значение его тональности по умолчанию nPPos, если слово не присутствует в тональных словарях. 
                                //Значение одушевленности смотреть MI=”***A*” – четвертая буква в MI==А.
                                //То есть, проверили по словарям, если ОМ SNT=nNeut & MI(4)==A   => SNT=nPPos                                
                               
                                var snt = nounTypeAndWeight.ToSNT( Nouns.nNeut )
                                                           .GetSNT4ProcessLexemesEntity( node, processingItem.RealTYPE );                               
                                node.SetAttributeSNT( snt );
                            }
                            */
                            #endregion
                        break;
                    }

                    node.SetAttributeFRT( adjadjFRT );
                    #endregion
                }
            }
        }
        private static Nouns? GetSNT4NounWord( LexemeEntityProcessingItem processingItem, Language language )
        {
            if ( processingItem.NounWord.IsEmptyOrNull() )
            {
                return (null);
            }

            switch ( processingItem.RealTYPE )
            {
                case TypeAttributeValue.OrgName:
                case TypeAttributeValue.OrgAbbr:
                case TypeAttributeValue.PlaceName:
                case TypeAttributeValue.PseudoGeo:
                case TypeAttributeValue.Product:
                    return (Nouns.nNeut);

                case TypeAttributeValue.PersonName:
                case TypeAttributeValue.PersonInfo:
                    return (Nouns.nPPos);

                /*
                При выборе объекта тональности: если ОМ имеет одушевленность, 
                то значение его тональности по умолчанию nPPos, если слово не присутствует в тональных словарях. 
                Значение одушевленности смотреть MI=”***A*” – четвертая буква в MI==А.
                То есть, проверили по словарям, если ОМ SNT=nNeut & MI(4)==A   => SNT=nPPos                                
                */
                /*
                case TypeAttributeValue.PersonName:
                if ( //processingItem.XElement.IsTypeInquiry()  &&
                     processingItem.XElement.MIhasCharAtPosition( 'A', 4 ) 
                    )
                {
                    return (Nouns.nPPos);
                }
                return (Nouns.nNeut);
                */
            }

            var nounTypeAndWeight = language.DictionaryManager.ExistsInNouns( processingItem.NounWord );
            if ( nounTypeAndWeight.HasValue )
            {
                return (nounTypeAndWeight.Value.DictionaryType);
            }
            return (null);
        }

        /// <summary>
        /// Возвратное  местоимение «себя» меняет сантимент некоторых классов глаголов на vFlxNeg.
        /// Правило определения таких случаев: если за глаголом идет местоимение «себя» 
        /// (сразу, через пробел), <ENTITY TYPE="Pronoun" VALUE="себя" />, то следующие классы глаголов меняют свой сантимент:
        /// </summary>
        private static void ProcessVerbReflexivePronoun( XDocument xdocument, Language language )
        {
            const string MAGIC_WORD_1 = "себя";
            const string MAGIC_WORD_2 = "себе";

            var nodes = xdocument.GetLexemesVerb( language );

            foreach ( XElement node in nodes )
            {
                var U = node.ElementsAfterSelfExceptT().FirstOrDefault();
                if ( U == null || !U.IsElementU() || 
                    !U.GetCDataTextFromElementU().TrimWhiteSpaces().IsEmptyOrNull() )
                    continue;

                var ENTITY = U.ElementsAfterSelfExceptT().FirstOrDefault();
                if ( ENTITY == null || !ENTITY.IsElementENTITY() || 
                     ENTITY.GetAttributeTYPE() != TypeAttributeValue.Pronoun  )
                    continue;

                var v = ENTITY.GetAttributeVALUE().TrimWhiteSpaces().ToLowerEx();
                if ( v != MAGIC_WORD_1 && 
                     v != MAGIC_WORD_2 )
                    continue;

                var SNT = node.GetAttributeSNT();
                var result = language.Rules.Lexemes.VerbReflexivePronoun.Check( SNT.ToText().ToEnum< Verbs >() );
                if ( result.HasValue )
                {
                    node.SetAttributeSNT( result );
                }
            }
        }

        /// <summary>
        /// В новой лингвистике стал использоваться новый тип – «Money»
        /// <ENTITY ID="4" VALUE="2000 рублей" POS="24" ROLE="Adj" TYPE="Money" SNT="nNeut" FRT="0">
        ///   <T><![CDATA[ 2000 рублей  ]]></T>
        /// </ENTITY>
        /// Так что рубль теперь не нормализуется.
        /// Поэтому у этого типа Entity@TYPE="Money" нужно делать  SNT="nPPos" 
        /// </summary>
        private static void ProcessEntityTYPEMoney( XDocument xdocument, Language language )
        {
            var nodes = from e in xdocument.DescendantsEntity()
                        where ( e.GetAttributeTYPE() == TypeAttributeValue.Money )
                        select e;
      
            foreach ( XElement node in nodes )
            {
                var SNT = node.Attribute( TextMining.Core.XLinqExtensions.Consts.SNT_ATTRIBUTE );
                if ( SNT == null )
                {
                    node.SetAttributeSNT( Nouns.nPPos );
                }
                else 
                {
                    var v = SNT.Value;
                    if ( v.IsEmptyOrNull() || v.ToLowerEx().Contains( "neut" ) )
                    {
                        node.SetAttributeSNT( Nouns.nPPos );
                    }
                }
            }
        }
        #endregion

        #region [.Subsent's.]
        //Process first-queue subsentence, nested with each other:
        //  Participle & DanglingParticiple & Subordinate & anaphoria-Default
        private static void ProcessFirstQueueSubsentence( XDocument xdocument, Language language )
        {
            #region [.Description.]
            /*
            В первую очередь отбираются все типы подпредложений, 
                кроме ApproximatingForceps (не содержащие в себе ни одного элемента verb (!new)) и EqualInRights
            1.1. Если это один из следующих типов:
            1) Participle, - имеющие SUBJ="N";
            2) DanglingParticiple – имеющие SUBJ="N";
            3) Subordinate, имеющие ENTITY || CET с Link=”N”;
            4) Default, имеющие ENTITY || CET с Link=”N”; 
            5) Default, имеющие SUBJ="N";
            */            
            #endregion

            #region [.Condition.]
            var tuples   = (from subsent in xdocument.GetSents( language )
                                                     .GetDescendantsSubsent()
                            let sent = subsent.GetAncestorSent()
                            where (sent.IsNull() || !sent.IsSentEndsWithQuestionMark())

                            let tuple1 = Algorithms.GetIfIsParticipialOrDanglingParticipleOrDefault( subsent )
                            let tuple2 = (tuple1 == null) ? Algorithms.GetIfIsSubordinateOrDefault( subsent ) : null

                            where ( tuple1 != null || tuple2 != null )

                            select new 
                            {
                                SUBSENT          = subsent,
                                SUBSENTENCE_TYPE = (tuple1 != null) ? tuple1.Item1 : tuple2.Item1,
                                LINK_ENTITYORCET = (tuple2 != null) ? tuple2.Item2 : null,
                                SUBJ_ENTITY      = (tuple1 != null) ? tuple1.Item2 : null,
                            }
                           )
                           .Reverse(); //[!Important!]            
            #endregion

            #region [.Switch by Subsent-type.]
            foreach ( var tuple in tuples )
            {
                switch ( tuple.SUBSENTENCE_TYPE )
                {
                    case SubsentenceType.Participial:
                    case SubsentenceType.DanglingParticiple:
                        ProcessSubsentence_Participial_DanglingParticiple( tuple.SUBSENT, language );
                    break;

                    case SubsentenceType.Subordinate:
                        ProcessSubsentence_Subordinate_Default( tuple.SUBSENT, language, tuple.LINK_ENTITYORCET );
                    break;

                    case SubsentenceType.Default:
                        if ( tuple.SUBJ_ENTITY != null )
                        {
                            ProcessSubsentence_Default( tuple.SUBSENT, language, tuple.SUBJ_ENTITY );
                        }
                        else
                        {
                            ProcessSubsentence_Subordinate_Default( tuple.SUBSENT, language, tuple.LINK_ENTITYORCET );
                        }
                    break;

                    default:
                        throw (new ArgumentException("Wrong subsentence-type: " + tuple.SUBSENTENCE_TYPE.ToString().InSingleQuote()));
                }

                //Important - set after run all rules!!!!
                tuple.SUBSENT.SetAttributeFIRSTQUEUESUBSENT();
            }
            #endregion
        }

        //Process second-queue subsentence, which all other
        private static void ProcessSecondQueueSubsentence( XDocument xdocument, Language language )
        {
            #region [.Description.]
            /*
            Отбираются все остальные типы подпредложений, 
                кроме ApproximatingForceps (не содержащие в себе ни одного элемента verb (!new)) и EqualInRights
            */
            #endregion

            #region [.Condition.]
            var tuples  =  (from subsent in xdocument.GetSents( language )
                                                     .GetDescendantsSubsent()
                            let sent = subsent.GetAncestorSent()
                            where (sent.IsNull() || !sent.IsSentEndsWithQuestionMark())

                            where subsent.AncestorsAndSelf().All( _ => !_.HasAttributeFIRSTQUEUESUBSENT() ) //!subsent.HasAttributeFIRSTQUEUESUBSENT()

                            select new 
                            {
                                SUBSENT          = subsent,
                                SUBSENTENCE_TYPE = subsent.GetAttributeSUBSENTTYPE(),
                            }
                           )
                           .Reverse(); //[!Important!]            
            #endregion

            #region [.Switch by Subsent-type.]
            foreach ( var tuple in tuples )
            {
                switch ( tuple.SUBSENTENCE_TYPE )
                {
                    case SubsentenceType.Participial:
                    case SubsentenceType.DanglingParticiple:
                        ProcessSubsentence_Participial_DanglingParticiple( tuple.SUBSENT, language );
                    break;

                    case SubsentenceType.Subordinate:
                        ProcessSubsentence_Subordinate_Default( tuple.SUBSENT, language );
                    break;

                    case SubsentenceType.Default:
                        ProcessSubsentence_Default( tuple.SUBSENT, language );
                    break;

                    /*case SubsentenceType.DirectSpeech:
                        ProcessSubsentence_DirectSpeech( tuple.SUBSENT, language );
                    break;*/

                    case SubsentenceType.ApproximatingForceps:
                    case SubsentenceType.EqualInRights:
                    case SubsentenceType.Introductory:
                    case SubsentenceType.DirectSpeech: //All is stolen before us!
                        //don't processed!
                    break;

                    default:
                        ProcessSubsentence_AllOtherType(  tuple.SUBSENT, language );
                        //---throw (new ArgumentException("Wrong subsentence-type: " + tuple.SUBSENTENCE_TYPE.ToString().InSingleQuote()));
                    break;
                }
            }
            #endregion
        }

        //Process 'DirectSpeech'
        /*private void ProcessSubsentence_DirectSpeech( XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();
            subsent.ThrowIfNotTypeDirectSpeech();

            //...
        }*/

        private static void ProcessSubsentence_Participial_DanglingParticiple( XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();
            subsent.ThrowIfNotTypeParticipialOrDanglingParticiple();

            //Собираем цепочки правилами id=000-099 (order_rules_participial.xml), 
            // для Participle || DanglingParticiple
            subsent.ExecRules_BuildChains_Participial_DanglingParticiple( language );

            //Если это подпредложение 
            //  1) Participle, - имеющие SUBJ="N", ссылающийся НЕ на Inquire, или  
            //  2) DanglingParticiple – имеющие SUBJ="N", ссылающийся НЕ на Inquire 
            //  – то сначала отрабатывает правилами id=100_101_102, 
            //  после этого отрабатывают правила 110-111-112-120-122-125-126, и создается фиктивные-ENTITIES по SUBJ="N".
            //Если это 
            //  1) Participle, - имеющие SUBJ="N", ссылающийся на Inquire или  
            //  2) DanglingParticiple –имеющие SUBJ="N", ссылающийся на Inquire 
            //  – то сначала отрабатывает правилами id=100_101_102, 
            //  после этого отрабатывают правила 110-111-112-120-122-125-126, 
            //          и не создаем фиктивные-ENTITIES, а выставляем финальные SNT&FRT.
            var SUBJ = subsent.AttributeSubjValue();
            if ( SUBJ.HasValue )
            {
                #region [.process.]
                //Получаем связанный элемент (на который показыает значение атрибуиа-@SUBJ)
                var entityBound = subsent.FindEntityById_4Subsent( SUBJ.Value );
                if ( entityBound != null )
                {
                    //Если имеем ссылку на себя (внутрь) – то ничего делать не надо вообще.
                    if ( entityBound.GetAncestorSubsent() != subsent )
                    {
                        //Отрабатываем правилами id=100_101_102-110-111-112-120-122-125-126
                        subsent.ExecRules_Sentences( language );

                        //не создаем фиктивные-ENTITIES, а выставляем финальные SNT&FRT
                        if ( entityBound.IsTypeInquirySelfOrAncestors() )
                        {
                            subsent.CopySNTnFRT2FINAL();
                        }
                        //создается фиктивные-ENTITIES
                        else
                        {
                            var fakeEntities = subsent.CreateEntitiesFromSubsent();
                            entityBound.AddAfterSelfWithProvision4Collocation( fakeEntities );
                            fakeEntities.AddBeforeAndAfterEmptyU();

                            /*
                            В подпредложениях типа DanglingParticiple & Participle размечать глаголы, 
                            если подпредложение содержит только глагол или глагольную коллокацию (+ могут быть subsentы), 
                            но не выставлять финальный сантимент, а создавать элемент в главном предложении (по правилам, если есть SUBJ=…), 
                            и если в главное предложение (в котором SUBJ ) получило сентимент, 
                            то и в придаточном выставлять финальный (такой же как и в главном).
                            */
                            var zeroVerbSnt = GetOneVerbSNT_Participial_DanglingParticiple( subsent );
                            if ( zeroVerbSnt.HasValue )
                            {
                                fakeEntities.SetAttributeSNT( zeroVerbSnt.Value );
                            }
                        }
                    }
                }
                //else
                //{
                //    throw (new ArgumentException("Not found linked 'Entity'-element"));
                //}
                #endregion
            }
            //Если это другой тип подпредложения, содержащий Inquire, то дальше оно обрабатывается 
            //  как самостоятельное 200-ми правилами и выставляются финальные SNT&FRT.
            else
            {
                ProcessSubsentenceInquiryRules( subsent, language );
            }
        }
        private static void ProcessSubsentence_Subordinate_Default( XElement subsent, Language language, XElement linkEntityOrCet = null )
        {
            subsent.ThrowIfNotSubsent();
            subsent.ThrowIfNotTypeSubordinateDefault();

            /*
            Если это Subordinate || Default (имеющие ENTITY с Link=”N”) содержащий Inquire, 
             * то подпредложение обрабатывается как самостоятельное 200-ми правилами и выставляются финальные SNT&FRT.
            Если это Subordinate || Default (имеющие ENTITY с Link=”N”) НЕ содержащий Inquire, 
             * то подпредложение обрабатывается id=114-115-127  правилами и создается фиктивные-ENTITIES по Link=”N”.
            Если это Subordinate || Default (имеющие CET с Link=”N”) содержащее Inquire, 
             * то внутри него создается фиктивный ENTITY и дальше подпредложение обрабатывается как самостоятельное 200-ми правилами и и выставляются финальные SNT&FRT.
            Если это Subordinate || Default (имеющие CET с Link=”N”) НЕ содержащее Inquire, 
             * то подпредложение обрабатывается id=114-115-127  правилами и создается фиктивные-ENTITIES по Link=”N”.*/
            if ( linkEntityOrCet != null )
            {
                #region [.process.]
                #region [.get 'entityBound'.]
                var linkId = linkEntityOrCet.GetAttributeLINK();
                if ( !linkId.HasValue )
                    throw (new ArgumentException("Element 'Entity-Or-Cet' not a have valid @LINK-attribute"));

                var entityBound = default(XElement);
                var parentSubsent = subsent.Parent;
                if ( parentSubsent != null && parentSubsent.IsElementSUBSENT() )
                {
                    entityBound = parentSubsent.DescendantsAndSelfEntity()
                        .FirstOrDefault( ent => ent.GetAttributeID() == linkId.Value );                
                }
                if ( entityBound == default(XElement) )
                {
                    entityBound = subsent.FindEntityById_4Subsent( linkId.Value ); //subsent.Document.FindEntityById( linkId.Value );
                }
                #endregion

                if ( entityBound != null )
                {
                    var tuple = new 
                    {
                        SUBSENT          = subsent,
                        ENTITYBOUND      = entityBound,
                        ENTITYORCETLINK  = linkEntityOrCet,
                    };

                    if ( subsent.HasElementTypeInquiry() )
                    {
                        if ( tuple.ENTITYORCETLINK.IsElementCET() && 
                             subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate ) )
                        {
                            //Create fake-'Entity'-element by 'Cet'-element for subsentece-type-of-'Subordinate'
                            CreateFakeEntityByCet4Subordinate( subsent );                        
                        }

                        //Собираем цепочки правилами id=000-099 (order_rules.xml), 
                        subsent.ExecRules_BuildChains( language );

                        ProcessSubsentenceInquiryRules( subsent, language );                      
                    }
                    else
                    {
                        //Только для предложений типа-Subordinate
                        if ( subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate ) )
                        {
                            //Собираем цепочки правилами id=000-099 (order_rules.xml), 
                            subsent.ExecRules_BuildChains( language );

                            #region [.run SubordinateSubsent-rules.]
                            var result = tuple.SUBSENT.ExecRules_SubordinateSubsent( language );
                            if ( result )
                            {
                                if ( tuple.ENTITYBOUND.IsTypeInquirySelfOrAncestors() )
                                {
                                    tuple.SUBSENT.CopySNTnFRT2FINAL();
                                }
                                else
                                {
                                    var fakeEntities = tuple.SUBSENT.CreateEntitiesFromSubsent();
                                    tuple.ENTITYBOUND.AddAfterSelfWithProvision4Collocation( fakeEntities );
                                    fakeEntities.AddBeforeAndAfterEmptyU();
                                }
                            }
                        }
                        #endregion
                    }
                }
                #region throw exception?
                //else
                //{
                //    throw (new ArgumentException("Not found linked 'Entity'-element"));
                //} 
                #endregion
                #endregion
            }
            //Если это другой тип подпредложения, содержащий Inquire, то дальше оно обрабатывается 
            //  как самостоятельное 200-ми правилами и выставляются финальные SNT&FRT.
            else
            {
                //Собираем цепочки правилами id=000-099 (order_rules.xml), 
                subsent.ExecRules_BuildChains( language );

                ProcessSubsentenceInquiryRules( subsent, language );
            }
        }
        private static void ProcessSubsentence_Default( XElement subsent, Language language, XElement subjEntity = null )
        {
            subsent.ThrowIfNotSubsent();
            subsent.ThrowIfNotTypeDefault();

            /*
            Если это тип Default, имеющий SUBJ="N", содержащее Inquire, 
             * то дальше это подпредложение обрабатывается как самостоятельное 200-ми правилами и выставляются финальные SNT&FRT.
            Если это тип Default, имеющий SUBJ="N", НЕ содержащее Inquire, 
             * то подпредложение обрабатывается id=124, 128 (!new) и создается фиктивный-ENTITIES по SUBJ="N".*/
            if ( (subjEntity != null) && !subsent.HasElementTypeInquiry() )
            {
                #region [.process.]
                var tuple = new 
                {
                    SUBJENTITY        = subjEntity,
                    SUBJENTITYSUBSENT = subjEntity.GetAncestorSubsent(),
                };
                #region commented
                /*//Получаем связанный элемент (на который показыает значение атрибуиа-@SUBJ)
                var tuple = (from subsentBefore in subsent.ElementsBeforeSelf( subsent.Name ).Reverse()
                             where subsentBefore.AttributeTypeAreEqual( SubsentenceType.Default, 
                                                                        SubsentenceType.Subordinate )

                             let entityBound = subsentBefore.ElementsEntity()
                                     .FirstOrDefault( e => (e.GetAttributeID() == SUBJ.Value) && 
                                                           e.HasRoleAttributeValue( RoleAttributeValue.Subj ) )
                             where ( entityBound != null )
                             select new 
                             {
                                 ENTITYBOUND        = entityBound,
                                 ENTITYBOUNDSUBSENT = subsentBefore,
                             }
                            )
                            .FirstOrDefault();*/
                
                #endregion

                //Если подпредложение, на которое ссылается SUBJ="N" имеет хотя бы один элемент VERB, то ENTITIES НЕ создается. 
                //  В подпредложении (в его начале) Default@SUBJ="N создается элемент ENTITY с SNT&FRT того элемента, на который ссылка SUBJ="N. 
                //  После этого подпредложение обрабатывается правилами id=000-099 (order_rules.xml) 
                //  и затем 200-ми правилами.
                if ( tuple.SUBJENTITYSUBSENT.ElementsVerb().Any() )
                {
                    #region [.process.]
                    if ( tuple.SUBJENTITY.IsTypeInquiry() )
                    {
                        subsent.ElementsExceptT().First().CreateFakeEntityBeforeSelf( tuple.SUBJENTITY );

                        //Собираем цепочки правилами id=000-099 (order_rules.xml)
                        subsent.ExecRules_BuildChains( language );

                        //После этого подпредложение обрабатывается 200-ми правилами.
                        ProcessSubsentenceInquiryRules( subsent, language );
                    }                                                
                    #endregion
                }
                //Если подпредложение, на которое ссылается SUBJ="N" НЕ содержит ни одного элемента VERB, 
                // то подпредложение обрабатывается правилами id=000-099 (order_rules.xml) 
                // и затем id=124-128 и создается фиктивный-ENTITIES по SUBJ="N".
                else
                {
                    #region [.process.]
                    //Собираем цепочки правилами id=000-099 (order_rules.xml)
                    subsent.ExecRules_BuildChains( language );

                    //Отрабатываем правилами id=124,125,128 (run SubsentsNoNoun-rule)
                    var doCreateEntitiesFromSubsent = true;
                    var result = subsent.ExecRules_Sentences_SubsentsNoNoun( language );
                    if ( !result )
                    {
                        #region [.process.]
                        //Если правила id=124,125,128 не сработали:
                        // - Если образовалось два и более ENTITY(ENTITIES), но только один имеет сантимент, 
                        //   отличный от nNeut, то берем его для фиктивного-ENTITIES.
                        var entityOrEntities = subsent.ElementsEntityOrEntities().ToArray();
                        if ( entityOrEntities.SequenceEqual
                                ( 
                                    subsent.ElementsExceptT().Where( e => !e.IsElementUorCET() ) 
                                ) 
                            )
                        {
                            var sntAndfrtNonNeut = (from _entityOrEntities in entityOrEntities
                                                    let snt = _entityOrEntities.GetAttributeSNT()
                                                    where (snt != TonalityType.nNeut)
                                                    let frt = _entityOrEntities.GetAttributeFRT()
                                                    select new
                                                    {
                                                        SNT = snt,
                                                        FRT = frt,
                                                    }
                                                    ).ToArray();
                            if ( sntAndfrtNonNeut.Length == 1 )
                            {
                                doCreateEntitiesFromSubsent = false;
   
                                //создается фиктивные-ENTITIES с одного внутреннего ENTITY(ENTITIES)
                                var fakeEntities = subsent.CreateEntitiesFromSubsentPartial
                                    (
                                    sntAndfrtNonNeut.First().SNT,
                                    sntAndfrtNonNeut.First().FRT 
                                    );
                                tuple.SUBJENTITY.AddAfterSelfWithProvision4Collocation( fakeEntities );
                                fakeEntities.AddBeforeAndAfterEmptyU();
                            }
                        }                       
                        #endregion
                    }
                    if ( doCreateEntitiesFromSubsent )
                    {
                        //создается фиктивные-ENTITIES с подпредложения
                        var fakeEntities = subsent.CreateEntitiesFromSubsent();
                        tuple.SUBJENTITY.AddAfterSelfWithProvision4Collocation( fakeEntities );
                        fakeEntities.AddBeforeAndAfterEmptyU();
                    }                        
                    #endregion
                }
                #endregion
            }
            //Если это другой тип подпредложения, содержащий Inquire, то дальше оно обрабатывается 
            //  как самостоятельное 200-ми правилами и выставляются финальные SNT&FRT.
            else
            {
                //Собираем цепочки правилами id=000-099 (order_rules.xml)
                subsent.ExecRules_BuildChains( language );

                ProcessSubsentenceInquiryRules( subsent, language );
            }
        }
        private static void ProcessSubsentence_AllOtherType( XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();

            //Собираем цепочки правилами id=000-099 (order_rules.xml), 
            // для НЕ (Participle || DanglingParticiple)
            subsent.ExecRules_BuildChains( language );

            //Если это другой тип подпредложения, содержащий Inquire, то дальше оно обрабатывается 
            //  как самостоятельное 200-ми правилами и выставляются финальные SNT&FRT.
            ProcessSubsentenceInquiryRules( subsent, language );
        }

        private static void CreateFakeEntityByCet4Subordinate( XElement subsent )
        {
            subsent.ThrowIfNotSubsent();
            subsent.ThrowIfNotTypeSubordinate();

            var pair = (from cet in subsent.ElementsCet()

                        let linkId = cet.GetAttributeLINK()
                        where ( linkId.HasValue && 
                                !cet.GetAttributeFULLNAME().IsEmptyOrNull() )

                        let entityBound = subsent.FindEntityById_4Subsent( linkId.Value ) //---subsent.Document.FindEntityById( linkId.Value )
                        where ( entityBound.IsNotNull() )

                        select new 
                        {
                            ENTITYBOUND = entityBound,
                            CETLINK     = cet,
                        }
                        ).FirstOrDefault();

            if ( pair != null )
            {
                pair.CETLINK.CreateFakeEntityAfterSelf( pair.ENTITYBOUND );
            }
        }

        private static void ProcessSubsentenceInquiryRules( XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();

            //Если это другой тип подпредложения, содержащий Inquire, то дальше оно обрабатывается 
            //  как самостоятельное 200-ми правилами и выставляются финальные SNT&FRT.
            if ( subsent.HasElementTypeInquiry() )
            {
                var result = subsent.ExecRules_Inquiry( language );
                //Check on zero-VERB
                if ( !result )
                {
                    result = ProcessSubsentenceInquiry4ZeroVerb( subsent );
                }

                //Выставление-перенос 'SNTFINAL' на сам элемент подпредложения.
                if ( result )
                {
                    #region [.process.]
                    var tuple = (from element in subsent.ElementsExceptT()
                                 where !element.IsElementSUBSENT()
                                 let sntfinl = element.TryGetAttributeSNTFINAL()
                                 where sntfinl.HasValue
                                 select new { SNTFINAL = sntfinl.Value, FRTFINAL = element.GetAttributeFRTFINAL() }
                                )
                                .FirstOrDefault();
                    if ( tuple != null )
                    {
                        subsent.SetAttributeSNTFINAL( tuple.SNTFINAL );
                        subsent.SetAttributeFRTFINAL( tuple.FRTFINAL );
                    }
                    #endregion
                }
            }
        }
        private static bool ProcessSubsentenceInquiry4ZeroVerb( XElement subsent )
        {
            subsent.ThrowIfNotSubsent();

            var inquiryEntity = subsent.ElementsExceptT()/*ElementsEntity()*/.FirstOrDefault( element => element.IsTypeInquiry() );
            if ( inquiryEntity != null )
            {
                if ( subsent.ElementsVerb().SelectMany( v => v.DescendantsAndSelfVerb() ).All( v => !v.HasTypeAttributeValue( TypeAttributeValue.Verb ) ) )
                {
                    //Specially for inquiry-zero-verb rule
                    //---RuleBase.MatchActionDebugInfoOutput( "zv_ov" );

                    var sntFinal = inquiryEntity.GetAttributeSNT();                    
                    switch ( sntFinal )
                    {
                        case TonalityType.collPos:
                        case TonalityType.adjPos:
                        case TonalityType.advPos:
                        case TonalityType.nAPos:
                            sntFinal = TonalityType.nPos; 
                        break;

                        case TonalityType.collNeg:
                        case TonalityType.adjNeg:
                        case TonalityType.advNeg:
                        case TonalityType.nANeg:
                            sntFinal = TonalityType.nNeg; 
                        break;
                    }

                    //Уточнение для нулевых глаголов: размечать (т.е. выставлять финальные SNT & FRT) только для nPos & nNeg (не для nPPos & nPNeg), чтобы не путаться при подсчете сантимента.
                    if ( (sntFinal == TonalityType.nPos) || (sntFinal == TonalityType.nNeg) )
                    {
                        //Specially for inquiry-zero-verb rule
                        RuleBase.MatchActionDebugInfoOutput( "zv_ov" );

                        var frtFinal = inquiryEntity.GetAttributeFRT();

                        inquiryEntity.SetAttributeSNTFINAL( sntFinal );
                        inquiryEntity.SetAttributeFRTFINAL( frtFinal );

                        var tuples = from entities in inquiryEntity.DescendantsEntities()
                                     where ( !entities.IsTypeInquiry() && !entities.HasElementsExceptT() )
                                     let ID = entities.GetAttributeID()
                                     where ID.HasValue

                                     let linkedSubsent = inquiryEntity.FindSubsentById( ID.Value )
                                     where (linkedSubsent != null)

                                     select new
                                     {
                                         LINKEDSUBSENT = linkedSubsent,
                                         ENTITIES      = entities,
                                     };
                        foreach ( var tuple in tuples )
                        {
                            tuple.LINKEDSUBSENT.SetAttributeSNTFINAL( sntFinal );
                            tuple.LINKEDSUBSENT.SetAttributeFRTFINAL( frtFinal );

                            tuple.ENTITIES.SetAttributeSNTFINAL( sntFinal );
                            tuple.ENTITIES.SetAttributeFRTFINAL( frtFinal );                            
                        }

                        return (true);
                    }
                }
            }
            return (false);
        }

        private static Nouns? GetOneVerbSNT_Participial_DanglingParticiple( XElement subsent )
        {
            subsent.ThrowIfNotSubsent();
            subsent.ThrowIfNotTypeParticipialOrDanglingParticiple();

            XElement oneVerb = null;
            foreach ( var e in subsent.Elements() )
            {
                if ( e.IsElementVERB() )
                {
                    if ( oneVerb != null )
                        return (null);

                    oneVerb = e;
                }
                else
                {
                    if ( e.IsElementENTITY() || e.IsElementENTITIES() || e.IsElementCET() )
                        return (null);
                }
            }

            if ( oneVerb != null )
            {
                var snt = oneVerb.GetAttributeSNT();
                switch ( snt )
                {
                    case TonalityType.collvPos:
                    case TonalityType.vPos:     
                        return (Nouns.nPos);
                    case TonalityType.collvNeg:
                    case TonalityType.vNeg:     
                        return (Nouns.nNeg);
                }
            }

            return (null);
        }
        #endregion

        #region [.UnionOfConcession & ReversIntoductory.]
        private static void ProcessUnionOfConcessionAndReversIntoductory( XDocument xdocument, Language language )
        {
            foreach ( var sentenceNode in xdocument.GetSents( language ) )
            {
                sentenceNode.ExecRules_UnionOfConcessionAndReversIntoductory( language );
            }
        }
        #endregion

        #region [.Explosive-Collocation.]
        private static void ProcessExplosiveCollocation( XDocument xdocument, Language language )
        {
            var nodes = xdocument.GetExplosiveCollocations( language );

            foreach ( XElement node in nodes )
            {
                var id = node.GetAttributeFIRST_ITEM_IN_COLLOCATION_ID().TryConvert2Int();
                if ( !id.HasValue ) 
                    continue;

                var subsent = node.GetAncestorSubsent();
                if ( subsent == null )
                    continue;

                var firstNodeInExplosiveCollocation = subsent.FindElementById( id.Value );
                if ( firstNodeInExplosiveCollocation == null )
                    continue;

                #region commened. previous
                /*
                var tuple = (from ancestor in firstNodeInExplosiveCollocation.AncestorsAndSelf()
                             let sntfinal = ancestor.TryGetAttributeSNTFINAL()
                             where (sntfinal.HasValue)
                             select new
                             {
                                SNTFINAL = sntfinal.Value,
                                FRTFINAL = ancestor.GetAttributeFRTFINAL()
                             }
                            ).FirstOrDefault();
                if ( tuple == null )
                    continue;

                node.SetAttributeSNTFINAL( tuple.SNTFINAL );
                node.SetAttributeFRTFINAL( tuple.FRTFINAL );
                */
                #endregion

                var tuple = (from ancestor in firstNodeInExplosiveCollocation.AncestorsAndSelf()
                                                                             .TakeWhile( a => !a.IsElementSUBSENT() )
                             let sntfinal = ancestor.TryGetAttributeSNTFINAL()
                             where (sntfinal.HasValue)
                             select new
                             {
                                SNTFINAL = sntfinal.Value,
                                FRTFINAL = ancestor.GetAttributeFRTFINAL()
                             }
                            ).FirstOrDefault();
                if ( tuple == null )
                    continue;

                node.SetAttributeSNTFINAL( tuple.SNTFINAL );
                node.SetAttributeFRTFINAL( tuple.FRTFINAL );
            }
        }
        #endregion

        #region [.Result.]
        /*1. Эти циферки нужно будет считывать и записывать их в FRT глаголов и глагольных колокаций.
        2. Теперь, после отработки всех правил, нужно будет суммировать FRT для позитива и негатива с учетом этого.
        Для этого расчет FRT начинается от 0 (было 0=1, 1=2 и т.д.), т.е. если FRT нет, то он 0.
        Считаем отдельно кол-во  предложений с негативом и отдельно с позитивом (как и было) – sumNeg/ sumPos.
        Считаем силу сантимента по всем предложениям отдельно для негатива и отдельно для позитива sumFrtNeg/ sumFrtPos.
        Возвращаем их отношение: 
        Общая сила негатива   frtNeg = sumFrtNeg/ sumNeg;
        Общая сила позитива   frtPos = sumFrtPos/ sumPos;
        Или в общем виде:     FRT_text=sumFRT /sumSNT

        [sumNeg] - сумма общего числа негативных объектов мониторинга для которых сработали 200-е правила и выставился финальный сантимент.
        [sumFrtNeg] - сумма финальных FRT у элементов [sumNeg]   

        3. Диапазон значений определения негатива/позитива по тексту
        Если 0<=Frt<=1 – слабый негатив / позитив (было 0<=Frt<1)
        Если 1<Frt<1,5 – средний негатив / позитив (было 1<=Frt<1,5)
        Если 1,5<=Frt  – сильный негатив / позитив

        4. Сантимент отдельного  предложения так же будет определяться:
        Если 0<=Frt<1 – слабый негатив / позитив
        Если 1<=Frt<2 – средний негатив / позитив
        Если 2<=Frt  – сильный негатив / позитив

        5. Сила сантимента по предложению будет демонстрироваться увеличением шрифта данного предложения в документе (слабый – обычный шрифт, средний – увеличенный, сильный - большой). 
        */
        /*Это то, что у нас было: 
        Общая сила негатива    frtNeg = sumFrtNeg /sumNeg;
        Общая сила позитива    frtPos = sumFrtPos /sumPos;

        если 0<=Frt<=1 – слабый негатив / позитив;
        Если 1<Frt<1,5 – средний негатив / позитив;
        Если 1,5<=Frt  – сильный негатив / позитив.

        Но для курьера хотят нормированное значение от 0 до 1
        Поэтому просто считаем и отдаем им цифру от 0 до 1, силу негатива пусть определяют сами. 

        Итак,
        Общая сила негатива  frtNeg = ((sumFrtNeg / (sumFrtNeg + sumNeg)) * (sumNeg / N);
        Общая сила позитива  frtPos = ((sumFrtPos / (sumFrtPos + sumPos)) * (sumPos / N);

        Где 
        sumPos - количество позитивного сантимента у объектов мониторинга в тексте
        sumNeg - количество негативного сантимента у объектов мониторинга в тексте
        N – количество подпредложений в тексте.
 
        В нашей версии, там где, например,
        Негатив (0,00): слабый, Позитив (0,67): слабый
        Пока не надо писать сильный/слабый и пр., а просто давать цифру, потому что я пока не знаю пределов.
        Вычислим их эмпирическим путем и потом выставим. */

        /// <summary>
        /// 
        /// </summary>
        private sealed class IntermediateResult
        {
            public IntermediateResult( XDocument xdocument, double? positive, double? negative )
            {
                XDocument = xdocument;
                Positive  = positive;
                Negative  = negative;
            }

            public XDocument XDocument
            {
                get;
                private set;
            }
            public double? Positive
            {
                get;
                private set;
            }
            public double? Negative
            {
                get;
                private set;
            }
        }

        /*Считаем FRT каждого подпредложения.
        Общая оценка тональности документа считается раздельно по негативному и позитивному компоненту разметки.

        Общая негативная оценка документа:
            AvrFRTNEG = (SumFrtNeg/(SumFrtNeg + SumFrtPos)) * (SumSntNeg/SumSnt)  //---SumFrtNeg/SumSntNeg

        Общая позитивная оценка документа:
            AvrFRTPOS = (SumFrtPos/(SumFrtNeg + SumFrtPos)) * (SumSntPos/SumSnt)  //---SumFrtPos/SumSntPos

        SumFrtNeg - сумма всех значений силы тональности для подпредложений с негативной тональностью;
        SumSntNeg – количество подпредложений, имеющих негативную тональность;
        SumFrtPos - сумма всех значений силы тональности для подпредложений с позитивной тональностью; 
        SumSntPos – количество подпредложений, имеющих позитивную тональность;
        */
        private static IntermediateResult CreateResult( XDocument xdocument, Language language/*, InputParamsBase inputParams*/ )
        {
            #region [.positive & negative.]
            double? positive = null;
            double? negative = null;

            var inquiriesSNTFinal = xdocument.GetAllLevelInquiriesWithSNTFINAL( language ).ToArray();
            if ( inquiriesSNTFinal.Length != 0 )
            {
                #region [.negative.]
                var inquiriesSNTFinalNegative = (from inquiry in inquiriesSNTFinal
                                                 where inquiry.IsSNTFINALNegative()
                                                 select inquiry
                                                ).ToArray();
                double sumSntNeg = inquiriesSNTFinalNegative.Length;
                #region
                /*if ( sumSntNeg != 0 )
                {
                    var sumFrtNeg = Convert.ToDouble( inquiriesSNTFinalNegative.Sum( e => e.GetAttributeFRTFINAL() ) );

                    negative = (sumFrtNeg / sumSntNeg);
                }*/
                    #endregion
                    #endregion

                #region [.positive.]
                var inquiriesSNTFinalPositive = (from inquiry in inquiriesSNTFinal
                                                 where inquiry.IsSNTFINALPositive()
                                                 select inquiry
                                                ).ToArray();
                double sumSntPos = inquiriesSNTFinalPositive.Length;
                #region
                /*if ( sumSntPos != 0 )
                {
                    var sumFrtPos = Convert.ToDouble( inquiriesSNTFinalPositive.Sum( e => e.GetAttributeFRTFINAL() ) );

                    positive = (sumFrtPos / sumSntPos);
                }*/
                    #endregion
                    #endregion

                #region [.negative & positive.]
                var sumSnt = sumSntNeg + sumSntPos;
                if ( sumSnt != 0 )
                {
                    if ( sumSntNeg != 0 || sumSntPos != 0 )
                    {
                        var sumFrtNeg = (sumSntNeg != 0) ? Convert.ToDouble( inquiriesSNTFinalNegative.Sum( e => e.GetAttributeFRTFINAL() ) ) : 0;
                        var sumFrtPos = (sumSntPos != 0) ? Convert.ToDouble( inquiriesSNTFinalPositive.Sum( e => e.GetAttributeFRTFINAL() ) ) : 0;

                        negative = (sumFrtNeg / (sumFrtNeg + sumFrtPos)) * (sumSntNeg / sumSnt);
                        positive = (sumFrtPos / (sumFrtNeg + sumFrtPos)) * (sumSntPos / sumSnt);

                        if ( double.IsNaN( negative.Value ) )
                            negative = 0.0;
                        if ( double.IsNaN( positive.Value ) )
                            positive = 0.0;
                    }
                }
                #endregion
            }
            #endregion

            #region commented. CompactOutputXmlFormatConverter
            /*
            #region [.to-compact.]
            if ( inputParams.OutputXmlFormat == OutputXmlFormat.Compact )
            {
                xdocument = Config.CompactOutputXmlFormatConverter.Convert( xdocument );
            } 
            #endregion
            */
            #endregion

            return (new IntermediateResult( xdocument, positive, negative ));
        }

        private static TonalityMarkingOutputResult CreateTonalityOutputResult(
            XDocument xdocument, Language language, InquiryAllocateStatus status, ObjectAllocateMethod objectAllocateMethod, ICoreferenceInfo coreferenceInfo )
        {
            var result = CreateResult( xdocument, language );

            var tonalityOutputResult = new TonalityMarkingOutputResult
                (
                result.XDocument, 
                status,
                result.Positive,
                result.Negative,
                result.XDocument.ExtractObjects( objectAllocateMethod, coreferenceInfo ),
                RuleBase.GetOperateRulesNamesDebugInfoOutput()
                );

            return (tonalityOutputResult); 
        }
        #endregion
        #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    internal static class ExtractorOfTonalityMarkingObject
    {
        private static bool IsHomogeneous( this XElement element )
        {
            return ((element.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE       ) == XLinqExtensions.HOMOGENEOUS_VALUE) ||
                    (element.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_SAVED_ATTRIBUTE ) == XLinqExtensions.HOMOGENEOUS_VALUE) 
                   );
        }

        private static Tuple< double?, double? > GetPosNegValues( this TonalityType tt )
        {
            var pos = default(double?);
            var neg = default(double?);

            switch ( tt )
            {
                case TonalityType.nPos:
                case TonalityType.collPos:
                case TonalityType.adjPos:
                case TonalityType.advPos:
                case TonalityType.nAPos:
                    pos = 1.0;
                    break;

                case TonalityType.nNeg:
                case TonalityType.collNeg:
                case TonalityType.adjNeg:
                case TonalityType.advNeg:
                case TonalityType.nANeg:
                    neg = 1.0;
                    break;
            }

            return (Tuple.Create( pos, neg ));
        }

        public static List< TonalityMarkingObject > ExtractObjects( this XElement element, 
            ObjectAllocateMethod objectAllocateMethod, ICoreferenceInfo coreferenceInfo )
        {
            var ls = new List< TonalityMarkingObject >();
            var hs = default(HashSet< XElement >);

            foreach ( var d in element.Descendants() )
            {
                if ( !d.IsINQUIRYINITIAL() )
                    continue;

                var _da = default(XElement);
                foreach ( var a in d.AncestorsAndSelf() )
                {
                    if ( a.HasAttributeSNTFINAL() )
                    {
                        _da = a;
                        break;
                    }
                    if ( !a.IsElementENTITY() && 
                         !a.IsElementENTITIES() && 
                         !a.IsElementCET() )
                    {
                        break;
                    }
                }

                var pos = default(double?);
                var neg = default(double?);
                if ( _da.IsNotNull() )
                {
                    var t = _da.GetAttributeSNTFINAL().GetPosNegValues();
                    pos = t.Item1;
                    neg = t.Item2;
                }

                if ( d.IsHomogeneous() )
                {
                    hs = d.GetObjectsFromHomogeneous( objectAllocateMethod, hs );
                    foreach ( var obj in hs )
                    {
                        var name = coreferenceInfo.TryGetMasterName( obj ) ?? obj.GetVALUETHEMEorVALUE();
                        ls.Add( new TonalityMarkingObject( d, obj, name, pos, neg ) );
                    }
                }
                else
                {             
                    var name = coreferenceInfo.TryGetMasterName( d ) ?? d.GetVALUETHEMEorVALUE();
                    ls.Add( new TonalityMarkingObject( d, name, pos, neg ) );
                }
            }

            return (ls);
        }

        public static List< TonalityMarkingObject > ExtractObjects( this XDocument xdoc, 
            ObjectAllocateMethod objectAllocateMethod, ICoreferenceInfo coreferenceInfo )
        {
            return (xdoc.Root.ExtractObjects( objectAllocateMethod, coreferenceInfo ));
        }
    }
}

