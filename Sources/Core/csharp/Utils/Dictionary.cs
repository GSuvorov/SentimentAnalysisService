using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Logging;

namespace Utils
{
	/// <summary>
	/// Класс для работы со словарями
	/// </summary>
	public static class Dictionary
	{
		/// <summary>
		/// Чтение словаря
		/// </summary>
		/// <param name="path">путь</param>
		/// <param name="encoding">кодировка</param>
		/// <returns>список считанных строк</returns>
		/*public static List< string > ReadAsList(string path, Encoding encoding)
        {
            try
            {
				var lines = FileReader.Read(path, encoding).ToList();
                return (lines);
            }
            catch
            {
				Logger.WriteInfo("Ошибка при чтении файла {0}", path);
            }
            return (new List< string >());
		}*/
        public static HashSet< string > ReadAsHashSet( string path, Encoding encoding )
        {            
            try
            {
                var lines = new HashSet< string >();
                foreach ( var line in FileReader.Read( path, encoding ) )
                {
                    lines.Add( line );
                }
                return (lines);
            }
            catch
            {
                Logger.WriteInfo( "Ошибка при чтении файла {0}", path );
            }
            return (new HashSet< string >());
        }

		/// <summary>
		/// Чтение всех профилей словарей
		/// </summary>
		/// <param name="path">путь к папке с профилями</param>
		/// <param name="fileName">имя файла-словаря</param>
		/// <param name="encoding">кодировка</param>
		/// <returns></returns>
		/*public static Dictionary<string, List<string>> ReadProfileDictionary(string path, string fileName, Encoding encoding)
		{
			Dictionary<string, List<string>> profileDictionary = new Dictionary<string, List<string>>();
			List<string> profiles = new List<string>();
			foreach (var value in Enum.GetValues(typeof(JobProfile)))
				profiles.Add(value.ToString());
			string[] profileDictList = Directory.GetDirectories(path);
			foreach (string profile in profileDictList)
			{
				DirectoryInfo info = new DirectoryInfo(profile);
				if (profiles.Contains(info.Name))
				{
					string fullPath = Path.Combine(path, profile, fileName);
					if (!profileDictionary.ContainsKey(info.Name))
                        profileDictionary.Add( info.Name, ReadAsList( fullPath, encoding ) );
				}
			}
			return profileDictionary;
		}
        */
        public static Dictionary< string, HashSet< string > > ReadProfileDictionary( string path, string fileName, Encoding encoding )
        {
            var profileDictionary = new Dictionary< string, HashSet< string > >();
            var profiles = new HashSet< string >();
            foreach ( var value in Enum.GetValues( typeof(JobProfile) ) )
            {
                profiles.Add( value.ToString() );
            }
            string[] profileDictList = Directory.GetDirectories( path );
            foreach ( string profile in profileDictList )
            {
                var info = new DirectoryInfo( profile );
                if ( profiles.Contains( info.Name ) )
                {
                    string fullPath = Path.Combine( path, profile, fileName );
                    if ( !profileDictionary.ContainsKey( info.Name ) )
                    {
                        profileDictionary.Add( info.Name, ReadAsHashSet( fullPath, encoding ) );
                    }
                }
            }
            return (profileDictionary);
        }
	}
}
