using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Linguistics.Core;
using System.Text.RegularExpressions;
using System.Globalization;
using System.Security;
using Linguistics.Core.Morpho;

namespace SemanticResolution
{
    /// <summary>
    /// класс расширений
    /// </summary>
    internal static class Extensions
    {
		private static Regex _cdataRegex;
		static Extensions()
		{
			string regex = "(?<value>([\r\n]|.)*)";
			string cdata = (new XCData("0")).ToString();
			char[] chars = new char[] { '[', ']' };
			for (int index = cdata.IndexOfAny(chars); index >= 0; index = cdata.IndexOfAny(chars, index + 2))
				cdata = cdata.Insert(index, "\\");
			cdata = cdata.Replace("0", regex);
			_cdataRegex = new Regex(cdata);
		}

		#region [string]
		public static bool IsEqual(this string str, object value)
        {
            return (value != null) && (str == value.ToString());
        }

        public static bool IsEmpty(this string str)
        {
            return string.IsNullOrEmpty(str);
        }

		/// <summary>
		/// Создать элемент U
		/// </summary>
		/// <returns></returns>
		public static XElement CreateElement_U(this string text, int start, int end)
		{
			return new XElement(UnitTextType.U.ToString(), text.Substring(start, end - start).CreateElementValue());
		}

		public static XElement CreateElement_U(this string text, int start)
		{
			return text.CreateElement_U(start, text.Length);
		}

		/// <summary>
		/// Создать значение элемента
		/// </summary>
		/// <param name="text"></param>
		/// <returns></returns>
		public static XCData CreateElementValue(this string text)
		{
			//return new XCData(SecurityElement.Escape(text));
			return new XCData(text);
		}
        #endregion

		#region [StringBuilder]
		/// <summary>
		/// Добавляет строку, в которой удалены повторяющиеся пробелы
		/// </summary>
		/// <param name="builder"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		public static StringBuilder AppendSimple(this StringBuilder builder, string value)
		{
			StringBuilder newString = new StringBuilder();
			foreach (string subLine in value.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries))
			{
				if (builder.Length != 0)
					builder.Append(" ");
				builder.Append(subLine);
			}
			return builder;
		}

		public static bool IsEmpty(this StringBuilder builder)
		{
			return (builder == null) || (builder.Length == 0);
		}
		#endregion

		#region [char]
		/// <summary>
        /// Проверяет совпадение первых букв
        /// </summary>
        /// <param name="str"></param>
        /// <param name="caseType"></param>
        /// <returns></returns>
        public static bool IsEqualCode(this char symbol, object value)
        {
            return symbol == value.ToString()[0];
        }

		/// <summary>
		/// Проверяет совпадение символа с одним из указанного массива
		/// </summary>
		/// <param name="symbol"></param>
		/// <param name="anyOf"></param>
		/// <returns></returns>
		public static bool EqualsAny(this char symbol, char[] anyOf)
		{
			foreach (char value in anyOf)
				if (symbol.Equals(value))
					return true;
			return false;
		}

		/// <summary>
		/// Проверяет символ на один из вариантов представления тире
		/// </summary>
		/// <param name="ch"></param>
		/// <returns></returns>
		public static bool IsTire(this char ch)
		{
			return ch.EqualsAny(new char[] {'-', '–', '—'});
		}

		/// <summary>
		/// Проверяет символ на совпадение с умолчательным
		/// </summary>
		/// <param name="ch">символ</param>
		/// <returns>результат проверки</returns>
		public static bool IsDefault(this char ch)
		{
			return ch == default(char);
		}
        #endregion

        #region [XName]

        public static bool IsEqual(this XName str, object value)
        {
            return str.ToString().IsEqual(value);
        }

        #endregion

        #region [XElement]

