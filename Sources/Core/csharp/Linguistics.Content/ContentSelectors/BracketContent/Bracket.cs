using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core.ContentSelectors
{
	/// <summary>
	/// Тип скобки
	/// </summary>
	enum BracketType
	{
		/// <summary>
		/// Круглая
		/// </summary>
		Circular,
		/// <summary>
		/// Фигурная
		/// </summary>
		Figural,
		/// <summary>
		/// Квадратная
		/// </summary>
		Square
	}

	/// <summary>
	/// Скобка
	/// </summary>
	internal class Bracket
	{
		/// <summary>
		/// Тип скобки
		/// </summary>
		public BracketType Type { get; private set; }
		/// <summary>
		/// Флаг открывающей скобки: true - открывающая, false - закрывающая
		/// </summary>
		public bool IsOpen { get; private set; }

		#region [.ctor().]
		private Bracket(BracketType type, bool isOpen)
		{
			Type = type;
			IsOpen = isOpen;
		}
		#endregion

		/// <summary>
		/// Создание скобки из символа
		/// </summary>
		/// <param name="symbol">символ</param>
		/// <returns>скобка</returns>
		public static Bracket Create(char symbol)
		{
			Bracket result = null;
			switch (symbol)
			{
				case '(':
					result = new Bracket(BracketType.Circular, true);
					break;
				case ')':
					result = new Bracket(BracketType.Circular, false);
					break;
				case '[':
					result = new Bracket(BracketType.Square, true);
					break;
				case ']':
					result = new Bracket(BracketType.Square, false);
					break;
				case '{':
					result = new Bracket(BracketType.Figural, true);
					break;
				case '}':
					result = new Bracket(BracketType.Figural, false);
					break;
			}
			return result;
		}
	}
}
