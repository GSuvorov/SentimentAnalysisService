using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Однородные сущности с предлогами
	/// </summary>
	internal class HomogeneousEntitiesWithPretext : HomogeneousFormatBase
	{
		private string _pretext;

		#region [.ctor().]
		public HomogeneousEntitiesWithPretext()
			: base("pretext entity", 2)
		{
		}
		#endregion

		#region [HomogeneousFormatBase]
		public override void InitializeChain()
		{
			_pretext = null;
		}

		public override bool IsHomogeneousItem(Entity entity)
		{
			if (entity.IsType(EntityType.Pronoun) || entity.IsType(EntityType.Adverb) || (entity.UnitTextType != UnitTextType.ENTITY))
				throw new ArgumentException();
			return true;
		}

		public override bool IsAvailable(UnitTextBase unit)
		{
			if (unit.IsPretext())
				return unit.Text.IsSameValue(ref _pretext);
			return true;
		}
		#endregion
	}
}
