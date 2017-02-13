using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace PTS
{
	/// <summary>
	/// Интервал чисел
	/// </summary>
	public class NumberRange
	{
		/// <summary>
		/// Пустое началое интервала
		/// </summary>
		public readonly NumberValue EmptyStart = new NumberValue(0) { IsExponential = false };
		/// <summary>
		/// Пустой конец интервала
		/// </summary>
		public readonly NumberValue EmptyEnd = new NumberValue("1E38") { IsExponential = true };

		/// <summary>
		/// Начало интервала
		/// </summary>
		public NumberValue Start { get; set; }
		/// <summary>
		/// Конец интервала
		/// </summary>
		public NumberValue End { get; set; }

		/// <summary>
		/// Стартовая позиция в тексте
		/// </summary>
		public int StartPosition { get; set; }
		/// <summary>
		/// Конечная позиция в текста
		/// </summary>
		public int EndPosition { get; set; }
		/// <summary>
		/// Текстовое значение интервала
		/// </summary>
		public string Text { get; set; }

		/// <summary>
		/// Получение xml для интервала
		/// </summary>
		/// <returns>xml</returns>
		public virtual XElement GetRangeXElement()
		{
			XElement result = new XElement("root");
			if ((Start != null) && (End != null) && (Start.Value > End.Value))
			{
				NumberValue temp = End;
				End = Start;
				Start = temp;
			}
			#region [start]
			if (Start != null)
				result.Add(GetXElement("start", Start));
			else
				result.Add(GetXElement("start", EmptyStart));
			#endregion
			#region [end]
			if (End != null)
				result.Add(GetXElement("end", End));
			else
				result.Add(GetXElement("end", EmptyEnd));
			#endregion
			return result;
		}

		/// <summary>
		/// Получение xml-элемента с заданным именем и значением
		/// </summary>
		/// <param name="name">имя элемента</param>
		/// <param name="value">значение элемента</param>
		/// <returns>xml-элемент</returns>
		protected XElement GetXElement(string name, NumberValue value)
		{
			XElement result = new XElement(name);
			result.Value = value.ToString();
			result.SetAttributeValue("type", "float");
			return result;
		}
	}
}
