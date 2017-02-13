using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Базовый класс для анализаторов предложения
	/// </summary>
	internal abstract class SyntaxAnalyzerBase
	{
		/// <summary>
		/// Разбор заданного подпредложения
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>выделенные подпредложения</returns>
		public abstract SubSentence[] AnalyzeSubSentence(SubSentence subSentence);
	}
}
