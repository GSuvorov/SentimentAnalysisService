using System;
using System.IO;
using System.Collections.Generic;

using TextMining.Core;

namespace TonalityMarking
{
    internal class OneWordWeightDictionary 
    {
        private readonly Dictionary< string, int > _Dictionary = new Dictionary< string, int >();

        public OneWordWeightDictionary( string fileName, bool? checkOnExistsWeightValue )
        {
            fileName.ThrowIfEmptyOrNull("fileName");

            this.FileName = fileName;
            
            using ( var sr = new StreamReader( this.FileName ) )
            {
                while ( !sr.EndOfStream )
                {
                    var value = sr.ReadLine().Trim().ToLower();

                    if ( value.IsEmptyOrNull() )
                        continue;

                    #region [.word & weight.]
                    var weight = default(int?);
                    var wordAndweight = value.SplitByTabulation();
                    if ( wordAndweight.Length == 2 )
                    {
                        weight = wordAndweight[ 1 ].TryConvert2Int();
                        if ( !weight.HasValue )
                            throw (new ArgumentException("Dictionary contains wrong weight in row: " + value.InSingleQuote() + ", FileName: " + this.FileName.InSingleQuote()));

                        value = wordAndweight[ 0 ].Trim();
                        if ( value.IsEmptyOrNull() )
                            throw (new ArgumentException("Dictionary contains wrong text in row: " + value.InSingleQuote() + ", FileName: " + this.FileName.InSingleQuote()));
                    }
                    else if ( 2 < wordAndweight.Length )
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

                    if ( !_Dictionary.ContainsKey( value ) )
                    {
                        _Dictionary.Add( value, weight.Value );
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
        }*/
        public int? GetWeigth( string text )
        {
            text.ThrowIfEmptyOrNull("text");

            var weight = default(int);
            if ( _Dictionary.TryGetValue( text.ToLower(), out weight ) )
            {
                return (weight);
            }
            return (null);
        }
    }
}