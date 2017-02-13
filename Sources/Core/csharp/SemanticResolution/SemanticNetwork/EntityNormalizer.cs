using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Lingvistics
{
	/// <summary>
	/// Нормализатор сущностей
	/// </summary>
	public static class EntityNormalizer
	{
		/// <summary>
		/// Нормализация значения сущности
		/// </summary>
		/// <param name="value">значение сущности</param>
		/// <returns>нормализованное значение сущности</returns>
		public static string Normalize(string value)
		{
			StringBuilder sb = new StringBuilder();
			/// удаление пунктуации
			bool space = false;
			foreach (char ch in value)
			{
				if (Char.IsLetterOrDigit(ch) || ch == '-' || ch == '_')
				{
					if (space)
					{
						sb.Append(' ');
						space = false;
					}
					sb.Append(ch);
				}
				else if (sb.Length > 0)
					space = true;
			}
			return sb.ToString();
		}
	}
}
