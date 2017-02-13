using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SurfaceSyntaxAnalyzer;
using Linguistics.Core;
using Utils;

namespace PTS
{
	/// <summary>
	/// Кодер/декодер юнитов для выделения интервалов чисел
	/// </summary>
	internal class NumberRangeUnitCoder : UnitCoderBase
	{
		#region [UnitCoderBase]
		protected override string CodeUnit(Linguistics.Core.UnitTextBase unit)
		{
			string result = CodeByText(unit);
			if (string.IsNullOrEmpty(result))
			{
				switch (unit.UnitTextType)
				{
					case UnitTextType.U:
						result = CodeUnmarked((UnmarkedText)unit);
						break;
					case UnitTextType.ENTITY:
						result = CodeEntity((Entity)unit);
						break;
					case UnitTextType.CET:
						result = CodeCeterus((Entity)unit);
						break;
					case UnitTextType.VERB:
						result = "verb";
						break;
					default:
						throw new ArgumentException("wrong UnitTextType");
				}
			}
			return result;
		}
		#endregion

		private static string CodeByText(UnitTextBase unit)
		{
			var simpleValue = new SimpleStringBuilder(unit.IsEntity ? ((Entity)unit).Value : unit.Text).LowerValue;
			if ((simpleValue == "не") || (simpleValue == "not") || (simpleValue == "no"))
				return "not";
			else if ((simpleValue == "или") || (simpleValue == "либо") || (simpleValue == "or"))
				return "or";
			else if ((simpleValue == "и") || (simpleValue == "and"))
				return "and";
			else if ((simpleValue == "-") || (simpleValue == "–") || (simpleValue == "—"))
				return "-";
			else if (simpleValue == "+")
				return "+";
			else if ((simpleValue == "…") || (simpleValue == "..."))
				return "...";
			else if ((simpleValue == "≥") || (simpleValue == "›") || (simpleValue == ">") || (simpleValue == "≫"))
				return ">";
			else if ((simpleValue == "≤") || (simpleValue == "‹") || (simpleValue == "<") || (simpleValue == "≪"))
				return "<";
			else if ((simpleValue == "±") || (simpleValue == "÷") ||
				(simpleValue == ".") || (simpleValue == ",") ||
				(simpleValue == "(") || (simpleValue == ")"))
			{
				return simpleValue;
			}
			else if (simpleValue == "between")
				return "between";
			else if ((simpleValue == "около") || (simpleValue == "about"))
				return "about";
			else if ((simpleValue == "ниже") || (simpleValue == "менее") || (simpleValue == "меньше") ||
				(simpleValue == "less") || (simpleValue == "fewer") || (simpleValue == "few") || (simpleValue == "small") || (simpleValue == "below") ||
				(simpleValue == "at most") || (simpleValue == "max.") || (simpleValue == "lower") || (simpleValue == "shorter"))
			{
				return "less";
			}
			else if ((simpleValue == "больше") || (simpleValue == "более") || (simpleValue == "выше") ||
				(simpleValue == "more") || (simpleValue == "great") || (simpleValue == "higher") || (simpleValue == "above") ||
				(simpleValue == "at least") || (simpleValue == "min.") || (simpleValue == "longer") || (simpleValue == "large"))
			{
				return "great";
			}
			else if ((simpleValue == "равно") || (simpleValue == "equal"))
				return "equal";
			else if ((simpleValue == "от") || (simpleValue == "с") || (simpleValue == "from") || (simpleValue == "of"))
				return "from";
			else if ((simpleValue == "до") || (simpleValue == "to"))
				return "to";
			else if (simpleValue == "по")
				return "po";
			else if (simpleValue == "than")
				return "than";
			return null;
		}

		/// <summary>
		/// Кодирование юнита неразмеченного текста
		/// </summary>
		/// <param name="unmarked">юнит неразмеченного текста</param>
		/// <returns>закодированное значение</returns>
		private static string CodeUnmarked(UnmarkedText unmarked)
		{
			return "other";
		}

		/// <summary>
		/// Кодирование сущности
		/// </summary>
		/// <param name="unmarked">сущности</param>
		/// <returns>закодированное значение</returns>
		private static string CodeEntity(Entity entity)
		{
			if (entity.Type.CustomType == RegexEntityType.Fraction)
				return "fraction";
			else if (RegexEntityType.IsNumberEntityType(entity.Type.CustomType))
				return "numeric";
			else
				return "entity";
		}

		/// <summary>
		/// Кодирование служебной сущности
		/// </summary>
		/// <param name="unmarked">служебная сущность</param>
		/// <returns>закодированное значение</returns>
		private static string CodeCeterus(Entity entity)
		{
			string result;
			switch (entity.EntityInfo.Type.EntityType)
			{
				case EntityType.Numeric:
					try
					{
						double value = NumberParser.Parse(entity.Value);
						result = "numeric";
					}
					catch (FormatException)
					{
						result = "other";
					}
					break;
				case EntityType.Quote:
					result = "quote";
					break;
				default:
					result = "cet";
					break;
			}
			return result;
		}
	}
}
