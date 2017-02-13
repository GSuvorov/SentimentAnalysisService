using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// декоратор дял выделения союзов и союзных слов
	/// </summary>
	internal class ConjunctionDecorator : SyntaxAnalyzerDecoratorBase
	{
		/// <summary>
		/// Преобразователь коллекции юнитов
		/// </summary>
		private static UnitCollectionTransformerBase _transformer = new ConjunctionTransformer();

		#region [.ctor().]
		public ConjunctionDecorator(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

		#region [SyntaxAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			subSentence.SetUnits(_transformer.Transform(subSentence.Units));
			return base.AnalyzeSubSentence(subSentence);
		}
		#endregion
	}
}
