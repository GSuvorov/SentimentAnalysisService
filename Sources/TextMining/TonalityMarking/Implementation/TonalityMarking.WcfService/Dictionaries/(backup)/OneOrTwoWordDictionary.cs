using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;

namespace TonalityMarking
{
    internal class OneOrTwoWordDictionary 
    {
        private Dictionary< string, List< string > > _Dictionary;

        public OneOrTwoWordDictionary( string fileName )
        {
            fileName.ThrowIfEmptyOrNull("fileName");

            this.FileName = fileName;

            _Dictionary = new Dictionary< string, List< string > >();
            
            using ( var sr = new StreamReader( this.FileName ) )
            {
                while ( !sr.EndOfStream )
                {
                    var value = sr.ReadLine().Trim().ToLower();
                    if ( value.IsEmptyOrNull() )
                        continue;
                    var words = value.SplitBySpace();

                    if ( 2 < words.Length )
                        throw (new ArgumentException(fileName.InSingleQuote() + " - more than two word in line"));
                    
                    var listOfWords = default(List< string >);
                    if ( !_Dictionary.TryGetValue( words[ 0 ], out listOfWords ) )
                    {
                        listOfWords = new List< string >();
                        _Dictionary.Add( words[ 0 ], listOfWords ); 
                    }
                    if ( words.Length == 2 ) 
                        listOfWords.Add( words[ 1 ] );
                }
            }
        }

        public string FileName
        {
            get;
            private set;
        }
        public bool Exists( string text )
        {
            text.ThrowIfEmptyOrNull("text");

            return (_Dictionary.ContainsKey( text.ToLower() ));
        }
        public List< string > GetListOfWords( string text )
        {
            text.ThrowIfEmptyOrNull("text");

            var listOfWords = default(List< string >);
            if ( _Dictionary.TryGetValue( text.ToLower(), out listOfWords ) )
            {
                return (listOfWords);
            }
            return (null);
        }
    }
}