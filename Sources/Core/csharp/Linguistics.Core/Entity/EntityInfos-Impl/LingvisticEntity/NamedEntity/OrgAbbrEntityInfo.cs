using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация об аббревиатуре
	/// </summary>
    [Serializable]
    public class OrgAbbrEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public OrgAbbrEntityInfo()
			: base(EntityType.OrgAbbr)
		{
		}
		#endregion
	}
}
