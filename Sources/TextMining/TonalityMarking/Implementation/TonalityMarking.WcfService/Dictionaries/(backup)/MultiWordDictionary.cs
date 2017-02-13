using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;

namespace TonalityMarking
{
    internal class MultiWordDictionary 
    {
        private Dictionary< string, List< LinkedList< string > > > _Dictionary;

        public MultiWordDictionary( string fileName )
        {
            fileName.ThrowIfEmptyOrNull("fileName");

            this.FileName = fileName;

            _Dictionary = new Dictionary< string, List< LinkedList< string > > >();
            
            using ( var sr = new StreamReader( this.FileName ) )
            {
                while ( !sr.EndOfStream )
                {
                    var value = sr.ReadLine().Trim().ToLower();
                    if ( value.IsEmptyOrNull() )
                        continue;
                    var words = value.SplitBySpace();

                    if ( words.Length <= 1 )
                        throw (new ArgumentException(fileName.InSingleQuote() + " - less than two word in line"));

                    var linkedList = new LinkedList< string >( words.Skip( 1 ) );

                    var listOfLinkedList = default(List< LinkedList< string > >);
                    if ( !_Dictionary.TryGetValue( words[ 0 ], out listOfLinkedList ) )
                    {
                        listOfLinkedList = new List< LinkedList< string> >( new[] { linkedList } );
                        _Dictionary.Add( words[ 0 ], listOfLinkedList ); 
                    }
                    else
                    {
                        listOfLinkedList.Add( linkedList );
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
        public List< LinkedList< string > > GetLinkedLists( string text )
        {
            text.ThrowIfEmptyOrNull("text");

            var listOfLinkedList = default(List< LinkedList< string > >);
            if ( _Dictionary.TryGetValue( text.ToLower(), out listOfLinkedList ) )
            {
                return (listOfLinkedList);
            }
            return (null);
        }
    }
}