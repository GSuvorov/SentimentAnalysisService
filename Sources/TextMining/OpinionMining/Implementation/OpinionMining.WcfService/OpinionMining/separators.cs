using System;
using System.Linq;
using System.Xml.Linq;
using System.Collections.Generic;

using TextMining.Core;
using TextMining.Service;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;

    /*
    separator1 ::= 
    <
          <U> - </U>
    >
    */
    internal static class separator1
    {
        private static readonly string[] U_WORDS = new[] { "-", "–" };
        public static bool uConditionFunction( string utext )
        {
            return (U_WORDS.Any( _ => utext.EndsWith( _ ) ));
        }

        public static bool IsCondition( XElement u )
        {
            return (u.IsElementU() && uConditionFunction( u.GetRawCDataTextFromElementU().TrimWhiteSpaces() ));
        }
    }

    /*
    separator2 ::= 
    <
          <U> - || , </U>
    >
    */
    internal static class separator2
    {
        private static readonly string[] U_WORDS = new[] { ",", "-", "–" };
        public static bool uConditionFunction( string utext )
        {
            return (U_WORDS.Any( _ => utext.EndsWith( _ ) ));
        }

        public static bool IsCondition( XElement u )
        {
            return (u.IsElementU() && uConditionFunction( u.GetRawCDataTextFromElementU().TrimWhiteSpaces() ));
        }
    }    
}