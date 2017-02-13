using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Декоратор для выделения подпредложений, на основе его текста
	/// </summary>
	internal abstract class SubSentenceByContentDecoratorBase : SyntaxAnalyzerDecoratorBase
	{
		#region [.ctor().]
		public SubSentenceByContentDecoratorBase(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

		/// <summary>
		/// Проверка, что юнит - разделитель подпредложений
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>результат проверки</returns>
		protected abstract bool IsSeparator(UnitTextBase unit);

		/// <summary>
		/// Проверка, что заданный текст - подпредложение
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="isFullSentence">флаг, что текст - это все предложение</param>
		/// <param isLast="text">флаг, что текст - последнее подпредложение</param>
		/// <returns>результат проверки</returns>
		protected abstract bool IsSubSentence(string text, bool isFullSentence, bool isLast);

		/// <summary>
		/// Тип подпредложения
		/// </summary>
		protected abstract SubSentenceType SubSentenceType { get; }

		/// <summary>
		/// Заполнение заданного подпредложения
		/// </summary>
		/// <param name="subSentence">родительское подпредложение</param>
		/// <param name="childSubSentences">дочерние подпредложения</param>
		protected void FillSubSentence(SubSentence subSentence, IEnumerable<SubSentence> childSubSentences)
		{
			IEnumerable<UnitTextBase> decomposingUnits = subSentence.Units;
			if (!subSentence.Units.Any(_ => _.ParentObject == subSentence))
			{
				var unit = new UnmarkedText("", subSentence.Units.Last().PositionInfo.End) { ParentObject = subSentence };
				decomposingUnits = decomposingUnits.Union(new UnitTextBase[] { unit });
			}

			decomposingUnits.DecomposedUnits(childSubSentences.Union(new SubSentence[] { subSentence }));
			subSentence.AppendSubSentences(childSubSentences);
		}

		/// <summary>
		/// Выделение подпредложений в заданном
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>список выделенных подпредложений</returns>
		protected List<SubSentence> SelectSubSentencesByContent(SubSentence subSentence)
		{
			List<SubSentence> result = new List<SubSentence>();

			UnitTextBase first = null;
			StringBuilder builder = new StringBuilder();
			foreach (UnitTextBase unit in subSentence.Units)
			{
				if (first == null)
					first = unit;

				if (IsSeparator(unit))
				{
					AnalyzeText(builder.ToString(), result, first, unit.Next, false);
					first = unit;
					builder.Clear();
					continue;
				}
				builder.Append(unit.Text);
			}
			AnalyzeText(builder.ToString(), result, first, null, first == subSentence.Units.First());
			return result;
		}

		/// <summary>
		/// Создание подпредложения, если необходимо
		/// </summary>
		/// <param name="builder">текст</param>
		/// <param name="collection">коллекция подпредложений</param>
		/// <param name="begin">начало интервала юнитов</param>
		/// <param name="end">конец интервала юнитов</param>
		/// <param name="isFullSentence">флаг, что текст - это все предложение</param>
		private void AnalyzeText(string text, List<SubSentence> collection, UnitTextBase begin, UnitTextBase end, bool isFullSentence)
		{
			if (IsSubSentence(text, isFullSentence, end == null))
				collection.Add(CreateSubSentence(begin, end));
		}

		/// <summary>
		/// Создание подпредложения
		/// </summary>
		/// <param name="begin">начало интервала юнитов</param>
		/// <param name="end">конец интервала юнитов</param>
		/// <returns>подпредложение</returns>
		private SubSentence CreateSubSentence(UnitTextBase begin, UnitTextBase end)
		{
			SubSentence result = new SubSentence(SubSentenceType, begin.PositionInfo.Start);
			result.SetSubSentenceUnits(begin, end);
			return result;
		}
	}
}
