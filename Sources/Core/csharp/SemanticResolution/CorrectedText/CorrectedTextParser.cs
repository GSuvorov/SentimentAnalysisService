using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Linguistics.Core;

namespace SemanticResolution
{
	/// <summary>
	/// Класс по работе с скорректированным текстом
	/// </summary>
	public class CorrectedTextParser
	{
		/// <summary>
		/// Теги скорректированного текста
		/// </summary>
		public enum Tags
		{
			/// <summary>
			/// перенос строки
			/// </summary>
			n,
			/// <summary>
			/// исправленный текст
			/// </summary>
			e,
			/// <summary>
			/// правильный текст
			/// </summary>
			p,
			/// <summary>
			/// целый текст
			/// </summary>
			text
		}

		/// <summary>
		/// Атрибуты у скорректированного слова
		/// </summary>
		public enum ErrorAttributes
		{
			/// <summary>
			/// исходное слово
			/// </summary>
			ow,
			/// <summary>
			/// исходное слово, "очищенное" от шумовых символов
			/// </summary>
			owc,
			/// <summary>
			/// скорректированное слово
			/// </summary>
			sw
		}

		/// <summary>
		/// смещения исправленных слов
		/// </summary>
		public static List<int> CorrectedWordsShift { get; set; }
		/// <summary>
		/// узлы исправленных слов
		/// синхронизирован со словарем CorrectedWordsShift
		/// </summary>
		private static List<XElement> _correctedWordsNodes;
		/// <summary>
		/// индекс текущего элемента коллекций
		/// </summary>
		private static int _correctedIndex;
		/// <summary>
		/// текущее смещение
		/// </summary>
		private static int _currentCorrectedShift;
		/// <summary>
		/// текущее смещение предложения
		/// </summary>
		private static int _currentSentenceShift;

		static CorrectedTextParser()
		{
			CorrectedWordsShift = new List<int>();
			_correctedWordsNodes = new List<XElement>();
		}

		/// <summary>
		/// Получение скорректированного текста с сохранением исправлений
		/// </summary>
		/// <param name="xmlText">исходный xml-текст</param>
		/// <returns>скорректированный текст</returns>
		public static string GetCorrectedText(string xmlText)
		{
			CorrectedWordsShift.Clear();
			_correctedWordsNodes.Clear();

			StringBuilder builder = new StringBuilder();
			XElement xml = XElement.Parse(xmlText);
			foreach (XElement element in xml.Elements())
			{
				if (element.Name.IsEqual(Tags.n))
					builder.Append('\n');
				else
				{
					if (element.Name.IsEqual(Tags.e) && element.HasAttributes)
					{
						CorrectedWordsShift.Add(builder.Length);
						_correctedWordsNodes.Add(element);
					}
					builder.Append(element.Value);
				}
			}
			return builder.ToString();
		}

		/// <summary>
		/// Добавление атрибутов об исправлениях
		/// </summary>
		/// <param name="xmlText"></param>
		public static void AddCorrectedInformation(ref XElement xmlText)
		{
			if (CorrectedWordsShift.Any() && (xmlText != null))
			{
				UpdateCurrentShift(0);
				UpdateNode(xmlText);
			}
		}

		/// <summary>
		/// Обновление текущего смещения
		/// </summary>
		/// <param name="index"></param>
		private static void UpdateCurrentShift(int index)
		{
			_correctedIndex = index;
			if (_correctedIndex < CorrectedWordsShift.Count)
				_currentCorrectedShift = CorrectedWordsShift[_correctedIndex];
		}

