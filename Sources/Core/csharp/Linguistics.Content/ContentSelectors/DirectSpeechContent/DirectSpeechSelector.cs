using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Utils;

namespace Linguistics.Core.ContentSelectors
{
	/// <summary>
	/// Класс для выделения прямой речи
	/// </summary>
	internal class DirectSpeechSelector : ContentSelectorBase
	{
		#region [regex]
		/// <summary>
		/// Регулярное выражение для выделения конца прямой речи
		/// </summary>
		private static readonly Regex _directSpeechEndRegex;
		/// <summary>
		/// Регулярное выражение для начала слов автора
		/// </summary>
		private static readonly Regex _authorWordsStart = new Regex("(([,?!\u2026])|(\\.\\.\\.))[ ]*((-)|(–)|(—))");
		/// <summary>
		/// Регулярное выражение для конца слов автора
		/// </summary>
		private static readonly Regex _authorWordsEnd = new Regex("[,\\.:][ ]*((-)|(–)|(—))");
		#endregion

		#region [.ctor().]
		static DirectSpeechSelector()
		{
			_directSpeechEndRegex = new Regex(string.Format("({0}[ ]*($|[\\.,:;-]|(–)|(—)))|((([?!\u2026])|(\\.\\.\\.))[ ]*{0}[, ]*)",
				"(?<quote>" + RegexHelper.GetVariantsFromCollection(QuoteText.CloseQuotes.Select(_ => _.ToString())) + ")"));
		}
		#endregion

		/// <summary>
		/// Проверка, что заданный символ - открывающая кавычка
		/// </summary>
		/// <param name="symbol">символ</param>
		/// <returns>результат проверки</returns>
		public static bool IsOpenQuote(char symbol)
		{
			return QuoteText.OpenQuotes.Contains(symbol);
		}

		/// <summary>
		/// Проверка, что заданный символ - закрывающая кавычка
		/// </summary>
		/// <param name="symbol">символ</param>
		/// <returns>результат проверки</returns>
		public static bool IsCloseQuote(char symbol)
		{
			return QuoteText.CloseQuotes.Contains(symbol);
		}

		#region [ContentSelector]
		protected override void Select(ref string text)
		{
			StringBuilder builder = new StringBuilder(text);
			MatchCollection directSpeechEnds = _directSpeechEndRegex.Matches(builder.ToString());
			for (int j = 0; j < directSpeechEnds.Count; ++j)
				AnalyzeDirectSpeechEnd(builder, directSpeechEnds[j]);
			text = builder.ToString();
		}
		#endregion

		/// <summary>
		/// Анализ конца прямой речи.
		/// Если найдена открывающая кавычка, то будет создана прямая речь и в заданной строке она будет убрана
		/// </summary>
		/// <param name="builder">строка</param>
		/// <param name="directSpeechEnd">конец прямой речи</param>
		private void AnalyzeDirectSpeechEnd(StringBuilder builder, Match directSpeechEnd)
		{
			Group closeQuote = directSpeechEnd.Groups["quote"];
			#region [.defense.]
			if (closeQuote.Value.Length != 1)
				throw new ArgumentException("very long quote");
			#endregion

			string text = builder.ToString();
			int shift = _saver.GetRealPositionShift(directSpeechEnd.Index);

			QuoteText quoteText = QuoteText.Create(text, closeQuote.Value.First(), closeQuote.Index - shift,
				directSpeechEnd.Index - shift, directSpeechEnd.Length);
			if (quoteText != null)
			{
				string replaceValue;
				if (TryCreateDirectSpeech(text, quoteText, out replaceValue))
					builder.Replace(quoteText.Content, replaceValue, quoteText.StartPosition, quoteText.Content.Length);
			}
		}

		/// <summary>
		/// Создание прямой речи
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="quoteText">текст в кавычках</param>
		/// <param name="replaceValue">значение, которым необходимо заменить текст в кавычках</param>
		/// <returns>флаг удалось ли выделить прямую речь</returns>
		private bool TryCreateDirectSpeech(string text, QuoteText quoteText, out string replaceValue)
		{
			bool result = false;
			var dashes = GetDashes(quoteText.Content);
			TextPart authorWords = new TextPart();
			if (dashes.Length == 2) /// необходима проверка на наличие слов автора
				authorWords = GetAuthorWords(quoteText.Content, dashes);
			if (authorWords.Success)
			{
				replaceValue = CreateDirectSpeechWithAuthorWords(quoteText, authorWords);
				result = true;
			}
			else
				result = CreateSimpleDirectSpeech(text, quoteText, out replaceValue);
			return result;
		}

