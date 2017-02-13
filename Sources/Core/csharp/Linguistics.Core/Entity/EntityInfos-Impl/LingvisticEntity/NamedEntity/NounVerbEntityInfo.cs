using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о прочем объекте с эвристически определенным отглагольным главным существительным
	/// </summary>
    [Serializable]
    public class NounVerbEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public NounVerbEntityInfo()
			: base(EntityType.NounVerb)
		{
		}
		#endregion
	}
}
