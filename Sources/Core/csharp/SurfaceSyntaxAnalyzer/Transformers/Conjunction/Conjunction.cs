using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Союз
	/// </summary>
	internal class Conjunction
	{
		/// <summary>
		/// значение
		/// </summary>
		public string Value { get; set; }
		/// <summary>
		/// начало
		/// </summary>
		public int StartPosition { get; set; }
		/// <summary>
		/// конец
		/// </summary>
		public int EndPosition { get; set; }
		/// <summary>
		/// Тип союза
		/// </summary>
		public SubSentenceType Type { get; set; }
	}
}
