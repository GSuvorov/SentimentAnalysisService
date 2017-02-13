using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Utils
{
	/// <summary>
	/// Часть текста
	/// </summary>
	public class TextPart
	{
		/// <summary>
		/// начало
		/// </summary>
		public int Start { get; set; }
		/// <summary>
		/// конец
		/// </summary>
		public int End { get; set; }

		/// <summary>
		/// Флаг, что заданный интервал корректен
		/// </summary>
		public bool Success { get { return End > Start; } }

		#region [.ctor().]
		public TextPart()
		{
			Start = -1;
			End = -1;
		}

		public TextPart(int start, int length)
		{
			Start = start;
			End = Start + length;
		}
		#endregion
	}
}
