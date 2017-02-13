using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Utils
{
	/// <summary>
	/// Класс для работы со словарями
	/// </summary>
	internal static class Dictionary
	{
		/// <summary>
		/// Чтение словаря
		/// </summary>
		/// <param name="path">путь</param>
		/// <param name="encoding">кодировка</param>
		/// <returns>список считанных строк</returns>
		public static List<string> Read(string path, Encoding encoding)
        {
            List<string> lines = new List<string>();
            try
            {
				using (StreamReader reader = new StreamReader(path, encoding))
				{
					var line = default(string);
                    while ((line = reader.ReadLine()) != null)
                    {
                        line = line.Trim();
                        if(!string.IsNullOrEmpty(line))
                            lines.Add(line);
                    }
                }
            }
            catch (Exception)
            {
				throw new InvalidOperationException(string.Format("Ошибка при чтении файла {0}", path));
            }
            return lines;
		}
	}
}
