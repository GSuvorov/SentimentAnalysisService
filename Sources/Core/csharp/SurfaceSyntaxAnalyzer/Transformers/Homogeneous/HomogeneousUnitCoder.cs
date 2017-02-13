using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Utils;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Класс для кодирования/декодирования юнитов дял объединения однородных
	/// </summary>
	internal class HomogeneousUnitCoder : UnitCoderBase
	{
		#region [UnitCoderBase]
		protected override string CodeUnit(Linguistics.Core.UnitTextBase unit)
		{
			string result;
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
				default:
					throw new ArgumentException("wrong UnitTextType");
			}
			return result;
		}
		#endregion

		/// <summary>
		/// Кодирование юнита неразмеченного текста
		/// </summary>
		/// <param name="unmarked">юнит неразмеченного текста</param>
		/// <returns>закодированное значение</returns>
		private static string CodeUnmarked(UnmarkedText unmarked)
		{
			if (unmarked.IsSeparator(","))
				return ",";
			else if (DictionaryResource.IsNegotiationParticle(unmarked.Text))
				return "neg";
			else if (DictionaryResource.IsOtherEnumeration(unmarked.Text))
				return "etc";
			return "other";
		}

		/// <summary>
		/// Кодирование сущности
		/// </summary>
		/// <param name="unmarked">сущности</param>
		/// <returns>закодированное значение</returns>
		private static string CodeEntity(Entity entity)
		{
			string result;
			switch (entity.EntityInfo.Type.EntityType)
			{
				case EntityType.Adverb:
					result = "adverb";
					break;
				case EntityType.Pretext:
					result = "pretext";
					break;
				case EntityType.Pronoun:
					result = "pronoun";
					break;
				case EntityType.Adjective:
                case EntityType.Superlative:
					result = "adjective";
					break;
                case EntityType.Comparative:
                    result = "comparative";
                    break;
				default:
					result = "entity";
					break;
			}
			return result;
		}

		/// <summary>
		/// Кодирование служебной сущности
		/// </summary>
		/// <param name="unmarked">служебная сущность</param>
		/// <returns>закодированное значение</returns>
		private static string CodeCeterus(Entity entity)
		{
			if (DictionaryResource.IsHomogeneousConjunction(entity.Text))
				return "conj";
			else if (new SimpleStringBuilder(entity.Text).LowerValue == "а также")
				return "also";
			return "other";
		}
	}
}
