using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SurfaceSyntaxAnalyzer;
using Linguistics.Core;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using Utils;

namespace PTS
{
	/// <summary>
	/// Выделитель интвервалов чисел
	/// </summary>
	public class NumberRangeSelector
	{
		/// <summary>
		/// Длина контекста
		/// </summary>
		private const int ContextLength = 10;

		/// <summary>
		/// Кодер/декодер юнитов для выделения интервалов чисел
		/// </summary>
		private static UnitCoderBase _coder = new NumberRangeUnitCoder();

		/// <summary>
		/// Регулярные выражения для выделения чисел
		/// </summary>
		private static List<NumberRangeExpressionBase> _expressions = new List<NumberRangeExpressionBase>();

		/// <summary>
		/// Регулярное выражение для выделения чисел с запятой
		/// </summary>
		private static readonly Regex _floatNumeric = new Regex("(?<integral>numeric) ((\\.)|(,)) (?<fractional>numeric)");
		
		/// <summary>
		/// Комбинации
		/// </summary>
		private static readonly Regex _combinationRegex = new Regex("((at[ ]+least)|(at[ ]+most)|(min\\.)|(max\\.))");
		/// <summary>
		/// Ключевые слова, которые должны стоять отдельно
		/// </summary>
		private static readonly string[] _singleKeywords = new string[] { "min", "max", "min.", "max.", "great", "above", "и", "или", "and", "or", "не", "not", "no" };

		#region [.ctor().]
		static NumberRangeSelector()
		{
			_expressions.Add(new NumberErrorExpression());
			_expressions.Add(new NumberRangeExpression());
			_expressions.Add(new NumberOpenRangeExpression());
			_expressions.Add(new VerbalRangeExpression());
			_expressions.Add(new VerbalOpenRangeExpression());
		}
		#endregion

		/// <summary>
		/// Выделение контекстов для интервалов чисел из текста
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="language">язык текста</param>
		/// <returns>контексты для интервалов чисел</returns>
		public List<TextRange> SelectTextRanges(IText text, string language)
		{
			List<TextRange> result = new List<TextRange>();
			foreach (var sentence in text.Sentences)
				foreach (var subSentence in sentence.SubsentsHierarchical)
					result.AddRange(SelectTextRangesFromHierarchy(subSentence, language));
			return result.OrderBy(_ => _.Range.StartPosition).ToList();
		}

		/// <summary>
		/// Выделение контекстов для интервалов чисел из подпредложения
		/// </summary>
		/// <param name="subSentence">подпредложения</param>
		/// <param name="language">язык текста</param>
		/// <returns>контексты для интервалов чисел</returns>
		private TextRange[] SelectTextRangesFromHierarchy(SubSentence subSentence, string language)
		{
			List<TextRange> result = new List<TextRange>();
			if (subSentence.SubTextInfo != null)
				return SelectTextRanges(subSentence.SubTextInfo, language).ToArray();
			else
			{
				foreach (var range in SelectTextRanges(subSentence, language))
					result.Add(range);
				foreach (var child in subSentence.Children)
				{
					foreach (var range in SelectTextRangesFromHierarchy(child, language))
					{
						string leftContext = subSentence.Units.Where(_ => _.PositionInfo.End <= child.StartPosition).GetTextWithSubText();
						string rightContext = subSentence.Units.Where(_ => _.PositionInfo.Start > child.StartPosition).GetTextWithSubText();
						if (child.SubTextInfo != null)
						{
							leftContext += child.SubTextInfo.OpenSymbol;
							rightContext = child.SubTextInfo.CloseSymbol + rightContext;
						}
						range.Range.StartPosition += child.StartPosition + child.ParentObject.StartPosition;
						range.Range.EndPosition += child.StartPosition + child.ParentObject.StartPosition;
						range.UpdateLeftContext(leftContext);
						range.UpdateRightContext(rightContext);
						result.Add(range);
					}
				}
			}
			return result.ToArray();
		}

