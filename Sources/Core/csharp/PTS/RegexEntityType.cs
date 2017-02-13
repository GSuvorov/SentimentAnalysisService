using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PTS
{
	static class RegexEntityType
	{
		#region [.const.]
		public const string StandartNumber = "StandartNumber";
		public const string NumberWithDiacritical = "NumberWithDiacriticalCharacters";
		public const string Table = "Tables";
		public const string SimpleDiacritical = "SimpleDiacriticalCharacters";
		public const string Diacritical = "DiacriticalCharacters";
		public const string Fraction = "Fraction";
		public const string PatentNumber = "PatentNumber";
		#endregion

		public static bool IsNumberEntityType(string type)
		{
			return (type == StandartNumber) || (type == NumberWithDiacritical) || (type == Fraction) || (type == PatentNumber);
		}
	}
}
