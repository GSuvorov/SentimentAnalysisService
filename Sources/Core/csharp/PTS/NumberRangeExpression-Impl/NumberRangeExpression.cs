using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using SurfaceSyntaxAnalyzer;

namespace PTS
{
	/// <summary>
	/// Цифровой интервал
	/// </summary>
	internal class NumberRangeExpression : NumberRangeExpressionBase
	{
		#region [.ctor().]
		public NumberRangeExpression()
			: base("(?<start>numeric) (other )?(-|÷|((, )?\\.\\.\\.)( ,)?) ([-+] )?(?<end>numeric)")
		{
		}
		#endregion

		#region [NumberRangeExpressionBase]
		protected override NumberRange GetNumberRange(Match match, UnitCoderBase coder)
		{
			var start = GetNumber(match.Groups["start"], coder);
			var end = GetNumber(match.Groups["end"], coder);
			return new NumberRange()
			{
				Start = new NumberValue(start.Value),
				End = new NumberValue(end.Value)
			};
		}
		#endregion
	}
}