		/// <summary>
		/// Выделение контекстов для интервалов чисел из подпредложения
		/// </summary>
		/// <param name="subSentence">подпредложения</param>
		/// <param name="language">язык текста</param>
		/// <returns>контексты для интервалов чисел</returns>
		private TextRange[] SelectTextRanges(SubSentence subSentence, string language)
		{
			List<TextRange> result = new List<TextRange>();
			MarkedTextMap map = new MarkedTextMap();
			var units = RegroupUnits(SelectFloatNumeric(subSentence.Units, language));
			string coded = _coder.CodeUnits(units);
			#region [.range.]
			foreach (var range in SelectNumberRanges(coded))
				if (map.TryMarkText(range.StartPosition, range.EndPosition))
					result.Add(new TextRange(range, units, ContextLength));
			#endregion
			#region [.numeric.]
			foreach (var unit in units)
			{
				if (!unit.IsEntity)
					continue;
				Entity entity = (Entity)unit;
				if (entity.IsNumber() && map.TryMarkText(entity.PositionInfo.Start, entity.PositionInfo.End))
				{
					var range = Number.Create(entity);
					if (range != null)
						result.Add(new TextRange(range, units, ContextLength));
				}
			}
			#endregion
			foreach (var textRange in result)
			{
				textRange.Range.StartPosition += subSentence.ParentObject.StartPosition;
				textRange.Range.EndPosition += subSentence.ParentObject.StartPosition;
			}
			return result.ToArray();
		}

		/// <summary>
		/// Проверка, что заданная коллекция юнитов содержит только одно целое число
		/// </summary>
		/// <param name="units">юниты</param>
		/// <returns>результат проверки</returns>
		private bool IsInteger(IEnumerable<UnitTextBase> units)
		{
			bool result = true;
			bool isContainNumber = false;
			foreach(var unit in units)
			{
				if (!unit.IsEntity)
					continue;
				var entity = (Entity)unit;
				if (entity.Type.EntityType.HasValue && (entity.Type.EntityType.Value == EntityType.Numeric))
				{
					if (isContainNumber)
						result = false;
					else
					{
						isContainNumber = true;
						result = entity.Value.IndexOf('.') < 0;
					}
				}
				else if (RegexEntityType.IsNumberEntityType(entity.Type.CustomType))
					result = false;
				if (!result)
					break;
			}
			return result;
		}

		/// <summary>
		/// Выделение интервалов чисел
		/// </summary>
		/// <param name="codedString">закодированная строка для выделения интервалов</param>
		/// <returns>интервалы чисел</returns>
		private NumberRange[] SelectNumberRanges(string codedString)
		{
			List<NumberRange> result = new List<NumberRange>();
			foreach (var regex in _expressions)
				result.AddRange(regex.GetNumberRanges(codedString, _coder));
			return result.ToArray();
		}

		/// <summary>
		/// Выделение чисел с запятой
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		/// <param name="language">язык текста</param>
		/// <returns>модифицированная коллекция юнитов</returns>
		private UnitTextBase[] SelectFloatNumeric(IEnumerable<UnitTextBase> units, string language)
		{
			List<UnitTextBase> result = new List<UnitTextBase>(units);
			string coded = _coder.CodeUnits(units);
			int startAt = 0;
			Match match = null;
			while ((match = _floatNumeric.Match(coded, startAt)).Success)
			{
				var replacedUnits = _coder.GetUnits(match.Index, match.Length);
				if (IsFloatNumber(replacedUnits))
				{
					int index = result.IndexOf(replacedUnits.First());
					Entity floatNumeric = CreateFloatNumeric(replacedUnits, match, language);
					floatNumeric.ParentObject = replacedUnits.First().ParentObject;
					result.RemoveRange(index, replacedUnits.Length);
					result.Insert(index, floatNumeric);
					startAt = match.Index + match.Length;
				}
				else
					startAt = match.Index + 1;
			}
			return result.ToArray();
		}

