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
	public class EventEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public EventEntityInfo()
			: base(EntityType.Event)
		{
		}
		#endregion
	}
}
