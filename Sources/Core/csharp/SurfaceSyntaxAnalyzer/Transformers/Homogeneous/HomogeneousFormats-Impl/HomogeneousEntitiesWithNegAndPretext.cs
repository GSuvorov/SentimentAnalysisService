using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Однородные сущности с частицами отрицания и предлогами
	/// </summary>
	internal class HomogeneousEntitiesWithNegAndPretext : HomogeneousFormatBase
	{
		#region [.ctor().]
		public HomogeneousEntitiesWithNegAndPretext()
			: base ("neg( adverb)? pretext entity")
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
