using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// декоратор для выделения однородных членов
	/// </summary>
	internal class HomogeneousDecorator : SyntaxAnalyzerDecoratorBase
	{
		/// <summary>
		/// Преобразователь для выделения однородных членов
		/// </summary>
		private static UnitCollectionTransformerBase _homogeneousTransformer = new HomogeneousTransformer();

		/// <summary>
		/// преобразователь для выделения отдельных текстовых элементов
		/// </summary>
		private IndividualTextItemsTransformer _individualTransformer = new IndividualTextItemsTransformer();

		#region [.ctor().]
		public HomogeneousDecorator(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

		#region [SyntaxAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			subSentence.SetUnits(_homogeneousTransformer.Transform(_individualTransformer.Transform(subSentence.Units)));
			return base.AnalyzeSubSentence(subSentence);
		}
		#endregion
	}
}
