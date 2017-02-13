using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Преобразователь, который объединяет юниты неразмеченного текста в один общий юнит 
	/// </summary>
	internal class UnmarkedUnionTransformer : UnitCollectionTransformerBase
	{
		#region [UnitCollectionTransformerBase]
		protected override bool IsTransformAvailableUnit(UnitTextBase unit, IList<UnitTextBase> sequence)
		{
			return !unit.IsEntity;
		}

		protected override IEnumerable<UnitTextBase> TransformUnitsInSequence(IList<UnitTextBase> units)
		{
			List<UnitTextBase> result = new List<UnitTextBase>(1);
			if (units.Count == 1)
				result.Add(units.First());
			else
			{
				List<UnitTextBase> interval = new List<UnitTextBase>();
				int position = units.First().PositionInfo.Start;
				foreach (var unit in units)
				{
					if (unit.PositionInfo.Start == position)
						interval.Add(unit);
					else
					{
						result.Add(new UnmarkedText(interval.GetText(), interval.First().PositionInfo.Start));
						interval.Clear();
						interval.Add(unit);
					}
					position = unit.PositionInfo.End;
				}
				result.Add(new UnmarkedText(interval.GetText(), interval.First().PositionInfo.Start));
			}
			return result;
		}
		#endregion
	}
}
