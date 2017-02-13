using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core.Morpho;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о подключаемой сущности
	/// </summary>
    [Serializable]
	public class AdditionalEntityInfo : EntityInfoBase
	{
		#region [EntityInfoBase]
		public override UnitTextType UnitTextType { get { return UnitTextType.ENTITY; } }
		#endregion

		#region [.ctor().]
		public AdditionalEntityInfo(string type)
			: base (new EntityTypeStructure() { EntityType = null, CustomType = type })
		{
			Morpho = new MorphoInformation[] { new MorphoInformation() };
		}
		#endregion

		#region [.morpho.]
		public override MorphoInformation ParseMorphoInfo(string morpho)
		{
			return new MorphoInformation();
		}

		public override string MorphoInfoToString(MorphoInformation morpho)
		{
			return "";
		}
		#endregion

		#region [ISerializerToRDF]
		public override XElement ToXElement()
		{
			XElement result = new XElement(UnitTextType.ToString());
			AddMorphoInfoAttribute(result);
			return result;
		}
		#endregion
	}
}
