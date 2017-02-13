using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация об организации
	/// </summary>
    [Serializable]
    public class OrgNameEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public OrgNameEntityInfo()
			: base(EntityType.OrgName)
		{
		}
		#endregion
	}
}
