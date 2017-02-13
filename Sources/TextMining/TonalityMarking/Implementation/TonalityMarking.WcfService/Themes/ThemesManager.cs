using System;
using System.IO;
using System.Collections.Generic;
using TextMining.Core;

namespace TonalityMarking
{
    internal class ThemesManager 
    {
        private Dictionary< ThemeType, Theme > _Themes;

        public ThemesManager( Dictionary< ThemeType, Theme > themes )
        {
            themes.ThrowIfNull("themes");

            _Themes = new Dictionary< ThemeType, Theme >( themes );
        }

        public Theme this[ ThemeType languageType ]
        {
            get { return (_Themes[ languageType ]); }
        }
        public Theme Default
        {
            get { return (_Themes[ ThemeType.Default ]); }
        }
    }
}



