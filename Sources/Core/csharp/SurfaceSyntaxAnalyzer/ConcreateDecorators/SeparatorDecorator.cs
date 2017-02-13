using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using System.Text.RegularExpressions;
using Utils;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Декоратор для выделения разделителей юнитов
	/// </summary>
	internal class SeparatorDecorator : SyntaxAnalyzerDecoratorBase
	{
		/// <summary>
		/// регулярное выражение для поиска разделителей
		/// </summary>
		private static readonly Regex _separators = new Regex("(,)|(:)|(-)|(–)|(—)");

		#region [.ctor().]
		public SeparatorDecorator(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

		#region [SyntaxAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			subSentence.SetUnits(ParseUnitsBySeparators(subSentence.Units));
			return base.AnalyzeSubSentence(subSentence);
		}
		#endregion

		/// <summary>
		/// Парсинг юнитов по разделителям
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		/// <returns>список юнитов после парсинга</returns>
		private List<UnitTextBase> ParseUnitsBySeparators(UnitTextBase[] units)
		{
			List<UnitTextBase> result = new List<UnitTextBase>(units.Length);
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
			var separators = _separators.Matches(unit.Text);
			List<TextPart> result = new List<TextPart>(separators.Count);
			foreach (Match separator in separators)
			{
				if (IsCorrectSeparator(unit.Text, separator.Index, separator.Value))
					result.Add(new TextPart() { Start = separator.Index, End = separator.Index + separator.Length });
			}
			return result.ToArray();
		}

		/// <summary>
		/// Проверка, что разделитель выделен правильно
		/// </summary>
		/// <param name="text">исходный текст</param>
		/// <param name="separatorIndex">индекс разделителя</param>
		/// <param name="separatorValue">значение разделителя</param>
		/// <returns>результат проверки</returns>
		private bool IsCorrectSeparator(string text, int separatorIndex, string separatorValue)
		{
			if ((separatorValue == "-") &&
				(!Char.IsWhiteSpace(text.GetPreviousSymbol(separatorIndex)) ||
				!Char.IsWhiteSpace(text.GetNextSymbol(separatorIndex))))
			{
				return false;
			}
			return true;
		}
	}
}
