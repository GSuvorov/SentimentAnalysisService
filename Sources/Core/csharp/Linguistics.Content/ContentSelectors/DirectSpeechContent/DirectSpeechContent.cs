using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core.ContentSelectors;

namespace Linguistics.Core
{
	/// <summary>
	/// Прямая речь
	/// </summary>
	internal class DirectSpeechContent : ChildContent
	{
		#region [.ctor().]
		public DirectSpeechContent(string text, int startPosition, char previousSymbol, char nextSymbol)
			: base(text, startPosition,  previousSymbol, nextSymbol)
		{
		}
		#endregion

		#region [ChildContent]
		public override string TextValue
		{
			get
			{
				string text = base.TextValue;
				int startShift = 0;
				int endShift = 0;
				if (!GetOpenQuote().IsDefault())
					startShift = 1;
				if (!GetCloseQuote().IsDefault())
					endShift = 1;
				return text.Substring(startShift, text.Length - startShift - endShift);
			}
		}

		protected override void SetFullText(string text)
		{
			StringBuilder textValue = new StringBuilder(text);
			char openQuote = GetOpenQuote();
			char closeQuote = GetCloseQuote();
			if (!openQuote.IsDefault())
				textValue.Insert(0, openQuote);
			if (!closeQuote.IsDefault())
				textValue.Append(closeQuote);
			base.SetFullText(textValue.ToString());
		}

		protected override int TextStartPosition
		{
			get { return StartPosition + (GetOpenQuote().IsDefault() ? 0 : 1); }
		}

		protected override SubSentence CreateSubSentence(int startPosition)
		{
			return new SubSentence(SubSentenceType.DirectSpeech, startPosition)
				{
					SubTextInfo = new SubTextInfo(GetOpenQuote(), GetCloseQuote()) { Sentences = GetAllSentences() }
				};
		}
		#endregion

		/// <summary>
		/// Получение открывающей кавычки
		/// </summary>
		/// <returns>открывающая кавычка</returns>
		private char GetOpenQuote()
		{
			char possibleQuote = base.TextValue.First();
			return DirectSpeechSelector.IsOpenQuote(possibleQuote) ? possibleQuote : default(char);
		}

		/// <summary>
		/// Получение закрывающей кавычки
		/// </summary>
		/// <returns>закрывающая кавычка</returns>
		private char GetCloseQuote()
		{
			char possibleQuote = base.TextValue.Last();
			return DirectSpeechSelector.IsCloseQuote(possibleQuote) ? possibleQuote : default(char);
		}
	}
}
