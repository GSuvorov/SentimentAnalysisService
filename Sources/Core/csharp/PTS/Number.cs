using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using System.Xml.Linq;

namespace PTS
{
	/// <summary>
	/// Конкретное число
	/// </summary>
	internal class Number : NumberRange
	{
		/// <summary>
		/// Значение числа
		/// </summary>
		public NumberValue Value { get; set; }

		#region [.ctor().]
		private Number()
		{
		}
		#endregion

		/// <summary>
		/// Создание числа из заданной сущности
		/// </summary>
		/// <param name="numeric">сущность</param>
		/// <returns></returns>
		public static Number Create(Entity numeric)
		{
			#region [.defense.]
			if (!numeric.IsNumber())
				throw new ArgumentException("entity isn't number");
			#endregion
			Number result = new Number()
			{
				Text = numeric.Text,
				StartPosition = numeric.PositionInfo.Start,
				EndPosition = numeric.PositionInfo.End
			};
			try
			{
				double value = NumberParser.Parse(numeric.Value);
				result.Value = new NumberValue(numeric.Value.Trim());
			}
			catch (FormatException)
			{
				result = null;
			}
			return result;
		}

		#region [NumberRange]
		public override XElement GetRangeXElement()
		{
			XElement result = new XElement("root");
			result.Add(GetXElement("val", Value));
			return result;
		}
		#endregion
	}
}
