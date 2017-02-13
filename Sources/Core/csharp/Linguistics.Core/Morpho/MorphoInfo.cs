using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// класс для доступа к массиву морфо-свойств
	/// </summary>
    [Serializable]
	public class MorphoInfo
	{
		/// <summary>
		/// падеж
		/// </summary>
		public static int Case { get { return 0; } }

		/// <summary>
		/// род
		/// </summary>
		public static int Gender { get { return 1; } }

		/// <summary>
		/// число
		/// </summary>
		public static int Number { get { return 2; } }

		/// <summary>
		/// одушевленность
		/// </summary>
		public static int Animation { get { return 3; } }

		/// <summary>
		/// переходность глагола
		/// </summary>
		public static int Transitional { get { return 3; } }

		/// <summary>
		/// лицо
		/// </summary>
		public static int Face { get { return 4; } }

		/// <summary>
		/// залог
		/// </summary>
		public static int Voice { get { return 5; } }
	}
}
