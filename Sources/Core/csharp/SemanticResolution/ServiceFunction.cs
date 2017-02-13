using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.Xml;
using System.Globalization;
using Linguistics.Core;
using System.Text.RegularExpressions;
using System.Security;
using System.IO;
using System.Net.Sockets;
using System.Net;
using System.Threading;

namespace SemanticResolution
{
    /// <summary>
    /// класс с служебными функциями
    /// </summary>
    public class ServiceFunction
	{
		#region [Переменные]
		/// <summary>
		/// разделитель морфологической информации
		/// </summary>
		public static char MI_Sep
		{
			get { return ','; }
		}

		/// <summary>
		/// союзы, по которым могут быть соединены однородные члены
		/// </summary>
		public static readonly string[] HomogeneousConjunction = new string[] { "и", "или", "да", "либо" };
		#endregion

		#region [Корректировка текста в валидный XML]
		private static string Encoder(Match m)
        {
            string pretext = m.Value.Substring(0, m.Value.IndexOf('"') + 1);
            string str = m.Value.Substring(pretext.Length, m.Value.Length - pretext.Length - 1);
            return pretext + SecurityElement.Escape(str) + "\"";
        }

        private static string CorrectCDATA(Match m)
        {
            string pretext = "<![CDATA[";
            int index = m.Value.LastIndexOf("]") - 1;
            string suffix = m.Value.Substring(index, m.Value.Length - index); 
            string str = m.Value.Substring(pretext.Length, m.Value.Length - pretext.Length - suffix.Length);
            return pretext + SecurityElement.Escape(str) + suffix;
        }

		private static string CorrectCDATA(string xmlText)
		{
			string result = xmlText;
			if (result != null)
			{
				try
				{
					XElement element = XElement.Parse(result);
				}
				catch (Exception)
				{
					Regex structureCorrect = new Regex(@"<!\[CDATA\[[\s\S]{0,}?\]\]><[\/]{0,1}[A-Z]");
					result = structureCorrect.Replace(result, new MatchEvaluator(CorrectCDATA));
				}
			}
			return result;
		}

		private static Regex XML_IllegalCharacters = new Regex("[^\u0009\u000A\u000D\u0020-\uD7FF\uE000-\uFFFD\u10000-\u10FFFF]");
		private static string CorrectIllegalCharacters(string xmlText)
		{
			return XML_IllegalCharacters.Replace(xmlText, " ");
		}

		private static string CorrectAttributeValue(string xmlText, BlockAttribute attribute)
		{
			Regex attributeValue = new Regex(attribute.ToString() + "=\".*?\"");
			return attributeValue.Replace(xmlText, new MatchEvaluator(Encoder));
		}

		private static string CreateEmptyRDF(string text)
		{
			XElement result = new XElement(UnitTextType.TEXT.ToString());
			XElement sentence = new XElement(UnitTextType.SENT.ToString());
			sentence.SetAttributeValue(BlockAttribute.POS, 0);
			sentence.SetAttributeValue(BlockAttribute.LANG, "RU");
			sentence.Add(text.CreateElement_U(0));
			result.Add(sentence);
			return result.ToString();
		}

        /// <summary>
        /// Замена запрещенных символов на их код в xml-тексте
        /// </summary>
        /// <param name="xmlText">xml-текст</param>
        /// <returns>отрекдактированный xml</returns>
        public static string CorrectXML(string xmlText, string sourceText)
        {
            string result = xmlText;
			if (result != null)
			{
				try
				{
					result = CorrectIllegalCharacters(result);
					XElement element = XElement.Parse(result);
					if (!element.Elements().Any())
						result = CreateEmptyRDF(sourceText);
				}
				catch (Exception)
				{
					result = CorrectAttributeValue(result, BlockAttribute.VALUE);
					result = CorrectAttributeValue(result, BlockAttribute.FULLNAME);
					result = CorrectAttributeValue(result, BlockAttribute.INFO);
					result = CorrectAttributeValue(result, BlockAttribute.PRE);
					result = CorrectAttributeValue(result, BlockAttribute.PROF);
					result = CorrectAttributeValue(result, BlockAttribute.FN);
					result = CorrectAttributeValue(result, BlockAttribute.LN);
					result = CorrectAttributeValue(result, BlockAttribute.MN);
				}
				finally
				{
					result = CorrectCDATA(result);
				}
			}
			else
				result = CreateEmptyRDF(sourceText);
			return result;
        }
		#endregion

