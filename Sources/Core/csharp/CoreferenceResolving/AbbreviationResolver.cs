using System;
using System.Collections.Generic;
using System.Linq;

using Linguistics.Core;

namespace CoreferenceResolving
{
    /// <summary>
    /// Резрешитель аббревиатур
    /// </summary>
    public class AbbreviationResolver
    {
        #region [.consts.]
        /// <summary>
        /// кавычки
        /// </summary>
        private static readonly string Quotes = "“”\"'«»" + (char)0x201C + (char)0x201D;
        /// <summary>
        /// Максимальное число удалений подряд при подсчете расстояния Левештейна
        /// </summary>
        private const int MaximumDeleteOperationsCount = 2;
        #endregion

        /// <summary>
        /// Разрешение аббревиатур в тексте
        /// </summary>
        /// <param name="text">текст</param>
        public void Resolve(IText text)
        {
            foreach (var sentence in text.Sentences)
                Resolve(null, sentence.SubsentsHierarchical);
        }

        /// <summary>
        /// Разрешение аббревиатур в коллекции подпредложений
        /// </summary>
        /// <param name="parentSubSentence">родительское подпредложение для коллекции</param>
        /// <param name="subSentences">коллекция подпредложений</param>
        private void Resolve(SubSentence parentSubSentence, IEnumerable<SubSentence> subSentences)
        {
            foreach (var subSentence in subSentences)
            {
                if ((subSentence.Type == SubSentenceType.ApproximatingForceps) && (parentSubSentence != null))
                {
                    var previousUnits = parentSubSentence.Units.Where(_ => _.PositionInfo.Start < subSentence.StartPosition).ToArray();
                    var potencialSource = previousUnits.LastOrDefault(_ => !(string.IsNullOrWhiteSpace(_.Text) || IsQuote(_.Text.Trim())));
                    Resolve(GetAbbreviationFromBrackets(subSentence), potencialSource);
                }
                
                if (subSentence.SubTextInfo != null)
                    Resolve(subSentence.SubTextInfo);
                else
                    Resolve(subSentence, subSentence.Children);
            }
        }

        /// <summary>
        /// Разрешение аббревиатуры
        /// </summary>
        /// <param name="abbreviation">аббревиатура</param>
        /// <param name="possibleAbbreviationSource">потенциальная расшифровка аббревиатуры</param>
        private void Resolve(Entity abbreviation, UnitTextBase possibleAbbreviationSource)
        {
            if ((abbreviation == null) || (possibleAbbreviationSource == null) || !possibleAbbreviationSource.IsEntity)
                return;

            string[] words = possibleAbbreviationSource.Text.Split(' ', '-');
            if (!IsOnlySimpleWords(words))
                return;

            char[] firstLetters = words.Select(_ => _[0]).ToArray();
            int maxLevenshteinDistance = GetMaximumLevenshteinDistance(abbreviation.Value);
            var source = (Entity)possibleAbbreviationSource;
            if ((firstLetters.Length >= abbreviation.Value.Length) && IsSameLetter(firstLetters[0], abbreviation.Value[0]))
            {
                int abbreviationIndex = 1;
                int deleteOperationCount = 0;
                for (int i = 1; i < firstLetters.Length; ++i)
                {
                    if ((abbreviationIndex < abbreviation.Value.Length) &&
                        IsSameLetter(firstLetters[i], abbreviation.Value[abbreviationIndex]))
                    {
                        ++abbreviationIndex;
                        while ((abbreviationIndex < abbreviation.Value.Length) && (abbreviation.Value[abbreviationIndex] == ' '))
                            ++abbreviationIndex;
                        deleteOperationCount = 0;
                    }
                    else
                    {
                        ++deleteOperationCount;
                        --maxLevenshteinDistance;
                    }
                    if ((deleteOperationCount > MaximumDeleteOperationsCount) || (maxLevenshteinDistance == 0))
                        break;
                }
                if ((abbreviationIndex >= abbreviation.Value.Length) &&
                    (deleteOperationCount <= MaximumDeleteOperationsCount) &&
                    (maxLevenshteinDistance > 0))
                /// удалось разрешить аббревиатуру
                {
                    ((NamedEntityInfoBase)abbreviation.EntityInfo).Name = source.Value;
                    return;
                }
            }
            Resolve(abbreviation, source.Children.FirstOrDefault(_ => _.PositionInfo.End == source.PositionInfo.End));
        }

        /// <summary>
        /// Проверка букв на одинаковость (без учета регистра)
        /// </summary>
        /// <param name="first">первая буква</param>
        /// <param name="second">вторая буква</param>
        /// <returns>результат проверки</returns>
        private bool IsSameLetter(char first, char second)
        {
            return Char.ToUpper(first) == Char.ToUpper(second);
        }

