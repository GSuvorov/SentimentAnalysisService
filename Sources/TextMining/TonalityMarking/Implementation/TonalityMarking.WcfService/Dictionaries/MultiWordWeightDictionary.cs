using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;

using TextMining.Core;

namespace TonalityMarking
{
    internal class StringArrayWeight
    {
        public StringArrayWeight( string[] stringArray, int weight )
        {
            StringArray = stringArray;
            Weight      = weight;
        }

        public string[] StringArray
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

    internal class MultiWordWeightDictionary
    {
        private Dictionary< string, IList< StringArrayWeight > > _Dictionary;

        public MultiWordWeightDictionary( string fileName, bool? checkOnExistsWeightValue )
        {
            fileName.ThrowIfEmptyOrNull("fileName");

            this.FileName = fileName;

            _Dictionary = new Dictionary< string, IList< StringArrayWeight > >();
            
            using ( var sr = new StreamReader( this.FileName ) )
            {
                while ( !sr.EndOfStream )
                {
                    var value = sr.ReadLine().Trim().ToLower();
                    if ( value.IsEmptyOrNull() )
                        continue;

                    #region [.words & weight.]
                    var weight = default(int?);
                    var wordsAndweight = value.SplitByTabulation();
                    if ( wordsAndweight.Length == 2 )
                    {
                        weight = wordsAndweight[ 1 ].TryConvert2Int();
                        if ( !weight.HasValue )
                            throw (new ArgumentException("Dictionary contains wrong weight in row: " + value.InSingleQuote() + ", FileName: " + this.FileName.InSingleQuote()));

                        value = wordsAndweight[ 0 ].Trim();
                        if ( value.IsEmptyOrNull() )
                            throw (new ArgumentException("Dictionary contains wrong text in row: " + value.InSingleQuote() + ", FileName: " + this.FileName.InSingleQuote()));
                    }
                    else if ( 2 < wordsAndweight.Length )
                    {
                        throw (new ArgumentException("Dictionary contains more than 2 tabulation in row: " + value.InSingleQuote() + ", FileName: " + this.FileName.InSingleQuote()));
                    }
                    else
                    {
                        if ( checkOnExistsWeightValue.GetValueOrDefault( false ) )
                            throw (new ArgumentException("Dictionary contains less than 2 tabulation in row: " + value.InSingleQuote() + ", FileName: " + this.FileName.InSingleQuote()));
                        else
                            weight = 0;
                    }
                    #endregion

                    var words = value.SplitBySpace();
                    var array = words.Skip( 1 ).ToArray();

                    var listOfArrays = default(IList< StringArrayWeight >);
                    if ( !_Dictionary.TryGetValue( words[ 0 ], out listOfArrays ) )
                    {
                        listOfArrays = new List< StringArrayWeight >( new[] { new StringArrayWeight( array, weight.Value ) } );
                        _Dictionary.Add( words[ 0 ], listOfArrays ); 
                    }
                    else
                    {
                        listOfArrays.Add( new StringArrayWeight( array, weight.Value ) );
                    }
                }
            }
        }

        public string FileName
        {
            get;
            private set;
        }
        /*//not use
        public bool Exists( string text )
        {
            text.ThrowIfEmptyOrNull("text");

            return (_Dictionary.ContainsKey( text.ToLower() ));
        }
        */
        public IList< StringArrayWeight > GetListOfStringArrayWeight( string text )
        {
            text.ThrowIfEmptyOrNull("text");

            var listOfArrays = default(IList< StringArrayWeight >);
            if ( _Dictionary.TryGetValue( text.ToLower(), out listOfArrays ) )
            {
                return (listOfArrays);
            }
            return (null);
        }
    }
}