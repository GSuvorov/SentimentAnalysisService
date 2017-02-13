using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о предикативе
	/// </summary>
    [Serializable]
    public class PredicativeEntityInfo : CeterusEntityInfoBase
	{
		#region [.ctor().]
		public PredicativeEntityInfo()
			: base(EntityType.Predicative)
		{
		}
		#endregion
	}
}
