using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace Linguistics.Core.ContentSelectors
{
	/// <summary>
	/// Текст в кавычках
	/// </summary>
	internal class QuoteText
	{
		#region [.constants.]
		/// <summary>
		/// открывающие кавычки
		/// </summary>
		public static readonly string OpenQuotes = "“\"'«" + (char)0x201C;
		/// <summary>
		/// закрывающие кавычки
		/// </summary>
		public static readonly string CloseQuotes = "”\"'»" + (char)0x201D;
		/// <summary>
		/// возможные разделители предложения
		/// </summary>
		private static readonly char[] PossibleSentenceSeparators = new char[] { '.', ';', '!', '?', '…' };
		#endregion

		/// <summary>
		/// Начальная позиция
		/// </summary>
		public int StartPosition { get; private set; }
		/// <summary>
		/// Конечная позиция
		/// </summary>
		public int EndPosition { get; private set; }
		/// <summary>
		/// Содержимое
		/// </summary>
		public string Content { get; private set; }
		/// <summary>
		/// Предыдущий непустой символ
		/// </summary>
		public char PreviousNotEmpty { get; private set; }
		/// <summary>
		/// Следующий непустой символ
		/// </summary>
		public char NextNotEmpty { get; private set; }

		#region [.ctor().]
		private QuoteText(string text, int start, int end)
		{
			StartPosition = start;
			EndPosition = end;
			Content = text.Substring(start, end - start);
		}
		#endregion

		/// <summary>
		/// Создание текста в кавычках
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="closeQuote">закрывающая кавычка</param>
		/// <param name="closeQuoteIndex">индекс закрывающей кавычки</param>
		/// <param name="quoteTextEndIndex">индекс конца текста в кавычках</param>
		/// <param name="quoteTextEndLength">длина конца текста в кавычках</param>
		/// <returns>текст в кавычках</returns>
		public static QuoteText Create(string text, char closeQuote, int closeQuoteIndex, int quoteTextEndIndex, int quoteTextEndLength)
		{
			int quoteTextStartIndex;
			if (TryGetDirectSpeechStart(text, quoteTextEndIndex, closeQuote, out quoteTextStartIndex))
			{
				char lastQuoteTextEnd = text[quoteTextEndIndex + quoteTextEndLength - 1];
				return new QuoteText(text, quoteTextStartIndex, closeQuoteIndex + 1)
					{
						PreviousNotEmpty = text.GetPreviousNotEmpty(quoteTextStartIndex),
						NextNotEmpty = lastQuoteTextEnd.IsTire() ? lastQuoteTextEnd : text.GetNextNotEmpty(quoteTextEndIndex + quoteTextEndLength - 1)
					};
			}
			return null;
		}

		/// <summary>
		/// Поиск начала прямой речи
		/// </summary>
		/// <returns>флаг удалось ли получить начало прямой речи</returns>
		/// 
		/// поиск производится путем нахождения перед закрывающей кавычкой разделителя предложения
		/// если в найденном интервале содержится:
		/// 1. четное количество кавычек: кавычки считаются относящимися к именованиям организаций,
		/// продуктов или иных слов в кавычках, не являющихся прямой речью
		/// 2. нечетное количество кавычек: поиск прекращается. Если кавычка одна, то она считается открывающей.
		private static bool TryGetDirectSpeechStart(string text, int directSpeechEndIndex, char closeQuote, out int directSpeechStartIndex)
		{
			directSpeechStartIndex = -1;
			/// индекс типа кавычки
			int quoteTypeIndex = CloseQuotes.IndexOf(closeQuote);
			/// индекс начала анализируемого интервала
			int startindex = directSpeechEndIndex;
			/// индекс конца анализируемого интервала
			int endIndex = startindex;
			while (startindex > 0)
			{
				startindex = GetSentenceSeparatorIndex(text, endIndex);
				string textPart = (startindex > 0) ? text.Substring(startindex, endIndex - startindex)
												   : text.Substring(0, endIndex);
				/// количество открывающих и закрывающих кавычек
				/// закрывающих кавычек 0 - невозможно различить открывающую и закрывающую кавычку
				var quotes = GetQuotesCount(textPart, OpenQuotes[quoteTypeIndex], CloseQuotes[quoteTypeIndex]);
				if (quotes.Item1 - quotes.Item2 == 1)
				{
					directSpeechStartIndex = textPart.IndexOf(OpenQuotes[quoteTypeIndex]);
					if (startindex > 0)
						directSpeechStartIndex += startindex;
					return true;
				}
				else
				{
					/// нечетное число кавычек
					bool isOddQuotes = (quotes.Item2 == 0) && (quotes.Item1 % 2 != 0);
					/// неодинаковое число открывающих и закрывающих кавычек
					bool isNotSameCountOfOpenAndCloseQuotes = (quotes.Item2 != 0) && (quotes.Item1 != quotes.Item2);
					if (isOddQuotes || isNotSameCountOfOpenAndCloseQuotes)
						break;
				}
				endIndex = startindex;
			}
			return false;
		}

		/// <summary>
		/// Получение в заданном тексте количества открывающих и закрывающих кавычек
		/// Если открывающая кавычка равна закрывающей, то возвращается только количество открывающих кавычек
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="openQuote">открывающая кавычка</param>
		/// <param name="closeQuote">закрывающая кавычка</param>
		/// <returns>пара "количество открывающих кавычек, количество закрывающих кавычек"</returns>
		private static Tuple<int, int> GetQuotesCount(string text, char openQuote, char closeQuote)
		{
			int openQuotesCount = text.Count(_ => openQuote == _);
			if (openQuote != closeQuote)
				return new Tuple<int, int>(openQuotesCount, text.Count(_ => closeQuote == _));
			return new Tuple<int, int>(openQuotesCount, 0);
		}

		/// <summary>
		/// Получение индекса разделителя предложения
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="currentIndex">текущий индекс</param>
		/// <returns>индекс разделителя предложения, -1 - если он не найден</returns>
		private static int GetSentenceSeparatorIndex(string text, int currentIndex)
		{
			if (currentIndex == 0)
				return -1;

			int result = text.LastIndexOfAny(PossibleSentenceSeparators, currentIndex - 1);
			if ((text.CharOrDefault(result) == '-') &&
				(Char.IsLetterOrDigit(text.CharOrDefault(result - 1)) || Char.IsLetterOrDigit(text.CharOrDefault(result + 1))))
			/// дефис
			{
				return GetSentenceSeparatorIndex(text, result);
			}
			return result;
		}
	}
}
