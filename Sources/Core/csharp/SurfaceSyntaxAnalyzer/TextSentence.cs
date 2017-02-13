using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Предложение текста
	/// </summary>
	internal class TextSentence
	{
		/// <summary>
		/// Список подпредложений
		/// </summary>
		public List<SubSentence> SubSentences { get; set; }

		#region [.ctor().]
		public TextSentence(SubSentence[] subSentences)
		{
			SubSentences = new List<SubSentence>(subSentences);
		}
		#endregion
	}
}
