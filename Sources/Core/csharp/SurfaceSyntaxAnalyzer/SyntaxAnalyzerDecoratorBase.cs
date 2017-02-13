using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// базовый класс для декораторов анализатора предложений
	/// </summary>
	internal abstract class SyntaxAnalyzerDecoratorBase : SyntaxAnalyzerBase
	{
		private SyntaxAnalyzerBase _component;

		#region [.ctor().]
		public SyntaxAnalyzerDecoratorBase(SyntaxAnalyzerBase component)
		{
			_component = component;
		}
		#endregion

		#region [SentenceAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			return _component.AnalyzeSubSentence(subSentence);
		}
		#endregion
	}
}
