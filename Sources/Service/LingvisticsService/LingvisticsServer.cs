using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.ServiceProcess;
using System.Runtime.Remoting;
using System.Diagnostics;

using Lingvistics.Client;
using Config = Lingvistics.Properties.Settings;

#if WITH_OM_TM
using Digest;
using TextMining.Core;
using TonalityMarking;
#endif

namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
	public partial class LingvisticsServer : ServiceBase, ILingvisticsService
	{
        private static readonly log4net.ILog Logger = LOG.GetLogger();

		private static LingvisticsProcessorPool _ProcessorPool = new LingvisticsProcessorPool();

		public LingvisticsServer()
		{
			InitializeComponent();
		}

		public void Start()
		{
            Logger.Info( "Сервер запущен" );
            RemotingConfiguration.Configure( AppDomain.CurrentDomain.SetupInformation.ConfigurationFile, false );
            Logger.Info( "Сетевое конфигурирование выполнено" );

            //Начальный запуск процессоров            
            int procCount = Math.Min( Config.Default.MaxProcessorCount, Config.Default.StartupProcessorCount );
            var itemList = new List< LingvisticsProcessor >( procCount );
            for ( int i = 0; i < procCount; i++ )
            {
                itemList.Add( _ProcessorPool.GetItem( ProcessingType.Light ) );
            }
            foreach ( var poolItem in itemList )
            {
                _ProcessorPool.FreeItem( poolItem );
            }

		}
		public new void Stop()
		{
			Logger.Info("Получена команда на остановку сервера");
			_ProcessorPool.Close();
			Logger.Info("Сервер остановлен");
		}

		protected override void OnStart(string[] args)
		{
			Start();
		}
		protected override void OnStop()
		{
			Stop();
		}

		private TResult CallLinguisticFunction< TResult >(Func< ILingvisticsProcessor, TResult > function, ProcessingType procType )
		{
			//Повтор в случае ошибки [maxRepeatTimes] раз 
            for ( int maxRepeatTimes = 1; true; maxRepeatTimes-- )
			{
                var sw = Stopwatch.StartNew();
                var poolItem = _ProcessorPool.GetItem( procType );
                var totalSecondsGetItem = sw.Elapsed.TotalSeconds;

                if ( Logger.IsDebugEnabled )
                {
                    Logger.DebugFormat( "LoadStatus: {0}", _ProcessorPool.GetLoadingStatus() );
                }
                try
                {
                    return function( poolItem );
                }
                catch ( System.TimeoutException )
                {
                    throw;
                }
                catch ( ApplicationException )
                {
                    throw;
                }
                catch ( Exception ex )
                {
                    Logger.ErrorFormat( "Proc({0})\n{1}", poolItem.ProcessID, ex.ToString() );
                    if ( maxRepeatTimes > 0 )
                    {
                        continue;
                    }
                    throw;
                }
                finally
                {
                    if ( poolItem.Busy && !poolItem.Closed )
                    {
                        _ProcessorPool.FreeItem( poolItem );
                    }
                    sw.Stop();
                    Logger.DebugFormat( "Proc({0}) dT={1:F3}c, dT(GetItem)={2:F3}c, dT(Fun)={3:F3}c",
                        poolItem.ProcessID, sw.Elapsed.TotalSeconds, totalSecondsGetItem, sw.Elapsed.TotalSeconds - totalSecondsGetItem );
                }
			}
		}

		#region [.ILingvisticService.]
		public ServerStatus GetLoadingStatus()
		{
			return (_ProcessorPool.GetLoadingStatus());
		}
		#endregion

        #region [.ILingvisticServer.]
        public string[] GetAllWordForms( string word )
        {
            return CallLinguisticFunction< string[] >( f => f.GetAllWordForms( word ), ProcessingType.Light );
        }

        public Tuple< string[], string[] > GetAllWordFormsWithPartsOfSpeech( string word )
        {
            return CallLinguisticFunction< Tuple< string[], string[] > >( f => f.GetAllWordFormsWithPartsOfSpeech( word ), ProcessingType.Light );
        }

        public string GetNormalWordForm( string word )
        {
            return CallLinguisticFunction< string >( f => f.GetNormalWordForm( word ), ProcessingType.Light );
        }

        public string[] GetAllNormalWordForm( string word )
        {
            return CallLinguisticFunction< string[] >( f => f.GetAllNormalWordForm( word ), ProcessingType.Light );
        }

        public LingvisticsResult ProcessText( string                   text, 
                                              bool                     afterSpellChecking, 
                                              DateTime                 baseDate,
                                              LingvisticsResultOptions options              = LingvisticsResultOptions.All,
                                              SelectEntitiesMode       mode                 = SelectEntitiesMode.Full, 
                                              bool                     generateAllSubthemes = false )
        {
            var input = new LingvisticsTextInput()
            {
                Text                 = text,
                AfterSpellChecking   = afterSpellChecking,
                BaseDate             = baseDate,
                Options              = options,
                Mode                 = mode,
                GenerateAllSubthemes = generateAllSubthemes,
#if WITH_OM_TM
				ObjectAllocateMethod = ObjectAllocateMethod.FirstEntityWithTypePronoun,
#endif
            };
            return ProcessText( input );
        }

        public LingvisticsResult ProcessText( LingvisticsTextInput input )
        {
            int maxTextSize = Math.Min( input.Text.Length, Config.Default.ProcessorMaxTextSize );
            //цикл на 2 повтора с уменьшением размера блока, если ни один блок за первый раз не обработан
            for ( int i = 0; i < 2 && 0 < maxTextSize; ++i )
            {
                var blockList = (input.Text.Length <= maxTextSize) ?
                                //обработка всего документа за раз
                                new[] { input.Text } :
                                //итерационная обработка документа по частям
                                TextSplitter.Split( input.Text, maxTextSize );

                Logger.DebugFormat( "DocSize={0}, PartCount={1}, i={2}", input.Text.Length, blockList.Length, i );

                var rcList = blockList.Select( t =>
                {
                    LingvisticsResult result = null;
                    try
                    {
                        //Повтор в случае ошибки [maxRepeatTimes] раз 
                        for ( int maxRepeatTimes = 1; true; --maxRepeatTimes )
                        {
                            var sw = Stopwatch.StartNew();
                            var poolItem = _ProcessorPool.GetItem( ProcessingType.Heavy );
                            var totalSecondsGetItem = sw.Elapsed.TotalSeconds;

                            if ( Logger.IsDebugEnabled )
                            {
                                Logger.DebugFormat( "LoadStatus: {0}", _ProcessorPool.GetLoadingStatus() );
                            }
                            try
                            {
                                var lti = new LingvisticsTextInput( input, t );
                                result = poolItem.ProcessText( lti );
                                break;
                            }
                            catch ( System.TimeoutException )
                            {
                                throw;
                            }
                            catch ( ApplicationException )
                            {
                                throw;
                            }
                            catch ( Exception ex )
                            {
                                Logger.ErrorFormat( "Proc({0})\n{1}", poolItem.ProcessID, ex.ToString() );
                                if ( 0 < maxRepeatTimes )
                                {
                                    continue;
                                }
                                throw;
                            }
                            finally
                            {
                                if ( poolItem.Busy && !poolItem.Closed )
                                {
                                    _ProcessorPool.FreeItem( poolItem );
                                }
                                sw.Stop();
                                Logger.DebugFormat( "Proc({0}) dT={1:F3}c, dT(GetItem)={2:F3}c, dT(Fun)={3:F3}c",
                                    poolItem.ProcessID, sw.Elapsed.TotalSeconds, totalSecondsGetItem, sw.Elapsed.TotalSeconds - totalSecondsGetItem );
                            }
                        }//Повтор в случае ошибки [maxRepeatTimes] раз 
                    }
                    catch ( Exception ex )
                    {
                        Logger.ErrorFormat( "TextPart={0}\n{1}", t, ex.Message );
                        throw;
                    }
                    return (new Tuple< LingvisticsResult, string >( result, t ));
                }
                ).ToArray();

                if ( rcList.Any( t => t.Item1 != null ) )
                {
                    //объединение частичных результатов
                    if ( 1 < blockList.Length )
                    {
                        var rdfIsEmpty = default(bool);
                        var rdf = RDFUnifier.Union( rcList.Select( t => new Tuple< string, int >( ((t.Item1 == null) ? null : t.Item1.RDF ), t.Item2.Length ) ), out rdfIsEmpty );
                        var semNet = SemnetUnifier.Union( rcList.Where( t => t.Item1 != null )
                                .Select( t => new Tuple< ThemeItem[], LinkItem[] >( t.Item1.ThemeList, t.Item1.LinkList ) ) 
                        );
#if WITH_OM_TM
                        if ( (input.Options & LingvisticsResultOptions.OpinionMiningWithTonality) == LingvisticsResultOptions.OpinionMiningWithTonality )
                        {
                            var opinionMiningWithTonalityResult = TonalityResultUnifier.Union(
                                rcList.Select( t => new Tuple< DigestOutputResult, int >(
                                    (t.Item1 == null) ? null : t.Item1.OpinionMiningWithTonalityResult, t.Item2.Length )
                                )
                            );
                            return (new LingvisticsResult( input.Options, rdf, semNet.Item1, semNet.Item2, opinionMiningWithTonalityResult ));
                        }
                        else
                        if ( (input.Options & LingvisticsResultOptions.Tonality) == LingvisticsResultOptions.Tonality )
                        {
                            var tonalityResult = TonalityResultUnifier.Union( rdf, rdfIsEmpty,
                                    rcList.Select( t => (t.Item1 == null) ? null : t.Item1.TonalityResult )
                            );
                            return (new LingvisticsResult( input.Options, rdf, semNet.Item1, semNet.Item2, tonalityResult ));
                        }
#endif
                        return (new LingvisticsResult( input.Options, rdf, semNet.Item1, semNet.Item2 ));
                    }
                    else
                    {
                        return (rcList.First().Item1);
                    }
                }
                maxTextSize = maxTextSize >> 1;
            }
            return (null);
        }

        public PTSResult ProcessPTS( string xml, bool buildSemanticNetwork, string language )
        {
            return CallLinguisticFunction< PTSResult >( f => f.ProcessPTS( xml, buildSemanticNetwork, language ), ProcessingType.Heavy );
        }

        public LingvisticsResult ProcessRDF( string rdf, LingvisticsResultOptions options, bool generateAllSubthemes = false )
        {
            return CallLinguisticFunction< LingvisticsResult >( f => f.ProcessRDF( rdf, options, generateAllSubthemes ), ProcessingType.Light );
        }
        public LingvisticsResult ProcessRDF( LingvisticsRDFInput input )
        {
            return CallLinguisticFunction< LingvisticsResult >( f => f.ProcessRDF( input ), ProcessingType.Light );
        }
        #endregion
	}
}
