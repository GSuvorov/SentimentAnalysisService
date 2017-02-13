using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace TextParsing
{
    /// <summary>
    /// Потенциальный конец предложения
    /// </summary>
    internal class PotencialSentenceEnd
    {
        /// <summary>
        /// Регулярное выражение для выделения потенциальных концов предложений
        /// </summary>
        private static readonly Regex _regex = new Regex( "((\\.\\.\\.)|([;!\\.?\u2026]))" );

        /// <summary>
        /// Начало
        /// </summary>
        public int Index { get; private set; }
        /// <summary>
        /// Конец
        /// </summary>
        public int End { get; private set; }

        #region [.ctor().]
        static PotencialSentenceEnd()
        {
        }

        public PotencialSentenceEnd( int index, int end )
        {
            Index = index;
            End = end;
        }
        #endregion

        /// <summary>
        /// Поиск потенциальных концов предложений в заданном тексте
        /// </summary>
        /// <param name="text">текст</param>
        /// <returns>потенциальные концы предложений</returns>
        public static IEnumerable<PotencialSentenceEnd> Select( string text )
        {
            return _regex
                .Matches( text )
                .Cast<Match>()
                .Select( _ => new PotencialSentenceEnd( _.Index, _.Index + _.Length ) ).ToArray();
        }
    }
}
