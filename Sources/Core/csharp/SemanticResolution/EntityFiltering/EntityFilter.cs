using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml.Linq;

using Linguistics.Core;
using Logging;
using DiskSearchEngine;
using DiskSearchEngine.Hashtable;

namespace SemanticResolution
{
    /// <summary>
    /// класс фильтрации по стоп-словарям и стоп-правилам
    /// </summary>
    public class EntityFilter
    {
        private delegate void SaveDictionary(string profile, string textLine);

		#region [Стоп-словари]
		/// <summary>
        /// стоп-словарь словосочетаний
        /// </summary>
		private static Dictionary<string, HashSet<string>> _StopExuberantlyDictionary = null;
		/// <summary>
		/// стоп-словарь прилагательных
		/// </summary>
		private static Dictionary<string, HashSet<string>> _StopAdjectiveDictionary = null;
        /// <summary>
        /// стоп-словарь существительных
        /// </summary>
        private static Dictionary<string, HashSet<string>> _StopWordDictionary = null;
        /// <summary>
        /// стоп-словарь сущностей
        /// </summary>
        private static Dictionary<string, HashSet<string>> _StopEntityDictionary = null;
		/// <summary>
		/// профиль функции
		/// </summary>
		private static string _profile = "";
		#endregion

		#region [Чтение словарей]
		private static void ReadDictionary(string path, string fileName, SaveDictionary function)
        {
            if ( function != null )
            {
                var dictionary = Utils.Dictionary.ReadProfileDictionary( path, fileName, Encoding.Unicode );
                foreach ( var profileDictionary in dictionary )
                {
                    foreach ( string line in profileDictionary.Value )
                    {
                        string value = line.Trim().ToUpper();
                        function( profileDictionary.Key, value );
                    }
                }
            }
        }

		private static void AddToDictionary(Dictionary<string, HashSet<string>> dictionary, string profile, string line)
		{
			HashSet<string> collection;
			if (dictionary.TryGetValue(profile, out collection))
			{
				if (!collection.Contains(line))
					collection.Add(line);
			}
			else
			{
				collection = new HashSet<string>();
				collection.Add(line);
				dictionary.Add(profile, collection);
			}
		}

        private static void ReadStopEntities(string profile, string line)
        {
			AddToDictionary(_StopEntityDictionary, profile, line);
        }

        private static void ReadStopWords(string profile, string line)
        {
            if (line.IndexOf(' ') >= 0)
				AddToDictionary(_StopExuberantlyDictionary, profile, line);
            else
				AddToDictionary(_StopWordDictionary, profile, line);
        }

        /// <summary>
        /// функция чтения стоп-словарей
        /// </summary>
        /// <param name="databasePath">путь к папке Database</param>
        public static void LoadStopDictionaries()
        {
            if ((_StopWordDictionary == null) || (_StopEntityDictionary == null))
            {
                string path = Utils.GlobalVariables.UserResourcesPath;

				path += @"StopDictionaries\";
                _StopEntityDictionary = new Dictionary<string, HashSet<string>>();
				_StopExuberantlyDictionary = new Dictionary<string, HashSet<string>>();
				_StopWordDictionary = new Dictionary<string, HashSet<string>>();
				ReadDictionary(path, "StopEntities.txt", ReadStopEntities);
                ReadDictionary(path, "StopWords.txt", ReadStopWords);
            }
        }

        public static void LoadStopAdjectivesDictionary( Dictionary< string, HashSet< string > > dictionary )
        {
            _StopAdjectiveDictionary = dictionary;
        }
		/*public static void LoadStopAdjectivesDictionary(Dictionary<string, List<string>> dictionary)
		{
            _StopAdjectiveDictionary = new Dictionary<string, HashSet<string>>();
            foreach ( var pair in dictionary )
            {
                var forms = new HashSet<string>();
                foreach ( string line in pair.Value )
                {
                    var value = line.Trim().ToUpper();
                    forms.Add( value );

                }
                _StopAdjectiveDictionary[ pair.Key ] = forms;
            }
		}*/
		#endregion

		#region [Проверка по геосправочнику]
		/// <summary>
		/// объект для проверки географии по словарю
		/// </summary>
		private static HashtableSearcher _geoNames = null;

