using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о типе организации
	/// </summary>
    [Serializable]
    public class OrgNoInfoEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public OrgNoInfoEntityInfo()
			: base(EntityType.OrgNoInfo)
		{
		}
		#endregion
	}
}
