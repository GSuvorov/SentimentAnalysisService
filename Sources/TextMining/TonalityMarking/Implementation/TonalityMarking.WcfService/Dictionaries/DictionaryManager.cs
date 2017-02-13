using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="T"></typeparam>
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

    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="T"></typeparam>
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

    /// <summary>
    /// 
    /// </summary>
    internal class DictionaryManager 
    {
        private Dictionary< Nouns       , OneWordWeightDictionary   > _Nouns;
        private Dictionary< Verbs       , MultiWordWeightDictionary > _Verbs;
        private Dictionary< Adjectives  , OneWordWeightDictionary   > _Adjectives;
        private Dictionary< Adverbs     , OneWordWeightDictionary   > _Adverbs;
        private Dictionary< Collocations, MultiWordWeightDictionary > _Collocations;
        private Dictionary< Nouns       , MultiWordWeightDictionary > _Ners;        

        public DictionaryManager( LanguageType languageType, 
                                  Dictionary< Nouns       , OneWordWeightDictionary   > nouns,
                                  Dictionary< Verbs       , MultiWordWeightDictionary > verbs,
                                  Dictionary< Adjectives  , OneWordWeightDictionary   > adjectives,
                                  Dictionary< Adverbs     , OneWordWeightDictionary   > adverbs,
                                  Dictionary< Collocations, MultiWordWeightDictionary > collocations,
                                  Dictionary< Nouns       , MultiWordWeightDictionary > ners
                                )
        {
            nouns       .ThrowIfNull("nouns"       );
            verbs       .ThrowIfNull("verbs"       );
            adjectives  .ThrowIfNull("adjectives"  );
            adverbs     .ThrowIfNull("adverbs"     );
            collocations.ThrowIfNull("collocations");
            ners        .ThrowIfNull("ners"        );


            this.LanguageType = languageType;
            _Nouns        = new Dictionary< Nouns       , OneWordWeightDictionary   >( nouns        );
            _Verbs        = new Dictionary< Verbs       , MultiWordWeightDictionary >( verbs        );
            _Adjectives   = new Dictionary< Adjectives  , OneWordWeightDictionary   >( adjectives   );
            _Adverbs      = new Dictionary< Adverbs     , OneWordWeightDictionary   >( adverbs      );
            _Collocations = new Dictionary< Collocations, MultiWordWeightDictionary >( collocations );
            _Ners         = new Dictionary< Nouns       , MultiWordWeightDictionary >( ners         );
        }

        public LanguageType LanguageType
        {
            get;
            private set;
        }

        private static OneWordWeightDictionaryResult< T >? GetWeigthInDictionariesByType< T >
                ( string text, Dictionary< T, OneWordWeightDictionary > dictionaries )
            where T : struct
        {
            foreach ( var dictionary in dictionaries )
            {
                var weight = dictionary.Value.GetWeigth( text );
                if ( weight.HasValue )
                    return (new OneWordWeightDictionaryResult< T >( dictionary.Key, weight.Value ));
            }
            return (null);
        }

        public OneWordWeightDictionaryResult< Nouns      >? ExistsInNouns( string text )
        {
            return (GetWeigthInDictionariesByType< Nouns >( text, _Nouns ));
        }
        public OneWordWeightDictionaryResult< Adjectives >? ExistsInAdjectives( string text )
        {
            return (GetWeigthInDictionariesByType< Adjectives >( text, _Adjectives ));
        }
        public OneWordWeightDictionaryResult< Adverbs    >? ExistsInAdverbs( string text )
        {
            return (GetWeigthInDictionariesByType< Adverbs >( text, _Adverbs ));
        }

        private static List< MultiWordWeightDictionaryResult< T > > GetListOfStringArrayWeightInDictionariesByType< T >
                ( string text, Dictionary< T, MultiWordWeightDictionary> dictionaries, 
                               List< MultiWordWeightDictionaryResult< T > > result )
            where T : struct
        {
            result.Clear();
            foreach ( var dictionary in dictionaries )
            {
                var listOfArrays = dictionary.Value.GetListOfStringArrayWeight( text );
                if ( listOfArrays != null ) 
                {
                    result.Add
                    ( 
                        new MultiWordWeightDictionaryResult< T >( dictionary.Key, listOfArrays ) 
                    );
                }
            }
            return (result);
        }

        public List< MultiWordWeightDictionaryResult< Collocations > > GetCollocationsLists
                ( string text, List< MultiWordWeightDictionaryResult< Collocations > > result )
        {
            return (GetListOfStringArrayWeightInDictionariesByType< Collocations >( text, _Collocations, result ));
        }
        public List< MultiWordWeightDictionaryResult< Verbs > > GetVerbsLists
                ( string text, List< MultiWordWeightDictionaryResult< Verbs > > result)
        {
            return (GetListOfStringArrayWeightInDictionariesByType< Verbs >( text, _Verbs, result ));
        }

        public List< MultiWordWeightDictionaryResult< Nouns > > GetNERsLists
                ( string text, List< MultiWordWeightDictionaryResult< Nouns > > result )
        {
            return (GetListOfStringArrayWeightInDictionariesByType< Nouns >( text, _Ners, result ));
        }
    }
}



