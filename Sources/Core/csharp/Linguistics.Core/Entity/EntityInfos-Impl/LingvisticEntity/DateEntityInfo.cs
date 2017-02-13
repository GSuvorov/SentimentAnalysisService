using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о дате
	/// </summary>
    [Serializable]
    public class DateEntityInfo : LingvisticEntityInfoBase
	{
		#region [.ctor().]
		public DateEntityInfo()
			: base(EntityType.Date)
		{
		}
		#endregion
	}
}