		#region [Нормализация]
		public static string ReplaceWrongSymbols(string str)
		{
			StringBuilder builder = new StringBuilder();
			foreach (char ch in str)
			{
				if (ch == 'ё')
					builder.Append('е');
				else if (ch == 'Ё')
					builder.Append('Е');
				else
					builder.Append(ch);
			}
			return builder.ToString();
		}

		/// <summary>
		/// Нормализация значения VALUE сущностей
		/// </summary>
		/// <param name="text"></param>
		public static void NormalizeValue(ref XElement xmlText)
		{
			if (xmlText != null)
			{
				foreach (XElement element in xmlText.Descendants())
				{
					if (element.IsContainAttribute(BlockAttribute.VALUE))
					{
						string value = element.Attribute(BlockAttribute.VALUE);
						element.SetAttributeValue(BlockAttribute.VALUE, NormalizeEntityValue(value));
					}
				}
			}
		}
		public static string NormalizeEntityValue(string value)
		{
			string normal = value;
			if (normal.Length >= 3)
			{
                if ((value.First() == '\'') && (value.Last() == '\''))
				/// если сущность в кавычках - уберем их
				{
					normal = value.Substring(1, value.Length - 2);
				}
			}
			return normal;
		}
		#endregion

		#region [Перенумерация ID]
		/// <summary>
		/// идентификатор сущности
		/// </summary>
		private static int _entityID;
		/// <summary>
		/// идентификатор подпредложения
		/// </summary>
		private static int _subSentID;
		/// <summary>
		/// атрибут процесса обработки элемента
		/// </summary>
		private static readonly string _attr = "P";

		/// <summary>
		/// Перенумерация ID
		/// </summary>
		/// <param name="xml"></param>
		public static void RenumberID(ref XElement xml)
		{
			if (xml != null)
			{
				_entityID = 0;
				_subSentID = 0;
				foreach (XElement sentence in xml.Elements(UnitTextType.SENT))
					SentenceRenumberID(sentence);
			}
		}

		/// <summary>
		/// Перенумерация ID в предложении
		/// </summary>
		/// <param name="sentence"></param>
		private static void SentenceRenumberID(XElement sentence, bool isTop = true)
		{
			Dictionary<string, string> renumberDict = new Dictionary<string, string>();
			List<XElement> collection = new List<XElement>();
			foreach (XElement element in sentence.Descendants())
			{
				if (element.IsContainAttribute(_attr))
					continue;
				element.SetAttributeValue(_attr, "");
				if (element.IsContainAttribute(BlockAttribute.SUBJ))
					collection.Add(element);

				if (element.Name.IsEqual(UnitTextType.SENT))
					SentenceRenumberID(element, false);
				else if (element.Name.IsEqual(UnitTextType.SUB_SENT))
					UpdateID(element, ref _subSentID);
				else if (element.Name.IsEqual(UnitTextType.ENTITY) || element.Name.IsEqual(UnitTextType.VERB) || element.Name.IsEqual(UnitTextType.CET))
				{
					XElement parent = element.Parent;
					string id = element.Attribute(BlockAttribute.ID);
					UpdateID(element, ref _entityID);

					/// Возможен случай встречи сущности с одинаковым ID, когда идет пересечение двух сущностей
					/// Сейчас решили в таком случае присваивать различные ID
					if (!(id.IsEmpty() || renumberDict.ContainsKey(id)))
						renumberDict.Add(id, element.Attribute(BlockAttribute.ID));
				}
			}
			
			foreach (var element in collection)
			/// обновление ссылок
			{
				string id;
				if (renumberDict.TryGetValue(element.Attribute(BlockAttribute.SUBJ), out id))
					element.SetSubjectID(id);
			}
			if (isTop)
			/// удаление атрибутов процесса обработки
			{
				foreach (XElement element in sentence.Descendants())
					element.DeleteAttribute(_attr);
			}
		}

		/// <summary>
		/// Обновление id
		/// </summary>
		/// <param name="element">элемент</param>
		/// <param name="id">идентификатор</param>
		private static void UpdateID(XElement element, ref int id)
		{
			if (element != null)
			{
				element.SetID(id.ToString());
				++id;
			}
		}
		#endregion
	}
}
