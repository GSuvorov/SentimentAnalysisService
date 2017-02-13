using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PTS
{
	/// <summary>
	/// Представление числа
	/// </summary>
	public class NumberValue
	{
		/// <summary>
		/// Строка максимального значения
		/// </summary>
		private static readonly string MaxValueStr = "1E38";
		/// <summary>
		/// Строка минимального значения
		/// </summary>
		private static readonly string MinValueStr = "1E-38";

		/// <summary>
		/// Максимальное значение
		/// </summary>
		private static readonly double MaxValue = double.Parse(MaxValueStr);
		/// <summary>
		/// Минимальное значение
		/// </summary>
		private static readonly double MinValue = double.Parse(MinValueStr);

		/// <summary>
		/// Флаг экспоненциальной записи
		/// </summary>
		public bool IsExponential { get; set; }
		/// <summary>
		/// Строка значения
		/// </summary>
		public string ValueStr { get; private set; }
		/// <summary>
		/// Значение
		/// </summary>
		private double? _value;
		/// <summary>
		/// Значение
		/// </summary>
		public double Value
		{
			get
			{
				if (!_value.HasValue)
					_value = NumberParser.Parse(ValueStr);
				return _value.Value;
			}
		}

		#region [.ctor().]
		public NumberValue(double value)
		{
			_value = value;
		}

		public NumberValue(string value)
		{
			ValueStr = value.Replace(',', '.');
		}
		#endregion

		public override string ToString()
		{
			if (Value > MaxValue)
				return MaxValueStr;
			else if ((Value < MinValue) && (ValueStr != null) && ValueStr.ToUpper().Contains('E'))
				return MinValueStr;
			else if (ValueStr != null)
				return ValueStr;
			else
				return IsExponential ? _value.Value.ToString("0.###E-0#", NumberParser.EnglishFormat)
									 : _value.Value.ToString("G", NumberParser.EnglishFormat);
		}
	}
}
