using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Преобразователь, который выделяет отдельные текстовые элементы
	/// </summary>
	internal class IndividualTextItemsTransformer : UnitCollectionTransformerBase
	{
		#region [UnitCollectionTransformerBase]
		protected override bool IsTransformAvailableUnit(UnitTextBase unit, IList<UnitTextBase> sequence)
		{
			return (unit.UnitTextType == UnitTextType.U);
		}

		protected override IEnumerable<UnitTextBase> TransformUnitsInSequence(IList<UnitTextBase> units)
		{
			List<UnitTextBase> result = new List<UnitTextBase>();
			foreach (UnitTextBase unit in units)
			{
				var separators = DictionaryResource.SelectIndividualTextItems(unit).ToArray();
				if (separators.Any())
					result.AddRange(unit.SplitUnit(separators));
				else
					result.Add(unit);
			}
			return result;
		}
		#endregion
	}
}
