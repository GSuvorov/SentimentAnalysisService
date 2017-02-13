using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.Text.RegularExpressions;
using Linguistics.Core.Morpho;

namespace Linguistics.Core
{
	public class SerializerFromRDF
	{
		/// <summary>
		/// регулярное выражение для получения значения из CDATA
		/// </summary>
		private static Regex _cdataRegex;

		#region [.ctor().]
		static SerializerFromRDF()
		{
			string regex = "(?<value>([\r\n]|.)*)";
			string cdata = (new XCData("0")).ToString();
			char[] chars = new char[] { '[', ']' };
			for (int index = cdata.IndexOfAny(chars); index >= 0; index = cdata.IndexOfAny(chars, index + 2))
				cdata = cdata.Insert(index, "\\");
			cdata = cdata.Replace("0", regex);
			_cdataRegex = new Regex(cdata);
		}
		#endregion

		/// <summary>
		/// Текущая позиция в предложении
		/// </summary>
		private static int _positionInSentence;
		/// <summary>
		/// Сущности предложения
		/// </summary>
		private static Dictionary<string, Entity> _sentenceEntities;

		/// <summary>
		/// Создание текста из RDF
		/// </summary>
		/// <param name="element">RDF</param>
		/// <returns>текст</returns>
		public static IText TextFromXElement(XElement element)
		{
			Text text = new Text();
			text.Sentences = element.Elements(UnitTextType.SENT.ToString()).Select(_ => SentenceFromXElement(_)).ToArray();
			return text;
		}

		/// <summary>
		/// Создание предложения из xml-элемента
		/// </summary>
		/// <param name="sentence">xml-элемент</param>
		/// <returns>предложение</returns>
		private static Sentence SentenceFromXElement(XElement sentence)
		{
			#region [.defense.]
			if (!sentence.IsName(UnitTextType.SENT))
				throw new ArgumentException("element isn't SENT");
			#endregion
			_positionInSentence = 0;
			_sentenceEntities = new Dictionary<string,Entity>();
			Sentence result = new Sentence(sentence.GetStartPosition());
			if (sentence.IsContainAttribute(RDF.Attribute.Language))
				result.Language = sentence.GetAttributeValue(RDF.Attribute.Language);
			result.SetSubSentencesHeirarchy(sentence.Elements().Select(_ => SubSentenceFromXElement(_, result.Language)).ToArray());
			return result;
		}

		/// <summary>
		/// Создание подпредложения из xml-элемента
		/// </summary>
		/// <param name="subSentence">xml-элемент</param>
		/// <param name="language">язык предложения</param>
		/// <returns>подпредложение</returns>
		private static SubSentence SubSentenceFromXElement(XElement subSentence, string language)
		{
			#region [.defense.]
			if (!subSentence.IsName(UnitTextType.SUB_SENT))
				throw new ArgumentException("element isn't SUB_SENT");
			#endregion
			SubSentenceType type = (SubSentenceType)Enum.Parse(typeof(SubSentenceType), subSentence.GetAttributeValue(RDF.Attribute.Type));
			SubSentence result = new SubSentence(type, _positionInSentence);

			switch (type)
			{
				case SubSentenceType.AdjunctPhrase:
				case SubSentenceType.DanglingParticiple:
				case SubSentenceType.Default:
				case SubSentenceType.Introductory:
				case SubSentenceType.Participial:
				case SubSentenceType.Subordinate:
					SimpleSubSentenceFromXElement(result, subSentence, language);
					break;
				case SubSentenceType.ApproximatingForceps:
				case SubSentenceType.DirectSpeech:
					SubTextFromXElement(result, subSentence, language);
					break;
				#region [.defense.]
				default:
					throw new InvalidOperationException("Unknown subSentence type");
				#endregion
			}
			return result;
		}

		/// <summary>
		/// Создание подтекста из xml-элемента
		/// </summary>
		/// <param name="subSentence">родительское подпредложение</param>
		/// <param name="subSentenceXML">xml-элемент подтекста</param>
		/// <param name="language">язык предложения</param>
		private static void SubTextFromXElement(SubSentence subSentence, XElement subSentenceXML, string language)
		{
			int position = _positionInSentence;
			var entities = _sentenceEntities;

			SubTextInfo subText = new SubTextInfo(
				GetSymbolFromXElement(subSentenceXML.Elements().First()),
				GetSymbolFromXElement(subSentenceXML.Elements().Last()));
			subText.Sentences = TextFromXElement(subSentenceXML).Sentences;
			subSentence.SubTextInfo = subText;

			_sentenceEntities = entities;
			_positionInSentence = position + GetFullValue(subSentenceXML).Length;
		}

		/// <summary>
		/// Получение символа из xml-элемента
		/// </summary>
		/// <param name="element">xml-элемент</param>
		/// <returns>символ</returns>
		private static char GetSymbolFromXElement(XElement element)
		{
			if (element.IsName(UnitTextType.U))
			{
				string value = GetFullValue(element);
				#region [.defense.]
				if (value.Length > 1)
					throw new InvalidOperationException("Very long subText symbol");
				#endregion
				return value.First();
			}
			return default(char);
		}

