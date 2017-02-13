using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SimpleEntities;
using System.IO;
using System.Text.RegularExpressions;
using Utils;
using Linguistics.Core;

namespace PTS
{
	/// <summary>
	/// Просто парсер XML
	/// </summary>
	public class SimpleXmlParser
	{
		/// <summary>
		/// Выделитель сущностей
		/// </summary>
		private static SimpleEntitySelector _selectorRU = null;
		/// <summary>
		/// Выделитель сущностей
		/// </summary>
		private static SimpleEntitySelector _selectorEN = null;
		/// <summary>
		/// Сохраняемые сущности
		/// </summary>
		private readonly string[] _savedTypes = new string[]
			{
				RegexEntityType.NumberWithDiacritical,
				RegexEntityType.Table,
				RegexEntityType.StandartNumber,
				RegexEntityType.Fraction,
				RegexEntityType.PatentNumber
			};
		/// <summary>
		/// Регулярное выражение для выделения тегов
		/// </summary>
		private static readonly Regex _tagRegex = new Regex("<[^>]*?>");
		/// <summary>
		/// Регулярное выражение для выделения таблиц
		/// </summary>
		private static readonly string _tableRegex = "<tables.*?>.*?<\\/tables>";
		/// <summary>
		/// Регулярное выражение для выделения чисел с надстрочными символами
		/// </summary>
		private static readonly Regex _numberWithDiacriticalCharactersRegex =
			new Regex("(((?<integral>-?\\d+)((\\.|,)(?<fractional>\\d+))?)\\s?[*••×]\\s?)?(?<powBase>\\d+)\\s?<sup>\\s*(?<pow>-?\\d+)</sup>");

		#region [.shifts.]
		/// <summary>
		/// Таблица сдвигов
		/// </summary>
		private StairTableShift _shiftTable;
		/// <summary>
		/// текущий сдвиг
		/// </summary>
		private int _shift = 0;
		/// <summary>
		/// Хранилище таблиц сдвигов
		/// </summary>
		private Dictionary<string , StairTableShift> _shiftTableRepository = new Dictionary<string,StairTableShift>();

		/// <summary>
		/// Получение сдвига для позиции из текущий таблицы сдвигов
		/// </summary>
		/// <param name="position">позиция</param>
		/// <returns>сдвиг</returns>
		public int GetShift(int position)
		{
			return _shiftTable.GetShift(position);
		}

		/// <summary>
		/// Получение сдвига для позиции из хранилища
		/// </summary>
		/// <param name="key">ключ</param>
		/// <param name="position">позиция</param>
		/// <returns>сдвиш</returns>
		public int GetShift(string key, int position)
		{
			return _shiftTableRepository[key].GetShift(position);
		}

		/// <summary>
		/// Сохранение текущей таблицы сдвигов в хранилище по заданному ключу
		/// </summary>
		/// <param name="key">ключ</param>
		public void SaveShiftTableToRepository(string key)
		{
			_shiftTableRepository[key] = _shiftTable;
		}

		/// <summary>
		/// Очистка хранилища таблиц сдвигов
		/// </summary>
		public void ClearShiftTableRepository()
		{
			_shiftTableRepository.Clear();
		}
		#endregion

		#region [.ctor().]
		static SimpleXmlParser()
		{
			_selectorRU = new SimpleEntitySelector(Path.Combine(GlobalVariables.UserResourcesPath, "PTS"), "ru_entity.xml");
			_selectorEN = new SimpleEntitySelector(Path.Combine(GlobalVariables.UserResourcesPath, "PTS"), "en_entity.xml");
		}
		#endregion

		/// <summary>
		/// Удаление неймспейсов и атрибутов
		/// </summary>
		/// <param name="xml">xml</param>
		/// <returns>xml без неймспейсов</returns>
		public string RemoveNamespacesAndAttributes(string xml)
		{
			return _tagRegex.Replace(xml, NamespaceAndAttributesRemover);
		}

