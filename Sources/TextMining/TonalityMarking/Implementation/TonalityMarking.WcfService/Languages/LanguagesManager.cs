using System;
using System.IO;
using System.Collections.Generic;
using TextMining.Core;

namespace TonalityMarking
{
    internal class LanguagesManager 
    {
        private Dictionary< LanguageType, Language > _Languages;

        public LanguagesManager( Dictionary< LanguageType, Language > languages )
        {
            languages.ThrowIfNull("languages");

            _Languages = new Dictionary< LanguageType, Language >( languages );
        }

        public Language this[ LanguageType languageType ]
        {
            get { return (_Languages[ languageType ]); }
        }
        public Language RU
        {
            get { return (_Languages[ LanguageType.RU ]); }
        }
    }
}



