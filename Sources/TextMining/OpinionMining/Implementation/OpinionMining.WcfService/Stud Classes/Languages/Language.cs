using System;
using System.IO;
using System.Collections.Generic;
using TextMining.Core;

namespace OpinionMining
{
    internal class Language
    {
        public Language( LanguageType languageType, DictionaryManager dictionaryManager )//, Patterns rules )
        {
            dictionaryManager.ThrowIfNull("dictionaryManager");
            //rules            .ThrowIfNull("rules");

            this.LanguageType      = languageType;
            this.DictionaryManager = dictionaryManager;
            this.Rules             = new Rules( this ); //rules;
        }

        public LanguageType LanguageType
        {
            get;
            private set;
        }
        public DictionaryManager DictionaryManager
        {
            get;
            private set;
        }
        public Rules Rules
        {
            get;
            private set;
        }

        public override bool Equals( object obj )
        {
            var _ = obj as Language;
            if ( _ != null )
            {
                return (_.LanguageType == this.LanguageType);
            }
            return (base.Equals( obj ));
        }
        public override int GetHashCode()
        {
            return (this.LanguageType.GetHashCode());
        }
    }
}



