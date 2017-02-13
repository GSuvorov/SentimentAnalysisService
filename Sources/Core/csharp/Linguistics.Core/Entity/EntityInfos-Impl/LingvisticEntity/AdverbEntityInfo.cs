using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о наречии
	/// </summary>
    [Serializable]
    public class AdverbEntityInfo : LingvisticEntityInfoBase
	{
		#region [.ctor().]
		public AdverbEntityInfo()
			: base(EntityType.Adverb)
		{
		}
		#endregion
	}
}
