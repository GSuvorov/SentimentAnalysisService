using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Однородные местоимения
	/// </summary>
	internal class HomogeneousPronouns : HomogeneousFormatBase
	{
		#region [.ctor().]
		public HomogeneousPronouns()
			: base("pronoun", 2)
		{
		}
		#endregion

		#region [HomogeneousFormatBase]
		public override bool IsHomogeneousItem(Entity entity)
		{
			if (!entity.IsType(EntityType.Pronoun))
				throw new ArgumentException();
			return true;
		}
		#endregion
	}
}
