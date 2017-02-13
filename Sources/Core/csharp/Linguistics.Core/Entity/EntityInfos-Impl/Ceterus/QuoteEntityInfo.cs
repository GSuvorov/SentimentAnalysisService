using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о кавычке
	/// </summary>
    [Serializable]
    public class QuoteEntityInfo : CeterusEntityInfoBase
	{
		#region [.ctor().]
		public QuoteEntityInfo()
			: base(EntityType.Quote)
		{
		}
		#endregion
	}
}
