using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// декоратор для выделения составных глаголов
	/// </summary>
	internal class CompoundVerbDecorator : SyntaxAnalyzerDecoratorBase
	{
		/// <summary>
		/// Преобразователь коллекции юнитов
		/// </summary>
		private static UnitCollectionTransformerBase _transformer = new CompoundVerbTransformer();

		#region [.ctor().]
		public CompoundVerbDecorator(SyntaxAnalyzerBase component)
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
