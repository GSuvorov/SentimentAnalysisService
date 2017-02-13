using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Интерфейс информации о расположении юнита
	/// </summary>
	public interface IUnitPositionInfo
	{
		/// <summary>
		/// Начальная позиция в тексте
		/// </summary>
		int Start { get; }
		/// <summary>
		/// Конечная позиция в тексте
		/// </summary>
		int End { get; }
	}
}
