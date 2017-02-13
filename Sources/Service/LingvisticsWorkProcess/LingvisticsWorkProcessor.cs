using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Threading;
using System.Xml.Linq;

using CoreferenceResolving;
using Linguistics.Coreference;
using Lingvistics.Client;
using LinguisticsKernelConroller = LinguisticsKernel.LinguisticsKernelConroller;
using Config = Lingvistics.Properties.Settings;

#if WITH_OM_TM
using Digest;
using TextMining.Core;
using TonalityMarking;

using BlockAttribute = Lingvistics.BlockAttribute;
using EntityRole     = Lingvistics.EntityRole;
using EntityType     = Lingvistics.EntityType;
using NodeName       = Lingvistics.NodeName;
#else
using BlockAttribute = Linguistics.Core.BlockAttribute;
using EntityRole     = Linguistics.Core.EntityRole;
using EntityType     = Linguistics.Core.EntityType;
using NodeName       = Linguistics.Core.UnitTextType;
#endif

namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
	public sealed class LingvisticsWorkProcessor : MarshalByRefObject, ILingvisticsProcessor
	{
		private static readonly WindowsEvent        _StartEvent;
		private static readonly ManualResetEvent    _StopEvent;
		private static readonly Thread              _WaitThread;
		private static readonly CoreferenceResolver _CoreferenceResolver;
		private static LinguisticsKernelConroller _LinguisticsKernelConroller;

		static LingvisticsWorkProcessor()
		{
//System.Diagnostics.Debugger.Break();
			_CoreferenceResolver = new CoreferenceResolver();

            _StopEvent  = new ManualResetEvent( false );

            _WaitThread = new Thread( WorkThreadRoutine );
            _WaitThread.Start();

            //Выставить событие инициализации
            //---Thread.Sleep( 10000 ); - WTF???

            _StartEvent = new WindowsEvent( string.Format( "LPStartEvent_{0}", Process.GetCurrentProcess().Id ) );
			_StartEvent.Set();
		}

        private static void WorkThreadRoutine()
        {
//System.Diagnostics.Debugger.Break();
            _LinguisticsKernelConroller = new LinguisticsKernelConroller();

            while ( !_StopEvent.WaitOne( 1000 ) )
            {
                long sizeInMB = Process.GetCurrentProcess().VirtualMemorySize64 / (1024 * 1024);
                if ( sizeInMB > Config.Default.MaxProcessorVirtualMemoryInMB )
                {
                    LOG.GetLogger().FatalFormat( "ProcID={0} закрыт из-за перерасхода памяти", Process.GetCurrentProcess().Id );
                    break;
                }
            }
            _LinguisticsKernelConroller.Close();
        }
		private static LinguisticsKernelConroller GetLinguisticsKernelConroller()
		{
			while ( _LinguisticsKernelConroller == null )
			{
				if ( _StopEvent.WaitOne( 10 ) )
				{
					throw (new ApplicationException("Процессор в состоянии закрытия"));
				}
			}
			return (_LinguisticsKernelConroller);
		}

		#region [.ILingvisticProcessor.]
		public void Close()
		{
			_StopEvent.Set();
			_StartEvent.Close();
		}
		#endregion

		#region [.ILingvisticServer.]
		public string[] GetAllWordForms( string word )
		{
            return (GetLinguisticsKernelConroller().GetAllWordForms( word ).ToArray());
		}

		public Tuple< string[], string[] > GetAllWordFormsWithPartsOfSpeech( string word )
		{
            var result = GetLinguisticsKernelConroller().GetAllWordFormsWithPartsOfSpeech( word );
			return (new Tuple< string[], string[] >(result.Item1.ToArray(), result.Item2.ToArray()));
		}

        public string GetNormalWordForm( string word )
		{
            var normWord = GetLinguisticsKernelConroller().GetNormalWordForm( word );
            return (string.IsNullOrEmpty( normWord ) ? word : normWord);
		}

        public string[] GetAllNormalWordForm( string word )
		{
            var rc = GetLinguisticsKernelConroller()
                            .GetAllNormalWordForm( word )
                            .Where( s => !string.IsNullOrEmpty( s ) )
                            .Distinct()
                            .ToArray();
            return (rc.Any() ? rc : new[] { word });
		}

		public LingvisticsResult ProcessText(
				  string                   text,
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
            return (ProcessText( input ));
		}

        public LingvisticsResult ProcessText( LingvisticsTextInput input )
		{
//System.Diagnostics.Debugger.Break();
            if ( input == null ) throw (new ArgumentNullException( "input" ));
            if ( input.Options == LingvisticsResultOptions.None )
			{
				return (null);
			}

			var lkc         = GetLinguisticsKernelConroller();
			var rdfXml      = lkc.GetRDF_New( input.Text, input.AfterSpellChecking, input.BaseDate, (int)input.Mode );
			var coreference = Config.Default.UseCoreferenceResolution ? _CoreferenceResolver.Process( rdfXml ) : null;
			var result      = GetResultFromRDF( rdfXml, coreference, input.Options, input.GenerateAllSubthemes );

#if WITH_OM_TM            
            if ( (input.Options & LingvisticsResultOptions.OpinionMiningWithTonality) == LingvisticsResultOptions.OpinionMiningWithTonality )
            {
                DigestOutputResult opinionMiningWithTonalityResult = CreateOpinionMiningWithTonalityResult( rdfXml, coreference, input.ObjectAllocateMethod );
                return (new LingvisticsResult( input.Options, result.RDF, result.ThemeList, result.LinkList, opinionMiningWithTonalityResult ));
            }
            else
            if ( (input.Options & LingvisticsResultOptions.Tonality) == LingvisticsResultOptions.Tonality )
            {
                TonalityMarkingOutputResult tonalityResult = CreateTonalityResult( rdfXml, coreference, input.ObjectAllocateMethod, input.TonalityMarkingInput );
                return (new LingvisticsResult( input.Options, result.RDF, result.ThemeList, result.LinkList, tonalityResult ));
            }
#endif
            return (result);
		}

        public PTSResult ProcessPTS( string xml, bool buildSemanticNetwork, string language )
		{
			var lkc    = GetLinguisticsKernelConroller();
            var rdfXml = lkc.ProcessPTS( xml, buildSemanticNetwork, string.IsNullOrEmpty( language ) ? null : language.ToUpper() );
			LingvisticsResult lingvisticResult = null;
            if ( buildSemanticNetwork )
            {
                lingvisticResult = GetResultFromRDF( rdfXml.Item1, null, LingvisticsResultOptions.All, true );
            }
			return (new PTSResult() { LingvisticResult = lingvisticResult, TextRanges = rdfXml.Item2.ToString() });
		}

		public LingvisticsResult ProcessRDF( string rdf, LingvisticsResultOptions options, bool generateAllSubthemes = false )
		{
			var input = new LingvisticsRDFInput()
			{
				Rdf                  = rdf,
				Options              = options,
				GenerateAllSubthemes = generateAllSubthemes,
#if WITH_OM_TM
				ObjectAllocateMethod = ObjectAllocateMethod.FirstEntityWithTypePronoun,
#endif
			};
            return (ProcessRDF( input ));
		}
        public LingvisticsResult ProcessRDF( LingvisticsRDFInput input )
		{
            if ( input == null ) throw (new ArgumentNullException( "input" ));
            if ( input.Options == LingvisticsResultOptions.None || input.Options == LingvisticsResultOptions.RDF )
            {
                return (null);
            }

			var lkc         = GetLinguisticsKernelConroller();
			var rdfXml      = XElement.Parse( input.Rdf );
			var coreference = _CoreferenceResolver.ReadFromRdf( rdfXml );
			var result      = GetResultFromRDF( rdfXml, coreference, input.Options, input.GenerateAllSubthemes );

#if WITH_OM_TM
            if ( (input.Options & LingvisticsResultOptions.OpinionMiningWithTonality) == LingvisticsResultOptions.OpinionMiningWithTonality )
			{
                DigestOutputResult opinionMiningWithTonalityResult = CreateOpinionMiningWithTonalityResult( rdfXml, coreference, input.ObjectAllocateMethod );
                return (new LingvisticsResult( input.Options, result.RDF, result.ThemeList, result.LinkList, opinionMiningWithTonalityResult ));
			}
            else
            if ( (input.Options & LingvisticsResultOptions.Tonality) == LingvisticsResultOptions.Tonality )
            {
                TonalityMarkingOutputResult tonalityResult = CreateTonalityResult( rdfXml, coreference, input.ObjectAllocateMethod, input.TonalityMarkingInput );
                return (new LingvisticsResult( input.Options, result.RDF, result.ThemeList, result.LinkList, tonalityResult ));
            }  
#endif
            return (result);
		}
		#endregion

		private LingvisticsResult GetResultFromRDF( XElement rdfXml, ICoreferenceInfo coreferenceInfo, LingvisticsResultOptions options, bool generateAllSubThemes )
		{
			var rdf       = default(string);
			var themeList = default(ThemeItem[]);
			var linkList  = default(LinkItem[]);
			var lkc       = GetLinguisticsKernelConroller();

            if ( (options & LingvisticsResultOptions.RDF) != LingvisticsResultOptions.None )
            {
                rdf = rdfXml.ToString();
            }
            if ( (options & LingvisticsResultOptions.ThemeList) == LingvisticsResultOptions.ThemeList )
            {
                if ( (options & LingvisticsResultOptions.SemNet) == LingvisticsResultOptions.SemNet )
                {
                    var sn = lkc.GetSemanticNetwork( rdfXml, coreferenceInfo, generateAllSubThemes );
                    themeList = GetThemeList( sn.Item1 );
                    linkList = GetLinkList( sn.Item2 );
                }
                else //LingvisticResultOptions.ThemeList
                {
                    themeList = GetThemeList( rdfXml, generateAllSubThemes, lkc );
                }
            }
			return (new LingvisticsResult( options, rdf, themeList, linkList ));
		}

        private static string GetAttr( XElement xe, object name )
        {
            var attr = xe.Attribute( name.ToString() );
            return ((attr == null) ? null : attr.Value);
        }
        private static ThemeItem[] GetThemeList( IEnumerable< Lingvistics.Types.ThemeItem > lingThemeList )
        {
            return lingThemeList.Select( t =>
            {
                EntityType entType;
                if ( !Enum.TryParse< EntityType >( t.Type, out entType ) )
                {
                    throw (new ApplicationException( string.Format( "Неправильный семантический тип [{0}]", t.Type ) ));
                }
                return (new ThemeItem()
                {
                    ID           = t.ID,
                    Name         = t.Name,
                    OriginalName = t.OriginalName,
                    Type         = entType,
                    FreqAdj      = t.FreqAdj,
                    FreqObj      = t.FreqObj,
                    FreqSubj     = t.FreqSubj,
                    FreqOther    = t.FreqOther
                });
            }
            ).ToArray();
        }
		private static ThemeItem[] GetThemeList( XElement rdf, bool generateAllSubthemes, LinguisticsKernelConroller lkc )
		{
            var entList = rdf.Descendants( NodeName.ENTITY.ToString() );
            if ( !generateAllSubthemes )
            {
                entList = entList.Where( t => t.Parent.Name == NodeName.SUB_SENT.ToString() );
            }
            return entList.Select( xe =>
                {
                    if ( !lkc.IsTheme( xe ) ) return (null);
                    EntityType entType;
                    if ( !Enum.TryParse< EntityType >( GetAttr( xe, BlockAttribute.TYPE ), out entType ) )
                    {
                        throw (new ApplicationException( string.Format( "Неправильный семантический тип [{0}]", GetAttr( xe, BlockAttribute.TYPE ) ) ));
                    }
                    string name = GetAttr( xe, BlockAttribute.FULLNAME );
                    if ( string.IsNullOrEmpty( name ) )
                    {
                        name = GetAttr( xe, BlockAttribute.VALUE );
                    }
                    return (new { Name = name, Type = entType, Role = GetAttr( xe, BlockAttribute.ROLE ) });
                }
            ).Where( t => t != null ).GroupBy( t => new { Name = t.Name.ToUpper(), Type = t.Type } ).Select( (gr, idx) =>
                {
                    var adj   = EntityRole.Adj  .ToString();
                    var subj  = EntityRole.Subj .ToString();
                    var obj   = EntityRole.Obj  .ToString();
                    var other = EntityRole.Other.ToString();
                    return new ThemeItem()
                    {
                        ID        = idx,
                        Name      = gr.First().Name,
                        Type      = gr.Key.Type,
                        FreqAdj   = gr.Count( t => t.Role == adj   ),
                        FreqSubj  = gr.Count( t => t.Role == subj  ),
                        FreqObj   = gr.Count( t => t.Role == obj   ),
                        FreqOther = gr.Count( t => t.Role == other ),
                    };
                }
            ).ToArray();
		}
        private static LinkItem[]  GetLinkList( IEnumerable< Lingvistics.Types.LinkItem > linkList )
		{
			return linkList.Select(t =>
				{
					return (new LinkItem()
					{
						SourceThemeID = t.SourceThemeID,
						DestThemeID   = t.DestThemeID,
						Type          = t.Type,
						Freq          = t.Freq
					});
				}
			).ToArray();
		}

#if WITH_OM_TM
		private static DigestOutputResult CreateOpinionMiningWithTonalityResult( 
            XElement rdf, ICoreferenceInfo coreferenceInfo, ObjectAllocateMethod objectAllocateMethod )
		{
			var xdoc = new XDocument( rdf );

            var result = DigestWcfService.ExecuteDigestInprocWithLinguisticService( xdoc, coreferenceInfo, objectAllocateMethod );

			return (result);
		}

        private static TonalityMarkingOutputResult CreateTonalityResult( 
            XElement rdf, ICoreferenceInfo coreferenceInfo, ObjectAllocateMethod objectAllocateMethod, 
            TonalityMarkingInputParams4InProcess inputParams )
		{
            var xdoc = new XDocument( rdf );

            var result = TonalityMarkingWcfService.ExecuteTonalityMarkingInprocWithLinguisticService( xdoc, coreferenceInfo, objectAllocateMethod, inputParams );

			return (result);
		}
#endif
	}
}
