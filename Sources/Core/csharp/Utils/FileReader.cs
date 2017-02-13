using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Utils
{
    /// <summary>
    /// Класс для чтения файлов
    /// </summary>
    internal static class FileReader
    {
        /// <summary>
        /// Чтение файла
        /// </summary>
        /// <param name="path">путь</param>
        /// <param name="encoding">кодировка</param>
        /// <returns>коллекция считанных строк</returns>
        public static IEnumerable<string> Read( string path, Encoding encoding )
        {
            List<string> lines = new List<string>();
            try
            {
                using ( StreamReader reader = new StreamReader( path, encoding ) )
                {
                    var line = default( string );
                    while ( (line = reader.ReadLine()) != null )
                    {
                        line = line.Trim();
                        if ( !string.IsNullOrEmpty( line ) )
                            lines.Add( line );
                    }
                }
            }
            catch
            {
                throw new InvalidOperationException( string.Format( "Ошибка при чтении файла {0}", path ) );
            }
            return lines;
        }
    }
}
