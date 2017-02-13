using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Однородные прилагательные
	/// </summary>
	internal class HomogeneousAdjectives : HomogeneousFormatBase
	{
		#region [.ctor().]
		public HomogeneousAdjectives()
			: base ("adjective", 2)
		{
		}
		#endregion

		#region [HomogeneousFormatBase]
		public override bool IsHomogeneousItem(Entity entity)
		{
			if (entity.IsType(EntityType.Adjective) || entity.IsType(EntityType.Superlative))
				return true;
			return false;
		}
		#endregion
	}
}