		/// <summary>
		/// Создание прямой речи
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="quoteText">текст в кавычках</param>
		/// <param name="replaceValue">значение, которым необходимо заменить текст в кавычках</param>
		/// <returns>флаг удалось ли выделить прямую речь</returns>
		private bool CreateSimpleDirectSpeech(string text, QuoteText quoteText, out string replaceValue)
		{
			bool result = false;
			replaceValue = quoteText.Content;
			if (IsDirectSpeech(quoteText.Content))
			{
				result = true;
				replaceValue = "";
				if (quoteText.NextNotEmpty.IsTire() && !Char.IsLetterOrDigit(quoteText.PreviousNotEmpty))
				{
					SaveDirectSpeech(
						quoteText.Content,
						quoteText.StartPosition,
						default(char), text.CharOrDefault(quoteText.EndPosition));
				}
				else if (quoteText.PreviousNotEmpty.Equals(':') && !QuoteText.OpenQuotes.Contains(quoteText.NextNotEmpty))
				{
					SaveDirectSpeech(
						quoteText.Content,
						quoteText.StartPosition,
						text.CharOrDefault(quoteText.StartPosition - 1), default(char));
				}
				else
				{
					replaceValue = quoteText.Content;
					result = false;
				}
			}
			return result;
		}

		/// <summary>
		/// Создание прямой речи со словами автора
		/// </summary>
		/// <param name="quoteText">текст в кавычках</param>
		/// <param name="authorWords">границы слов автора</param>
		/// <returns>значение, которым необходимо заменить текст в кавычках</returns>
		private string CreateDirectSpeechWithAuthorWords(QuoteText quoteText, TextPart authorWords)
		{
			SaveDirectSpeech(
				quoteText.Content.Substring(0, authorWords.Start),
				quoteText.StartPosition,
				default(char), quoteText.Content[authorWords.Start]);

			SaveDirectSpeech(
				quoteText.Content.Substring(authorWords.End),
				quoteText.StartPosition + authorWords.End - authorWords.Start,
				quoteText.Content[authorWords.End - 1], default(char));

			return quoteText.Content.Substring(authorWords.Start, authorWords.End - authorWords.Start);
		}

		/// <summary>
		/// Сохранение блока прямой речи
		/// </summary>
		/// <param name="directSpeech">прямая речь</param>
		/// <param name="position">начальная позиция</param>
		/// <param name="previousSymbol">предыдущий символ</param>
		/// <param name="nextSymbol">следующий символ</param>
		private void SaveDirectSpeech(string directSpeech, int position, char previousSymbol, char nextSymbol)
		{
			_saver.Save(new DirectSpeechContent(directSpeech, _saver.GetRealPosition(position), previousSymbol, nextSymbol),
				position);
		}

		/// <summary>
		/// Проверка текста в кавычках на прямую речь
		/// </summary>
		/// <param name="directSpeech">текст в кавычках</param>
		/// <returns>результат проверки</returns>
		private bool IsDirectSpeech(string directSpeech)
		{
			if (directSpeech.Length > 2)
			{
				string content = directSpeech.Substring(1, directSpeech.Length - 2).Trim();
				if (!string.IsNullOrEmpty(content) && (content.IndexOfAny(new char[] { '\n', '\r' }) < 0))
				{
					string firstWord = (new string(content.TakeWhile(_ => Char.IsLetterOrDigit(_)).ToArray())).ToLower();
					/// надо отбросить прямую речь, которая начинается с любой формы глагола "скандировать"
					if (!firstWord.StartsWith("скандирова") && !firstWord.StartsWith("скандиру"))
						return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Получение слов автора
		/// </summary>
		/// <param name="directSpeech">прямая речь</param>
		/// <param name="dashesIndexes">коллекция тире</param>
		/// <returns>слова автора</returns>
		private TextPart GetAuthorWords(string directSpeech, IEnumerable<int> dashesIndexes)
		{
			TextPart result = new TextPart();
			Match authorWordsStart = _authorWordsStart.Match(directSpeech);
			MatchCollection authorWordsEndCollection = _authorWordsEnd.Matches(directSpeech);
			Match authorWordsEnd = (authorWordsEndCollection.Count > 0) ? authorWordsEndCollection[authorWordsEndCollection.Count - 1]
																		: null;
			if (authorWordsStart.Success &&
				(authorWordsStart.Index < dashesIndexes.First()) &&
				(authorWordsEnd != null))
			{
				result.Start = authorWordsStart.Index + authorWordsStart.Length;
				result.End = authorWordsEnd.Index + 1;
			}
			return result;
		}

		/// <summary>
		/// Получение коллекции индексов тире для заданного текста
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>коллекция индексов тире</returns>
		private int[] GetDashes(string text)
		{
			List<int> result = new List<int>();
			for (int i = 0; i < text.Length; ++i)
			{
				if (text[i].IsTire() &&
					!Char.IsLetterOrDigit(text.CharOrDefault(i - 1)) &&
					Char.IsWhiteSpace(text.CharOrDefault(i + 1)))
				{
					result.Add(i);
				}
			}
			return result.ToArray();
		}
	}
}
