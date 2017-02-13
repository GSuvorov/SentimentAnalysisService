using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core.Morpho;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о глаголе
	/// </summary>
    [Serializable]
    public class VerbEntityInfo : VerbFormEntityInfoBase
	{
		#region [.ctor().]
		public VerbEntityInfo()
			: base(EntityType.Verb)
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
			result.SetTransitionalFromCode(morpho[MorphoInfo.Transitional]);
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
			result.Append(morpho.GetPropertyCode(morpho.Transitional));
			result.Append(morpho.GetPropertyCode(morpho.Face));
			result.Append(morpho.GetPropertyCode(morpho.Voice));
			return result.ToString();
		}
		#endregion
	}
}
