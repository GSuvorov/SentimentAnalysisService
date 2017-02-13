using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Linguistics.Core.Morpho;
using Utils;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// класс расширений
	/// </summary>
	internal static class Extensions
	{
		#region [string]
		/// <summary>
		/// Получение предыдущего символа
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="index">индекс текущей позиции</param>
		/// <returns>предыдущий символ</returns>
		public static char GetPreviousSymbol(this string text, int index)
		{
			return (index > 0) ? text[index - 1] : '\0';
		}

		/// <summary>
		/// Получение следующего символа
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="index">индекс текущей позиции</param>
		/// <returns>следующий символ</returns>
		public static char GetNextSymbol(this string text, int index)
		{
			return (index < text.Length - 1) ? text[index + 1] : '\0';
		}

		/// <summary>
		/// Проверка, что в заданной строке нет текста (только пробелы)
		/// </summary>
		/// <param name="str">строка</param>
		/// <returns>результат проверки</returns>
		public static bool IsEmptyText(this string str)
		{
			return string.IsNullOrWhiteSpace(str);
		}

		/// <summary>
		/// Проверка, что заданная строка - пустая
		/// </summary>
		/// <param name="str">строка</param>
		/// <returns>результат проверки</returns>
		public static bool IsEmpty(this string str)
		{
			return string.IsNullOrEmpty(str);
		}

		/// <summary>
		/// Проверка, что текущее значение такое же, как и предыдущее
		/// В случае, если предыдущее значение пустое - оно станет равно текущему
		/// </summary>
		/// <param name="currentValue">текущее значение</param>
		/// <param name="previousValue">предыдущее значение</param>
		/// <returns>результат проверки</returns>
		public static bool IsSameValue(this string currentValue, ref string previousValue)
		{
			string lowerCurrent = currentValue.ToLower();
			if (previousValue.IsEmptyText())
				previousValue = lowerCurrent;
			else if (previousValue != lowerCurrent)
				return false;
			return true;
		}
		#endregion		

		#region [UnitTextBase]
		/// <summary>
		/// Парсинг заданного юнита по указанной позиции
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <param name="splitPosition">позиция</param>
		/// <returns>пара юнитов</returns>
		public static Tuple<UnitTextBase, UnitTextBase> SplitUnit(this UnitTextBase unit, int splitPosition)
		{
			var units = unit.SplitUnit(new TextPart() { Start = splitPosition, End = splitPosition });
			return new Tuple<UnitTextBase, UnitTextBase>(units[0], units[1]);
		}

		/// <summary>
		/// Парсинг заданного юнита по указанным разделителям
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <param name="separators">коллекция разделителей</param>
		/// <returns>коллекция юнитов</returns>
		public static List<UnitTextBase> SplitUnit(this UnitTextBase unit, params TextPart[] separators)
		{
			List<UnitTextBase> result = new List<UnitTextBase>();
			int currentTextPosition = 0;
			foreach (var unitSeparator in separators)
			{
				result.Add(unit.CreateUnmarkedText(currentTextPosition, unitSeparator.Start));
				if (unitSeparator.Start != unitSeparator.End)
					result.Add(unit.CreateUnmarkedText(unitSeparator.Start, unitSeparator.End));
				currentTextPosition = unitSeparator.End;
			}
			result.Add(unit.CreateUnmarkedText(currentTextPosition, unit.Text.Length));
			return result;
		}

		/// <summary>
		/// Создание неразмеченного текста из заданного юнита
		/// </summary>
		/// <param name="text">юнит</param>
		/// <param name="startPosition">начало неразмеченного текста</param>
		/// <param name="endPosition">конец неразмеченного текста</param>
		/// <returns>неразмеченный текст</returns>
		public static UnmarkedText CreateUnmarkedText(this UnitTextBase unit, int startPosition, int endPosition)
		{
			return new UnmarkedText
				(
					unit.Text.Substring(startPosition, endPosition - startPosition),
					unit.PositionInfo.Start + startPosition
				);
		}

		/// <summary>
		/// Проверка, что у юнита отсутствует текст (есть только пробелы)
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>результат проверки</returns>
		public static bool IsEmptyText(this UnitTextBase unit)
		{
			return unit.Text.IsEmptyText();
		}

		/// <summary>
		/// Получение предыдущего юнита с текстом
		/// </summary>
		/// <param name="unit">текущий юнит</param>
		/// <returns>предыдущий юнит с текстом</returns>
		public static UnitTextBase GetNonEmptyPrevious(this UnitTextBase unit)
		{
			UnitTextBase result = unit.Previous;
			while (true)
			{
				if ((result == null) || !result.IsEmptyText())
					break;
				result = result.Previous;
			}
			return result;
		}

		/// <summary>
		/// Получение следующего юнита с текстом
		/// </summary>
		/// <param name="unit">текущий юнит</param>
		/// <returns>следующий юнит с текстом</returns>
		public static UnitTextBase GetNonEmptyNext(this UnitTextBase unit)
		{
			UnitTextBase result = unit.Next;
			while (true)
			{
				if ((result == null) || !result.IsEmptyText())
					break;
				result = result.Next;
			}
			return result;
		}

		/// <summary>
		/// Проверка, что юнит является одним из заданных разделителей
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <param name="separators">коллекция разделителей</param>
		/// <returns>результат проверки</returns>
		public static bool IsSeparator(this UnitTextBase unit, params string[] separators)
		{
            if (unit != null)
            {
                string text = unit.Text.Trim();
                return separators.Any(_ => _ == text);
            }
            return false;
		}

		/// <summary>
		/// Проверка, что юнит - предлог
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>результат проверки</returns>
		public static bool IsPretext(this UnitTextBase unit)
		{
			return (unit != null) && unit.IsEntity && ((Entity)unit).IsType(EntityType.Pretext);
		}
		#endregion

		#region [Entity]
		/// <summary>
		/// Проверка, что заданная сущность - составной глагол
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public static bool IsCompoundVerb(this Entity entity)
		{
			return (entity.UnitTextType == UnitTextType.VERB) && entity.Children.Any();
		}

		/// <summary>
		/// Проверка, что заданная сущность - потенциальное подлежащее подпредложения (Им.п.)
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public static bool IsPossibleSubject(this Entity entity)
		{
			return entity.IsContainCases(Case.Nominative);
		}

		/// <summary>
		/// Проверка, что заданная сущность - подлежащее подпредложения
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public static bool IsSubject(this Entity entity)
		{
			return entity.EntityInfo.Morpho.Any() && !entity.EntityInfo.Morpho.Any(_ => _.Case != Case.Nominative);
		}

		/// <summary>
		/// Проверка, что заданная сущность может стоять в заданных падежах
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <param name="cases">падежы</param>
		/// <returns>результат проверки</returns>
		public static bool IsContainCases(this Entity entity, params Case[] cases)
		{
			return entity.EntityInfo.Morpho.Any(_ => cases.Contains(_.Case));
		}

		/// <summary>
		/// Проверка, что заданная сущность является членом предложения
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public static bool IsSentenceMember(this Entity entity)
		{
			return (entity.EntityInfo.Morpho != null) &&
				((entity.UnitTextType == UnitTextType.ENTITY) ||
				((entity.UnitTextType == UnitTextType.CET) && !entity.IsType(EntityType.Numeric) && entity.EntityInfo.Morpho.Any()));
		}
		#endregion

		#region [SubSentence]
		/// <summary>
		/// Задание юнитам нового родительского подпредложения
		/// </summary>
		/// <param name="subSentence">родительское подпредложение</param>
		/// <param name="begin">начало интервала юнитов</param>
		/// <param name="end">конец интервала юнитов</param>
		public static void SetSubSentenceUnits(this SubSentence subSentence, UnitTextBase begin, UnitTextBase end)
		{
			UnitTextBase current = begin;
			while (current != end)
			{
				current.ParentObject = subSentence;
				current = current.Next;
			}
		}

		/// <summary>
		/// Поиск родительского подпредложения для заданного дочернего подпредложения
		/// </summary>
		/// <param name="child">дочернее</param>
		/// <param name="parents">коллекция возможных родителей</param>
		/// <param name="startIndex">индекс, с которого начинать поиск</param>
		/// <returns>родительское подпредложение</returns>
		private static SubSentence GetParentSubSentence(this SubSentence child, IList<SubSentence> parents, ref int startIndex)
		{
			for (; startIndex < parents.Count; ++startIndex)
			{
				if (child.IsChildSubSentence(parents, startIndex))
					return parents[startIndex];
			}
			#region [.defense.]
			throw new InvalidOperationException("Can't find parent subSentence");
			#endregion
		}

		/// <summary>
		/// Проверка, что текущее подпредложение является дочерним для заданного
		/// </summary>
		/// <param name="current">текущее подпредложение</param>
		/// <param name="parents">коллекция потенциальных родительских подпредложений</param>
		/// <param name="index">индекс заданного подпредложения</param>
		/// <returns>результат проверки</returns>
		private static bool IsChildSubSentence(this SubSentence current, IList<SubSentence> parents, int index)
		{
			if (parents[index].StartPosition <= current.StartPosition)
			{
				SubSentence next = (index < parents.Count - 1) ? next = parents[index + 1] : null;
				if ((next == null) || (current.StartPosition < next.StartPosition))
					return true;
			}
			return false;
		}

		/// <summary>
		/// Получение начальной позиции последнего юнита подпредложения
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>начальная позиция последнего юнита</returns>
		public static int GetLastUnitStartPosition(this SubSentence subSentence)
		{
			if (subSentence.Units.Any())
				return subSentence.Units.Last().PositionInfo.Start;
			else
				return subSentence.StartPosition;
		}
		#endregion

		#region [IEnumerable<UnitTextBase>]
		/// <summary>
		/// Раскладка юнитов по подпредложениям
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		/// <param name="subSentences">коллекция подпредложений</param>
		public static void DecomposedUnits(this IEnumerable<UnitTextBase> units, IEnumerable<SubSentence> subSentences)
		{
			var groups = units.GroupBy(_ => _.ParentObject).ToDictionary(_ => _.Key);
			if (groups.Count != subSentences.Count())
				throw new InvalidOperationException("possible subsentence count don't equal count in subsentence collection");
			foreach (var subSentence in subSentences)
				subSentence.SetUnits(groups[subSentence]);
		}

		/// <summary>
		/// Получение текста из заданной коллекции юнитов
		/// </summary>
		/// <param name="unitsInterval">коллекция юнитов</param>
		/// <returns>текст</returns>
		public static string GetText(this IEnumerable<UnitTextBase> unitsInterval)
		{
			StringBuilder result = new StringBuilder();
			foreach (UnitTextBase unit in unitsInterval)
				result.Append(unit.Text);
			return result.ToString();
		}
		#endregion

		#region [IEnumerable<SubSentence>]
		/// <summary>
		/// Раскладка дочерних подпредложений по подпредложениям
		/// </summary>
		/// <param name="units">коллекция дочерних подпредложений</param>
		/// <param name="subSentences">коллекция подпредложений</param>
		public static void DecomposedSubSentences(this IEnumerable<SubSentence> collection, IList<SubSentence> subSentences)
		{
			int currentIndex = 0;
			SubSentence current = subSentences[currentIndex];
			List<SubSentence> childList = new List<SubSentence>();
			foreach (var child in collection)
			{
				if (child.IsChildSubSentence(subSentences, currentIndex))
					childList.Add(child);
				else
				{
					current.AppendSubSentences(childList);
					childList.Clear();
					childList.Add(child);
					current = child.GetParentSubSentence(subSentences, ref currentIndex);
				}
			}
			current.AppendSubSentences(childList);
			for (++currentIndex; currentIndex < subSentences.Count; ++currentIndex)
				subSentences[currentIndex].AppendSubSentences(new SubSentence[] { });
		}

		public delegate void SubSentenceAnalyzerDelegate(SubSentence subSentence);
		/// <summary>
		/// Применение указанной функции-обработки всей иерархий подпредложений
		/// </summary>
		/// <param name="collection">коллекция вершин подпредложений</param>
		/// <param name="func">функция-обработчик</param>
		public static void ForAllHierarchy(this IEnumerable<SubSentence> collection, SubSentenceAnalyzerDelegate func)
		{
			#region [.defense.]
			if (func == null)
				throw new ArgumentNullException("func");
			#endregion
			foreach (var subSentence in collection)
			{
				if (subSentence.SubTextInfo == null)
				{
					func(subSentence);
					ForAllHierarchy(subSentence.Children, func);
				}
			}
		}
		#endregion
	}
}
