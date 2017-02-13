using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о профессии или должности
	/// </summary>
    [Serializable]
    public class PersonNoInfoEntityInfo : PersonEntityInfoBase
	{
		#region [.ctor().]
		public PersonNoInfoEntityInfo()
			: base(EntityType.PersonNoInfo)
		{
		}
		#endregion
	}
}
