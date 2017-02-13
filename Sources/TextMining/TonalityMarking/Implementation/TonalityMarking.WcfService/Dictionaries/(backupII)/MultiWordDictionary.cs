using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;

namespace TonalityMarking
{
    internal class MultiWordDictionary 
    {
        private Dictionary< string, IList< string[] > > _Dictionary;

        public MultiWordDictionary( string fileName )
        {
            fileName.ThrowIfEmptyOrNull("fileName");

            this.FileName = fileName;

            _Dictionary = new Dictionary< string, IList< string[] > >();
            
            using ( var sr = new StreamReader( this.FileName ) )
            {
                while ( !sr.EndOfStream )
                {
                    var value = sr.ReadLine().Trim().ToLower();
                    if ( value.IsEmptyOrNull() )
                        continue;
                    var words = value.SplitBySpace();

                    /*
                    if ( words.Length <= 1 )
                        throw (new ArgumentException(fileName.InSingleQuote() + " - less than two word in line"));
                    */

                    var array = words.Skip( 1 ).ToArray();

                    var listOfArrays = default(IList< string[] >);
                    if ( !_Dictionary.TryGetValue( words[ 0 ], out listOfArrays ) )
                    {
                        listOfArrays = new List< string[] >( new[] { array } );
                        _Dictionary.Add( words[ 0 ], listOfArrays ); 
                    }
                    else
                    {
                        listOfArrays.Add( array );
                    }
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
        public IList< string[] > GetListOfArrays( string text )
        {
            text.ThrowIfEmptyOrNull("text");

            var listOfArrays = default(IList< string[] >);
            if ( _Dictionary.TryGetValue( text.ToLower(), out listOfArrays ) )
            {
                return (listOfArrays);
            }
            return (null);
        }
    }
}