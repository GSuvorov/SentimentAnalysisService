using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// декоратора для объединения юнитов неразмеченного текста
	/// </summary>
	internal class UnmarkedUnionDecorator : SyntaxAnalyzerDecoratorBase
	{
		/// <summary>
		/// Преобразователь коллекции юнитов
		/// </summary>
		private static UnitCollectionTransformerBase _transformer = new UnmarkedUnionTransformer();

		#region [.ctor().]
		public UnmarkedUnionDecorator(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

		#region [SyntaxAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			SubSentence[] result = base.AnalyzeSubSentence(subSentence);
			result.ForAllHierarchy(UnionUnmarked);
			return result;
		}
		#endregion

		/// <summary>
		/// Объединение элементов неразмеченного текста в заданном подпредложении
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		private void UnionUnmarked(SubSentence subSentence)
		{
			subSentence.SetUnits(_transformer.Transform(subSentence.Units));
		}
	}
}
