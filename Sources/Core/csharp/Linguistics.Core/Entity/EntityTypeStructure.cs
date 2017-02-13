using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
    [Serializable]
	public class EntityTypeStructure
	{
		public EntityType? EntityType { get; set; }

		public string CustomType { get; set; }

		public string GetFullType()
		{
			if (EntityType.HasValue)
				return EntityType.Value.ToString();
			return CustomType;
		}
	}
}