        /// <summary>
        /// Проверка, что все слова в коллекции - простые
        /// </summary>
        /// <param name="unit">коллекция слов</param>
        /// <returns>результат проверки</returns>
        private bool IsOnlySimpleWords(IEnumerable<string> words)
        {
            foreach (string word in words)
            {
                if (!IsSimpleWord(word))
                    return false;
            }
            return true;
        }

        /// <summary>
        /// Проверка, что заданный текст - это слово, состоящее только из букв
        /// </summary>
        /// <param name="text">текст</param>
        /// <returns>результат проверки</returns>
        private bool IsSimpleWord(string text)
        {
            return text.All(_ => Char.IsLetter(_) || (_ == '.'));
        }

        /// <summary>
        /// Получение максимально допустимого расстояния Левенштейна для аббревиатуры
        /// </summary>
        /// <param name="abbreviation">аббревиатуры</param>
        /// <returns>максимально допустимое расстояние Левенштейна</returns>
        private int GetMaximumLevenshteinDistance(string abbreviation)
        {
            int result = 0;
            switch (abbreviation.Length)
            {
                case 2:
                case 3:
                    result = 1;
                    break;
                case 4:
                case 5:
                    result = 2;
                    break;
                case 6:
                case 7:
                    result = 3;
                    break;
                case 8:
                case 9:
                    result = 4;
                    break;
                case 10:
                    result = 5;
                    break;
                default:
                    throw new InvalidOperationException("very long abbreviation");
            }
            return result;
        }

        /// <summary>
        /// Получение аббревиатуры из подпредложения в скобках
        /// </summary>
        /// <param name="subSentence">подпредложение</param>
        /// <returns>аббревиатура</returns>
        private Entity GetAbbreviationFromBrackets(SubSentence subSentence)
        {
            #region [.defense.]
            if (subSentence.Type != SubSentenceType.ApproximatingForceps)
                throw new InvalidOperationException("subSentence not ApproximatingForceps");
            #endregion
            Entity result = null;
            var firstSentence = subSentence.SubTextInfo.Sentences.First();
            var firstSubSentence = firstSentence.SubsentsFlatten.First();
            if ((subSentence.SubTextInfo.Sentences.Length == 1) &&
                (firstSentence.SubsentsFlatten.Length == 1) &&
                !firstSubSentence.Children.Any())
            /// на разрешение попадают только те аббревиатуры, которые находятся одни в скобках (можно в кавычках)
            {
                foreach (var unit in firstSubSentence.Units)
                {
                    if (unit.IsEntity)
                    {
                        Entity entity = (Entity)unit;
                        if (IsAbbreviation(entity))
                            result = entity;
                    }
                    else if (string.IsNullOrWhiteSpace(unit.Text) || IsQuote(unit.Text.Trim()))
                        continue;
                    else
                    {
                        result = null;
                        break;
                    }
                }
            }
            return result;
        }

        /// <summary>
        /// Проверка, что заданный текст - кавычка
        /// </summary>
        /// <param name="text">текст</param>
        /// <returns>результат проверки</returns>
        private bool IsQuote(string text)
        {
            return (text.Length == 1) && Quotes.Contains(text[0]);
        }

        /// <summary>
        /// Проверка, что заданная сущность - аббревиатура
        /// </summary>
        /// <param name="entity">сущность</param>
        /// <returns>результат проверки</returns>
        private bool IsAbbreviation(Entity entity)
        {
            return entity.IsType(EntityType.OrgAbbr) || ((entity.EntityInfo is NamedEntityInfoBase) && IsEvristicAbbreviation(entity));
        }

        /// <summary>
        /// Проверка, что заданная сущность - аббревиатура
        /// </summary>
        /// <param name="entity">сущность</param>
        /// <returns>результат проверки</returns>
        private bool IsEvristicAbbreviation(Entity entity)
        {
            if (entity.Value.Length < 2)
                return false;
            int countLower = 0;
	        int countDigit = 0;
	        int count = 0;
            foreach (char symbol in entity.Value)
            {
                if (Char.IsLetterOrDigit(symbol))
                {
                    ++count;
                    if (Char.IsDigit(symbol))
                        ++countDigit;
                    else if (Char.IsLower(symbol))
                        ++countLower;
                }
            }
            if (count > 0)
	        {
                return ((countDigit == 0) && ((countLower == 0) || ((countLower == 1) && (entity.Value.Length > 3)))) ||
                    ((countLower < entity.Value.Length - countLower) && (entity.Value.Length < 6) && (countDigit < entity.Value.Length - countDigit));
	        }
            return false;
        }
    }
}
