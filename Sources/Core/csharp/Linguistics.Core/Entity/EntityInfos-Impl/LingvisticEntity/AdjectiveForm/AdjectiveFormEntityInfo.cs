using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
    /// <summary>
    /// Информация о форме прилагательного
    /// </summary>
    [Serializable]
    public abstract class AdjectiveFormEntityInfo : LingvisticEntityInfoBase
    {
        #region [.ctor().]
        protected AdjectiveFormEntityInfo(EntityType type)
			: base(type)
		{
		}
		#endregion

        #region [ISerializerToRDF]
        public override XElement ToXElement()
        {
            XElement result = base.ToXElement();
            AddMorphoInfoAttribute(result);
            return result;
        }
        #endregion
    }
}
