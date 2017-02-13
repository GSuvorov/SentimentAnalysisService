using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Класс расширений
	/// </summary>
	internal static class Extensions
	{
		/// <summary>
		/// Установка связей в заданной коллекции
		/// </summary>
		/// <param name="collection">коллекция</param>
		/// <param name="parentSentence">родитель</param>
		public static void SetRelations<T, P>(this IEnumerable<IRelations<T, P>> collection, P parent) where T : class
		{
			IRelations<T, P> previous = null;
			foreach (var current in collection)
			{
				current.ParentObject = parent;
				current.SetRelation(previous);
				previous = current;
			}
		}

		/// <summary>
		/// Установка связи между двумя соседними элементами
		/// </summary>
		/// <param name="current">текущий элемент</param>
		/// <param name="previous">предыдущий элемент</param>
		public static void SetRelation<T, P>(this IRelations<T, P> current, IRelations<T, P> previous) where T : class
		{
			current.Next = null;
			if (previous != null)
			{
				current.Previous = previous.Current;
				previous.Next = current.Current;
			}
			else
				current.Previous = null;
		}
		
		#region [string]
		/// <summary>
		/// Получение блока CDATA из заданного текста
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>блок CDATA</returns>
		public static XCData GetCData(this string text)
		{
			return new XCData(text);
		}
		#endregion

		#region [XElement]
		/// <summary>
		/// Проверка, что у xml-узел имеет заданный тип
		/// </summary>
		/// <param name="element">xml-узел</param>
		/// <param name="type">тип узла</param>
		/// <returns>результат проверки</returns>
		public static bool IsName(this XElement element, UnitTextType type)
		{
			return element.Name == type.ToString();
		}

		/// <summary>
		/// Получение из xml значения атрибута начальной позиции
		/// </summary>
		/// <param name="element">xml-узел</param>
		/// <returns>начальная позиция</returns>
		public static int GetStartPosition(this XElement element)
		{
			int result = -1;
			XAttribute positionAttribute = element.Attribute(RDF.Attribute.StartPosition);
			if (positionAttribute != null)
				result = int.Parse(positionAttribute.Value);
			return result;
		}

		/// <summary>
		/// Получение значения атрибута
		/// </summary>
		/// <param name="element">xml-узел</param>
		/// <param name="attributeName">имя атрибута</param>
		/// <returns>значение атрибута</returns>
		public static string GetAttributeValue(this XElement element, string attributeName)
		{
			return element.Attribute(attributeName).Value;
		}

		/// <summary>
		/// Проверка наличия атрибута
		/// </summary>
		/// <param name="element">xml-узел</param>
		/// <param name="attributeName">имя атрибута</param>
		/// <returns>результат проверки</returns>
		public static bool IsContainAttribute(this XElement element, string attributeName)
		{
			return (element != null) ? (element.Attribute(attributeName) != null) : false;
		}
		#endregion

		#region [StringBuilder]
		/// <summary>
		/// Проверка на пустоту
		/// </summary>
		/// <param name="builder">строка</param>
		/// <returns>результат проверки</returns>
		public static bool IsEmpty(this StringBuilder builder)
		{
			return builder.Length == 0;
		}
		#endregion
	}
}
