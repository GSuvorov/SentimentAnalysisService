using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о кратком прилагательном
	/// </summary>
    [Serializable]
    public class ShortAdjectiveEntityInfo : AdjectiveFormEntityInfo
	{
		#region [.ctor().]
		public ShortAdjectiveEntityInfo()
			: base(EntityType.ShortAdjective)
		{
		}
		#endregion
	}
}
