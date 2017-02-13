using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Предложение
	/// </summary>
    [Serializable]
	public class Sentence : ISerializerToRDF
	{
		/// <summary>
		/// Подпредложения в виде списка
		/// </summary>
		/// должен быть всегда заполнен
		public SubSentence[] SubsentsFlatten { get; private set; }

		/// <summary>
		/// Подпредложения в виде дерева
		/// </summary>
		/// должен быть всегда заполнен
		public SubSentence[] SubsentsHierarchical { get; private set; }

		/// <summary>
		/// Начальная позиция в тексте
		/// </summary>
		public int StartPosition { get; protected set; }

		/// <summary>
		/// Язык
		/// </summary>
		public string Language { get; set; }

		#region [.ctor().]
		public Sentence(int startPosition)
		{
			StartPosition = startPosition;
		}
		#endregion

		/// <summary>
		/// Задание иерархии подпредложений
		/// </summary>
		/// <param name="hierarchy">иерархия подпредложений</param>
		public void SetSubSentencesHeirarchy(SubSentence[] hierarchy)
		{
			SubsentsHierarchical = hierarchy;
			SubsentsFlatten = GetFlattenSubSentences();
			SubsentsFlatten.SetRelations(this);
		}

		/// <summary>
		/// Получение коллекции всех подпредложений из иерархии в порядке следования
		/// </summary>
		/// <returns>коллекция подпредложений</returns>
		private SubSentence[] GetFlattenSubSentences()
		{
			List<SubSentence> result = new List<SubSentence>();
			foreach (var subSentence in SubsentsHierarchical)
				GetFlattenSubSentences(subSentence, ref result);
			return result.ToArray();
		}

		/// <summary>
		/// Пополнение заданной коллекции заданным подпредложением и всеми его подпредложениями-потомками
		/// </summary>
		/// <param name="parentSubSentence">заданное подпредложение</param>
		/// <param name="result">пополняемая коллекция</param>
		private void GetFlattenSubSentences(SubSentence parentSubSentence, ref List<SubSentence> result)
		{
			result.Add(parentSubSentence);
			if (parentSubSentence.SubTextInfo == null)
			{
				var children = ((SubSentence)parentSubSentence).Children;
				if (children == null)
					return;
				foreach (var subSentence in children)
					GetFlattenSubSentences(subSentence, ref result);
			}
		}

		#region [ISerializeRDF]
		public XElement ToXElement()
		{
			XElement result = new XElement(UnitTextType.SENT.ToString());
			result.SetAttributeValue(RDF.Attribute.StartPosition, StartPosition);
			result.SetAttributeValue(RDF.Attribute.Language, Language);
			foreach (var subSentence in SubsentsHierarchical)
				result.Add(subSentence.ToXElement());
			return result;
		}
		#endregion
	}
}