		private const string _path2GeoDict = @"GeographicNames\GeoNames.utf8.data";

		public static void FilterGeographic(ref XElement xmlText)
        {
			if (_geoNames == null)
            {
                try
                {
					_geoNames = new HashtableSearcher(Utils.GlobalVariables.UserResourcesPath + _path2GeoDict,
						HashtableEngineHelper.NormlizeText);
                }
				catch (StupidSearchEngineException ex)
                {
                    Logger.WriteError("Ошибка при загрузке словаря");
                    Logger.Write(ex);
                }
            }
			if (xmlText != null)
			{
				foreach (XElement entity in xmlText.Descendants())
					if (entity.Name == UnitTextType.ENTITY.ToString())
						FilterGeographicEntity(entity);
			}
        }

        private static void FilterGeographicEntity(XElement xml)
        {
            bool isPlace   = xml.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.PlaceName);
            bool isUnknown = xml.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.UnknownProper) &&
                             xml.IsContainAttribute(BlockAttribute.PLACE);
            if (isPlace || isUnknown)
            {
                if (_geoNames != null)
                {
                    string value = xml.Attribute(BlockAttribute.VALUE);
                    if (value.IsEmpty() || ContainsWrongSymbols(value))
                    {
                        //xml.SetType(EntityType.UnknownProper);
						xml.SetType(EntityType.PseudoGeo);
                        return;
                    }

                    string valueForTest = value;
					string[] words = value.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string word in words)
                    {
                        if (Char.IsUpper(word[0]))
                            break;
                        else
                            valueForTest = valueForTest.Substring(word.Length, valueForTest.Length - word.Length).Trim();
                    }

                    if ( !valueForTest.IsEmpty() )
                    {
                        bool isValid = _geoNames.IsExists( valueForTest );
                        /*if (!isValid && isPlace)
                        {
                            xml.SetType(EntityType.UnknownProper);
                        }*/
                        if ( !isValid )
                        {
                            xml.SetType( EntityType.PseudoGeo );
                        }
                        else if ( /*isValid &&*/ isUnknown )
                        {
                            xml.SetType( EntityType.PlaceName );
                            xml.SetAttributeValue( BlockAttribute.INFO, "" );
                            if ( valueForTest != value )
                                xml.SetAttributeValue( BlockAttribute.VALUE, valueForTest );
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Проверка на нахождение в строке символов, отличных от букв и цифр
        /// </summary>
        /// <param name="str">строка для проверки</param>
        /// <returns></returns>
        private static bool ContainsWrongSymbols(string str)
        {
            bool contains = false;
            string goodSymbols = " -.";
            for(int i = 0; !contains && (i < str.Length); ++i)
                if (!Char.IsLetterOrDigit(str[i]) && (goodSymbols.IndexOf(str[i]) < 0))
                    contains = true;
            return contains;
        }
		#endregion

		#region [Фильтрация сущностей]
		private static List<string> _ParentForFilteredThemes = null;

		/// <summary>
		/// Фильтрация сущностей по их типу в иерархии
		/// </summary>
		/// <param name="xmlText"></param>
		public static void FilterEntitiesByType(ref XElement xmlText)
		{
			if (xmlText != null)
			{
				foreach (XElement element in xmlText.Descendants())
				{
					if (!element.Name.IsEqual(UnitTextType.ENTITY))
						continue;
					XElement parent = element.Parent;
					while (parent.IsContainAttribute(BlockAttribute.NOT_THEME))
						parent = parent.Parent;
					if (parent.Name.IsEqual(UnitTextType.ENTITY) && IsFilteredEntity(element, parent))
						element.SetAttributeValue(BlockAttribute.NOT_THEME, "");
				}
			}
		}

		private static bool IsFilteredEntity(XElement entity, XElement parent)
		{
			string entityType = entity.Attribute(BlockAttribute.TYPE);
			string parentType = parent.Attribute(BlockAttribute.TYPE);

			bool parentIsOrg = parentType.IsEqual(EntityType.OrgName) || parentType.IsEqual(EntityType.OrgAbbr);
			bool entityIsOrg = entityType.IsEqual(EntityType.OrgName) || entityType.IsEqual(EntityType.OrgAbbr);
			bool entityIsPerson = entityType.IsEqual(EntityType.PersonName) || entityType.IsEqual(EntityType.PersonInfo) ||
				entityType.IsEqual(EntityType.PersonNoInfo);

			/// даты внутри сущностей не выделяем
			bool result = entityType.IsEqual(EntityType.Date);
			/// персоны и орги внутри географии не берем
			result = result || (parentType.IsEqual(EntityType.PlaceName) && (entityIsPerson || entityIsOrg));
			/// персоны внутри организаций не берем
            /// ПОКА не берем организации внутри организаций
			result = result || (parentIsOrg && (entityIsPerson || entityIsOrg));
			/// отсеиваем некоторые одинаковые типы
			result = result || (entityType.IsEqual(EntityType.PlaceName) &&
				(parentType.IsEqual(EntityType.PlaceName) || parentType.IsEqual(EntityType.PlaceNoInfo)));
			/// отсеиваем некоторые типы внутри PersonName
			result = result || (parentType.IsEqual(EntityType.PersonName) &&
				(entityType.IsEqual(EntityType.PersonName) || entityType.IsEqual(EntityType.PersonNoInfo)));
            result = result || ((entityType.IsEqual(EntityType.Unknown) || entityType.IsEqual(EntityType.UnknownProper)
				|| entityType.IsEqual(EntityType.NounVerb)) && (parentIsOrg || parentType.IsEqual(EntityType.PlaceName)));

			return result;
		}

		/// <summary>
		/// фильтрация сущностей по стоп-правилам и стоп-словарям
		/// </summary>
		/// <param name="xmlText">xml-текст</param>
		/// <returns>отфильтрованный xml</returns>
		public static void FilterEntities(ref XElement xmlText, Utils.JobProfile profile, bool isOnlyDictionaryFilter)
		{
			if (xmlText != null)
			{
				_profile = profile.ToString();
				_ParentForFilteredThemes = new List<string>();
				/// флаг заполненности словаря стоп-прилагательных
				bool isAdjectivesDict = GetProfileDictionary(_StopAdjectiveDictionary, _profile).Any();
				foreach (XElement entity in xmlText.Descendants())
				{
					if (entity.Name.IsEqual(UnitTextType.ENTITY) || entity.Name.IsEqual(UnitTextType.THEME.ToString()))
					{
						if (isAdjectivesDict)
							RemoveStopAdjectives(entity);
						FilterByStopRule(entity, isOnlyDictionaryFilter);
					}
				}
			}
		}

		private static string[] _homogeneousSplitter = null;
		private static Regex _tokenizer = new Regex(string.Format("(?<=(^|[^{0}]))([-{0}]){{1,}}(?=($|[^{0}]))", "A-Za-zА-Яа-я0-9"));
		private static void RemoveStopAdjectives(XElement entity)
		{
			if (_homogeneousSplitter == null)
				_homogeneousSplitter = ServiceFunction.HomogeneousConjunction.Union(new string[] { ",", "or", "and" }).ToArray();

			string value = entity.Attribute(BlockAttribute.VALUE);
			string[] filterindAdjectives = GetAdjectivesForFilter(entity);
			Match[] wordsCollection = _tokenizer.Matches(value).Cast<Match>().Where(_ => filterindAdjectives.Contains(_.Value)).ToArray();
			if (!wordsCollection.Any())
				return;

			StringBuilder newValue = new StringBuilder();
			int previous = 0;
			foreach (Match word in wordsCollection)
			{
				string subStr = value.Substring(previous, word.Index - previous).TrimEnd();
				previous = word.Index + word.Length;
				int index = subStr.LastIndexOf(' ');
				string str = subStr.Substring(index + 1).TrimEnd();
				if (!_homogeneousSplitter.Contains(str))
				{
					newValue.Append(subStr);
					bool isStart = true;
					for (index = previous; index < value.Length; ++index)
					{
						if (isStart && (value[index] != ' '))
							isStart = false;
						else if (!isStart && (value[index] == ' '))
							break;
					}
					str = value.Substring(previous, index - previous).Trim();
					if (_homogeneousSplitter.Contains(str))
						previous = index;
				}
				else
					newValue.Append(subStr.Substring(0, index).TrimEnd());
			}
			newValue.Append(value.Substring(previous));
			string entityValue = newValue.ToString().Trim();
			if ((previous > 0) && !entityValue.IsEmpty())
				entity.SetAttributeValue(BlockAttribute.VALUE_THEME, entityValue);
		}

		/// <summary>
		/// Получение прилагательных для фильтрации
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>прилагательные для фильтрации</returns>
		private static string[] GetAdjectivesForFilter(XElement entity)
		{
			List<string> result = new List<string>();
			HashSet<string> stopAdjectives = GetProfileDictionary(_StopAdjectiveDictionary, _profile);
			string type = entity.Attribute(BlockAttribute.TYPE);
			if (type.IsEqual(EntityType.PersonInfo) || type.IsEqual(EntityType.PersonName) || type.IsEqual(EntityType.PersonNoInfo))
				stopAdjectives.Add("ЛЕТНИЙ");
			string[] allEntityAdjectives = entity.Attribute(BlockAttribute.ADJ).Split(',');
			result.AddRange(allEntityAdjectives.Where(_ => stopAdjectives.Contains(_.Trim().ToUpper())));
			if (type.IsEqual(EntityType.PersonName) || type.IsEqual(EntityType.PersonInfo))
				result.AddRange(entity.Attribute(BlockAttribute.PRE).Split(' ').Where(_ => allEntityAdjectives.Contains(_)));
			return result.ToArray();
		}

		private static HashSet<string> GetProfileDictionary(Dictionary<string, HashSet<string>> dictionary, string profile)
		{
			HashSet<string> collection;
			if ((dictionary == null) || !dictionary.TryGetValue(profile, out collection))
				collection = new HashSet<string>();
			return collection;
		}

		private static bool FilterByStopRule(XElement xml, bool isOnlyDictionaryFilter)
        {
            bool isFilter = true;
            if (xml.IsContainAttribute(BlockAttribute.NOT_THEME))
                return isFilter;

			string type = xml.Attribute(BlockAttribute.TYPE);
			if (type.IsEqual(EntityType.OrgName) || type.IsEqual(EntityType.OrgAbbr) ||
				type.IsEqual(EntityType.PlaceName) || type.IsEqual(EntityType.PersonName))
			{
				return false;
			}

            string value = xml.AttributeUpper(BlockAttribute.FULLNAME);
            bool needAnalyze = true;

            /// отсев подсущностей у отфильтрованных сущностей
            if (xml.Name.IsEqual(UnitTextType.THEME) && _ParentForFilteredThemes.Contains(xml.Parent.Attribute(BlockAttribute.ID)))
				needAnalyze = false;

            if(needAnalyze)
            {
				HashSet<string> stopEntities = GetProfileDictionary(_StopEntityDictionary, _profile);
				HashSet<string> stopWords = GetProfileDictionary(_StopWordDictionary, _profile);
				HashSet<string> stopExuberantly = GetProfileDictionary(_StopExuberantlyDictionary, _profile);

                string mainWord = xml.AttributeUpper(BlockAttribute.PROF);
				string prefix = xml.AttributeUpper(BlockAttribute.PRE);
				bool isEmptyPrefix = prefix.IsEmpty() || !value.Contains(prefix);
				isFilter = stopEntities.Contains(value) || (isEmptyPrefix && stopWords.Contains(mainWord));
                if (!isFilter)
                {
					foreach (string collocation in stopExuberantly)
					{
						int index = value.IndexOf(collocation);
						if (index >= 0)
						{
							if (((index == 0) || !Char.IsLetterOrDigit(value[index - 1])) &&
								((index + collocation.Length == value.Length) || !Char.IsLetterOrDigit(value[index + collocation.Length])))
							{
								isFilter = true;
								_ParentForFilteredThemes.Add(xml.Attribute(BlockAttribute.ID));
								break;
							}
						}
					}
					if (!isOnlyDictionaryFilter)
					{
						isFilter = isFilter || FilterBySymbolsCount(xml);
						isFilter = isFilter || FilterByDefis(xml, value);
						isFilter = isFilter || FilterUnknown(xml, value);
					}
                }
            }
            if (isFilter)
                xml.SetAttributeValue(BlockAttribute.NOT_THEME, "");
            return isFilter;
        }

		private static bool FilterUnknown(XElement element, string value)
		{
			bool isFilter = false;
			string type = element.Attribute(BlockAttribute.TYPE);
			if (type.IsEqual(EntityType.Unknown) || type.IsEqual(EntityType.UnknownProper))
			/// отфильтруем сущности, нормализации которых - два символа через дефис,
			///	если количество символов на 1 меньше, чем количество букв,
			///	если одинаковые буквы
			{
				isFilter = (value.Length == 3) && (value.IndexOf('-') == 1);
				if (!isFilter)
				{
					int defisCount = 0;
					int letterCount = 0;
					bool isSameSymbol = true;
					char symbol = '\0';
					for (int i = 0; i < value.Length; ++i)
					{
						if (i == 0)
							symbol = value[i];
						else if (symbol != value[i])
							isSameSymbol = false;

						if (Char.IsLetter(value[i]))
							++letterCount;
						else if (value[i] == '-')
							++defisCount;
					}
					isFilter = isSameSymbol || (defisCount == letterCount - 1);
				}
			}
			return isFilter;
		}

		/// <summary>
		/// Фильтрация по дефисам
		/// </summary>
		/// <param name="element"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		private static bool FilterByDefis(XElement element, string value)
		{
			bool isFilter = false;
			string type = element.Attribute(BlockAttribute.TYPE);
			if (type.IsEqual(EntityType.Date) || type.IsEqual(EntityType.Address) || type.IsEqual(EntityType.BankAccount) ||
				type.IsEqual(EntityType.Email) || type.IsEqual(EntityType.Phone) || type.IsEqual(EntityType.Slogan))
			{
				return isFilter;
			}
			foreach (string part in value.Split(' '))
			{
				if (part.IsEmpty())
					continue;
				if ((part[0] == '-') || (part[part.Length - 1] == '-'))
				/// отфильтровываем сущности, если какое-нибудь ее слова начинается или заканчивается '-'
				{
					isFilter = true;
					break;
				}
				if (part.Count(_ => _ == '-') > 1)
					isFilter = true; /// отфильтровываем сущности с несколькими дефисами в одном слове
			}
			return isFilter;
		}

		/// <summary>
		/// Фильтрация по количеству символам в нормализации
		/// </summary>
		/// <param name="element"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		private static bool FilterBySymbolsCount(XElement element)
		{
			bool isFilter = false;
			/// тип сущности
			string type = element.Attribute(BlockAttribute.TYPE);
			string value = element.Attribute(BlockAttribute.VALUE);
			if (value.Length == 1)
				isFilter = true; /// отфильтровываем односимвольные сущности
			else
			{
				int letterCount = 0;
				int digitCount = 0;
				int otherCount = 0;
				for (int i = 0; i < value.Length; ++i)
				{
					if (Char.IsLetter(value[i]))
						++letterCount;
					else if (Char.IsDigit(value[i]))
						++digitCount;
					else
						++otherCount;
				}
				if (((letterCount == 1) && (digitCount == 1) && (otherCount == 0)) ||
					((letterCount == 0) && (digitCount == 0)))
				/// отфильтровываем сущности, состоящие из буквы и цифры
				/// отфильтровываем сущности без букв и без цифр
				{
					isFilter = true;
				}
				else if (letterCount == 0)
				/// отфильтровываем некоторые типы сущностей, если в ней нет букв
				{
					if (!type.IsEqual(EntityType.Date) && !type.IsEqual(EntityType.INN) && !type.IsEqual(EntityType.Index) &&
						!type.IsEqual(EntityType.Phone) && !type.IsEqual(EntityType.BankAccount))
					{
						isFilter = true;
					}
				}
				else if (digitCount > 0)
				/// перетипизация некоторых типов сущностей с цифрами
				{
					if (type.IsEqual(EntityType.PlaceNoInfo))
					{
						element.SetType(EntityType.PlaceName);
						FilterGeographicEntity(element);
					}
					else if (type.IsEqual(EntityType.OrgNoInfo))
					{
						if (value.Any(_ => Char.IsLetter(_) && Char.IsUpper(_)))
							element.SetType(EntityType.UnknownProper);
						else
							element.SetType(EntityType.Unknown);
					}
				}
			}	
			return isFilter;
		}
		#endregion
	}
}
