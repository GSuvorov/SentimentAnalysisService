using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using SurfaceSyntaxAnalyzer;

namespace PTS
{
	/// <summary>
	/// Словесный интервал
	/// </summary>
	internal class VerbalRangeExpression : NumberRangeExpressionBase
	{
		#region [.ctor().]
		public VerbalRangeExpression()
            : base("((?<from>(from )?(about )?([-+] )?(?<start>numeric) ((entity )?(other )?(\\. )?((entity|other|from) )?(\\. )?))(to|po) (less than )?(about )?([-+] )?(?<end>numeric))|" +
                   "((?<from>(between )(about )?([-+] )?(?<start>numeric)) (((entity|other) )?(other )?(\\. )?((entity|other|from) )?(\\. )?)and (about )?([-+] )?(?<end>numeric))")
		{
		}
		#endregion

		#region [NumberRangeExpressionBase]
		protected override NumberRange GetNumberRange(Match match, UnitCoderBase coder)
		{
			NumberRange result = new NumberRange();
			result.End = new NumberValue(GetNumber(match.Groups["end"], coder).Value);
			Group startGroup = match.Groups["from"];
			if (startGroup.Success)
				result.Start = new NumberValue(GetNumber(match.Groups["start"], coder).Value);
			return result;
		}
		#endregion
	}
}
