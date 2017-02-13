using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core.Morpho
{
	/// <summary>
	/// Падеж
	/// </summary>
	public enum Case
	{
		/// <summary>
		/// Не определено
		/// </summary>
		Undefined,
		/// <summary>
		/// Именительный
		/// </summary>
		Nominative,
		/// <summary>
		/// Родительный
		/// </summary>
		Genetive,
		/// <summary>
		/// Дательный
		/// </summary>
		Dative,
		/// <summary>
		/// Винительный
		/// </summary>
		Accusative,
		/// <summary>
		/// Творительный
		/// </summary>
		Instrumental,
		/// <summary>
		/// Предложный
		/// </summary>
		Locative
	}
}
