using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Lingvistics.Types
{
	/// <summary>
	/// Тема
	/// </summary>
	public sealed class ThemeItem
	{
		/// <summary>
		/// Идентификатор
		/// </summary>
		public int ID { get; set; }
		/// <summary>
		/// Нормализованное значение
		/// </summary>
		public string Name { get; set; }
		/// <summary>
		/// Исходная форма
		/// </summary>
		public string OriginalName { get; set; }
		/// <summary>
		/// Семантический тип
		/// </summary>
		public string Type { get; set; }
		/// <summary>
		/// Частота в роли подлежащего
		/// </summary>
		public int FreqSubj { get; set; }
		/// <summary>
		/// Частота в роли дополнения
		/// </summary>
		public int FreqObj { get; set; }
		/// <summary>
		/// Частота в роли обстоятельства
		/// </summary>
		public int FreqAdj { get; set; }
		/// <summary>
		/// Частота внутренней цепочки
		/// </summary>
		public int FreqOther { get; set; }
		public int Freq
		{
			get
			{
				return FreqSubj + FreqObj + FreqAdj + FreqOther;
			}
		}
	}
}
