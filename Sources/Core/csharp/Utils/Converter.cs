using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Utils
{
	/// <summary>
	/// Конвертер
	/// </summary>
	public static class Converter
	{
		/// <summary>
		/// Преобразование в строку
		/// </summary>
		/// <param name="value">значение</param>
		/// <returns>строковый эквивалент</returns>
		public static string ToString(int value)
		{
			return value.ToString();
		}

		/// <summary>
		/// Преобразование в строку
		/// </summary>
		/// <param name="value">значение</param>
		/// <returns>строковый эквивалент</returns>
		public static string ToString(uint value)
		{
			return value.ToString();
		}

		/// <summary>
		/// Преобразование в XName
		/// </summary>
		/// <param name="value">строка</param>
		/// <returns>преобразованное значение</returns>
		public static XName ToXName(string value)
		{
			return value;
		}

		/// <summary>
		/// Преобразование в список
		/// </summary>
		/// <typeparam name="T">тип списка</typeparam>
		/// <param name="collection">коллекция</param>
		/// <returns>список</returns>
		public static List<T> ToList<T>(IEnumerable<T> collection)
		{
            /*
			StringBuilder builder = new StringBuilder();
			foreach (T item in collection)
				builder.AppendLine(item.ToString());
            */
			return (collection.ToList());
		}
        /*public static List<T> ToList<T>( List<T> collection )
        {
            return (collection);
        }*/

		/// <summary>
		/// Обновление сдвига
		/// </summary>
		/// <param name="offsets"></param>
		/// <param name="index"></param>
		/// <param name="newValue"></param>
		public static void UpdateOffset(ref List<int> offsets, int index, int newValue)
		{
			offsets[index] = newValue;
		}
	}
}
