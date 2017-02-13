using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о служебных сущностях
	/// </summary>
    [Serializable]
    public abstract class CeterusEntityInfoBase : StandartEntityInfoBase
	{
		public override UnitTextType UnitTextType { get { return UnitTextType.CET; } }

		#region [.ctor().]
		protected CeterusEntityInfoBase(EntityType type)
			: base(type)
		{
		}
		#endregion
	}
}
