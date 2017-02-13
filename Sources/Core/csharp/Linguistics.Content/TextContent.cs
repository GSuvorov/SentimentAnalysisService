using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Контент текста
	/// </summary>
	public class TextContent
	{
		/// <summary>
		/// Выделитель дочерних контентов
		/// </summary>
		private static ChildContentSelector _selector = new ChildContentSelector();

		/// <summary>
		/// Парсер текста
		/// </summary>
		public static TextParsing.TextParser Parser { get; set; }

		/// <summary>
		/// Базовая дата для относительных дат
		/// </summary>
		public readonly string BaseDate;

		/// <summary>
		/// Родительский контент
		/// </summary>
		public Content ParentContent { get; private set; }

		#region [.ctor().]
		/// <summary>
		/// Конструктор
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="baseDate">дата для выделения относительных дат</param>
		/// <param name="mode">режим выделения дочерних контентов</param>
		public TextContent(string text, string baseDate, ChildContentSelectorMode mode = ChildContentSelectorMode.All)
		{
			BaseDate = baseDate;
			ParentContent = CreateContent(text, mode);
		}

		/// <summary>
		/// Создание контента
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="mode">режим выделения дочерних контентов</param>
		/// <returns>контент</returns>
		private Content CreateContent(string text, ChildContentSelectorMode mode)
		{
			string content = text;
			var children = _selector.Select(ref content, mode);

			Content result = new Content(content);
			result.Children = children;
			return result;
		}
		#endregion

		/// <summary>
		/// Получение объекта текста
		/// </summary>
		/// <returns>объект текста</returns>
		public IText GetTextObject()
		{
			return new Text() { Sentences = ParentContent.GetAllSentences() };
		}
	}
}
