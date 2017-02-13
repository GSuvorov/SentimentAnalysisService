using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Дочерний контент
	/// </summary>
	internal abstract class ChildContent : Content
	{
		#region [.self properties.]
		/// <summary>
		/// Начало в родительском контенте
		/// (данная позиция учитывает все предыдущие контенты такого же уровня в родительском контенте)
		/// </summary>
		public int StartPosition { get; private set; }

		/// <summary>
		/// Предыдущий символ
		/// </summary>
		public char PreviousSymbol { get; private set; }
		/// <summary>
		/// Следующий символ
		/// </summary>
		public char NextSymbol { get; private set; }
		#endregion

		#region [.ctor().]
		public ChildContent(string text, int startPosition, char previousSymbol, char nextSymbol)
			: base (text)
		{
			StartPosition = startPosition;
			PreviousSymbol = previousSymbol;
			NextSymbol = nextSymbol;
		}
		#endregion

		/// <summary>
		/// Добавление дочернего контента
		/// </summary>
		/// <param name="newChild">дочерний контент</param>
		public void AddChild(ChildContent newChild)
		{
			newChild.StartPosition -= TextStartPosition;

			int index = -1;
			int childrenLength = 0;
			for (index = 0; index < Children.Count; ++index)
			{
				ChildContent child = Children[index];
				if (child.IsContain(newChild))
				{
					child.AddChild(newChild);
					return;
				}
				else if (newChild.IsContain(child))
				{
					newChild.AddChild(child);
					Children.RemoveAt(index);
					--index;
					continue;
				}
				else if (newChild.StartPosition + newChild.Length < child.StartPosition)
					break;
				childrenLength += child.GetFullText().Length;
			}
			newChild.PreviousSymbol = TextValue.CharOrDefault(newChild.StartPosition - childrenLength - 1);
			newChild.NextSymbol = TextValue.CharOrDefault(newChild.StartPosition - childrenLength);
			Children.Insert(index, newChild);
		}

		/// <summary>
		/// Получение полного текстового значения контента (с учетом дочерних контентов)
		/// </summary>
		/// <returns>полное текстовое значение контента</returns>
		public string GetFullText()
		{
			StringBuilder result = new StringBuilder(base.TextValue);
			foreach (var child in Children)
				result.Insert(TextStartPosition - StartPosition + child.StartPosition, child.GetFullText());
			return result.ToString();
		}

		/// <summary>
		/// Проверка, что текущий контент содержит заданный
		/// </summary>
		/// <param name="content">заданный контент</param>
		/// <returns>результат проверки</returns>
		private bool IsContain(ChildContent content)
		{
			return (StartPosition < content.StartPosition) && (content.StartPosition < StartPosition + Length);
		}

		/// <summary>
		/// Задание дочерних контентов в текущем контенте
		/// </summary>
		/// <param name="children">дочерние контенты</param>
		internal void SetContentChildren(IEnumerable<ChildContent> children)
		{
			base.SetContentAndParse(GetContentNewFullText(children));
			Children = new List<ChildContent>(children);
		}

		/// <summary>
		/// Задание рядом стоящего предыдущего контента
		/// </summary>
		/// <param name="content">контент</param>
		internal void SetPreviousContent(ChildContent content)
		{
			if (StartPosition == content.StartPosition + content.Length)
				PreviousSymbol = content.PreviousSymbol;
		}

		/// <summary>
		/// Получение нового полного текстового значения контента
		/// </summary>
		/// <param name="children">дочерние контенты</param>
		/// <returns>новое полное текствое значение контента</returns>
		private string GetContentNewFullText(IEnumerable<ChildContent> children)
		{
			StringBuilder result = new StringBuilder();
			result.Append(base.TextValue.Substring(0, TextStartPosition - StartPosition));
			result.Append(RemoveContentsText(children));
			result.Append(base.TextValue.Substring(TextStartPosition - StartPosition + TextValue.Length));
			return result.ToString();
		}

		/// <summary>
		/// Удаление текста дочерних контентов из текстового значения текущего контента
		/// </summary>
		/// <param name="collection">дочерние контенты</param>
		/// <returns>новое текстовое значение контента</returns>
		private string RemoveContentsText(IEnumerable<ChildContent> collection)
		{
			StringBuilder result = new StringBuilder(TextValue);
			RemoveContentsText(result, collection, 0);
			return result.ToString();
		}

		/// <summary>
		/// Удаление текста дочерних контентов из заданного текста
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="collection">дочерние контенты</param>
		/// <param name="startPosition">начальная позиция</param>
		/// <returns>суммарная длина удаленного текста</returns>
		private int RemoveContentsText(StringBuilder text, IEnumerable<ChildContent> collection, int startPosition)
		{
			int length = 0;
			foreach (var content in collection)
			{
				int contentStartPosition = startPosition + content.StartPosition;
				int removedLength = RemoveContentsText(text, content.Children, contentStartPosition);
				text.Remove(contentStartPosition - length, content.Length);
				length += content.Length + removedLength;
			}
			return length;
		}
	}
}
