using System.Collections.Generic;
using System.IO;
using System.Text;

namespace TextParsing
{
    /// <summary>
    /// Тип словаря
    /// </summary>
    internal enum DictionaryType
    {
        /// <summary>
        /// Сокращение не перед именем собственным
        /// </summary>
        BeforeNoProper,
        /// <summary>
        /// Сокращение перед именем собственным или числом
        /// </summary>
        BeforeProperOrNumber,
        /// <summary>
        /// Доменные зоны
        /// </summary>
        Domens,
        /// <summary>
        /// Междометия
        /// </summary>
        Interjections,
        /// <summary>
        /// Префиксы фамилий
        /// </summary>
        FamilyPrefixes,
        /// <summary>
        /// Смайлики
        /// </summary>
        Smiles
    }

    /// <summary>
    /// Класс словарей-ресурсов
    /// </summary>
    internal class DictionaryResources
    {
        /// <summary>
        /// Cловари
        /// </summary>
        private Dictionary<DictionaryType, IEnumerable<string>> _dictionaryCollection;

        #region [.ctor().]
        /// <summary>
        /// конструктор
        /// </summary>
        /// <param name="path">путь к словарям</param>
        public DictionaryResources( string path )
        {
            _dictionaryCollection = new Dictionary<DictionaryType, IEnumerable<string>>();
            ReadDictionaries( path );
        }

        /// <summary>
        /// Чтение словарей
        /// </summary>
        /// <param name="path">путь к словарям</param>
        private void ReadDictionaries( string path )
        {
            ReadDictionary( path, DictionaryType.BeforeNoProper );
            ReadDictionary( path, DictionaryType.BeforeProperOrNumber );
            ReadDictionary( path, DictionaryType.Interjections );
            ReadDictionary( path, DictionaryType.Domens );
            ReadDictionary( path, DictionaryType.FamilyPrefixes );
            ReadDictionary( path, DictionaryType.Smiles );
        }

        /// <summary>
        /// Чтение словаря
        /// </summary>
        /// <param name="directory">Путь к файлу</param>
        /// <param name="type">Тип считываемого словаря</param>
        private void ReadDictionary( string directory, DictionaryType type )
        {
            string dictionaryName = type.ToString() + ".txt";
            _dictionaryCollection[ type ] = Utils.FileReader.Read( Path.Combine( directory, dictionaryName ), Encoding.UTF8 );
        }
        #endregion

        /// <summary>
        /// Получение элементов словаря
        /// </summary>
        /// <param name="type">тип словаря</param>
        /// <returns>элементы словаря</returns>
        public IEnumerable<string> GetDictionaryElements( DictionaryType type )
        {
            IEnumerable<string> elements;
            if ( !_dictionaryCollection.TryGetValue( type, out elements ) )
                elements = new string[] { };
            return elements;
        }
    }
}
