using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Класс анализатора предложения на русском языке
	/// </summary>
	internal class SyntaxAnalyzerRU : SyntaxAnalyzerBase
	{
		/// <summary>
		/// Разделители подпредложений
		/// </summary>
		private static readonly string[] c_subSentenceSeparators = new string[] { ",", ":" };

		/// <summary>
		/// иерархия подпредложений
		/// </summary>
		private static SubSentenceHierarchy _hierarchy;

		#region [.ctor().]
		static SyntaxAnalyzerRU()
		{
		}
		#endregion

		#region [SentenceAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			var introductories = subSentence.Children.Where(_ => _.Type == SubSentenceType.Introductory).ToArray();
			int introductoryIndex = 0;

			_hierarchy = new SubSentenceHierarchy(subSentence);
			foreach (var unit in subSentence.Units)
			{
				if (!unit.IsEmptyText())
					AnalyzeUnit(unit);
				
				if ((introductoryIndex < introductories.Length) &&
					(unit.PositionInfo.End == introductories[introductoryIndex].StartPosition))
				{
					InsertIntroductory(introductories[introductoryIndex], unit);
					++introductoryIndex;
				}
			}
			return _hierarchy.GetHierarchy();
		}
		#endregion

		private void InsertIntroductory(SubSentence introductory, UnitTextBase unit)
		{
			#region [.defense.]
			if (introductory.Type != SubSentenceType.Introductory)
				throw new ArgumentException("subSentence isn't Introductory");
			#endregion
			if ((_hierarchy.CurrentSubSentence != null) && (_hierarchy.CurrentType == SubSentenceType.Participial))
				_hierarchy.CreateNewSubSentence(unit, true);
		}

		/// <summary>
		/// Анализ заданного юнита
		/// </summary>
		/// <param name="unit">юнит</param>
		private void AnalyzeUnit(UnitTextBase unit)
		{
			switch (unit.UnitTextType)
			{
				case UnitTextType.U:
					AnalyzeUnmarkedText((UnmarkedText)unit);
					break;
				case UnitTextType.CET:
					AnalyzeCeterus((Entity)unit);
					break;
				case UnitTextType.ENTITY:
					break;
				case UnitTextType.VERB:
					AnalyzeVerbForm((Entity)unit);
					break;
				default:
					throw new ArgumentException("wrong UnitTextType");
			}
		}

		/// <summary>
		/// Анализ глагольных форм
		/// </summary>
		/// <param name="verb">глагольная форма</param>
		private void AnalyzeVerbForm(Entity verb)
		{
			_hierarchy.AddVerbFormToCurrent(verb);
		}

		/// <summary>
		/// Анализ прочего юнита
		/// </summary>
		/// <param name="ceterus">прочий юнит</param>
		private void AnalyzeCeterus(Entity ceterus)
		{
			if (ceterus.IsType(EntityType.Conjunction))
				_hierarchy.AddConjunctionToCurrent(ceterus);
		}

		/// <summary>
		/// Анализ неразмеченного текста
		/// </summary>
		/// <param name="text">неразмеченный текст</param>
		private void AnalyzeUnmarkedText(UnmarkedText text)
		{
			if (IsSubSentenceSeparator(text))
				_hierarchy.CreateNewSubSentence(text);
		}

		/// <summary>
		/// Проверка, что заданный юнит является разделителем подпредложений
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>результат проверки</returns>
		private bool IsSubSentenceSeparator(UnitTextBase unit)
		{
			if (unit.IsSeparator(c_subSentenceSeparators))
			{
				UnitTextBase previous = unit.GetNonEmptyPrevious();
				UnitTextBase next = unit.GetNonEmptyNext();
				if (IsNumericWithoutSeparator(previous, unit.Text) && IsNumericWithoutSeparator(next, unit.Text))
					return false;
				return true;
			}
			return false;
		}

		/// <summary>
		/// Проверка, что заданный юнит - числительное без заданного разделителя
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <param name="separator">разделитель</param>
		/// <returns>результат проверки</returns>
		private bool IsNumericWithoutSeparator(UnitTextBase unit, string separator)
		{
			bool isNumeric = (unit != null) && (unit.UnitTextType == UnitTextType.CET) && ((Entity)unit).IsType(EntityType.Numeric);
			return isNumeric && (unit.Text.IndexOf(separator) < 0);
		}
	}
}
