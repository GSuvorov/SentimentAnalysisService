using System;
using System.IO;
using System.Collections.Generic;

namespace TonalityMarking
{
    internal static class DictionaryManager 
    {
        private static readonly Dictionary< DictionaryType, FileDictionary > _Dictionaries;
        private static readonly DictionaryType[] _NounsDictionaryTypes = new[] { DictionaryType.nouns_action_negative,
                                                                                 DictionaryType.nouns_action_positive,
                                                                                 DictionaryType.nouns_negative,
                                                                                 DictionaryType.nouns_positive,
                                                                                 DictionaryType.nouns_ppotential_negative,
                                                                                 DictionaryType.nouns_ppotential_positive, };
        private static readonly DictionaryType[] _AdjectivesDictionaryTypes = new[] { DictionaryType.adjectives_negative,
                                                                                      DictionaryType.adjectives_positive,
                                                                                      DictionaryType.adjectives_amplificator, };

        static DictionaryManager()
        {
            _Dictionaries = new Dictionary< DictionaryType, FileDictionary >();

            foreach ( var _ in Config.FileListOfDictionaries )
            {
                _Dictionaries.Add( _.Key, new FileDictionary( _.Value ) );
            }
        }

        private static DictionaryType? ExistsInDictionariesByType( ref string text, DictionaryType[] dictionaryTypes )
        {
            foreach ( var dictionaryType in dictionaryTypes )
            {
                if ( _Dictionaries[ dictionaryType ].Exists( text ) )
                {
                    return (dictionaryType);
                }
            }
            return (null);
        }

        public static bool Exists( DictionaryType dictionaryType, string text )
        {
            return (_Dictionaries[ dictionaryType ].Exists( text ));
        }

        public static DictionaryType? ExistsInNouns( string text )
        {
            return (ExistsInDictionariesByType( ref text, _NounsDictionaryTypes ) );
        }
        public static DictionaryType? ExistsInAdjectives( string text )
        {
            return (ExistsInDictionariesByType( ref text, _AdjectivesDictionaryTypes ) );
        }

    }
}



