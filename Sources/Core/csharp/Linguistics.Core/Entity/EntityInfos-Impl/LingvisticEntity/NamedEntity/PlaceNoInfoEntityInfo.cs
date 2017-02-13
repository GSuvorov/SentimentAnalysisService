using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о типе географического объекта
	/// </summary>
    [Serializable]
    public class PlaceNoInfoEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public PlaceNoInfoEntityInfo()
			: base(EntityType.PlaceNoInfo)
		{
		}
		#endregion
	}
}
