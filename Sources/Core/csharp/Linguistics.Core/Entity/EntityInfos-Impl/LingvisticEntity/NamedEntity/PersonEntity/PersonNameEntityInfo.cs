using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о физическом лице с фамилией
	/// </summary>
    [Serializable]
    public class PersonNameEntityInfo : PersonEntityInfoBase
	{
		#region [.ctor().]
		public PersonNameEntityInfo()
			: base(EntityType.PersonName)
		{
		}
		#endregion
	}
}
