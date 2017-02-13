using System;
using System.Text;

namespace TextParsing
{
    /// <summary>
    /// Расширения
    /// </summary>
    internal static class Extensions
    {
        #region [char]
        /// <summary>
        /// Проверка на совпадение указанного символа с одним из списка
        /// </summary>
        /// <param name="ch">указанный символ</param>
        /// <param name="args">список символов для проверки</param>
        /// <returns>результат проверки</returns>
        public static bool EqualsAny( this char ch, params char[] args )
        {
            foreach ( var arg in args )
                if ( ch.Equals( arg ) )
                    return true;
            return false;
        }

        /// <summary>
        /// Проверка, что символ является тире
        /// </summary>
        /// <param name="ch">символ</param>
        /// <returns>результат проверки</returns>
        public static bool IsTire( this char ch )
        {
            return ch.EqualsAny( '-', '–', '—' );
        }

        /// <summary>
        /// Проверка символа на соответствие пробелу
        /// </summary>
        /// <param name="ch">символ</param>
        /// <returns>результат проверки</returns>
        public static bool IsSpace( this char ch )
        {
            return Char.IsWhiteSpace( ch );
        }

        /// <summary>
        /// Проверка символа на соответствие точке
        /// </summary>
        /// <param name="ch">символ</param>
        /// <returns>результат проверки</returns>
        public static bool IsPoint( this char ch )
        {
            return ch == '.';
        }

        /// <summary>
        /// Проверка, что символ - буква в нижнем регистре
        /// </summary>
        /// <param name="ch">символ</param>
        /// <returns>результат проверки</returns>
        public static bool IsLowerLetter( this char ch )
        {
            return Char.IsLetter( ch ) && Char.IsLower( ch );
        }
        #endregion

        #region [string]
        /// <summary>
        /// Получение слова, стоящего непосредственно перед заданной позицией (с пропуском пробелов)
        /// </summary>
        /// <param name="text">строка</param>
        /// <param name="positionIndex">индекс позиции</param>
        /// <param name="isWithDigits">флаг, что слово может содержать цифры</param>
        /// <returns>слово, стоящее непосредственно перед заданной позицией</returns>
        public static string GetWordBeforeCurrentPosition( this string text, int positionIndex, bool isWithDigits = false )
        {
            var result = new StringBuilder();
            for ( int i = positionIndex - 1; i >= 0; --i )
            {
                if ( (result.Length == 0) && text[ i ].IsSpace() )
                    continue;
                if ( Char.IsLetter( text[ i ] ) || (text[ i ] == '-') || (isWithDigits && Char.IsDigit( text[ i ] )) )
                    result.Insert( 0, text[ i ] );
                else
                    break;
            }
            return result.ToString();
        }

        /// <summary>
        /// Получение слова, стоящего непосредственно после заданной позиции (с пропуском пробелов)
        /// </summary>
        /// <param name="text">строка</param>
        /// <param name="positionIndex">индекс позиции</param>
        /// <param name="isWithDigits">флаг, что слово может содержать цифры</param>
        /// <returns>слово, стоящее непосредственно после заданной позиции</returns>
        public static string GetWordAfterCurrentPosition( this string text, int positionIndex, bool isWithDigits = false )
        {
            StringBuilder result = new StringBuilder();
            for ( int i = positionIndex + 1; i < text.Length; ++i )
            {
                if ( (result.Length == 0) && text[ i ].IsSpace() )
                    continue;
                if ( Char.IsLetter( text[ i ] ) || (text[ i ] == '-') || (isWithDigits && Char.IsDigit( text[ i ] )) )
                    result.Append( text[ i ] );
                else
                    break;
            }
            return result.ToString();
        }

        /// <summary>
        /// Получение текста, стоящего перед текущим словом, до ближайшего пробела
        /// </summary>
        /// <param name="text">строка</param>
        /// <param name="positionIndex">индекс позиции</param>
        /// <returns>текст, стоящий перед текущим словом</returns>
        public static string GetPreviousTextToSpace( this string text, int positionIndex )
        {
            #region [.defense.]
            if ( (positionIndex < 0) || (positionIndex >= text.Length) )
                throw new ArgumentOutOfRangeException();
            #endregion
            bool isCurrentWord = Char.IsLetter( text[ positionIndex ] );
            int position = positionIndex - 1;
            StringBuilder result = new StringBuilder();
            while ( position >= 0 )
            {
                if ( text[ position ].IsSpace() )
                {
                    if ( isCurrentWord )
                        isCurrentWord = false;
                    else if ( result.Length > 0 )
                        break;
                }
                if ( !isCurrentWord )
                    result.Insert( 0, text[ position ] );
                --position;
            }
            return result.ToString();
        }

        /// <summary>
        /// Получение предыдущего символа для указанного индекса
        /// </summary>
        /// <param name="text">строка</param>
        /// <param name="index">индекс</param>
        /// <returns>предыдущий сисвол</returns>
        public static char GetPreviousChar( this string text, int index )
        {
            #region [.defense.]
            if ( (index < 0) || (index >= text.Length) )
                throw new ArgumentOutOfRangeException();
            #endregion
            return (index > 0) ? text[ index - 1 ] : '\0';
        }

        /// <summary>
        /// Получение предыдущего символа (с пропуском пробелов) для указанного индекса
        /// </summary>
        /// <param name="text">строка</param>
        /// <param name="index">индекс</param>
        /// <returns>предыдущий символ</returns>
        public static char GetPreviousCharAfterSpace( this string text, int index )
        {
            char result = text.GetPreviousChar( index );
            while ( result.IsSpace() )
                result = text.GetPreviousChar( --index );
            return result;
        }

        /// <summary>
        /// Получение следующего символа для указанного индекса
        /// </summary>
        /// <param name="text">строка</param>
        /// <param name="index">индекс</param>
        /// <returns>следующий сисвол</returns>
        public static char GetNextChar( this string text, int index )
        {
            #region [.defense.]
            if ( (index < 0) || (index >= text.Length) )
                throw new ArgumentOutOfRangeException();
            #endregion
            return (index < text.Length - 1) ? text[ index + 1 ] : '\0';
        }

        /// <summary>
        /// Получение следующего символа (с пропуском пробелов) для указанного индекса
        /// </summary>
        /// <param name="text">строка</param>
        /// <param name="index">индекс</param>
        /// <returns>следующий символ</returns>
        public static char GetNextCharAfterSpace( this string text, int index )
        {
            #region [.defense.]
            if ( (index < 0) || (index >= text.Length) )
                throw new ArgumentOutOfRangeException();
            #endregion
            int i = index + 1;
            while ( i < text.Length )
            {
                if ( !text[ i ].IsSpace() )
                    break;
                ++i;
            }
            return (i < text.Length) ? text[ i ] : '\0';
        }
        #endregion
    }
}
