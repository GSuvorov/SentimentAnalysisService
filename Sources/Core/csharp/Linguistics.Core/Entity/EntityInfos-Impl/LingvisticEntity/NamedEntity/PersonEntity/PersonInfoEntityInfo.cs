using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о физическом лице
	/// </summary>
    [Serializable]
    public class PersonInfoEntityInfo : PersonEntityInfoBase
	{
		#region [.ctor().]
		public PersonInfoEntityInfo()
			: base(EntityType.PersonInfo)
		{
		}
		#endregion
	}
}
