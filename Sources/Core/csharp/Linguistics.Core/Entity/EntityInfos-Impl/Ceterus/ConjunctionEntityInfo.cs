using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о союзе
	/// </summary>
    [Serializable]
    public class ConjunctionEntityInfo : CeterusEntityInfoBase
	{
		/// <summary>
		/// Тип союза
		/// </summary>
		public SubSentenceType ConjunctionType { get; set; }

		#region [.ctor().]
		public ConjunctionEntityInfo(SubSentenceType conjunctionType)
			: base(EntityType.Conjunction)
		{
			ConjunctionType = conjunctionType;
		}
		#endregion

		#region [ISerializerToRDF]
		public override XElement ToXElement()
		{
			XElement result = base.ToXElement();
			if ((Morpho != null) && Morpho.Any())
				AddMorphoInfoAttribute(result);
			return result;
		}
		#endregion
	}
}
