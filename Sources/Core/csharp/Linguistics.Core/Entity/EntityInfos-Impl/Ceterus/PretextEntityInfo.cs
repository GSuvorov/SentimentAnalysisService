using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о предлоге
	/// </summary>
    [Serializable]
    public class PretextEntityInfo : CeterusEntityInfoBase
	{
		#region [.ctor().]
		public PretextEntityInfo()
			: base(EntityType.Pretext)
		{
		}
		#endregion
	}
}
