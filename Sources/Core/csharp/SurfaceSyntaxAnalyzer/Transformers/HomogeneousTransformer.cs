using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Utils;
using SurfaceSyntaxAnalyzer.Homogeneous;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Декоратор для выделения однородных членов
	/// </summary>
	internal class HomogeneousTransformer : UnitCollectionTransformerBase
	{
		/// <summary>
		/// Форматы выделения однородных членов
		/// </summary>
		private static readonly List<HomogeneousFormatBase> _formats;

		#region [.ctor().]
		static HomogeneousTransformer()
		{
			_formats = new List<HomogeneousFormatBase>();
			_formats.Add(new HomogeneousAdverbs());
			_formats.Add(new HomogeneousAdjectives());
			_formats.Add(new HomogeneousPronouns());
			_formats.Add(new HomogeneousEntitiesWithNeg());
			_formats.Add(new HomogeneousEntitiesWithNegAndPretext());
			_formats.Add(new HomogeneousEntitiesWithPretext());
			_formats.Add(new HomogeneousEntities());
		}
		#endregion

		#region [UnitCollectionTransformerBase]
		protected override bool IsTransformAvailableUnit(UnitTextBase unit, IList<UnitTextBase> sequence)
		{
			return (unit.UnitTextType == UnitTextType.ENTITY) || (unit.UnitTextType == UnitTextType.CET) ||
				(unit.UnitTextType == UnitTextType.U);
		}

		protected override IEnumerable<UnitTextBase> TransformUnitsInSequence(IList<UnitTextBase> unitCollection)
		{
			List<UnitTextBase> result = new List<UnitTextBase>(unitCollection);

			foreach (HomogeneousFormatBase format in _formats)
				format.ReplaceWithHomogeneous(result);

			return result;
		}
		#endregion
	}
}
