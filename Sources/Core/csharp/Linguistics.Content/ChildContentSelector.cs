using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core.ContentSelectors;

namespace Linguistics.Core
{
	/// <summary>
	/// Режим выделения дочерних контентов
	/// </summary>
	public enum ChildContentSelectorMode
	{
		/// <summary>
		/// Все
		/// </summary>
		All,
		/// <summary>
		/// Текст в скобках
		/// </summary>
		BracketContent,
		/// <summary>
		/// Прямая речь
		/// </summary>
		DirectSpeech,
		/// <summary>
		/// Ничего не выделять
		/// </summary>
		None
	}

	/// <summary>
	/// Класс для выделения дочерних контентов
	/// </summary>
	internal class ChildContentSelector : IChildContentSaver
	{
		/// <summary>
		/// Информация для вычисления сдвига
		/// </summary>
		private struct ShiftContentInfo
		{
			/// <summary>
			/// Контент
			/// </summary>
			public ChildContent Content { get; set; }
			/// <summary>
			/// Позиция, по которой вычисляется сдвиг
			/// </summary>
			public int Position { get; set; }
		}

		private static Dictionary<ChildContentSelectorMode, ContentSelectorBase[]> _selectorsDictionary;
		/// <summary>
		/// Выделители дочерних контентов
		/// </summary>
		private static ContentSelectorBase[] _selectors;

		/// <summary>
		/// Индекс текущего выделителя дочерних контентов
		/// </summary>
		private static int _currentSelectorIndex;

		/// <summary>
		/// Выделенные контенты
		/// </summary>
		private List<ChildContent> _selectedContents;

		/// <summary>
		/// Выделенные контенты, по которым вычисляется сдвиг
		/// </summary>
		private List<ShiftContentInfo> _shiftContents;

		#region [.ctor().]
		static ChildContentSelector()
		{
			var bracketContentSelector = new BracketContentSelector();
			var directSpeechSelector = new DirectSpeechSelector();

			_selectorsDictionary = new Dictionary<ChildContentSelectorMode, ContentSelectorBase[]>();
			_selectorsDictionary.Add(ChildContentSelectorMode.All, new ContentSelectorBase[] { bracketContentSelector, directSpeechSelector });
			_selectorsDictionary.Add(ChildContentSelectorMode.BracketContent, new ContentSelectorBase[] { bracketContentSelector });
			_selectorsDictionary.Add(ChildContentSelectorMode.DirectSpeech, new ContentSelectorBase[] { directSpeechSelector });
			_selectorsDictionary.Add(ChildContentSelectorMode.None, new ContentSelectorBase[] { });
		}

		public ChildContentSelector()
		{
			InitializeSelectorCollections();
		}

		/// <summary>
		/// Инициализация
		/// </summary>
		private void InitializeSelectorCollections()
		{
			_selectedContents = new List<ChildContent>();
			_shiftContents = new List<ShiftContentInfo>();
		}
		#endregion

		/// <summary>
		/// Выделение дочерние контенты из заданного текста
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="mode">режим выделения дочерних контентов</param>
		/// <returns>дочерние контенты</returns>
		public List<ChildContent> Select(ref string text, ChildContentSelectorMode mode)
		{
			InitializeSelectorCollections();
			_selectors = _selectorsDictionary[mode];

			for (int i = 0; i < _selectors.Length; ++i)
			{
				_currentSelectorIndex = i;
				_selectors[i].Select(ref text, this);
			}

			return _selectedContents;
		}

		#region [IContentSaver]
		public void Save(ChildContent content, int position)
		{
			SelectChildren(content);

			int shiftContentIndex = 0;
			int index;
			int contentLength = content.Length;
			for (index = 0; index < _selectedContents.Count; ++index)
			{
				if (content.StartPosition < _selectedContents[index].StartPosition)
				{
					if (_selectedContents[index].StartPosition < content.StartPosition + contentLength)
					/// дочерний контент
					{
						contentLength += _selectedContents[index].GetFullText().Length;
						content.AddChild(_selectedContents[index]);
						RemoveContent(shiftContentIndex, ref index);
					}
					else
					{
						_selectedContents[index].SetPreviousContent(content);
						break;
					}
				}
				else if ((shiftContentIndex < _shiftContents.Count) &&
					(_selectedContents[index] == _shiftContents[shiftContentIndex].Content))
				{
					++shiftContentIndex;
				}
			}

			_selectedContents.Insert(index, content);
			_shiftContents.Insert(shiftContentIndex, new ShiftContentInfo() { Content = content, Position = position });
		}

		public void ClearShift()
		{
			_shiftContents.Clear();
		}

		public int GetRealPosition(int currentPosition)
		{
			int result = currentPosition;
			foreach (var content in _selectedContents)
			{
				if (content.StartPosition <= result)
					result += GetFullContentLength(content);
				else
					break;
			}
			return result;
		}

		public int GetRealPositionShift(int realPosition)
		{
			int result = 0;
			foreach (var shiftContent in _shiftContents)
			{
				if (shiftContent.Position <= realPosition)
					result += GetFullContentLength(shiftContent.Content);
				else
					break;
			}
			return result;
		}
		#endregion

		/// <summary>
		/// Удаление заданного контента
		/// </summary>
		/// <param name="shiftContentIndex">индекс в коллекции контентов для вычисления сдвига</param>
		/// <param name="contentIndex">индекс в коллекции выделенных контентов</param>
		private void RemoveContent(int shiftContentIndex, ref int contentIndex)
		{
			if ((shiftContentIndex < _shiftContents.Count) &&
				(_selectedContents[contentIndex] == _shiftContents[shiftContentIndex].Content))
			{
				_shiftContents.RemoveAt(shiftContentIndex);
			}

			_selectedContents.RemoveAt(contentIndex);
			--contentIndex;
		}

		/// <summary>
		/// Получение полной длины (с учетом длин дочерних контентов) заданного контента
		/// </summary>
		/// <param name="content">контент</param>
		/// <returns>полная длина контента</returns>
		private int GetFullContentLength(ChildContent content)
		{
			int result = content.Length;
			foreach (var child in content.Children)
				result += GetFullContentLength(child);
			return result;
		}

		private void SelectChildren(ChildContent parent)
		{
			ChildContentSelector childSelector = new ChildContentSelector();
			string text = parent.TextValue;
			for (int i = _currentSelectorIndex + 1; i < _selectors.Length; ++i)
			{
				_currentSelectorIndex = i;
				_selectors[i].Select(ref text, childSelector);
			}
			if (childSelector._selectedContents.Any())
				parent.SetContentChildren(childSelector._selectedContents);
		}
	}
}
