using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Linguistics.Core.Morpho;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Однородные наречия
	/// </summary>
	internal class HomogeneousAdverbs : HomogeneousFormatBase
	{
		#region [.ctor().]
		public HomogeneousAdverbs()
			: base ("adverb", 2)
		{
		}
		#endregion

		#region [HomogeneousFormatBase]
		public override bool IsHomogeneousItem(Entity entity)
		{
			if (entity.IsType(EntityType.Adverb) && !DictionaryResource.IsParenthesis(entity.Text))
				return true;
			return false;
		}
		#endregion
	}
}
