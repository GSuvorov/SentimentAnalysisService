using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core.Morpho;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о форме глагола
	/// </summary>
    [Serializable]
    public abstract class VerbFormEntityInfoBase : StandartEntityInfoBase
	{
		public override UnitTextType UnitTextType { get { return UnitTextType.VERB; } }

		#region [.ctor().]
		protected VerbFormEntityInfoBase(EntityType type)
			: base(type)
		{
		}
		#endregion

		#region [.morpho.]
		public override MorphoInformation ParseMorphoInfo(string morpho)
		{
			MorphoInformation result = new MorphoInformation();
			result.SetCaseFromCode(morpho[MorphoInfo.Case]);
			result.SetGenderFromCode(morpho[MorphoInfo.Gender]);
			result.SetNumberFromCode(morpho[MorphoInfo.Number]);
			result.SetAnimationFromCode(morpho[MorphoInfo.Animation]);
			result.SetFaceFromCode(morpho[MorphoInfo.Face]);
			result.SetVoiceFromCode(morpho[MorphoInfo.Voice]);
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
			result.Append(morpho.GetPropertyCode(morpho.Voice));
			return result.ToString();
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
