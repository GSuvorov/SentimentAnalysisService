using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о географическом объекте
	/// </summary>
    [Serializable]
    public class PlaceNameEntityInfo : NamedEntityInfoBase
	{
		public string LinkedPlaces { get; set; }

		#region [.ctor().]
		public PlaceNameEntityInfo()
			: base(EntityType.PlaceName)
		{
		}
		#endregion
	}
}