		/// <summary>
		/// Парсинг xml и выделение сущностей
		/// </summary>
		/// <param name="xml">xml</param>
		/// <param name="xml">язык документа</param>
		/// <param name="forFilter">сущности для фильтрации</param>
		/// <param name="forSave">сущности для сохранения</param>
		/// <returns>текст</returns>
		public string ParseAndSelectEntities(string xml, string language, List<SimpleEntity> forFilter, List<SimpleEntity> forSave)
		{
			SimpleEntity[] entities = GetSimpleEntities(xml, language);
			string result = GetText(xml, entities);
			foreach (var entity in entities)
			{
				int startShift = GetShift(entity.StartPosition - 1);
				int startPosition = entity.StartPosition - startShift;
				int length = entity.Length - (GetShift(entity.StartPosition + entity.Length - 1) - startShift);
				if (length == 0)
					continue;
				var correctedEntity = new SimpleEntity(result.Substring(startPosition, length), startPosition, entity.Type);
				if (_savedTypes.Contains(correctedEntity.Type))
				{
					if (correctedEntity.Type == RegexEntityType.NumberWithDiacritical)
					{
						var match = _numberWithDiacriticalCharactersRegex.Match(entity.Text);
						#region [.defense.]
						if (!match.Success)
							throw new InvalidOperationException("NumberWithDiacriticalCharacters changed regex");
						#endregion
						var fractionalGroup = match.Groups["fractional"];
						string number = match.Groups["integral"].Value + ((fractionalGroup.Success) ? "." + fractionalGroup.Value : "");
						string value = string.IsNullOrEmpty(number) ? "1" : number;
						string powBase = match.Groups["powBase"].Value;
						string pow = match.Groups["pow"].Value;
						correctedEntity.Value = string.Format("{0}E{1}", value, pow);
					}
					else if (correctedEntity.Type == RegexEntityType.PatentNumber)
					{
						correctedEntity.Value = new string(entity.Text.Where(_ => Char.IsDigit(_)).ToArray());
					}
					else if (correctedEntity.Type == RegexEntityType.Fraction)
					{
						switch (correctedEntity.Value)
						{
							case "¼":
								correctedEntity.Value = "0.25";
								break;
							case "½":
								correctedEntity.Value = "0.5";
								break;
							case "¾":
								correctedEntity.Value = "0.75";
								break;
							default:
								throw new InvalidOperationException("Unknown fraction");
						}
					}
					if (forSave != null)
						forSave.Add(correctedEntity);
				}
				else if (forFilter != null)
					forFilter.Add(correctedEntity);
			}
			return result;
		}

		/// <summary>
		/// Выделение сущностей по регулярным выражениям для заданного языка
		/// </summary>
		/// <param name="xml">xml</param>
		/// <param name="language">язык документа</param>
		/// <returns>сущности</returns>
		private SimpleEntity[] GetSimpleEntities(string xml, string language)
		{
			SimpleEntity[] result = null;
			if (language == Language.Russian)
				result = _selectorRU.SelectSimpleEntities(xml);
			else if (language == Language.English)
				result = _selectorEN.SelectSimpleEntities(xml);
			else if (language == null)
				result = GetSimpleEntities(xml);
			else
				throw new ArgumentException("Wrong language");
			return result;
		}

		/// <summary>
		/// Выделение сущностей по регулярным выражениям для всех языков
		/// </summary>
		/// <param name="xml">xml</param>
		/// <returns>сущности</returns>
		private SimpleEntity[] GetSimpleEntities(string xml)
		{
			List<SimpleEntity> result = new List<SimpleEntity>();
			var resultRU = _selectorRU.SelectSimpleEntities(xml);
			var resultEN = _selectorEN.SelectSimpleEntities(xml);
			int indexRU = 0;
			int indexEN = 0;
			for (; (indexRU < resultRU.Length) && (indexEN < resultEN.Length); )
			{
				var itemRU = resultRU[indexRU];
				var itemEN = resultEN[indexEN];
				if (itemRU.StartPosition < itemEN.StartPosition)
				{
					result.Add(itemRU);
					++indexRU;
				}
				else if (itemEN.StartPosition < itemRU.StartPosition)
				{
					result.Add(itemEN);
					++indexEN;
				}
				else
				/// itemEN.StartPosition == itemRU.StartPosition
				{
					if (itemEN.Length > itemRU.Length)
					{
						result.Add(itemEN);
						++indexEN;
					}
					else if (itemRU.Length > itemEN.Length)
					{
						result.Add(itemRU);
						++indexRU;
					}
					else
					{
						result.Add(itemRU);
						++indexRU;
						++indexEN;
					}
				}
			}
			for (; indexRU < resultRU.Length; ++indexRU)
				result.Add(resultRU[indexRU]);
			for (; indexEN < resultEN.Length; ++indexEN)
				result.Add(resultEN[indexEN]);
			return result.ToArray();
		}

		/// <summary>
		/// Выделение таблиц из текста
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>таблицы</returns>
		public List<TableBlock> GetTablesFromText(string text)
		{
			List<TableBlock> result = new List<TableBlock>();
			var collection = Regex.Matches(text, _tableRegex, RegexOptions.Singleline);
			for (int i = 0; i < collection.Count; ++i)
				result.Add(TableBlock.Create(collection[i].Value, collection[i].Index));
			return result;
		}

