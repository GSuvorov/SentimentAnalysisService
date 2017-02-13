using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о деепричастии
	/// </summary>
    [Serializable]
    public class DeeprEntityInfo : VerbFormEntityInfoBase
	{
		#region [.ctor().]
		public DeeprEntityInfo()
			: base(EntityType.Deepr)
		{
		}
		#endregion
	}
}
