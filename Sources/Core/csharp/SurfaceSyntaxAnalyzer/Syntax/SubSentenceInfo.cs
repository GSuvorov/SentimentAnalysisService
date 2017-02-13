using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Информация по подпредложению
	/// </summary>
	internal class SubSentenceInfo
	{
		/// <summary>
		/// Подпредложение
		/// </summary>
		public SubSentence SubSentence { get; set; }
		/// <summary>
		/// Первый юнит
		/// </summary>
		public UnitTextBase StartUnit { get; set; }

		/// <summary>
		/// Флаг наличия союза
		/// </summary>
		public bool IsContainConjunction { get; set; }

		/// <summary>
		/// Список причастий
		/// </summary>
		public List<Entity> Participles { get; set; }
		/// <summary>
		/// Список глаголов и деепричастий
		/// </summary>
		public List<Entity> Verbs { get; set; }

		/// <summary>
		/// Информация о родительском подпредложении
		/// </summary>
		public SubSentenceInfo Parent { get; private set; }
		/// <summary>
		/// Список информации о дочерних подпредложениях
		/// </summary>
		public List<SubSentenceInfo> Childs { get; private set; }

		#region [.ctor().]
		public SubSentenceInfo(int startPosition)
		{
			Initialize(startPosition);
			StartUnit = null;
		}

		public SubSentenceInfo(UnitTextBase startUnit, int startPosition)
		{
			Initialize(startPosition);
			StartUnit = startUnit;
		}

		private void Initialize(int startPosition)
		{
			SubSentence = new SubSentence(SubSentenceType.Default, startPosition);
			Participles = new List<Entity>();
			Verbs = new List<Entity>();
			Childs = new List<SubSentenceInfo>();
		}
		#endregion

		/// <summary>
		/// Добавление информации о дочернем подпредложении
		/// </summary>
		/// <param name="child">информация о дочернем подпредложении</param>
		public void AddChild(SubSentenceInfo child)
		{
			Childs.Add(child);
			child.Parent = this;
		}

		/// <summary>
		/// Проверка, что в данном подпредложении есть субъект
		/// </summary>
		/// <returns></returns>
		public bool IsExistSubject()
		{
			return SubSentence.Subject != null;
		}

		/// <summary>
		/// Проверка, что в данном подпредложении есть глагол или деепричастие
		/// </summary>
		/// <returns></returns>
		public bool IsExistVerb()
		{
			return Verbs.Any();
		}

		/// <summary>
		/// Проверка, что в данном подпредложении есть лингвистическая сущность
		/// </summary>
		/// <returns></returns>
		public bool IsHasLingvisticEntity()
		{
			return SubSentence.Units.Any(_ => _.UnitTextType == UnitTextType.ENTITY);
		}

		/// <summary>
		/// Присоединение заданной информации о подпредложении
		/// </summary>
		/// <param name="subSentenceInfo">информация о подпредложении</param>
		public void Append(SubSentenceInfo subSentenceInfo)
		{
			Participles.AddRange(subSentenceInfo.Participles);
			Verbs.AddRange(subSentenceInfo.Verbs);
			AddChildrenSubSentences(subSentenceInfo.Childs);

			SubSentence.SetUnits(SubSentence.Units.Union(subSentenceInfo.SubSentence.Units));
			SubSentence.AppendSubSentences(subSentenceInfo.SubSentence.Children);
		}

		/// <summary>
		/// Добавление коллекции информации о дочерних подпредложениях
		/// </summary>
		/// <param name="childs">коллекция информации о дочерних подпредложениях</param>
		private void AddChildrenSubSentences(IEnumerable<SubSentenceInfo> childs)
		{
			foreach (var child in childs)
				AddChild(child);
		}
	}
}
