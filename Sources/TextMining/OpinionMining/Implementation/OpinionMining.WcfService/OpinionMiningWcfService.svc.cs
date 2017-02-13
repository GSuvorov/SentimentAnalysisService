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

using log4net;
[assembly: log4net.Config.XmlConfigurator( ConfigFile = "OpinionMiningWcfService_log4net.xml", Watch = false )]

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;

    #region [.wcf-service attribute's.]
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
    public class OpinionMiningWcfService : IOpinionMining
    {
        #region [.Static field's & properties.]
        internal static readonly ILog LOG = LogManager.GetLogger( typeof(OpinionMiningWcfService).Name );
        private static readonly ICoreferenceResolver _CoreferenceResolver;
        #endregion

        #region [.ctor().]
        static OpinionMiningWcfService()
        {
            OperationContext.Current.AdjustBinding4AllEndpoints();
#if !WITH_INPROC_LINGVISTIC
            _CoreferenceResolver = new CoreferenceResolver();
#endif
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

        #region [.IOpinionMining.]
        public OpinionMiningOutputResult ExecuteOpinionMining( OpinionMiningInputParams inputParams )
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
                    break;
                    case InputTextFormat.LinguisticXml:
                        xdocument = Algorithms.PreprocessRdf( inputParams.InputText );
                    break;
                    default: 
                        throw (new ArgumentException(inputParams.InputTextFormat.ToString()));
                }                
                linguisticStopwatch.Stop();

                if ( !xdocument.Root.HasElements ) throw (new ArgumentException("Linguistics returned empty xml."));
            #endregion

            #region [.read coreference-info from Rdf.]
                var coreferenceInfo = _CoreferenceResolver.ReadFromRdf( xdocument.Root );
            #endregion

            #region [.Opinion-mining.]
                var opinionMiningStopwatch = Stopwatch.StartNew();
                
                var opinionMiningTuples = ExecuteInternal( xdocument, inputParams, coreferenceInfo );

                opinionMiningStopwatch.Stop();
            #endregion

            #region [.write in log.]
                LOG.InfoEx( MethodBase.GetCurrentMethod().Name, WcfHelper.ClientRemoteAddress, linguisticStopwatch.Elapsed, opinionMiningStopwatch.Elapsed, inputParams );
            #endregion

            #region [.result.]
                var opinionMiningOutputResult = new OpinionMiningOutputResult
                    (
                    opinionMiningTuples,
                    RuleBase.GetOperateRulesNamesDebugInfoOutput()
                    );

                return (opinionMiningOutputResult); 
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

        #region [.4 use only from Linguistic-service. use as-inprocess build with Linguistic-service.]
        public static OpinionMiningOutputResult ExecuteOpinionMiningInprocWithLinguisticService( 
            XDocument rdf, bool callPreprocessRdf, ObjectAllocateMethod objectAllocateMethod, ICoreferenceInfo coreferenceInfo )
        {
            #region [.check input.]
            rdf.ThrowIfNull( "rdf" );
            if ( !rdf.Root.HasElements ) throw (new ArgumentException("Input linguistics-RDF is empty."));
            #endregion

            #region [.Opinion-mining.]
            if ( callPreprocessRdf )
            {
                rdf = Algorithms.PreprocessRdf( rdf );
            }

            var inputParams = new OpinionMiningInputParams( "[NEVER MIND]", InputTextFormat.LinguisticXml )
                { ObjectAllocateMethod = objectAllocateMethod };

            var opinionMiningTuples = ExecuteInternal( rdf, inputParams, coreferenceInfo );
            #endregion

            #region [.result.]
            var opinionMiningOutputResult = new OpinionMiningOutputResult( opinionMiningTuples, RuleBase.GetOperateRulesNamesDebugInfoOutput() );
            return (opinionMiningOutputResult); 
            #endregion
        }
        #endregion

        #region [.Private method's & properties.]
        private static IEnumerable< OpinionMiningTuple > ExecuteInternal(
            XDocument xdocument, InputParamsBase inputParams, ICoreferenceInfo coreferenceInfo )
        {
            var language = Config.ThemesManager[ inputParams.ThemeType ].LanguagesManager[ inputParams.LanguageType ];

            int sentGlobalNumber = 0;
            int directAndIndirectSpeechGlobalNumber = 0;
            var opinionMiningTuples  = new List< OpinionMiningTuple >();            
            var objectAllocateMethod = inputParams.ObjectAllocateMethod;
            var isi = new InquiriesSynonymsInfo( inputParams.InquiriesSynonyms );

            #region [.Direct-speech & Indirect-speech.]
            var sents = xdocument.GetSentences( language );
            foreach ( var sent in sents )
            {
                #region [.check 'max-sent-count-in-text'.]
                if ( Config.ResultLimit_MaxSentCountInText <= sentGlobalNumber )
                {
                    LOG.Error( "Превышено допустимое число предложений в тексте, будут обработанны только первые '" + Config.ResultLimit_MaxSentCountInText + "' предложений");
                    break;
                }
                #endregion

                #region [.check 'max-sent-length-without-space'.]
                var lengthWithoutSpace = sent.Value.LengthWithoutSpace();
                if ( Config.ResultLimit_MaxSentLengthWithoutSpace < lengthWithoutSpace )
                {
                    LOG.Error( "Превышена допустимая длина (в '" + Config.ResultLimit_MaxSentLengthWithoutSpace + "' символов) одного предложения: '" + lengthWithoutSpace + "', данное предложение не будет обработанно: \r\n" + sent.ToString() );
                    continue;
                }
                #endregion

                var subjectObjectsTuples = language.Rules.Process( sent, ref directAndIndirectSpeechGlobalNumber, inputParams.ObjectAllocateMethod );
                //[some-concrete-subject]
                if ( subjectObjectsTuples.AnyEx() )
                {
                    #region
                    foreach ( var t in subjectObjectsTuples )
                    {
                        //[some-objects] are exists
                        if ( t.Objects.Any() )
                        {
                            #region
                            var omts1 = from o  in t.Objects
                                        from sd in t.Subjects
                                        from s  in sd.SubjectEssences
                                        let fs = inputParams.InquiriesSynonyms.IsContainsInSynonyms( s, o, coreferenceInfo )
                                        where fs.HasValue 
                                        select new OpinionMiningTuple
                                                ( 
                                                s, 
                                                o, 
                                                sent, 
                                                sentGlobalNumber, 
                                                fs.Value,
                                                coreferenceInfo
                                                );

                            opinionMiningTuples.AddRange( omts1 );
                            #endregion
                        }
                        //not has [some-objects]
                        else
                        {
                            #region
                            var omts2 = from sd in t.Subjects
                                        from s  in sd.SubjectEssences
                                        where s.IsContainsInSynonyms( inputParams.InquiriesSynonyms, coreferenceInfo )
                                        select new OpinionMiningTuple
                                                ( 
                                                s, 
                                                sent, 
                                                sentGlobalNumber, 
                                                FilterBySynonyms.Subject,
                                                coreferenceInfo
                                                );

                            opinionMiningTuples.AddRange( omts2 );
                            #endregion
                        }

                        //[Author-subject]-[subject-as-object]
                        #region
                        var omts3 = from sd in t.Subjects
                                    from s  in sd.SubjectEssences
                                    where s.IsContainsInSynonyms( inputParams.InquiriesSynonyms, coreferenceInfo )
                                    select OpinionMiningTuple.Create4AuthorSubject
                                            ( 
                                            s.ToObjectEssence(), 
                                            sent.Copy().RemoveDirectAndIndirectSpeechBeginEndAttributes(),
                                            sentGlobalNumber, 
                                            FilterBySynonyms.Object,
                                            coreferenceInfo
                                            );

                        opinionMiningTuples.AddRange( omts3 );
                        #endregion
                    }
                    #endregion
                }
                //[Author-subject]-[some-objects]
                else
                {
                    #region
                    var os = sent.TryAllocateObjects4AuthorSubject( objectAllocateMethod, isi );
                    #region [.check 'max-object-in-one-sent'.]
                    if ( Config.ResultLimit_MaxObjectInOneSent  < os.Count )
                    {
                        LOG.Error( "Превышено допустимое число объектов в одном предложении: '" + os.Count + "', будут использоваться только первые '" + Config.ResultLimit_MaxObjectInOneSent + "' объектов: \r\n" + sent.ToString() );
                    }
                    #endregion
                    var omts = from o in os.Take( Config.ResultLimit_MaxObjectInOneSent )
                               select OpinionMiningTuple.Create4AuthorSubject
                                        ( 
                                        o, 
                                        sent, 
                                        sentGlobalNumber, 
                                        FilterBySynonyms.Object,
                                        coreferenceInfo
                                        );

                    opinionMiningTuples.AddRange( omts );
                    #endregion
                }

                sentGlobalNumber++;
            }
            #endregion

            #region [.Reprocess EssenceItems 4 Homogeneous.]
            foreach ( var omt in opinionMiningTuples )
            {
                Common.ReprocessEssenceItems4Homogeneous( omt, coreferenceInfo /*, inputParams.ObjectAllocateMethod*/ );
            }
            #endregion

            return (opinionMiningTuples);
        }
        #endregion
    }
}

