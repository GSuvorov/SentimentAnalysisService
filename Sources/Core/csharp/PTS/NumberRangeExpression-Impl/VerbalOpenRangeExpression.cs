using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using SurfaceSyntaxAnalyzer;

namespace PTS
{
	/// <summary>
	/// Открытый словесный интервал
	/// </summary>
	internal class VerbalOpenRangeExpression : NumberRangeExpressionBase
	{
		#region [.ctor().]
		public VerbalOpenRangeExpression()
			: base("((to )?(about )?([-+] )?(?<number>numeric) (entity )?((entity|other|from) )?(or|and) (less|(?<great>great))(?! (than )?numeric))|" +
				   "((numeric or )?(?<not>not )?(less|to|(?<great>great)) (than )?((about )|(or equal to ))?([-+] )?(?<number>numeric))")
		{
		}
		#endregion

		#region [NumberRangeExpressionBase]
		protected override NumberRange GetNumberRange(Match match, UnitCoderBase coder)
		{
			var number = new NumberValue(GetNumber(match.Groups["number"], coder).Value);
			NumberRange result = new NumberRange();
			bool isGreat = match.Groups["great"].Success;
			bool isNot = match.Groups["not"].Success;
			if (isGreat ^ isNot)
				result.Start = number;
			else
				result.End = number;
			return result;
		}
		#endregion
	}
}
