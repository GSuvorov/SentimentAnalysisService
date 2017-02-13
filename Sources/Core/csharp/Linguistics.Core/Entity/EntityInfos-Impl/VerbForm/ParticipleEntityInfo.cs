using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о причастии
	/// </summary>
    [Serializable]
    public class ParticipleEntityInfo : VerbFormEntityInfoBase
	{
		#region [.ctor().]
		public ParticipleEntityInfo()
			: base(EntityType.Participle)
		{
		}
		#endregion
	}
}
