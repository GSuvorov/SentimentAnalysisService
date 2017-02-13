using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Utils
{
    /// <summary>
    /// Класс для помощи в работе с регулярными выражениями
    /// </summary>
#if UTILS_INTERNAL
    internal static class RegexHelper
#else
	public static class RegexHelper
#endif
    {
        public delegate string StringToRegexConverter( string source );

        /// <summary>
        /// Спецсимволы языка регулярных выражений
        /// </summary>
        private static readonly string SpecialSymbols = ".$^{[(|)*+?\\";

        #region [.comparer.]
        /// <summary>
        /// Компаратор
        /// </summary>
        private class StringsLengthComparer : IComparer<string>
        {
            public int Compare( string s1, string s2 )
            {
                if ( s1.Length != s2.Length )
                    return s2.Length.CompareTo( s1.Length );
                else
                    return string.Compare( s1, s2 );
            }
        }

        private static readonly StringsLengthComparer _comparer = new StringsLengthComparer();
        #endregion

        /// <summary>
        /// Получение регулярного выражения (список вариантов) для заданной коллекции
        /// </summary>
        /// <param name="collection">коллекция строк</param>
        /// <param name="converter">конвертер для преобразования каждого элемента коллекции</param>
        /// <returns>регулярное выражение</returns>
        public static string GetVariantsFromCollection( IEnumerable<string> collection, StringToRegexConverter converter = null )
        {
            return GetVariantsFromSortedCollection( GetSortedCollection( collection, _comparer ), converter );
        }

        /// <summary>
        /// Получение регулярного выражения (список вариантов) для заданной отсортированной коллекции
        /// </summary>
        /// <param name="sortedCollection">отсортированная коллекция строк</param>
        /// <param name="converter">конвертер для преобразования каждого элемента коллекции</param>
        /// <returns>регулярное выражение</returns>
        private static string GetVariantsFromSortedCollection( List<string> sortedCollection, StringToRegexConverter converter )
        {
            StringBuilder builder = new StringBuilder();
            foreach ( string str in sortedCollection )
            {
                builder.Append( "|" );
                string convertedValue = ConvertStringForRegex( str );
                if ( converter != null )
                    convertedValue = converter( convertedValue );
                builder.Append( "(" + convertedValue + ")" );
            }
            builder.Remove( 0, 1 );

            if ( sortedCollection.Count() > 1 )
            {
                builder.Insert( 0, "(" );
                builder.Append( ")" );
            }

            return builder.ToString();
        }

        /// <summary>
        /// Получение из исходной коллекции отсортированной с помощью указанного компаратора
        /// </summary>
        /// <param name="collection">исходная коллекция</param>
        /// <param name="collection">компаратор</param>
        /// <returns>отсортированная коллекция</returns>
        private static List<string> GetSortedCollection( IEnumerable<string> collection, IComparer<string> comparer )
        {
            List<string> result = collection.ToList();
            result.Sort( comparer );
            return result;
        }

        /// <summary>
        /// Конвертирование строки для использования в регулярном выражении
        /// </summary>
        /// <param name="str">строка</param>
        /// <returns>сконвертированная строка</returns>
        private static string ConvertStringForRegex( string str )
        {
            StringBuilder builder = new StringBuilder();

            bool isPreviousSpecial = false;
            foreach ( char ch in str )
            {
                if ( isPreviousSpecial )
                    builder.Append( "[ ]*" );
                isPreviousSpecial = false;
                if ( SpecialSymbols.Any( _ => _ == ch ) )
                {
                    builder.Append( "\\" );
                    builder.Append( ch );
                    isPreviousSpecial = true;
                }
                else if ( Char.IsWhiteSpace( ch ) )
                    builder.Append( "[ ]+" );
                else
                    builder.Append( ch );
            }

            return builder.ToString();
        }
    }
}
