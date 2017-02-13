using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PTS
{
	public class TableBlock
	{
		public int StartPosition { get; set; }

		public string[] Rows { get; private set; }

		#region [.ctor().]
		private TableBlock()
		{
		}
		#endregion

		public static TableBlock Create(string xmlText, int position)
		{
			XElement xml = XElement.Parse(xmlText);
			return new TableBlock() { StartPosition = position, Rows = SelectRows(xml) };
		}

		private static string[] SelectRows(XElement element)
		{
			List<string> result = new List<string>();
			foreach (XElement child in element.Elements())
			{
				if (child.Name == "row")
					result.Add(child.ToString());
				else
					result.AddRange(SelectRows(child));
			}
			return result.ToArray();
		}
	}
}
