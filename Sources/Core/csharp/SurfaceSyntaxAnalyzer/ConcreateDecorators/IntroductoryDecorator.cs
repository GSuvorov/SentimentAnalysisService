using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// декоратор для выделения вводных подпредложений
	/// </summary>
	internal class IntroductoryDecorator : SubSentenceByContentDecoratorBase
	{
		/// <summary>
		/// Разделители вводных
		/// </summary>
		private static readonly string[] IntroductorySeparators = new string[] { ",", "-", "—" };

		/// <summary>
		/// Конец предложения
		/// </summary>
		private static readonly char[] SentenceEnd = new char[] { '…', '.', '!', '?', ';' };

		#region [.ctor().]
		public IntroductoryDecorator(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

		#region [SyntaxAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			var childSubSentences = SelectSubSentencesByContent(subSentence);
			FillSubSentence(subSentence, childSubSentences);
			return base.AnalyzeSubSentence(subSentence);
		}
		#endregion

		#region [SubSentenceByContentDecorator]
		protected override bool IsSeparator(UnitTextBase unit)
		{
			return unit.IsSeparator(IntroductorySeparators);
		}

		protected override bool IsSubSentence(string text, bool isFullSentence, bool isLast)
		{
			if (isFullSentence)
				return false;
			return DictionaryResource.IsParenthesis(isLast ? text.TrimEnd(SentenceEnd) : text);
		}

		protected override SubSentenceType SubSentenceType
		{
			get { return SubSentenceType.Introductory; }
		}
		#endregion
	}
}
