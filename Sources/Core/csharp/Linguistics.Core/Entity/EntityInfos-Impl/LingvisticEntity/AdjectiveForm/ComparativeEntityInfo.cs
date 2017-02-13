using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
    /// <summary>
    /// Информация о сравнительной форме прилагательного
    /// </summary>
    [Serializable]
    public class ComparativeEntityInfo : AdjectiveFormEntityInfo
    {
        #region [.ctor().]
        public ComparativeEntityInfo()
			: base(EntityType.Comparative)
		{
		}
		#endregion
    }
}
