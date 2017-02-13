using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Текст в скобках
	/// </summary>
	internal class BracketContent : ChildContent
	{
		/// <summary>
		/// Открывающая скобка
		/// </summary>
		private char _openBracket;
		/// <summary>
		/// Закрывающая скобка
		/// </summary>
		private char _closeBracket;

		#region [.ctor().]
		public BracketContent(string text, int startPosition, char previousSymbol, char nextSymbol)
			: base(text, startPosition, previousSymbol, nextSymbol)
		{
			_openBracket = text.First();
			_closeBracket = text.Last();
		}
		#endregion

		#region [ChildContent]
		public override string TextValue
		{
			get
			{
				string text = base.TextValue;
				/// текст без скобок
				return text.Substring(1, text.Length - 2);
			}
		}

		protected override void SetFullText(string text)
		{
			base.SetFullText(_openBracket + text + _closeBracket);
		}

		protected override int TextStartPosition
		{
			get { return StartPosition + 1; }
		}

		protected override SubSentence CreateSubSentence(int startPosition)
		{
			return new SubSentence(SubSentenceType.ApproximatingForceps, startPosition)
				{
					SubTextInfo = new SubTextInfo(GetOpenBracket(), GetCloseBracket()) { Sentences = GetAllSentences() }
				};
		}

		public override bool IsPotencialParentPart
		{
			get
			{
				bool result = GetParagraphs().Sum(_ => _.Sentences.Length) <= 1;
				foreach (var child in Children)
				{
					if (!result)
						break;
					result &= child.IsPotencialParentPart;
				}
				return result;
			}
		}

		protected override string GetTextWithChildrenParts(bool isOnlyPotencial)
		{
			return GetOpenBracket() + base.GetTextWithChildrenParts(isOnlyPotencial) + GetCloseBracket();
		}
		#endregion

		/// <summary>
		/// Получение открывающей скобки
		/// </summary>
		/// <returns>открывающая скобка</returns>
		private char GetOpenBracket()
		{
			return base.TextValue.First();
		}

		/// <summary>
		/// Получение закрывающей скобки
		/// </summary>
		/// <returns>закрывающая скобка</returns>
		private char GetCloseBracket()
		{
			return base.TextValue.Last();
		}
	}
}