		/// <summary>
		/// Создание простого подпредложения из xml-элемента
		/// </summary>
		/// <param name="subSentence">xml-элемент</param>
		/// <param name="language">язык предложения</param>
		/// <returns>простое подпредложение</returns>
		private static void SimpleSubSentenceFromXElement(SubSentence subSentence, XElement subSentenceXML, string language)
		{
			List<UnitTextBase> units = new List<UnitTextBase>();
			List<SubSentence> subSentences = new List<SubSentence>();
			foreach (XElement element in subSentenceXML.Elements())
			{
				if (element.IsName(UnitTextType.U))
					units.Add(UnmarkedFromXElement(element));
				else if (element.IsName(UnitTextType.SUB_SENT))
					subSentences.Add(SubSentenceFromXElement(element, language));
				else if (IsEntity(element))
					units.Add(EntityFromXElement(element, language, subSentence.Type));
				#region [.defense.]
				else
					throw new InvalidOperationException("unknown xml element");
				#endregion
			}
			if (subSentenceXML.IsContainAttribute(RDF.Attribute.Subject))
				subSentence.Subject = _sentenceEntities[subSentenceXML.GetAttributeValue(RDF.Attribute.Subject)];
            if (subSentenceXML.IsContainAttribute(RDF.Attribute.IsPassiveSubject))
                subSentence.IsPassiveSubject = bool.Parse(subSentenceXML.GetAttributeValue(RDF.Attribute.IsPassiveSubject));
			subSentence.AppendSubSentences(subSentences);
			subSentence.SetUnits(units);
		}

		/// <summary>
		/// Создание неразмеченного текста из xml-элемента
		/// </summary>
		/// <param name="entity">xml-элемент</param>
		/// <returns>неразмеченный текст</returns>
		private static UnmarkedText UnmarkedFromXElement(XElement unmarked)
		{
			string text = GetFullValue(unmarked);
			UnmarkedText result = new UnmarkedText(text, _positionInSentence);
			_positionInSentence += text.Length;
			return result;
		}

		/// <summary>
		/// Создание сущности из xml-элемента
		/// </summary>
		/// <param name="entity">xml-элемент</param>
		/// <param name="language">язык предложения</param>
		/// <param name="subSentenceType">тип подпредложения</param>
		/// <returns>сущность</returns>
		private static Entity EntityFromXElement(XElement entity, string language, SubSentenceType subSentenceType)
		{
			TextEntity textEntity = new TextEntity();
			textEntity.Type = entity.GetAttributeValue(RDF.Attribute.Type);
			textEntity.Value = entity.GetAttributeValue(RDF.Attribute.Value);
			textEntity.Text = GetFullValue(entity);
			textEntity.Language = language;
			textEntity.Start = _positionInSentence;

			textEntity.IsPseudoPlace = entity.IsContainAttribute(RDF.Attribute.PseudoPlace);
			if (entity.IsContainAttribute(RDF.Attribute.FirstName))
				textEntity.FirstName = entity.GetAttributeValue(RDF.Attribute.FirstName);
			if (entity.IsContainAttribute(RDF.Attribute.MiddleName))
				textEntity.MiddleName = entity.GetAttributeValue(RDF.Attribute.MiddleName);
			if (entity.IsContainAttribute(RDF.Attribute.LastName))
				textEntity.LastName = entity.GetAttributeValue(RDF.Attribute.LastName);
			if (entity.IsContainAttribute(RDF.Attribute.MorphoInformation))
			{
				string morpho = entity.GetAttributeValue(RDF.Attribute.MorphoInformation);
				if (!string.IsNullOrEmpty(morpho))
					textEntity.MorphoInfo = morpho.Split(RDF.ValueSeparator).ToList();
			}
			if (entity.IsContainAttribute(RDF.Attribute.MainNoun))
				textEntity.MainNoun = entity.GetAttributeValue(RDF.Attribute.MainNoun);
            if (entity.IsContainAttribute(RDF.Attribute.Post))
                textEntity.Post = entity.GetAttributeValue(RDF.Attribute.Post);
			if (entity.IsContainAttribute(RDF.Attribute.Prefix))
				textEntity.MainNounPrefix = entity.GetAttributeValue(RDF.Attribute.Prefix);
			if (entity.IsContainAttribute(RDF.Attribute.Adjectives))
				textEntity.Adjectives = entity.GetAttributeValue(RDF.Attribute.Adjectives).Split(RDF.ValueSeparator).ToList();

			Entity result = textEntity.CreateEntity();
			if (result.IsType(EntityType.Conjunction))
				((ConjunctionEntityInfo)result.EntityInfo).ConjunctionType = subSentenceType;
			if (entity.IsContainAttribute(RDF.Attribute.Role))
				result.Role = (EntityRole)Enum.Parse(typeof(EntityRole), entity.GetAttributeValue(RDF.Attribute.Role));
			foreach (XElement element in entity.Elements())
			{
				if (IsEntity(element))
					result.AddChild(EntityFromXElement(element, language, subSentenceType));
				else
					_positionInSentence += GetFullValue(element).Length;
			}
			_sentenceEntities.Add(entity.GetAttributeValue(RDF.Attribute.ID), result);
			return result;
		}

		/// <summary>
		/// Проверка, что заданный xml-элемент соответствует сущности
		/// </summary>
		/// <param name="element">xml-элемент</param>
		/// <returns>результат проверки</returns>
		private static bool IsEntity(XElement element)
		{
			return element.IsName(UnitTextType.CET) || element.IsName(UnitTextType.VERB) || element.IsName(UnitTextType.ENTITY);
		}

		/// <summary>
		/// Получение полного текстового значения xml-элемента
		/// </summary>
		/// <param name="element">xml-элемент</param>
		/// <returns>полное текстовое значение xml-элемента</returns>
		private static string GetFullValue(XElement element)
		{
			StringBuilder value = new StringBuilder();
			if (element.IsName(UnitTextType.U))
			{
				foreach (XNode node in element.Nodes())
				{
					Match match = _cdataRegex.Match(node.ToString());
					if (match.Success)
						value.Append(match.Groups["value"]);
				}
			}
			else
			{
				foreach (XElement child in element.Elements())
					value.Append(GetFullValue(child));
			}
			return value.ToString();
		}
	}
}