		/// <summary>
		/// Проверка, что заданная коллекция юнитов представляет собой число с точкой
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		/// <returns>результат проверки</returns>
		private bool IsFloatNumber(IEnumerable<UnitTextBase> units)
		{
			foreach (UnitTextBase unit in units)
				if ((unit.UnitTextType == UnitTextType.U) && unit.Text.Contains(' '))
					return false;
			return true;
		}

		/// <summary>
		/// Создание числа с запятой
		/// </summary>
		/// <param name="units">юниты</param>
		/// <param name="match">найденное число</param>
		/// <param name="language">язык текста</param>
		/// <returns>число с запятой</returns>
		private Entity CreateFloatNumeric(IEnumerable<UnitTextBase> units, Match match, string language)
		{
			Group integralPart = match.Groups["integral"];
			Group fractionalPart = match.Groups["fractional"];
			string integral = _coder.GetUnits(integralPart.Index, integralPart.Length).GetTextWithSubText();
			string fractional = _coder.GetUnits(fractionalPart.Index, fractionalPart.Length).GetTextWithSubText();
			NumericEntityInfo numericInfo = null;
			if ((language == Language.English) && (fractional == "000"))
				numericInfo = new NumericEntityInfo() { Value = integral + fractional };
			else
				numericInfo = new NumericEntityInfo() { Value = integral + "." + fractional };
			return new Entity(units.GetTextWithSubText(), units.First().PositionInfo.Start, numericInfo);
		}

		/// <summary>
		/// Перегрупппировка юнитов
		/// </summary>
		/// <param name="units">юниты</param>
		/// <returns>перегруппированные юниты</returns>
		private List<UnitTextBase> RegroupUnits(UnitTextBase[] units)
		{
			var result = ParseUnitsBySeparators(units);
			result = ParseUnitsByEndKeywords(result);
			result = UnionCombinatoryUnits(result);
			return result;
		}

		/// <summary>
		/// Отрезание от юнитов ключевых слов, идущих в конце
		/// </summary>
		/// <param name="units">юниты</param>
		/// <returns>скорректированные юниты</returns>
		private List<UnitTextBase> ParseUnitsByEndKeywords(List<UnitTextBase> units)
		{
			List<UnitTextBase> result = new List<UnitTextBase>(units.Count);
			foreach (var unit in units)
			{
				bool isAdded = false;
				string value = unit.Text.TrimEnd().ToLower();
				foreach (string keyword in _singleKeywords)
				{
					int keywordStartPosition = value.Length - keyword.Length;
					if ((keywordStartPosition > 0) && value.EndsWith(keyword) && Char.IsWhiteSpace(value[keywordStartPosition - 1]))
					{
						if (unit.IsEntity)
							result.Add(new Entity(unit.Text.Substring(0, keywordStartPosition), unit.PositionInfo.Start, ((Entity)unit).EntityInfo));
						else
							result.Add(new UnmarkedText(unit.Text.Substring(0, keywordStartPosition), unit.PositionInfo.Start));
						result.Add(new UnmarkedText(unit.Text.Substring(keywordStartPosition), unit.PositionInfo.Start + keywordStartPosition));
						isAdded = true;
						break;
					}
				}
				if (!isAdded)
					result.Add(unit);
			}
			return result;
		}

