using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Linguistics.Core;
using SurfaceSyntaxAnalyzer;

namespace PTS
{
	/// <summary>
	/// Цифровой, задание погрешности
	/// </summary>
	internal class NumberErrorExpression : NumberRangeExpressionBase
	{
		#region [.ctor().]
		public NumberErrorExpression()
			: base("(?<base>numeric) ± (?<delta>numeric)")
		{
		}
		#endregion

		#region [NumberRangeExpressionBase]
		protected override NumberRange GetNumberRange(Match match, UnitCoderBase coder)
		{
			string baseNumber = GetNumber(match.Groups["base"], coder).Value;
			double baseValue = NumberParser.Parse(baseNumber);
			double delta = NumberParser.Parse(GetNumber(match.Groups["delta"], coder).Value);
			bool isExponential = baseNumber.ToUpper().Contains('E');
			return new NumberRange()
			{
				Start = new NumberValue(baseValue - delta) { IsExponential = isExponential },
				End = new NumberValue(baseValue + delta) { IsExponential = isExponential }
			};
		}
		#endregion
	}
}
