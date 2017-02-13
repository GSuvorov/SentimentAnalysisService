using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// декоратор выделения вопросительных слов
	/// </summary>
	internal class QuestionWordDecorator : SyntaxAnalyzerDecoratorBase
	{
		/// <summary>
		/// Вопросительные слова
		/// </summary>
		private static readonly string[] QuestionWords = new string[]
			{
				"что", "кто", "почему", "какой", "чей", "который", "где", "куда", "откуда",
				"когда", "как", "сколько", "зачем", "отчего"
			};

		#region [.ctor().]
		public QuestionWordDecorator(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

		#region [SyntaxAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			var lastUnit = subSentence.ParentObject.SubsentsHierarchical.Last().Units.LastOrDefault(_ => !_.IsEmptyText());
			if ((lastUnit != null) && !lastUnit.IsEntity && (lastUnit.Text.EndsWith("?") || lastUnit.Text.EndsWith("?!")))
			{
				Entity questionWord = GetQuestionWord(subSentence);
				if (questionWord != null)
					questionWord.Type.EntityType = EntityType.QuestionWord;
			}
			return base.AnalyzeSubSentence(subSentence);
		}
		#endregion

		/// <summary>
		/// Получение вопросительного слова из подпредложения
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>вопросительное слово</returns>
		private Entity GetQuestionWord(SubSentence subSentence)
		{
			#region [.defense.]
			if (subSentence == null)
				throw new ArgumentNullException("subSentence");
			#endregion
			Entity result = null;
			UnitTextBase unit = subSentence.Units.FirstOrDefault(_ => !_.IsEmptyText());
			if ((unit != null) && unit.IsEntity)
			{
				Entity entity = (Entity)unit;
				if (entity.IsType(EntityType.Pretext))
				{
					unit = unit.GetNonEmptyNext();
					if (unit.IsEntity)
						entity = (Entity)unit;
					else
						entity = null;
				}
				if ((entity != null) && IsQuestionWord(entity))
					result = entity;
			}
			return result;
		}

		/// <summary>
		/// Проверка, что заданная сущность - вопросительное слово
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsQuestionWord(Entity entity)
		{
			return entity.IsType(EntityType.Conjunction) && QuestionWords.Contains(entity.Value.ToLower());
		}
	}
}
