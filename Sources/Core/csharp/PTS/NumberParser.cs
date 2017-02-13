using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Globalization;

namespace PTS
{
	/// <summary>
	/// Парсер чисел
	/// </summary>
	internal static class NumberParser
	{
		/// <summary>
		/// формат чисел на английском
		/// </summary>
		public static readonly NumberFormatInfo EnglishFormat = new NumberFormatInfo() { NumberDecimalSeparator = "." };
		/// <summary>
		/// формат чисел на русском
		/// </summary>
		public static readonly NumberFormatInfo RussianFormat = new NumberFormatInfo() { NumberDecimalSeparator = "," };

		/// <summary>
		/// коллекция чисел и их формы в виде слов
		/// </summary>
		private static readonly Dictionary<string, int> _numberValue;
		/// <summary>
		/// коллекция модификаторов для чисел
		/// </summary>
		private static readonly Dictionary<string, int> _numberModificatorsValue;

		#region [.ctor().]
		static NumberParser()
		{
			_numberValue = new Dictionary<string, int>();
			_numberModificatorsValue = new Dictionary<string, int>();
			#region [.number RU.]
			_numberValue.Add("один", 1);
			_numberValue.Add("одна", 1);
			_numberValue.Add("два", 2);
			_numberValue.Add("две", 2);
			_numberValue.Add("три", 3);
			_numberValue.Add("четыре", 4);
			_numberValue.Add("пять", 5);
			_numberValue.Add("шесть", 6);
			_numberValue.Add("семь", 7);
            _numberValue.Add("восемь", 8);
			_numberValue.Add("девять", 9);
			_numberValue.Add("десять", 10);
			_numberValue.Add("одинадцать", 11);
			_numberValue.Add("двенадцать", 12);
			_numberValue.Add("тринадцать", 13);
			_numberValue.Add("четырнадцать", 14);
			_numberValue.Add("пятнадцать", 15);
			_numberValue.Add("шестнадцать", 16);
			_numberValue.Add("семнадцать", 17);
			_numberValue.Add("восемнадцать", 18);
			_numberValue.Add("девятнадцать", 19);
			_numberValue.Add("двадцать", 20);
			_numberValue.Add("тридцать", 30);
			_numberValue.Add("сорок", 40);
			_numberValue.Add("пятьдесят", 50);
			_numberValue.Add("шестьдесят", 60);
			_numberValue.Add("семьдесят", 70);
			_numberValue.Add("восемьдесят", 80);
			_numberValue.Add("девяносто", 90);
			_numberValue.Add("сто", 100);
			_numberValue.Add("двести", 200);
			_numberValue.Add("триста", 300);
			_numberValue.Add("четыреста", 400);
			_numberValue.Add("пятьсот", 500);
			_numberValue.Add("шестьсот", 600);
			_numberValue.Add("семьсот", 700);
			_numberValue.Add("восемьсот", 800);
			_numberValue.Add("девятьсот", 900);
			#endregion
			#region [.number EN.]
			_numberValue.Add("one", 1);
			_numberValue.Add("two", 2);
			_numberValue.Add("three", 3);
			_numberValue.Add("four", 4);
			_numberValue.Add("five", 5);
			_numberValue.Add("six", 6);
			_numberValue.Add("seven", 7);
            _numberValue.Add("eight", 8);
			_numberValue.Add("nine", 9);
			_numberValue.Add("ten", 10);
			_numberValue.Add("eleven", 11);
			_numberValue.Add("twelve", 12);
			_numberValue.Add("thirteen", 13);
			_numberValue.Add("fourteen", 14);
			_numberValue.Add("fifteen", 15);
			_numberValue.Add("sixteen", 16);
			_numberValue.Add("seventeen", 17);
			_numberValue.Add("eighteen", 18);
			_numberValue.Add("nineteen", 19);
			_numberValue.Add("twenty", 20);
			_numberValue.Add("thirty", 30);
			_numberValue.Add("forty", 40);
			_numberValue.Add("fifty", 50);
			_numberValue.Add("sixty", 60);
			_numberValue.Add("seventy", 70);
			_numberValue.Add("eighty", 80);
			_numberValue.Add("ninety", 90);
			#endregion
			#region [.modificator RU.]
			_numberModificatorsValue.Add("сотня", 2);
			_numberModificatorsValue.Add("сотни", 2);
			_numberModificatorsValue.Add("сотен", 2);
			_numberModificatorsValue.Add("тысяча", 3);
			_numberModificatorsValue.Add("тысячи", 3);
			_numberModificatorsValue.Add("тысяч", 3);
			_numberModificatorsValue.Add("тыс.", 3);
			_numberModificatorsValue.Add("тыс", 3);
			_numberModificatorsValue.Add("миллионов", 6);
			_numberModificatorsValue.Add("миллиона", 6);
			_numberModificatorsValue.Add("миллион", 6);
			_numberModificatorsValue.Add("млн.", 6);
			_numberModificatorsValue.Add("млн", 6);
			_numberModificatorsValue.Add("миллиардов", 9);
			_numberModificatorsValue.Add("миллиарда", 9);
			_numberModificatorsValue.Add("миллиард", 9);
			_numberModificatorsValue.Add("млрд.", 9);
			_numberModificatorsValue.Add("млрд", 9);
			_numberModificatorsValue.Add("триллионов", 12);
			_numberModificatorsValue.Add("триллиона", 12);
			_numberModificatorsValue.Add("триллион", 12);
			_numberModificatorsValue.Add("трлн.", 12);
			_numberModificatorsValue.Add("трлн", 12);
			#endregion
			#region [.modificator EN.]
			_numberModificatorsValue.Add("and", 1);
			_numberModificatorsValue.Add("hundred", 2);
			_numberModificatorsValue.Add("thousand", 3);
			_numberModificatorsValue.Add("million", 6);
			_numberModificatorsValue.Add("mln.", 6);
			_numberModificatorsValue.Add("mln", 6);
			_numberModificatorsValue.Add("milliard", 9);
			_numberModificatorsValue.Add("billion", 9);
			_numberModificatorsValue.Add("mlrd.", 9);
			_numberModificatorsValue.Add("mlrd", 9);
			_numberModificatorsValue.Add("bln.", 9);
			_numberModificatorsValue.Add("bln", 9);
			_numberModificatorsValue.Add("trillion", 12);
			_numberModificatorsValue.Add("trln.", 12);
			_numberModificatorsValue.Add("trln", 12);
			#endregion
		}
		#endregion

		/// <summary>
		/// Парсинг числа из строки
		/// </summary>
		/// <param name="number">строка</param>
		/// <returns>число</returns>
		public static double Parse(string number)
		{
			try
			{
				return double.Parse(number.Replace(',', '.'), NumberStyles.Any, EnglishFormat);
			}
			catch (OverflowException ex)
			{
				return double.MaxValue;
			}
		}

		/// <summary>
		/// Парсинг числа, записанного словами
		/// </summary>
		/// <param name="number">число</param>
		/// <returns>полученное число</returns>
		private static double ParseVerbalNumber(string number)
		{
			double result = 0;
			double modifier = 1;
			var words = number.Split(' ');
			for (int i = 0; i < words.Length; ++i)
			{
				string lowerWord = words[i].ToLower();
				int value;
				if (_numberValue.TryGetValue(lowerWord, out value))
				{
					result += value * modifier;
					if (i == 0)
						modifier = 1;
				}
				else if (_numberModificatorsValue.TryGetValue(lowerWord, out value))
				{
					if (modifier > value)
						modifier *= value;
					else
						modifier = value;
				}
				else
					throw new FormatException(number + " - isn't number");
			}
			if (modifier != 1)
				result += modifier;
			return result;
		}
	}
}
