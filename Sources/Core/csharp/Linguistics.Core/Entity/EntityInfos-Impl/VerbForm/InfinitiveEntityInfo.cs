using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
    /// <summary>
    /// Информация об инфинитиве
    /// </summary>
    [Serializable]
    public class InfinitiveEntityInfo : VerbFormEntityInfoBase
    {
        #region [.ctor().]
        public InfinitiveEntityInfo()
			: base(EntityType.Infinitive)
		{
		}
		#endregion
    }
}
