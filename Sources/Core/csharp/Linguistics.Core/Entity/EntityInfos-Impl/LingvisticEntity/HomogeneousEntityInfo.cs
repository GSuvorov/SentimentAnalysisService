using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация об списке однородных сущностей
	/// </summary>
	[Serializable]
	public class HomogeneousEntityInfo : LingvisticEntityInfoBase
	{
		/// <summary>
		/// Предлог
		/// </summary>
		public UnitTextBase Pretext { get; set; }

		#region [.ctor().]
		public HomogeneousEntityInfo()
			: base(EntityType.Homogeneous)
		{
		}
		#endregion

		#region [ISerializerToRDF]
		public override XElement ToXElement()
		{
			XElement result = base.ToXElement();
			AddMorphoInfoAttribute(result);
			return result;
		}
		#endregion
	}
}
