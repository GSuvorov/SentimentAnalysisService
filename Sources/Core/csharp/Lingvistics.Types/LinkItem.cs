using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Lingvistics.Types
{
	/// <summary>
	/// Связь
	/// </summary>
	public sealed class LinkItem
	{
		/// <summary>
		/// Идентификатор первой темы
		/// </summary>
		public int SourceThemeID { get; set; }
		/// <summary>
		/// Идентификатор второй темы
		/// </summary>
		public int DestThemeID { get; set; }
		/// <summary>
		/// Тип связи
		/// </summary>
		public string Type { get; set; }
		/// <summary>
		/// Частота связи
		/// </summary>
		public int Freq { get; set; }
	}
}
