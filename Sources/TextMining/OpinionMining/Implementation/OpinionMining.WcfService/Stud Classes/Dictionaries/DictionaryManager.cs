using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using TextMining.Core;

namespace OpinionMining
{
    internal struct OneWordWeightDictionaryResult< T > 
        where T : struct
    {
        public OneWordWeightDictionaryResult( T dictionaryType, int weight ) : this()
        {
            DictionaryType = dictionaryType;
            Weight         = weight;
        }
        public T DictionaryType
        {
            get;
            private set;
        }
        public int Weight
        {
            get;
            private set;
        }
    }

    internal class MultiWordWeightDictionaryResult< T > 
        where T : struct
    {
        public MultiWordWeightDictionaryResult( T dictionaryType, IList< StringArrayWeight > stringArrayWeightList )
        {
            DictionaryType        = dictionaryType;
            StringArrayWeightList = stringArrayWeightList;
        }
        public T DictionaryType
        {
            get;
            private set;
        }
        public IList< StringArrayWeight > StringArrayWeightList
        {
            get;
            private set;
        }
    }

    internal class DictionaryManager 
    {
        private Dictionary< DictionaryType, OneWordWeightDictionary   > _Verbs;
        private Dictionary< DictionaryType, MultiWordWeightDictionary > _Phrasis;
        private Dictionary< DictionaryType, OneWordWeightDictionary   > _SingleWords;
        private Dictionary< DictionaryType, MultiWordWeightDictionary > _MultiWords;

        public DictionaryManager( LanguageType languageType, 
                                  Dictionary< DictionaryType, OneWordWeightDictionary   > verbs,
                                  Dictionary< DictionaryType, MultiWordWeightDictionary > phrasis,
                                  Dictionary< DictionaryType, OneWordWeightDictionary   > singleWords,
                                  Dictionary< DictionaryType, MultiWordWeightDictionary > multiWords )
        {
            verbs      .ThrowIfNull("verbs"      );
            phrasis    .ThrowIfNull("phrasis"    );
            singleWords.ThrowIfNull("singleWords");
            multiWords .ThrowIfNull("multiWords" );

            LanguageType = languageType;
            _Verbs       = new Dictionary< DictionaryType, OneWordWeightDictionary   >( verbs       );
            _Phrasis     = new Dictionary< DictionaryType, MultiWordWeightDictionary >( phrasis     );
            _SingleWords = new Dictionary< DictionaryType, OneWordWeightDictionary   >( singleWords );
            _MultiWords  = new Dictionary< DictionaryType, MultiWordWeightDictionary >( multiWords  );
        }

        public LanguageType LanguageType
        {
            get;
            private set;
        }

        private static OneWordWeightDictionaryResult< T >? GetWeigthInDictionariesByType< T >( string text, Dictionary< T, OneWordWeightDictionary > dictionaries )
            where T : struct
        {
            foreach ( var _ in dictionaries )
            {
                var weight = _.Value.GetWeigth( text );
                if ( weight.HasValue )
                    return (new OneWordWeightDictionaryResult< T >( _.Key, weight.Value ));
            }
            return (null);
        }

        private static List< MultiWordWeightDictionaryResult< T > > GetListOfStringArrayWeightInDictionariesByType< T >( string text, Dictionary< T, MultiWordWeightDictionary> dictionaries )
            where T : struct
        {
            var result = new List< MultiWordWeightDictionaryResult< T > >();
            foreach ( var _ in dictionaries )
            {
                var listOfArrays = _.Value.GetListOfStringArrayWeight( text );
                if ( listOfArrays != null ) 
                {
                    result.Add
                    ( 
                        new MultiWordWeightDictionaryResult< T >( _.Key, listOfArrays ) 
                    );
                }
            }
            return (result);
        }

        public string[] FirstWords1
        {
            get { return (_SingleWords[ DictionaryType.FirstWords1 ].Words); }
        }
        public string[] IntrWords2
        {
            get { return (_SingleWords[ DictionaryType.IntrWords2 ].Words); }
        }

        public OneWordWeightDictionaryResult< DictionaryTypeSingleWords >? Exists( string text )
        {
            var result = GetWeigthInDictionariesByType< DictionaryType >( text, _SingleWords );
            if ( !result.HasValue )
                return (null);
            return (new OneWordWeightDictionaryResult< DictionaryTypeSingleWords >( (DictionaryTypeSingleWords) result.Value.DictionaryType, result.Value.Weight ));
        }
        /*public bool ExistsInVerbs( string text, DictionaryType dictionaryType )
        {
            return (_Verbs[ dictionaryType ].GetWeigth( ref text ).HasValue);
        }*/
        public bool ExistsInVerbs( string text, DictionaryTypeVerbs dictionaryType )
        {
            return (_Verbs[ (DictionaryType) dictionaryType ].GetWeigth( text ).HasValue);
        }

        /*public List< MultiWordWeightDictionaryResult< DictionaryType > > GetPhrasisLists( string text )
        {
            return (GetListOfStringArrayWeightInDictionariesByType< DictionaryType >( ref text, _Phrasis ));
        }*/
        public MultiWordWeightDictionaryResult< DictionaryTypePhrasis1 > GetPhrasis1Lists( string text, DictionaryTypePhrasis1 dictionaryType )
        {
            var list = _Phrasis[ (DictionaryType) dictionaryType ].GetListOfStringArrayWeight( text );
            if ( list == null )
                return (null);

            return (new MultiWordWeightDictionaryResult< DictionaryTypePhrasis1 >( dictionaryType, list ) );
        }
        public MultiWordWeightDictionaryResult< DictionaryTypePhrasis2 > GetPhrasis2Lists( string text, DictionaryTypePhrasis2 dictionaryType )
        {
            var list = _Phrasis[ (DictionaryType) dictionaryType ].GetListOfStringArrayWeight( text );
            if ( list == null )
                return (null);

            return (new MultiWordWeightDictionaryResult< DictionaryTypePhrasis2 >( dictionaryType, list ) );
        }
        public MultiWordWeightDictionaryResult< DictionaryTypeMultiWords > GetMultiWordsLists( string text, DictionaryTypeMultiWords dictionaryType )
        {
            var list = _MultiWords[ (DictionaryType) dictionaryType ].GetListOfStringArrayWeight( text );
            if ( list == null )
                return (null);

            return (new MultiWordWeightDictionaryResult< DictionaryTypeMultiWords >( dictionaryType, list ) );
        }
        public bool ExistsInPhrasis( string[] text, DictionaryTypePhrasis1 dictionaryType )
        {
            if ( !text.Any() )
                return (false);

            var text0 = text[ 0 ];
            var list = _Phrasis[ (DictionaryType) dictionaryType ].GetListOfStringArrayWeight( text0 );
            if ( list == null )
                return (false);

            text = text.Skip( 1 ).ToArray();

            return (list.Any( _ => _.StringArray.SequenceEqual( text ) ));
        }
    }
}



