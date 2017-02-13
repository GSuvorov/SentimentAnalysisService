using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
    /// <summary>
    /// Информация о превосходной форме прилагательного
    /// </summary>
    [Serializable]
    public class SuperlativeEntityInfo : AdjectiveFormEntityInfo
    {
        #region [.ctor().]
        public SuperlativeEntityInfo()
			: base(EntityType.Superlative)
		{
		}
		#endregion
    }
}
