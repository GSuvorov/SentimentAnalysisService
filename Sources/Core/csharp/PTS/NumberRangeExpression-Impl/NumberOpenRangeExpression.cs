using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using SurfaceSyntaxAnalyzer;

namespace PTS
{
	/// <summary>
	/// Открытый числовой интервал
	/// </summary>
	internal class NumberOpenRangeExpression : NumberRangeExpressionBase
	{
		#region [.ctor().]
		public NumberOpenRangeExpression()
			: base("((?<great>(>))|(?<less>(<))) ([-+] )?(?<number>numeric)")
		{
		}
		#endregion

		#region [NumberRangeExpressionBase]
		protected override NumberRange GetNumberRange(Match match, UnitCoderBase coder)
		{
			var number = new NumberValue(GetNumber(match.Groups["number"], coder).Value);
			NumberRange result = new NumberRange();
			if (match.Groups["great"].Success)
				result.Start = number;
			else
				result.End = number;
			return result;
		}
		#endregion
	}
}
