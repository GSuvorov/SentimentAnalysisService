using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Текст
	/// </summary>
    [Serializable]
	public class Text : IText
	{
		/// <summary>
		/// Дочерние предложения
		/// </summary>
		/// должен быть всегда заполнен
		public Sentence[] Sentences { get; set; }

		#region [ISerializeRDF]
		public XElement ToXElement()
		{
			IdGenerator.ReleaseID();
			IdentifyEntities(this);

			XElement result = new XElement(UnitTextType.TEXT.ToString());
			foreach (var sentence in Sentences)
				result.Add(sentence.ToXElement());
			AddSyntaxChildren(result);
			return result;
		}

		#region [.entities identification.]
		/// <summary>
		/// Идентификация всех сущностей в тексте
		/// </summary>
		/// <param name="text">текст</param>
		private void IdentifyEntities(IText text)
		{
			foreach (var sentence in text.Sentences)
				IdentifyEntities(sentence);
		}

		/// <summary>
		/// Идентификация всех сущностей в предложении
		/// </summary>
		/// <param name="sentence">предложение</param>
		private void IdentifyEntities(Sentence sentence)
		{
			foreach (var subSentence in sentence.SubsentsHierarchical)
				IdentifyEntities(subSentence);
		}

		/// <summary>
		/// Идентификация всех сущностей в подпредложении
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		private void IdentifyEntities(SubSentence subSentence)
		{
			if (subSentence.SubTextInfo != null)
				IdentifyEntities(subSentence.SubTextInfo);
			else
			{
				int childIndex = 0;
				foreach (var unit in subSentence.Units)
				{
					while ((childIndex < subSentence.Children.Length) &&
						(subSentence.Children[childIndex].StartPosition < unit.PositionInfo.Start))
					{
						IdentifyEntities(subSentence.Children[childIndex]);
						++childIndex;
					}
					if (unit.IsEntity)
						IdentifyEntity((Entity)unit);
				}
				for (; childIndex < subSentence.Children.Length; ++childIndex)
					IdentifyEntities(subSentence.Children[childIndex]);
			}
		}

		/// <summary>
		/// Идентификация заданной сущности
		/// </summary>
		/// <param name="entity">сущность</param>
		private void IdentifyEntity(Entity entity)
		{
			if (entity != null)
			{
				IdGenerator.GetID(entity);
				foreach (Entity child in entity.Children)
					IdentifyEntity(child);
			}
		}
		#endregion

		#region [.syntax children.]
		/// <summary>
		/// Добавление синтаксически дочерних сущностей
		/// </summary>
		/// <param name="parent"></param>
		private void AddSyntaxChildren(XElement parent)
		{
			foreach (var entity in IdGenerator.GetAllEntities())
			{
				if (entity.SyntaxChildren == null)
					continue;
				XElement theme = new XElement(RDF.Attribute.Themes);
				theme.SetAttributeValue(RDF.Attribute.ID, IdGenerator.GetID(entity));
				foreach (var child in entity.SyntaxChildren)
					theme.Add(child.ToXElement());
				parent.Add(theme);
			}
		}
		#endregion
		#endregion
	}
}
