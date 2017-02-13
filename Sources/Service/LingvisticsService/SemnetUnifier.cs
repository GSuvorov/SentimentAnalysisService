using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Lingvistics.Client;

namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
    public class SemnetUnifier
    {
        public static Tuple< ThemeItem[], LinkItem[] > Union( IEnumerable< Tuple< ThemeItem[], LinkItem[] > > semnetList )
        {
            var themeList = new Dictionary< ThemeItem, ThemeItem >();
            var linkList  = new Dictionary< LinkItem, LinkItem >();

            //цикл по всем объединяемым семсетям
            foreach ( var semnet in semnetList.Where( t => t.Item1 != null && t.Item1.Length  > 0 ) )
            {
                var idMap = new Dictionary< int, int >();
                //цикл по темам
                foreach ( var theme in semnet.Item1 )
                {
                    ThemeItem targetThemeItem;
                    if ( themeList.TryGetValue( theme, out targetThemeItem ) )
                    {
                        targetThemeItem.FreqSubj += theme.FreqSubj;
                        targetThemeItem.FreqObj += theme.FreqObj;
                        targetThemeItem.FreqAdj += theme.FreqAdj;
                        targetThemeItem.FreqOther += theme.FreqOther;
                        idMap[ theme.ID ] = targetThemeItem.ID;
                    }
                    else
                    {
                        int newID = themeList.Count + 1;
                        idMap[ theme.ID ] = newID;
                        theme.ID = newID;
                        themeList.Add( theme, theme );
                    }
                }
                if ( semnet.Item2 == null || semnet.Item2.Length == 0 ) continue;
                //цикл по связям
                foreach ( var link in semnet.Item2 )
                {
                    link.SourceThemeID = idMap[ link.SourceThemeID ];
                    link.DestThemeID = idMap[ link.DestThemeID ];

                    LinkItem targetLinkItem;
                    if ( linkList.TryGetValue( link, out targetLinkItem ) )
                    {
                        targetLinkItem.Freq += link.Freq;
                    }
                    else
                    {
                        linkList.Add( link, link );
                    }
                }
            }
            var themes = (themeList.Count  > 0) ? themeList.Keys.ToArray() : null;
            var links  = (linkList.Count   > 0) ? linkList .Keys.ToArray() : null;
            return (new Tuple< ThemeItem[], LinkItem[] >( themes, links ));
        }
    }
}