		/// <summary>
		/// Анализ узла и всех его детей
		/// </summary>
		/// <param name="element">узел</param>
		private static void UpdateNode(XElement element)
		{
			bool isDirectSpeechSentence = false;
			int tempShift = 0;
			if (element.Name.IsEqual(UnitTextType.SENT) &&
				(element.Parent != null) && element.Parent.Name.IsEqual(UnitTextType.SUB_SENT) &&
				element.Parent.Attribute(BlockAttribute.TYPE).IsEqual(SubSentenceType.DirectSpeech))
			{
				isDirectSpeechSentence = true;
				tempShift = _currentSentenceShift;
				_currentSentenceShift += element.Parent.GetPosition();
			}
			if (UpdateCurrentNode(element, isDirectSpeechSentence))
			{
				foreach (XElement child in element.Elements())
				{
					UpdateNode(child);
					if (_correctedIndex >= CorrectedWordsShift.Count)
						break;
				}
			}
			if (isDirectSpeechSentence)
				_currentSentenceShift = tempShift;
		}

		/// <summary>
		/// Анализ текущего узла
		/// </summary>
		/// <param name="element">узел</param>
		/// <returns>флаг, следует ли обрабатывать детей</returns>
		private static bool UpdateCurrentNode(XElement element, bool isDirectSpeechSent)
		{
			if (_correctedIndex >= CorrectedWordsShift.Count)
				return false;
			else if (element.Name.IsEqual(UnitTextType.SENT))
			{
				int left = element.GetPosition();
				int right = element.GetRightBorder();
				if (isDirectSpeechSent)
				{
					left += _currentSentenceShift;
					right += _currentSentenceShift;
				}
				if ((left <= CorrectedWordsShift[_correctedIndex]) && (right >= CorrectedWordsShift[_correctedIndex]))
				{
					_currentSentenceShift = left;
				}
				else
					return false;
			}
			else if (!element.Name.IsEqual(UnitTextType.SUB_SENT) &&
				!element.Name.IsEqual(UnitTextType.TEXT) && !element.Name.IsEqual(UnitTextType.THEME))
			{
				int left = element.GetPosition();
				int right = element.GetRightBorder();
				if (left < 0)
				{
					left = GetPositionByTextAnalyzing(element);
					right += left;
				}
				int shift = _currentCorrectedShift - _currentSentenceShift;
				if ((left >= 0) && (right >= 0) && (left <= shift) && (right >= shift))
				{
					XElement corrected = _correctedWordsNodes[_correctedIndex];
					int suffix = 1;
					foreach (XAttribute attr in corrected.Attributes())
						AddAttribute(element, attr, ref suffix);
					UpdateCurrentShift(_correctedIndex + 1);
					return UpdateCurrentNode(element, isDirectSpeechSent);
				}
			}
			return true;
		}

		/// <summary>
		/// Получение позиции xml-узла в тексте
		/// </summary>
		/// <param name="element">узел</param>
		/// <returns>позиция</returns>
		private static int GetPositionByTextAnalyzing(XElement element)
		{
			int position = 0;
			if (element != null)
			{
				XElement last = element.ElementsBeforeSelf().LastOrDefault(_ => _.IsContainAttribute(BlockAttribute.POS));
				if (last != null)
				{
					position = last.GetRightBorder() + 1;
					foreach (XElement elem in last.ElementsAfterSelf())
					{
						if (elem == element)
							break;
						position += elem.GetRightBorder() + 1;
					}
				}
				else if (element.Parent != null)
				{
					if (element.Parent.Name.IsEqual(UnitTextType.SENT))
						position = 0;
					else if (element.Parent.IsContainAttribute(BlockAttribute.POS))
						position = element.Parent.GetPosition();
					else
						position = GetPositionByTextAnalyzing(element.Parent);
				}
				else
					position = 0;
			}
			return position;
		}

		/// <summary>
		/// Добавление атрибута в xml-узел.
		/// Если атрибут уже существует, то находится суффикс, с которым атрибута еще нет
		/// </summary>
		/// <param name="element">узел</param>
		/// <param name="attribute">аттрибут</param>
		/// <param name="suffix">суффикс</param>
		private static void AddAttribute(XElement element, XAttribute attribute, ref int suffix)
		{
			string name = string.Format("{0}-{1}", attribute.Name, suffix);
			if (element.IsContainAttribute(name))
			{
				++suffix;
				AddAttribute(element, attribute, ref suffix);
			}
			else
				element.SetAttributeValue(name, attribute.Value);
		}
	}
}