		public static string GetFullValue(this XElement element)
		{
			StringBuilder value = new StringBuilder();
			if (element.Name.IsEqual(UnitTextType.U))
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
					value.Append(child.GetFullValue());
			}
			return value.ToString();
		}

        /// <summary>
        /// Получить значение атрибута
        /// </summary>
        /// <param name="element"></param>
        /// <param name="attribute">имя атрибута</param>
        /// <returns></returns>
        public static string Attribute(this XElement element, object attribute)
        {
			string value = "";
			if (element != null)
			{
				string attributeName = attribute.ToString();
				if (element.Attribute(attributeName) != null)
					value = element.Attribute(attributeName).Value;
				else if (attributeName.IsEqual(BlockAttribute.PROF) || attributeName.IsEqual(BlockAttribute.FULLNAME))
				{
					if (element.IsContainAttribute(BlockAttribute.VALUE_THEME))
						value = element.Attribute(BlockAttribute.VALUE_THEME.ToString()).Value;
					else
						value = element.Attribute(BlockAttribute.VALUE.ToString()).Value;
				}
			}
			return value.Trim();
        }

		/// <summary>
		/// Получить множество морфохарактеристик
		/// </summary>
		/// <param name="element"></param>
		/// <returns></returns>
		public static string[] GetMorphoInfo(this XElement element)
		{
			return element.Attribute(BlockAttribute.MI).Split(ServiceFunction.MI_Sep);
		}

		/// <summary>
		/// задание значения атрибута
		/// </summary>
		/// <param name="element"></param>
		/// <param name="attributeName"></param>
		/// <param name="value"></param>
		public static void SetAttributeValue(this XElement element, object attributeName, object value)
		{
			element.SetAttributeValue(attributeName.ToString(), value);
		}

		/// <summary>
		/// Задание значения атрибута POS
		/// </summary>
		/// <param name="element"></param>
		/// <param name="position"></param>
		public static void SetPosition(this XElement element, int position)
		{
			element.SetAttributeValue(BlockAttribute.POS.ToString(), position);
		}

		/// <summary>
		/// Обновление значения атрибута POS
		/// </summary>
		/// <param name="element"></param>
		/// <param name="shift">сдвиг</param>
		public static void UpdatePosition(this XElement element, int shift)
		{
			if (element.IsContainAttribute(BlockAttribute.POS))
				element.SetAttributeValue(BlockAttribute.POS.ToString(), element.GetPosition() + shift);
		}

		/// <summary>
		/// Получить значение атрибута в верхнем регистре
		/// </summary>
		/// <param name="element"></param>
		/// <param name="attribute">имя атрибута</param>
		/// <returns></returns>
		public static string AttributeUpper(this XElement element, object attribute)
		{
			return element.Attribute(attribute).ToUpper(CultureInfo.InvariantCulture);
		}

        /// <summary>
        /// Получение дочерних элементов, удовлетворяющих фильтру
        /// </summary>
        /// <param name="element"></param>
        /// <param name="name">имя узла</param>
        /// <returns></returns>
        public static IEnumerable<XElement> Elements(this XElement element, UnitTextType name)
        {
            return element.Elements(name.ToString());
        }

        /// <summary>
        /// Проверяет наличие атрибута
        /// </summary>
        /// <param name="element"></param>
        /// <param name="attribute">имя атрибута</param>
        /// <returns></returns>
        public static bool IsContainAttribute(this XElement element, BlockAttribute attribute)
        {
			return element.IsContainAttribute(attribute.ToString());
        }
		public static bool IsContainAttribute(this XElement element, string attribute)
		{
			bool contain = false;
			if (element != null)
				contain = (element.Attribute(attribute) != null);
			return contain;
		}

        /// <summary>
        /// Удаляет атрибут
        /// </summary>
        /// <param name="element"></param>
        /// <param name="attribute">имя атрибута</param>
        public static void DeleteAttribute(this XElement element, object attribute)
        {
			element.SetAttributeValue(attribute.ToString(), null);
        }

		/// <summary>
		/// Возвращает значение атрибута POS. Если его нет, то -1.
		/// </summary>
		/// <param name="element"></param>
		/// <returns></returns>
		public static int GetPosition(this XElement element)
		{
			return element.IsContainAttribute(BlockAttribute.POS) ? int.Parse(element.Attribute(BlockAttribute.POS)) : -1;
		}

		/// <summary>
		/// Возвращает правую границу элемента
		/// </summary>
		/// <param name="element"></param>
		/// <returns></returns>
		public static int GetRightBorder(this XElement element)
		{
			int border = element.GetPosition();
			if (border < 0)
				border = 0;
			XElement lastWithPos = element.Elements().LastOrDefault(_ => _.IsContainAttribute(BlockAttribute.POS));
			if (lastWithPos != null)
			{
				if (!element.Name.IsEqual(UnitTextType.SENT))
					border = 0;
				border += GetRightBorder(lastWithPos) + 1;
				foreach (XElement elem in lastWithPos.ElementsAfterSelf())
					border += elem.GetFullValue().Length;
			}
			else
				border += element.GetFullValue().Length;
			return border - 1;
		}

        /// <summary>
        /// Задать значение атрибута "ROLE"
        /// </summary>
        /// <param name="element"></param>
        /// <param name="role">роль</param>
        public static void SetRole(this XElement element, object role)
        {
            element.SetAttributeValue(BlockAttribute.ROLE.ToString(), role.ToString());
        }

        /// <summary>
        /// Задать значение атрибута "TYPE"
        /// </summary>
        /// <param name="element"></param>
        /// <param name="type">тип</param>
        public static void SetType(this XElement element, object type)
        {
            element.SetAttributeValue(BlockAttribute.TYPE.ToString(), type.ToString());
        }

        /// <summary>
        /// Задает значение атрибута "SUBJ"
        /// </summary>
        /// <param name="element"></param>
        /// <param name="id"></param>
        public static void SetSubjectID(this XElement element, string id)
        {
            element.SetAttributeValue(BlockAttribute.SUBJ.ToString(), id);
        }

        /// <summary>
        /// Задает значение атрибута "ID"
        /// </summary>
        /// <param name="element"></param>
        /// <param name="id"></param>
        public static void SetID(this XElement element, string id)
        {
            element.SetAttributeValue(BlockAttribute.ID.ToString(), id);
        }

		/// <summary>
		/// Установка ссылки на сущность
		/// </summary>
		/// <param name="element"></param>
		/// <param name="link"></param>
		public static void SetLinkToElement(this XElement element, XElement link)
		{
			element.SetAttributeValue(BlockAttribute.LINK.ToString(),
				link.Attribute(BlockAttribute.ID));
			element.SetAttributeValue(BlockAttribute.FULLNAME.ToString(),
				link.Attribute(BlockAttribute.FULLNAME));
		}

		/// <summary>
		/// Проверяет согласованность глагола с существительным
		/// </summary>
		/// <param name="verb">глагол</param>
		/// <param name="noun">существительное</param>
		/// <returns></returns>
		public static bool IsConsistWithNoun(this XElement verb, XElement noun)
		{
			if (!verb.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Verb))
				return false;

			var nounInfo =
				(from info in noun.Attribute(BlockAttribute.MI).Split(ServiceFunction.MI_Sep)
				 where !info.IsEmpty()
				 select new { _case = info[0], _gender = info[1], _number = info[2] }).ToArray();
			var verbInfo =
				(from info in verb.Attribute(BlockAttribute.MI).Split(ServiceFunction.MI_Sep)
				 select new { _case = info[0], _gender = info[1], _number = info[2] }).ToArray();

			var subject =
				from nInfo in nounInfo
				from vInfo in verbInfo
				where
				(
				/// единственное число
				nInfo._number.IsEqualCode(Number.Single) &&
				vInfo._number.IsEqualCode(Number.Single) &&
					/// согласованность рода
					((nInfo._gender == vInfo._gender) ||
					nInfo._gender.IsEqualCode(Gender.Undefined) ||
					vInfo._gender.IsEqualCode(Gender.Undefined))
				) ||
				(
				/// множественное число
				vInfo._number.IsEqualCode(Number.Plural) ||
				vInfo._number.IsEqualCode(Number.Undefined)
				)
				select nInfo;

			return subject.Any();
		}

        #endregion

		#region [IEnumareble<XElement>]
		/// <summary>
		/// Получение последней цепочки сущности в коллекции
		/// </summary>
		/// <param name="element"></param>
		/// <returns></returns>
		public static XElement LastEntityChain(this IEnumerable<XElement> collection)
		{
			return collection.LastOrDefault(_ => _.Name.IsEqual(UnitTextType.ENTITY));
		}
		#endregion

		#region [List<XElement>]
		/// <summary>
		/// Соединение узлов внутри коллекции. К узлу под индексом start присоединяются узлы.
		/// Последним добавленным узлом является lastChild
		/// </summary>
		/// <param name="elements"></param>
		/// <param name="start">узел, к которому будут добавляться дочерние</param>
		/// <param name="lastChild">последний добавляемый узел</param>
		public static void UnionEntity(this List<XElement> elements, int start, XElement lastChild)
		{
			if (start < 0)
				return;
			for (int i = start + 1; i < elements.Count; ++i)
			{
				XElement element = elements[i];
				elements.RemoveAt(i);
				--i;

				if (element.Name.IsEqual(UnitTextType.U))
				{
					XElement last = elements[start].Elements().Last();
					if (last.Name.IsEqual(UnitTextType.U))
						last.ReplaceNodes(new XCData(last.GetFullValue() + element.GetFullValue()));
					else
						elements[start].Add(element);
				}
				else
					elements[start].Add(element);

				if (element == lastChild)
					break;
			}
		}
		#endregion		

		#region [TextParser.Sentence]
		/// <summary>
		/// Начало предложение относительного целого текста
		/// </summary>
		/// <param name="sentence">предложение</param>
		/// <returns>начало предложения</returns>
		public static int GetFullStartPosition(this TextParsing.Sentence sentence)
		{
			return sentence.StartPosition + sentence.Parent.StartPosition;
		}

		/// <summary>
		/// Конец предложения относительного целого текста
		/// </summary>
		/// <param name="sentence">предложение</param>
		/// <returns>конец предложения</returns>
		public static int GetFullEndPosition(this TextParsing.Sentence sentence)
		{
			return sentence.EndPosition + sentence.Parent.StartPosition;
		}
		#endregion

		#region[UnitTextBase]
		/// <summary>
		/// Начало юнита относительного целого текста
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>начало юнита</returns>
		public static int GetFullStartPosition(this UnitTextBase unit)
		{
			return unit.PositionInfo.Start + unit.ParentObject.ParentObject.StartPosition;
		}

		/// <summary>
		/// Конец юнита относительного целого текста
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>конец юнита</returns>
		public static int GetFullEndPosition(this UnitTextBase unit)
		{
			return unit.PositionInfo.End + unit.ParentObject.ParentObject.StartPosition;
		}

		public static int get_Length(this UnitTextBase unit)
		{
			return unit.PositionInfo.End - unit.PositionInfo.Start;
		}
		#endregion
	}
}
