using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using System.Xml.Linq;

namespace PTS
{
	/// <summary>
	/// Контекст
	/// </summary>
	public class TextRange
	{
		/// <summary>
		/// Интервал чисел
		/// </summary>
		public NumberRange Range { get; private set; }

		/// <summary>
		/// Левый контекст
		/// </summary>
		public string LeftContext { get; private set; }
		/// <summary>
		/// Количество еще доступных слов для левого контекста
		/// </summary>
		private int _leftContextWordQuote;
		/// <summary>
		/// Правый контекст
		/// </summary>
		public string RightContext { get; private set; }
		/// <summary>
		/// Количество еще доступных слов для правого контекста
		/// </summary>
		private int _rightContextWordQuote;

		/// <summary>
		/// Получение xml для интервала
		/// </summary>
		/// <returns>xml</returns>
		public XElement ToXElement()
		{
			XElement range = new XElement("textrange");
			range.Value = string.Format("{0}ↂ{1}ↂ{2}", LeftContext, Range.Text, RightContext).Trim();
			range.Add(Range.GetRangeXElement().Elements());
			return range;
		}

		#region [.ctor().]
		public TextRange(NumberRange range, IEnumerable<UnitTextBase> units, int contextLength)
		{
			Range = range;
			LeftContext = GetLeftContext(units, range.StartPosition, contextLength);
			RightContext = GetRightContext(units, range.EndPosition, contextLength);
		}

		public TextRange(NumberRange range, string leftContext, string rightContext)
		{
			Range = range;
			LeftContext = leftContext;
			RightContext = rightContext;
		}
		#endregion

		/// <summary>
		/// Дополнение левого котекста словами из заданного текста
		/// </summary>
		/// <param name="text">текст</param>
		public void UpdateLeftContext(string text)
		{
			LeftContext = GetWordsFromText(text, ref _leftContextWordQuote, true) + LeftContext;
		}

		/// <summary>
		/// Дополнение правого котекста словами из заданного текста
		/// </summary>
		/// <param name="text">текст</param>
		public void UpdateRightContext(string text)
		{
			RightContext += GetWordsFromText(text, ref _rightContextWordQuote, false);
		}

		/// <summary>
		/// Выделение левого контекста
		/// </summary>
		/// <param name="units">юниты</param>
		/// <param name="position">позиция, до которой искать контекст</param>
		/// <param name="contextLength">максимальная длина контекста в словах</param>
		/// <returns>контекст</returns>
		private string GetLeftContext(IEnumerable<UnitTextBase> units, int position, int contextLength)
		{
			_leftContextWordQuote = contextLength;
			var leftUnits = units.Where(_ => _.PositionInfo.Start < position).ToArray();
			if (leftUnits.Any())
			{
				string leftContext = leftUnits.GetTextWithSubText().Substring(0, position - leftUnits.First().PositionInfo.Start);
				return GetWordsFromText(leftContext, ref _leftContextWordQuote, true);
			}
			else
				return String.Empty;
		}

		/// <summary>
		/// Выделение правого контекста
		/// </summary>
		/// <param name="units">юниты</param>
		/// <param name="position">позиция, после которой искать контекст</param>
		/// <returns>контекст</returns>
		private string GetRightContext(IEnumerable<UnitTextBase> units, int position, int contextLength)
		{
			_rightContextWordQuote = contextLength;
			return GetWordsFromText(units.Where(_ => _.PositionInfo.Start >= position).GetTextWithSubText(), ref _rightContextWordQuote, false);
		}

		/// <summary>
		/// Получение заданного количества слов из текста
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="wordQuota">количество слов</param>
		/// <param name="isToLeft">флаг, указывающий порядок слов справа налево</param>
		/// <returns>заданное количество слов</returns>
		private string GetWordsFromText(string text, ref int wordQuota, bool isToLeft)
		{
			StringBuilder result = new StringBuilder();
			var words = text.Split(' ', '\n', '\r', '\t');
			if (isToLeft)
				words = words.Reverse().ToArray();
			words = TakeWords(words, ref wordQuota);
			if (isToLeft)
				words = words.Reverse().ToArray();
			if (words.Any())
			{
				foreach (string word in words)
				{
					result.Append(' ');
					result.Append(word);
				}
				result.Remove(0, 1);
			}
			return result.ToString();
		}

		/// <summary>
		/// Выбор из коллекции заданного количества слов
		/// </summary>
		/// <param name="collection">коллекция слов</param>
		/// <param name="wordCount">количество слов</param>
		/// <returns>выбранные слова</returns>
		private string[] TakeWords(IEnumerable<string> collection, ref int wordCount)
		{
			List<string> result = new List<string>();
			foreach (string word in collection)
			{
				if (wordCount <= 0)
					break;
				result.Add(word);
				if (string.IsNullOrEmpty(word))
					continue;

				--wordCount;
			}
			return result.ToArray();
		}
	}
}
