using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Неразмеченный текст
	/// </summary>
	/// раньше - U
    [Serializable]
	public class UnmarkedText : UnitTextBase
	{
		public override UnitTextType UnitTextType
		{
			get { return (UnitTextType.U); }
		}

		#region [.ctor().]
		public UnmarkedText(string text, int startPosition)
			: base (text, startPosition)
		{
		}
		#endregion

		#region [ISerializeRDF]
		public override XElement ToXElement()
		{
			XElement result = new XElement(UnitTextType.U.ToString());
			result.Add(Text.GetCData());
			return result;
		}
		#endregion
	}
}
