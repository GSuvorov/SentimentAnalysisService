using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о лингвистической сущности
	/// </summary>
    [Serializable]
    public abstract class LingvisticEntityInfoBase : StandartEntityInfoBase
	{
		public override UnitTextType UnitTextType { get { return UnitTextType.ENTITY; } }

		#region [.ctor().]
		protected LingvisticEntityInfoBase(EntityType type)
			: base(type)
		{
		}
		#endregion
	}
}
