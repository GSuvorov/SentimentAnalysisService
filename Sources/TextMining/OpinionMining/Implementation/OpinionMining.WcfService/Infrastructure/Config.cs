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

namespace OpinionMining
{
    internal class ConfigImpl : ConfigBase
    {
        private const string RESULTLIMIT_XPATH = "ResultLimit";
        private const string RESULTLIMIT_MAXSENTCOUNTINTEXT_XPATH        = RESULTLIMIT_XPATH + "/MaxSentCountInText";
        private const string RESULTLIMIT_MAXOBJECTCOUNTINSENT_XPATH      = RESULTLIMIT_XPATH + "/MaxObjectCountInSent";
        private const string RESULTLIMIT_MAXSENTLENGTHWITHOUTSPACE_XPATH = RESULTLIMIT_XPATH + "/MaxSentLengthWithoutSpace";

        public override string ConfigSectionName
        {
            get { return (ConfigBase.OPINION_MINING_SECTION_NAME); }
        }

        private int? _ResultLimit_MaxSentCountInText;
        public int ResultLimit_MaxSentCountInText
        {
            get 
            {
                if ( !_ResultLimit_MaxSentCountInText.HasValue )
                {
                    lock ( SyncRoot )
                    {
                        if ( !_ResultLimit_MaxSentCountInText.HasValue )
                        {
                            _ResultLimit_MaxSentCountInText = GetIntValue( RESULTLIMIT_MAXSENTCOUNTINTEXT_XPATH );
                        }
                    }
                }
                return (_ResultLimit_MaxSentCountInText.Value);
            }
        }
        private int? _ResultLimit_MaxObjectInOneSent;
        public int ResultLimit_MaxObjectInOneSent
        {
            get 
            {
                if ( !_ResultLimit_MaxObjectInOneSent.HasValue )
                {
                    lock ( SyncRoot )
                    {
                        if ( !_ResultLimit_MaxObjectInOneSent.HasValue )
                        {
                            _ResultLimit_MaxObjectInOneSent = GetIntValue( RESULTLIMIT_MAXOBJECTCOUNTINSENT_XPATH );
                        }                                                
                    }
                }
                return (_ResultLimit_MaxObjectInOneSent.Value);
            }
        }
        private int? _ResultLimit_MaxSentLengthWithoutSpace;
        public int ResultLimit_MaxSentLengthWithoutSpace
        {
            get 
            {
                if ( !_ResultLimit_MaxSentLengthWithoutSpace.HasValue )
                {
                    lock ( SyncRoot )
                    {
                        if ( !_ResultLimit_MaxSentLengthWithoutSpace.HasValue )
                        {
                            _ResultLimit_MaxSentLengthWithoutSpace = GetIntValue( RESULTLIMIT_MAXSENTLENGTHWITHOUTSPACE_XPATH );
                        }
                    }
                }
                return (_ResultLimit_MaxSentLengthWithoutSpace.Value);
            }
        }

        private int GetIntValue( string xpath )
        {
            var value = ConfigSection.Root.GetSingleNode( xpath ).Value.Trim();
            if ( value.IsEmptyOrNull() )
                throw (new ConfigException( xpath.InSingleQuote() + " section is empty"));

            var result = value.TryConvert2Int();
            if ( !result.HasValue || result.Value <= 0 )
                throw (new ConfigException( xpath.InSingleQuote() + " section has wrong value: " + value.InSingleQuote() ));

            return (result.Value);
        }
    }

    internal static class Config
    {
        private static readonly ConfigImpl _ConfigImpl = new ConfigImpl();

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

        public static int ResultLimit_MaxSentCountInText
        {
            get { return (_ConfigImpl.ResultLimit_MaxSentCountInText); }
        }
        public static int ResultLimit_MaxObjectInOneSent
        {
            get { return (_ConfigImpl.ResultLimit_MaxObjectInOneSent); }
        }
        public static int ResultLimit_MaxSentLengthWithoutSpace
        {
            get { return (_ConfigImpl.ResultLimit_MaxSentLengthWithoutSpace); }
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
                            _ThemesManager = CreateThemesManager();
                        }
                    }
                }
                return (_ThemesManager);
            }
        }

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

            var languageXmlNodes = languagesXmlNode.GetMultiNodes( "Language" ); 
            foreach ( var languageXmlNode in languageXmlNodes )
            {
                var type     = languageXmlNode.AttributeValue2Enum< LanguageType >( "Type" );
                var dictmrg  = languageXmlNode.GetDictionaryManager( type );
                //var patterns = new Patterns(); // languageXmlNode.GetPatterns();

                var language = new Language( type, dictmrg );//, patterns );

                dict.Add( type, language );
            }

            return (new LanguagesManager( dict ));
        }
        private static DictionaryManager GetDictionaryManager( this XElement node, LanguageType languageType )
        {
            var dictionariesNode = node.GetSingleNode( "Dictionaries" );

            var basePathNode = dictionariesNode.Attribute( "BasePath" );
            var basePath     = (basePathNode != null) ? basePathNode.Value : null;

            var verbsNode       = dictionariesNode.GetSingleNode( "Verbs"       );
            var phrasisNode     = dictionariesNode.GetSingleNode( "Phrasis"     );
            var singleWordsNode = dictionariesNode.GetSingleNode( "SingleWords" );
            var multiWordsNode  = dictionariesNode.GetSingleNode( "MultiWords"  );

            var verbs       = verbsNode      .GetVerbs      ( basePath );
            var phrasis     = phrasisNode    .GetPhrasis    ( basePath );
            var singleWords = singleWordsNode.GetSingleWords( basePath );
            var multiWords  = multiWordsNode .GetMultiWords ( basePath );

            return (new DictionaryManager( languageType, verbs, phrasis, singleWords, multiWords ));
        }
        
        private static Dictionary< DictionaryType, OneWordWeightDictionary > GetVerbs( this XElement node, string basePath )
        {
            return (node.GetOneWordWeightDictionaries< DictionaryType >( basePath ));
        }
        private static Dictionary< DictionaryType, MultiWordWeightDictionary > GetPhrasis( this XElement node, string basePath )
        {
            return (node.GetMultiWordWeightDictionaries< DictionaryType >( basePath ));
        }
        private static Dictionary< DictionaryType, OneWordWeightDictionary > GetSingleWords( this XElement node, string basePath )
        {
            return (node.GetOneWordWeightDictionaries< DictionaryType >( basePath ));
        }
        private static Dictionary< DictionaryType, MultiWordWeightDictionary > GetMultiWords( this XElement node, string basePath )
        {
            return (node.GetMultiWordWeightDictionaries< DictionaryType >( basePath ));
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
    }    
}