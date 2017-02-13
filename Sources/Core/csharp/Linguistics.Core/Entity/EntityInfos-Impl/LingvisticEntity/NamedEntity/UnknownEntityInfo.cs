using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о прочем объекте
	/// </summary>
    [Serializable]
    public class UnknownEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public UnknownEntityInfo()
			: base(EntityType.Unknown)
		{
		}
		#endregion
	}
}
