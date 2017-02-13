using System;
using System.IO;
using System.Collections.Generic;
using TextMining.Core;

namespace OpinionMining
{
    internal class Theme
    {
        public Theme( ThemeType themeType, LanguagesManager languagesManager )
        {
            languagesManager.ThrowIfNull("languagesManager");

            this.ThemeType        = themeType;
            this.LanguagesManager = languagesManager;
        }

        public ThemeType ThemeType
        {
            get;
            private set;
        }
        public LanguagesManager LanguagesManager
        {
            get;
            private set;
        }

        public override bool Equals( object obj )
        {
            var _ = obj as Theme;
            if ( _ != null )
            {
                return (_.ThemeType == this.ThemeType);
            }
            return (base.Equals( obj ));
        }
        public override int GetHashCode()
        {
            return (this.ThemeType.GetHashCode());
        }
    }
}



