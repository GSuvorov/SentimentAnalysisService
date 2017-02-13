using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Web.Hosting;
using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;
using TextMining.Core;
using TextMining.Service;

#if LICENSED
using System.Reflection;
using System.Resources;
using TakeoutMachineIdentifier;
#endif

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    internal class ConfigImpl : ConfigBase
    {
        public override string ConfigSectionName
        {
            get { return (ConfigBase.TONALITY_MARKING_SECTION_NAME); }
        }

#if WITH_INPROC_LINGVISTIC
        public XElement GetLinguisticsNode( XDocument configSection )
        {
            return (GetXElementByXPath( configSection, ConfigBase.LINGUISTICS_XPATH ));
        }
        public XElement GetTonalityMarkingSection( XDocument configSection )
        {
            return (GetXElementByXPath( configSection, ConfigBase.TONALITY_MARKING_SECTION_NAME ));
        }
        public XDocument GetReallyRootSection()
        {
            return (ConfigSection ?? (XDocument) ConfigurationManager.GetSection( "Digest" ));
        }

        private XElement GetXElementByXPath( XDocument configSection, string url )
        {
            var linguistics = configSection.Root.GetSingleNode( url );

            if ( linguistics.IsNull() )
            {
                throw (new ConfigException( url.InSingleQuote() + " section is empty" ));
            }

            return (linguistics);
        }
#endif
    }

    /// <summary>
    /// 
    /// </summary>
    internal static class Config
    {
        private static readonly ConfigImpl _ConfigImpl = new ConfigImpl();

#if WITH_INPROC_LINGVISTIC
        internal static void ExecuteConfigure()
        {
            lock ( _ConfigImpl.SyncRoot )
            {
                var configSection = _ConfigImpl.GetReallyRootSection();

                //.Get Digest config-section
                var linguistics = _ConfigImpl.GetLinguisticsNode( configSection );
                //---var compactOutputXmlFormat = _ConfigImpl.GetCompactOutputXmlFormatNode();

                //.Set Tonality-marking config-section
                var tonalityMarkingSection = _ConfigImpl.GetTonalityMarkingSection( configSection );
                tonalityMarkingSection.Add( linguistics );
                //---tonalityMarkingSection.Add( compactOutputXmlFormat );
                TonalityMarkingWcfService.SetConfigSection( tonalityMarkingSection.ToXDocument() );
            }
        }
#endif
        private static XDocument ConfigSection
        {
            get { return (_ConfigImpl.ConfigSection); }
        }
        internal static void SetConfigSection( XDocument configSection )
        {
            _ConfigImpl.SetConfigSection( configSection );            
        }
        internal static string ConfigSectionName
        {
            get { return (_ConfigImpl.ConfigSectionName); }
        }

        private static string AppDataPath
        {
            get { return (ConfigImpl.AppDataPath); }
        }

        public static string LinguisticsUrl
        {
            get { return (_ConfigImpl.LinguisticsUrl); }
        }
        //---------------------------------------------------------------------------//
        #region commented. CompactOutputXmlFormatConverter
        /*
        public static CompactOutputXmlFormatConverter CompactOutputXmlFormatConverter
        {
            get { return (_ConfigImpl.CompactOutputXmlFormatConverter); }
        }
        */
        #endregion
        //---------------------------------------------------------------------------//

        private static ThemesManager _ThemesManager;
        public static ThemesManager ThemesManager
        {
            get
            {
                if ( _ThemesManager == null )
                {
                    lock ( _ConfigImpl.SyncRoot )
                    {
                        if ( _ThemesManager == null )
                        {

#if LICENSED
                            CheckLicenseByMachineIdentifier();
#endif

                            _ThemesManager = CreateThemesManager();
                        }
                    }
                }
                return (_ThemesManager);
            }
        }

#if LICENSED
        private static volatile bool _LicenseIsChecked;
        private static void CheckLicenseByMachineIdentifier()
        {
            if ( _LicenseIsChecked )
            {
                return;
            }

            var identifier = GetMachineIdentifierHelper.GetMachineIdentifier().TrimWhiteSpaces();

            // лицензионная информация
            ResourceManager _licRM = new ResourceManager("lic",
                        Assembly.Load("lic, Version=0.0.0.0, Culture=neutral, PublicKeyToken=0ab1e0136c77fed1")); 

			//Проверка лицензииk
            var lic_identifier = _licRM.GetString( "MachineIdentifier" ).TrimWhiteSpaces();
            if ( string.Compare( identifier, lic_identifier, true ) != 0 )
			{
                var a1 = identifier    .SplitBy( ';' );
                var a2 = lic_identifier.SplitBy( ';' );

                if ( a1.Except( a2 ).Any() || a2.Except( a1 ).Any() )
                {
				throw (new UnauthorizedAccessException(
                        "Недействительная лицензия для данной машины.\r\n(\r\n current-identifier: '{0}'\r\n,\r\n licence-identifier: '{1}'\r\n)"
                        .FormatEx( identifier, lic_identifier ))
                      );
                }
			}

            _LicenseIsChecked = true;
        }
#endif

        private static ThemesManager CreateThemesManager()
        {
            var dict = new Dictionary< ThemeType, Theme >();

            var themesXmlNodes = ConfigSection.Root.GetMultiNodes( "Themes/Theme" );

            foreach ( var themeXmlNode in themesXmlNodes )
            {
                var type  = themeXmlNode.AttributeValue2Enum< ThemeType >( "Type" );
                var theme = themeXmlNode.GetTheme( type );

                dict.Add( type, theme );
            }

            return (new ThemesManager( dict ));
        }
        private static Theme GetTheme( this XElement themeXmlNode, ThemeType themeType )
        {
            var languagesXmlNode = themeXmlNode.GetSingleNode( "Languages" );

            var languagesManager = languagesXmlNode.GetLanguagesManager();

            return (new Theme( themeType, languagesManager ));
        }
        private static LanguagesManager GetLanguagesManager( this XElement languagesXmlNode )
        {
            var dict = new Dictionary< LanguageType, Language >();

            var languageXmlNodes = languagesXmlNode.GetMultiNodes("Language"); 
            foreach ( var languageXmlNode in languageXmlNodes )
            {
                var type    = languageXmlNode.AttributeValue2Enum< LanguageType >( "Type" );
                var dictmrg = languageXmlNode.GetDictionaryManager( type );
                var rules   = languageXmlNode.GetRules();

                var language = new Language( type, dictmrg, rules );

                dict.Add( type, language );
            }

            return (new LanguagesManager( dict ));
        }
        private static DictionaryManager GetDictionaryManager( this XElement node, LanguageType languageType )
        {
            var dictionariesNode = node.GetSingleNode( "Dictionaries" );

            var basePathNode = dictionariesNode.Attribute( "BasePath" );
            var basePath     = (basePathNode != null) ? basePathNode.Value : null;

            var nounsNode        = dictionariesNode.GetSingleNode( "Nouns"        );
            var verbsNode        = dictionariesNode.GetSingleNode( "Verbs"        );
            var adjectivesNode   = dictionariesNode.GetSingleNode( "Adjectives"   );
            var adverbsNode      = dictionariesNode.GetSingleNode( "Adverbs"      );
            var collocationsNode = dictionariesNode.GetSingleNode( "Collocations" );
            var nerNode          = dictionariesNode.GetSingleNode( "NER"          );

            var nouns        = nounsNode       .GetNouns       ( basePath );
            var verbs        = verbsNode       .GetVerbs       ( basePath );
            var adjectives   = adjectivesNode  .GetAdjectives  ( basePath );
            var adverbs      = adverbsNode     .GetAdverbs     ( basePath );
            var collocations = collocationsNode.GetCollocations( basePath );
            var ners         = nerNode         .GetNERs        ( basePath );

            var dm = new DictionaryManager
                    (
                    languageType, 
                    nouns, 
                    verbs, 
                    adjectives, 
                    adverbs, 
                    collocations,
                    ners
                    ); 
            return (dm);
        }
        
        private static Dictionary< Nouns       , OneWordWeightDictionary   > GetNouns( this XElement node, string basePath )
        {
            return (node.GetOneWordWeightDictionaries< Nouns >( basePath ));
        }
        private static Dictionary< Verbs       , MultiWordWeightDictionary > GetVerbs( this XElement node, string basePath )
        {
            return (node.GetMultiWordWeightDictionaries< Verbs >( basePath ));
        }
        private static Dictionary< Adjectives  , OneWordWeightDictionary   > GetAdjectives( this XElement node, string basePath )
        {
            return (node.GetOneWordWeightDictionaries< Adjectives >( basePath ));
        }
        private static Dictionary< Adverbs     , OneWordWeightDictionary   > GetAdverbs( this XElement node, string basePath )
        {
            return (node.GetOneWordWeightDictionaries< Adverbs >( basePath ));
        }
        private static Dictionary< Collocations, MultiWordWeightDictionary > GetCollocations( this XElement node, string basePath )
        {
            return (node.GetMultiWordWeightDictionaries< Collocations >( basePath ));
        }
        private static Dictionary< Nouns       , MultiWordWeightDictionary > GetNERs( this XElement node, string basePath )
        {
            return (node.GetMultiWordWeightDictionaries< Nouns >( basePath ));
        }
        private static Dictionary< T, OneWordWeightDictionary > GetOneWordWeightDictionaries< T >( this XElement node, string basePath )
            where T : struct
        {
            var dictionaries = new Dictionary< T, OneWordWeightDictionary >();

            foreach ( var dictionary in node.GetDictionary()  )
            {
                var type                     = dictionary.GetDictionaryType< T >();
                var fullFilename             = dictionary.GetFilename( "@FileName", basePath );
                var checkOnExistsWeightValue = dictionary.GetAttribute( "CheckOnExistsWeightValue" ).TryConvert2Bool();

                dictionaries.Add( type, new OneWordWeightDictionary( fullFilename, checkOnExistsWeightValue ) );
            }

            return (dictionaries);
        }
        private static Dictionary< T, MultiWordWeightDictionary > GetMultiWordWeightDictionaries< T >( this XElement node, string basePath )
            where T : struct
        {
            var dictionaries = new Dictionary< T, MultiWordWeightDictionary >();

            foreach ( var dictionary in node.GetDictionary()  )
            {
                var type                     = dictionary.GetDictionaryType< T >();
                var fullFilename             = dictionary.GetFilename( "@FileName", basePath );
                var checkOnExistsWeightValue = dictionary.GetAttribute( "CheckOnExistsWeightValue" ).TryConvert2Bool();

                dictionaries.Add( type, new MultiWordWeightDictionary( fullFilename, checkOnExistsWeightValue ) );
            }

            return (dictionaries);
        }
        private static IEnumerable< XElement > GetDictionary( this XElement node )
        {
            return (node.GetMultiNodes( "Dictionary" ));
        }
        private static T GetDictionaryType< T >( this XElement node )
            where T : struct
        {
            var type = ConfigBase.GetAttributeNode( node, "@DictionaryType" ).Value;

            return ((T) Enum.Parse( typeof(T), type ));
        }
        public static string GetFilename( this XElement node, string fileNameXpath, string basePath )
        {
            return (ConfigImpl.GetFilename( node, fileNameXpath, basePath ));
        }
        
        //---------------------------------------------------------------------------//
        private static Rules GetRules( this XElement node )
        {            
            var rulesNode = node.GetSingleNode( "Rules" );

            var basePathNode = rulesNode.Attribute( "BasePath" );
            var basePath     = (basePathNode != null) ? basePathNode.Value : null;

            var mainFile             = rulesNode.GetFilename( "MainFile"            , basePath );
            var orderFile            = rulesNode.GetFilename( "OrderFile"           , basePath );
            var orderParticipialFile = rulesNode.GetFilename( "OrderParticipialFile", basePath );
            var orderHomogenousFile  = rulesNode.GetFilename( "OrderHomogenousFile" , basePath );
           
            var mainFileXDocument         = File.ReadAllText( mainFile             ).ToXDocument();
            var orderFileXDocument        = File.ReadAllText( orderFile            ).ToXDocument();
            var orderParticipialXDocument = File.ReadAllText( orderParticipialFile ).ToXDocument();
            var orderHomogenousXDocument  = File.ReadAllText( orderHomogenousFile  ).ToXDocument();

            var rootRulesNode                 = mainFileXDocument        .Root.GetSingleNode( "Rules"      );
            var rootRulesOrderNode            = orderFileXDocument       .Root.GetSingleNode( "RulesOrder" );
            var rootRulesOrderParticipialNode = orderParticipialXDocument.Root.GetSingleNode( "RulesOrder" );
            var rootRulesOrderHomogenousNode  = orderHomogenousXDocument .Root.GetSingleNode( "RulesOrder" );            
            
            var rules = new Rules( rootRulesNode, 
                                   rootRulesOrderNode, 
                                   rootRulesOrderParticipialNode, 
                                   rootRulesOrderHomogenousNode 
                                 );

            var lexemesNode = rootRulesNode.GetSingleNode( "Lexemes" );
            rules.Lexemes.AdverbAdverbLoad        ( lexemesNode );
            rules.Lexemes.AdjectiveAdjectiveLoad  ( lexemesNode );
            rules.Lexemes.AdjectiveNounLoad       ( lexemesNode );
            rules.Lexemes.VerbNounLoad            ( lexemesNode );
            rules.Lexemes.VerbPassiveVoiceLoad    ( lexemesNode );
            rules.Lexemes.VerbReflexivePronounLoad( lexemesNode );

            return (rules);
        }
        private static void AdverbAdverbLoad( this Lexemes santimenOfLexemes, XElement santimenOfLexemesNode )
        {
            var adverbAdverbRulesNodes = santimenOfLexemesNode.GetMultiNodes( "AdverbAdverb/Rule" )
                                                              .RevealConfigNodes();
            foreach ( var adverbAdverbRuleNode in adverbAdverbRulesNodes )
            {
                santimenOfLexemes.AdverbAdverb.AppendRule
                    ( 
                    adverbAdverbRuleNode.AttributeValue2Enum< Adverbs >( "Adverb1" ),
                    adverbAdverbRuleNode.AttributeValue2Enum< Adverbs >( "Adverb2" ),
                    adverbAdverbRuleNode.AttributeValue2Enum< Adverbs >( "SNT" )
                    );
            }
        }
        private static void AdjectiveAdjectiveLoad( this Lexemes santimenOfLexemes, XElement santimenOfLexemesNode )
        {
            var adjectiveAdjectiveRulesNodes = santimenOfLexemesNode.GetMultiNodes( "AdjectiveAdjective/Rule" ) 
                                                                    .RevealConfigNodes();
            foreach ( var adjectiveAdjectiveRuleNode in adjectiveAdjectiveRulesNodes )
            {
                santimenOfLexemes.AdjectiveAdjective.AppendRule
                    ( 
                    adjectiveAdjectiveRuleNode.AttributeValue2Enum< Adjectives >( "Adjective1" ),
                    adjectiveAdjectiveRuleNode.AttributeValue2Enum< Adjectives >( "Adjective2" ),
                    adjectiveAdjectiveRuleNode.AttributeValue2Enum< Adjectives >( "SNT" )
                    );
            }
        }
        private static void AdjectiveNounLoad( this Lexemes santimenOfLexemes, XElement santimenOfLexemesNode )
        {
            var adjectiveNounRulesNodes = santimenOfLexemesNode.GetMultiNodes( "AdjectiveNoun/Rule" )
                                                               .RevealConfigNodes();
            foreach ( var adjectiveNounRuleNode in adjectiveNounRulesNodes )
            {
                santimenOfLexemes.AdjectiveNoun.AppendRule
                    ( 
                    adjectiveNounRuleNode.AttributeValue2Enum< Adjectives >( "Adjective" ),
                    adjectiveNounRuleNode.AttributeValue2Enum< Nouns      >( "Noun" ),
                    adjectiveNounRuleNode.AttributeValue2Enum< Nouns      >( "SNT" )
                    );
            }
        }
        private static void VerbNounLoad( this Lexemes santimenOfLexemes, XElement santimenOfLexemesNode )
        {
            var verbNounRulesNodes = santimenOfLexemesNode.GetMultiNodes( "ParticipleNoun/Rule" )
                                                          .RevealConfigNodes();
            foreach ( var verbNounRulesNode in verbNounRulesNodes )
            {
                santimenOfLexemes.ParticipleNoun.AppendRule
                    ( 
                    verbNounRulesNode.AttributeValue2Enum< Verbs >( "Participle" ),
                    verbNounRulesNode.AttributeValue2Enum< Nouns >( "Noun" ),
                    verbNounRulesNode.AttributeValue2Enum< Nouns >( "SNT"  )
                    );
            }
        }
        private static void VerbPassiveVoiceLoad( this Lexemes santimenOfLexemes, XElement santimenOfLexemesNode )
        {
            var verbPassiveVoiceRulesNodes = santimenOfLexemesNode.GetMultiNodes( "VerbPassiveVoice/Rule" )
                                                                  .RevealConfigNodes();
            foreach ( var verbPassiveVoiceRulesNode in verbPassiveVoiceRulesNodes )
            {
                santimenOfLexemes.VerbPassiveVoice.AppendRule
                    ( 
                    verbPassiveVoiceRulesNode.AttributeValue2Enum< Verbs >( "Verb" ),
                    verbPassiveVoiceRulesNode.AttributeValue2Enum< Verbs >( "SNT"  )
                    );
            }
        }
        private static void VerbReflexivePronounLoad( this Lexemes santimenOfLexemes, XElement santimenOfLexemesNode )
        {
            var verbReflexivePronounRulesNodes = santimenOfLexemesNode.GetMultiNodes( "VerbReflexivePronoun/Rule" )
                                                                      .RevealConfigNodes();
            foreach ( var verbReflexivePronounRulesNode in verbReflexivePronounRulesNodes )
            {
                santimenOfLexemes.VerbReflexivePronoun.AppendRule
                    ( 
                    verbReflexivePronounRulesNode.AttributeValue2Enum< Verbs >( "Verb" ),
                    verbReflexivePronounRulesNode.AttributeValue2Enum< Verbs >( "SNT"  )
                    );
            }
        }

        public static IEnumerable< XElement > RevealConfigNodes( this IEnumerable< XElement > elements )
        {
            foreach ( var e in elements )
            {
                foreach ( var _e in RevealWithChildren( e ) )
                {
                    yield return (_e);
                }
            }
        }

        private static IEnumerable< XElement > RevealWithChildren( XElement e )
        {
            if ( e.HasElements )
            {
                var reveals = Reveal( e );
                var after_text_nodes = GetAfterTextNodes( e );
                foreach ( var r in reveals )
                {
                    foreach ( var _r in RevealWithChildren_Recurrent( r, e.Elements().FirstOrDefault(), after_text_nodes ) )
                    {                    
                        yield return (_r);
                    }        
                }
            }
            else
            {
                var reveals = Reveal( e );
                foreach ( var r in reveals )
                {
                    yield return (r);
                }
            }
        }
        private static IEnumerable< XElement > RevealWithChildren_Recurrent( XElement parent, XElement nextChild,
            XNode[] previousElementAfterTextNodes )
        {
            if ( nextChild == null )
            {
                yield return (parent);
            }
            else
            {
                var reveals = Reveal( nextChild );
                var after_text_nodes = GetAfterTextNodes( nextChild );
                foreach ( var r in reveals )
                {
                    if ( nextChild.HasElements )
                    {
                        var children = RevealWithChildren_Recurrent( r, nextChild.Elements().First(), after_text_nodes ).ToArray();
                        foreach ( var c in children )
                        {
                            var new_parent = new XElement( parent );

                            new_parent.Add( c );
                            //new_parent.Add( previousElementAfterTextNodes );

                            var next_nextChild = nextChild.ElementsAfterSelf().FirstOrDefault();

                            foreach ( var p in RevealWithChildren_Recurrent( new_parent, next_nextChild, after_text_nodes ) )
                            {
                                yield return (p);
                            }
                        }
                    }
                    else
                    {
                        var new_parent = new XElement( parent );

                        new_parent.Add( r );
                        new_parent.Add( previousElementAfterTextNodes );

                        var next_nextChild = nextChild.ElementsAfterSelf().FirstOrDefault();

                        foreach ( var p in RevealWithChildren_Recurrent( new_parent, next_nextChild, after_text_nodes ) )
                        {
                            yield return (p);
                        }
                    }
                }
            }
        }

        private static IEnumerable< XElement > Reveal( XElement e )
        {
            var new_e = new XElement( e.Name, e.Attributes() );

            var before_text_nodes = e.Nodes().TakeWhile( n => n.NodeType == XmlNodeType.Text );
            new_e.Add( before_text_nodes );

            return (Reveal_ByAttributes( new_e, new_e.FirstAttribute ));
        }
        private static XNode[] GetAfterTextNodes( XElement e )
        {
            var after_text_nodes = e.Nodes().Reverse()
                                            .TakeWhile( n => n.NodeType == XmlNodeType.Text )
                                            .ToArray();

            return (after_text_nodes);
        }
        private static IEnumerable< XElement > Reveal_ByAttributes( XElement e, XAttribute a )
        {
            if ( a == null )
            {
                yield return (e);
            }
            else
            {
                var vals = a.GetMultiAttributeValues();
                foreach ( var v in vals )
                {
                    a.Value = v;
                    
                    var new_e = new XElement( e.Name, e.Attributes() );
                    var before_text_nodes = e.Nodes().TakeWhile( n => n.NodeType == XmlNodeType.Text );
                    new_e.Add( before_text_nodes );

                    var next_a = (a.NextAttribute != null) ? new_e.Attribute( a.NextAttribute.Name ) : null;

                    foreach ( var _e in Reveal_ByAttributes( new_e, next_a ) )
                    {
                        yield return (_e);
                    }
                }
            }
        }

        private static string[] GetMultiAttributeValues( this XAttribute attr )
        {
            if ( attr.Value.IsEmptyOrNull() )
                throw (new InvalidOperationException( attr.Name.LocalName.InSingleQuote() + " is empty in " + /*attr.Parent.FullXPath()*/attr.Parent.ToString().InSingleQuote() ));

            return (attr.Value.SplitByComma()
                              .Select( v => v.TrimWhiteSpaces() )
                              .Where ( v => !v.IsEmptyOrNull() )
                              .ToArray()
                   );
        }        
    }    
}