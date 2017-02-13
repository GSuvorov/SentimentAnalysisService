using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о неидентифицированном объекте
	/// </summary>
    [Serializable]
    public class UnknownProperEntityInfo : NamedEntityInfoBase
	{
		#region [.ctor().]
		public UnknownProperEntityInfo()
			: base(EntityType.UnknownProper)
		{
		}
		#endregion

		#region [ISerializerToRDF]
		public override XElement ToXElement()
		{
			XElement result = base.ToXElement();
			if (IsPseudoPlace)
				result.SetAttributeValue(RDF.Attribute.PseudoPlace, "");
			return result;
		}
		#endregion
	}
}
