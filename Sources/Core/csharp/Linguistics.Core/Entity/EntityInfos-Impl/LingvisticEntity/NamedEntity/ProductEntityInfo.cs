using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация об организации
	/// </summary>
	[Serializable]
	public class ProductEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public ProductEntityInfo()
			: base(EntityType.Product)
		{
		}
		#endregion
	}
}
