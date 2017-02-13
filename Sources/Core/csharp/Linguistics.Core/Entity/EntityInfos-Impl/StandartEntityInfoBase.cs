using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core.Morpho;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о стандартной сущности
	/// </summary>
    [Serializable]
    public abstract class StandartEntityInfoBase : EntityInfoBase
	{
		#region [.ctor().]
		protected StandartEntityInfoBase(EntityType type)
			: base (new EntityTypeStructure() { EntityType = type })
		{
		}
		#endregion

		#region [.morpho.]
		public override MorphoInformation ParseMorphoInfo(string morpho)
		{
			MorphoInformation result = new MorphoInformation();
			if (!string.IsNullOrEmpty(morpho))
			{
				result.SetCaseFromCode(morpho[MorphoInfo.Case]);
				result.SetGenderFromCode(morpho[MorphoInfo.Gender]);
				result.SetNumberFromCode(morpho[MorphoInfo.Number]);
				result.SetAnimationFromCode(morpho[MorphoInfo.Animation]);
				result.SetFaceFromCode(morpho[MorphoInfo.Face]);
			}
			return result;
		}

		public override string MorphoInfoToString(MorphoInformation morpho)
		{
			StringBuilder result = new StringBuilder();
			result.Append(morpho.GetPropertyCode(morpho.Case));
			result.Append(morpho.GetPropertyCode(morpho.Gender));
			result.Append(morpho.GetPropertyCode(morpho.Number));
			result.Append(morpho.GetPropertyCode(morpho.Animation));
			result.Append(morpho.GetPropertyCode(morpho.Face));
			return result.ToString();
		}
		#endregion

		#region [ISerializerToRDF]
		public override XElement ToXElement()
		{
			return new XElement(UnitTextType.ToString());
		}
		#endregion
	}
}
