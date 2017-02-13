using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Linguistics.Core
{
    /// <summary>
    /// Класс расширений
    /// </summary>
    internal static class Extensions
    {
        #region [string]
        /// <summary>
        /// Получение символа или значения по умолчания по заданному индексу в строке
        /// </summary>
        /// <param name="str">строка</param>
        /// <param name="charIndex">индекс символа</param>
        /// <returns>символ или значение по умолчанию</returns>
        public static char CharOrDefault(this string str, int charIndex)
        {
            return (charIndex >= 0) && (charIndex < str.Length) ? str[charIndex] : default(char);
        }

        /// <summary>
        /// Получение непустого предыдущего символа
        /// </summary>
        /// <param name="str">строка</param>
        /// <param name="currentIndex">индекс текущего символа</param>
        /// <returns>непустой предыдущий символ</returns>
        public static char GetPreviousNotEmpty(this string str, int currentIndex)
        {
            for (int i = currentIndex - 1; i >= 0; --i)
                if (!Char.IsWhiteSpace(str[i]))
                    return str[i];
            return default(char);
        }

        /// <summary>
        /// Получение непустого следующего символа
        /// </summary>
        /// <param name="str">строка</param>
        /// <param name="currentIndex">индекс текущего символа</param>
        /// <returns>непустой следующий символ</returns>
        public static char GetNextNotEmpty(this string str, int currentIndex)
        {
            for (int i = currentIndex + 1; i < str.Length; ++i)
                if (!Char.IsWhiteSpace(str[i]))
                    return str[i];
            return default(char);
        }
        #endregion

        #region [char]
        /// <summary>
        /// Проверяет символ на совпадение с умолчательным
        /// </summary>
        /// <param name="ch">символ</param>
        /// <returns>результат проверки</returns>
        public static bool IsDefault(this char ch)
        {
            return ch == default(char);
        }

        /// <summary>
        /// Проверяет совпадение символа с одним из указанного массива
        /// </summary>
        /// <param name="ch">символ</param>
        /// <param name="anyOf">массив символов для проверки</param>
        /// <returns>результат проверки</returns>
        public static bool EqualsAny(this char ch, char[] anyOf)
        {
            foreach (char value in anyOf)
                if (ch.Equals(value))
                    return true;
            return false;
        }

        /// <summary>
        /// Проверяет символ на один из вариантов представления тире
        /// </summary>
        /// <param name="ch">символ</param>
        /// <returns>результат проверки</returns>
        public static bool IsTire(this char ch)
        {
            return ch.EqualsAny(new char[] { '-', '–', '—' });
        }
        #endregion

        #region[UnitTextBase]
        /// <summary>
        /// Начало юнита относительного целого текста
        /// </summary>
        /// <param name="unit">юнит</param>
        /// <returns>начало юнита</returns>
        public static int GetFullStartPosition(this UnitTextBase unit)
        {
            return unit.PositionInfo.Start + unit.ParentObject.ParentObject.StartPosition;
        }

        /// <summary>
        /// Конец юнита относительного целого текста
        /// </summary>
        /// <param name="unit">юнит</param>
        /// <returns>конец юнита</returns>
        public static int GetFullEndPosition(this UnitTextBase unit)
        {
            return unit.PositionInfo.End + unit.ParentObject.ParentObject.StartPosition;
        }

        /// <summary>
        /// Получение длины юнита
        /// </summary>
        /// <param name="unit">юнит</param>
        /// <returns>длина юнита</returns>
        public static int get_Length(this UnitTextBase unit)
        {
            return unit.PositionInfo.End - unit.PositionInfo.Start;
        }
        #endregion

        #region [TextParser.Sentence]
        /// <summary>
        /// Начало предложение относительного целого текста
        /// </summary>
        /// <param name="sentence">предложение</param>
        /// <returns>начало предложения</returns>
        public static int GetFullStartPosition(this TextParsing.Sentence sentence)
        {
            return sentence.StartPosition + sentence.Parent.StartPosition;
        }

        /// <summary>
        /// Конец предложения относительного целого текста
        /// </summary>
        /// <param name="sentence">предложение</param>
        /// <returns>конец предложения</returns>
        public static int GetFullEndPosition(this TextParsing.Sentence sentence)
        {
            return sentence.EndPosition + sentence.Parent.StartPosition;
        }
        #endregion

        #region [IEnumerable<TextParser.TextPart>]
        /// <summary>
        /// Получение текста из заданной коллекции частей текста
        /// </summary>
        /// <param name="unitsInterval">коллекция частей текста</param>
        /// <returns>текст</returns>
        public static string GetText(this IEnumerable<TextParsing.TextPart> textParts)
        {
            StringBuilder result = new StringBuilder();
            foreach (var part in textParts)
                result.Append(part.Text);
            return result.ToString();
        }
        #endregion
    }
}
