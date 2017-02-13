using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о местоимении
	/// </summary>
    [Serializable]
    public class PronounEntityInfo : LingvisticEntityInfoBase
	{
		#region [.ctor().]
		public PronounEntityInfo()
			: base(EntityType.Pronoun)
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
