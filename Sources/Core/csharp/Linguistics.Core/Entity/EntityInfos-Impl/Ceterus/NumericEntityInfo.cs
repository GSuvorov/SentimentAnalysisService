using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о числе
	/// </summary>
    [Serializable]
    public class NumericEntityInfo : CeterusEntityInfoBase
	{
		#region [.ctor().]
		public NumericEntityInfo()
			: base(EntityType.Numeric)
		{
		}
		#endregion

        #region [ISerializerToRDF]
        public override XElement ToXElement()
        {
            XElement result = base.ToXElement();
            if ((Morpho != null) && Morpho.Any())
                AddMorphoInfoAttribute(result);
            return result;
        }
        #endregion
	}
}