		/// <summary>
		/// Объединение комбинаторных юнитов
		/// </summary>
		/// <param name="units">юниты</param>
		/// <returns>юниты с объединенными комбинаторными</returns>
		private List<UnitTextBase> UnionCombinatoryUnits(List<UnitTextBase> units)
		{
			if (!units.Any())
				return units;
			List<UnitTextBase> result = new List<UnitTextBase>(units.Count);
			var current = units.GetEnumerator();
			current.MoveNext();
			foreach (Match match in _combinationRegex.Matches(units.GetTextWithSubText().ToLower()))
			{
				var previousUnits = GetUnitsBeforePosition(units, ref current, match.Index);
				if (previousUnits.Any())
					result.AddRange(previousUnits);
				if (current.Current != null)
				{
					int startPosition = current.Current.PositionInfo.Start;
					var unitsForUnion = GetUnitsBeforePosition(units, ref current, match.Index + match.Length);
					if (unitsForUnion.Any() && match.Value.EndsWith(unitsForUnion.Last().Text.Trim().ToLower()))
					{
						StringBuilder text = new StringBuilder(unitsForUnion.GetTextWithSubText());
						if (startPosition != match.Index)
						{
							var lastUnit = previousUnits.Last();
							text.Insert(0, lastUnit.Text.Substring(match.Index - lastUnit.PositionInfo.Start, lastUnit.PositionInfo.End - match.Index));
						}
						result.Add(new UnmarkedText(text.ToString(), match.Index));
					}
					else
						result.AddRange(unitsForUnion);
				}
			}
			result.AddRange(GetUnitsBeforePosition(units, ref current));
			return result;
		}

		/// <summary>
		/// Получение юнитов, начинающихся перед заданной позицией
		/// </summary>
		/// <param name="start">первый юнит</param>
		/// <param name="position">позиция</param>
		/// <returns>юниты, начинающиеся перед заданной позицией</returns>
		private UnitTextBase[] GetUnitsBeforePosition(IList<UnitTextBase> units, ref List<UnitTextBase>.Enumerator start, int position = -1)
		{
			List<UnitTextBase> result = new List<UnitTextBase>();
			while ((start.Current != null) && ((position < 0) || (start.Current.PositionInfo.Start < position)))
			{
				result.Add(start.Current);
				start.MoveNext();
			}
			return result.ToArray();
		}

		/// <summary>
		/// Парсинг юнитов по разделителям
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		/// <returns>список юнитов после парсинга</returns>
		private List<UnitTextBase> ParseUnitsBySeparators(IList<UnitTextBase> units)
		{
			List<UnitTextBase> result = new List<UnitTextBase>(units.Count);
			foreach (UnitTextBase unit in units)
			{
				if (unit.UnitTextType == UnitTextType.U)
				{
					var separators = GetUnitSeparators(unit);
					if (separators.Any())
					{
						result.AddRange(unit.SplitUnit(separators));
						continue;
					}
				}
				result.Add(unit);
			}
			return result;
		}

		/// <summary>
		/// Получение коллекции раздителей в заданном юните
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>коллекция разделителей</returns>
		private TextPart[] GetUnitSeparators(UnitTextBase unit)
		{
			List<TextPart> result = new List<TextPart>();
			int wordPosition = 0;
			int separatorPosition = -1;
			bool isLetter = false;
			foreach (var word in unit.Text.Split(' '))
			{
				int symbolPosition = wordPosition;
				bool isPreviousPoint = false;
				foreach (var symbol in word)
				{
					bool isPoint = symbol == '.';
					if (!(isPoint && isPreviousPoint))
					{
						if (!isLetter && (separatorPosition >= 0))
						{
							result.Add(new TextPart() { Start = separatorPosition, End = symbolPosition });
							separatorPosition = -1;
						}
						isPreviousPoint = isPoint;
						if (Char.IsLetterOrDigit(symbol) || (symbol == '-'))
							isLetter = true;
						else
						{
							isLetter = false;
							separatorPosition = symbolPosition;
						}
					}
					++symbolPosition;
				}
				if (separatorPosition >= 0)
				{
					result.Add(new TextPart() { Start = separatorPosition, End = wordPosition + word.Length });
					separatorPosition = -1;
				}
				wordPosition += word.Length + 1;
			}
			return result.ToArray();
		}
	}
}
