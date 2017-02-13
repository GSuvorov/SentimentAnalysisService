using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о прилагательном
	/// </summary>
    [Serializable]
    public class AdjectiveEntityInfo : AdjectiveFormEntityInfo
	{
		#region [.ctor().]
		public AdjectiveEntityInfo()
			: base(EntityType.Adjective)
		{
		}
		#endregion
	}
}
