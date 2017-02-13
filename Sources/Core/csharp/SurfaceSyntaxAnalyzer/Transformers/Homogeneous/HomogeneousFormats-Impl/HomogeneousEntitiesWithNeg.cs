using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Однородные сущности с частицами отрицания
	/// </summary>
	internal class HomogeneousEntitiesWithNeg : HomogeneousFormatBase
	{
		#region [.ctor().]
		public HomogeneousEntitiesWithNeg()
			: base ("neg( adverb)? entity")
		{
		}
		#endregion

		#region [HomogeneousFormatBase]
		public override bool IsHomogeneousItem(Entity entity)
		{
			if (entity.IsType(EntityType.Pronoun) || (entity.UnitTextType != UnitTextType.ENTITY))
				throw new ArgumentException();
			return !entity.IsType(EntityType.Adverb);
		}
		#endregion
	}
}
