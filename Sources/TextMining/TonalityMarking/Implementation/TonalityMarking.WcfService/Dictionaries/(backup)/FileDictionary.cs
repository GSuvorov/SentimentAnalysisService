using System;
using System.IO;
using System.Collections.Generic;

namespace TonalityMarking
{
    internal class FileDictionary 
    {
        private readonly HashSet< string > _HashSet = new HashSet< string >();

        public FileDictionary( string fileName )
        {
            fileName.ThrowIfEmptyOrNull("fileName");

            this.FileName = fileName;
            
            using ( var sr = new StreamReader( this.FileName ) )
            {
                while ( !sr.EndOfStream )
                {
                    var value =  sr.ReadLine().Trim().ToLower();

                    if ( !value.IsEmptyOrNull() )
                    {
                        _HashSet.Add( value );
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

            return (_HashSet.Contains( text.ToLower() ));
        }
    }
}