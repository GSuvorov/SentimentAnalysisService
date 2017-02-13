using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Однородные сущности
	/// </summary>
	internal class HomogeneousEntities : HomogeneousFormatBase
	{
		#region [.ctor().]
		public HomogeneousEntities()
			: base ("entity", 3)
		{
		}
		#endregion

		#region [HomogeneousFormatBase]
		public override bool IsHomogeneousItem(Entity entity)
		{
			return true;
		}
		#endregion
	}
}