		/// <summary>
		/// Получение текста из xml
		/// </summary>
		/// <param name="xml">xml</param>
		/// <returns>текст</returns>
		private string GetText(string xml, IEnumerable<SimpleEntity> entities)
		{
			_shiftTable = new StairTableShift();
			_shift = 0;
			var replace = new List<Tuple<TextPart, string>>();
			foreach (var match in _tagRegex.Matches(xml).Cast<Match>())
			{
				string result = " ";
				if ((match.Value == "</p>") || (match.Value == "</claim>"))
					result = "\n";
				else if ((match.Value == "<sub>") || (match.Value == "</sub>") || (match.Value == "<sup>") || (match.Value == "</sup>"))
					result = "";
				replace.Add(new Tuple<TextPart, string>(new TextPart(match.Index, match.Length), result));
			}
			foreach (var entity in entities)
			{
				if ((entity.Type == RegexEntityType.Diacritical) ||
					(entity.Type == RegexEntityType.NumberWithDiacritical) ||
					(entity.Type == RegexEntityType.SimpleDiacritical))
				{
					int start = entity.Text.IndexOf("<sup>") + 5;
					int end = entity.Text.IndexOf("</sup>");
					string replaced = entity.Text.Substring(start, end - start).IndexOf(' ') >= 0 ? " " : "";
					replace.Add(new Tuple<TextPart, string>(new TextPart(entity.StartPosition + start, end - start), replaced));
				}
			}
			AddSimpleReplace(replace, xml);
			replace = replace.OrderBy(_ => _.Item1.Start).ToList();
			StringBuilder text = new StringBuilder();
			int position = 0;
			int shift = 0;
			foreach (var replaced in replace)
			{
				shift += replaced.Item1.End - replaced.Item1.Start - replaced.Item2.Length;
				_shiftTable.UpdateShift(replaced.Item1.Start, shift);
				text.Append(xml.Substring(position, replaced.Item1.Start - position));
				text.Append(replaced.Item2);
				position = replaced.Item1.End;
			}
			text.Append(xml.Substring(position));
			return text.ToString();
		}

		/// <summary>
		/// Добавление простых замен
		/// </summary>
		/// <param name="replace">коллекция замен</param>
		/// <param name="xml">текст</param>
		private void AddSimpleReplace(List<Tuple<TextPart, string>> replace, string xml)
		{
			var simpleReplace = new List<Tuple<TextPart, string>>();
			simpleReplace.AddRange(GetSimpleReplace(xml, "&lt;", "<"));
			simpleReplace.AddRange(GetSimpleReplace(xml, "&gt;", ">"));
			simpleReplace.AddRange(GetSimpleReplace(xml, "&amp;", "&"));
			simpleReplace.AddRange(GetSimpleReplace(xml, "&quot;", "\""));
			simpleReplace.AddRange(GetSimpleReplace(xml, "\n|\r", " "));
			bool[] map = new bool[xml.Length];
			foreach (var replaced in replace)
				SelectRange(map, replaced.Item1);
			foreach (var simple in simpleReplace)
			{
				if (IsEmptyRange(map, simple.Item1))
				{
					SelectRange(map, simple.Item1);
					replace.Add(simple);
				}
			}
		}

		/// <summary>
		/// Проверка, что заданный интервал свободен на карте
		/// </summary>
		/// <param name="map">карта</param>
		/// <param name="range">интервал</param>
		/// <returns>результат проверки</returns>
		private bool IsEmptyRange(bool[] map, TextPart range)
		{
			for (int i = range.Start; i < range.End; ++i)
				if (map[i])
					return false;
			return true;
		}

		/// <summary>
		/// Отметка заданного интервала на карте
		/// </summary>
		/// <param name="map">карта</param>
		/// <param name="range">интервал</param>
		private void SelectRange(bool[] map, TextPart range)
		{
			for (int i = range.Start; i < range.End; ++i)
				map[i] = true;
		}

		/// <summary>
		/// Получение простой замены
		/// </summary>
		/// <param name="xml">текст</param>
		/// <param name="regex">регулярное выражение</param>
		/// <param name="replace">замена</param>
		/// <returns>коллекция блоков замены</returns>
		private Tuple<TextPart, string>[] GetSimpleReplace(string xml, string regex, string replace)
		{
			var result = new List<Tuple<TextPart, string>>();
			foreach (var match in Regex.Matches(xml, regex).Cast<Match>())
				result.Add(new Tuple<TextPart, string>(new TextPart(match.Index, match.Length), replace));
			return result.ToArray();
		}

		/// <summary>
		/// Удаление из заданного тега неймспейса и атрибутов
		/// </summary>
		/// <param name="m">тег</param>
		/// <returns>скорректированный тег</returns>
		private string NamespaceAndAttributesRemover(Match m)
		{
			return RemoveNamespace(RemoveAttributes(m.Value));
		}

		/// <summary>
		/// Удаление из тега всех атрибутов
		/// </summary>
		/// <param name="tag">тег</param>
		/// <returns>тег без атрибутов</returns>
		private string RemoveAttributes(string tag)
		{
			int spaceIndex = tag.IndexOf(' ');
			if (spaceIndex > 0)
			{
				int index = tag.Length - 1;
				if (tag[index - 1] == '/')
					--index;
				return tag.Remove(spaceIndex, index - spaceIndex);
			}
			return tag;
		}

		/// <summary>
		/// Удаление из тега неймспейса
		/// </summary>
		/// <param name="tag">тег</param>
		/// <returns>тег без неймспейса</returns>
		private string RemoveNamespace(string tag)
		{
			int colonIndex = tag.IndexOf(':');
			if (colonIndex > 0)
			{
				int index = 1;
				if (tag[index] == '/')
					++index;
				return tag.Remove(index, colonIndex - index + 1);
			}
			return tag;
		}
	}
}
