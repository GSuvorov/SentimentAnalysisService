using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Utils;

namespace PTS
{
	/// <summary>
	/// класс расширений
	/// </summary>
	internal static class Extensions
	{
		/// <summary>
		/// Получение текста из заданной коллекции юнитов
		/// </summary>
		/// <param name="unitsInterval">коллекция юнитов</param>
		/// <returns>текст</returns>
		public static string GetTextWithSubText(this IEnumerable<UnitTextBase> unitsInterval)
		{
			StringBuilder result = new StringBuilder();
			int position = -1;
			foreach (UnitTextBase unit in unitsInterval)
			{
				if ((position >= 0) && (unit.PositionInfo.Start != position) && (unit.ParentObject != null))
				/// есть дочерний контент
				{
					var parentSubSentence = unit.ParentObject;
					var child = parentSubSentence.Children.First(_ => _.StartPosition == position);
					result.Append(child.SubTextInfo.OpenSymbol);
					result.Append(child.GetText());
					result.Append(child.SubTextInfo.CloseSymbol);
				}
				result.Append(unit.Text);
				position = unit.PositionInfo.End;
			}
			return result.ToString();
		}

		/// <summary>
		/// Проверка, что заданная сущность - число
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public static bool IsNumber(this Entity entity)
		{
			return entity.IsType(EntityType.Numeric) || RegexEntityType.IsNumberEntityType(entity.Type.CustomType);
		}

		/// <summary>
		/// Получение текста из заданного подпредложения
		/// </summary>
		/// <param name="unitsInterval">коллекция юнитов</param>
		/// <returns>текст</returns>
		public static string GetText(this SubSentence subSentence)
		{
			StringBuilder result = new StringBuilder();
			if (subSentence.SubTextInfo != null)
			{
				foreach (var sentence in subSentence.SubTextInfo.Sentences)
					foreach (var childSubSentence in sentence.SubsentsHierarchical)
						result.Append(childSubSentence.GetText());
			}
			else
				result.Append(subSentence.Units.GetTextWithSubText());
			return result.ToString();
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
	}
}
