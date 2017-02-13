using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace SimpleEntities
{
	/// <summary>
	/// Класс для доступа к регулярным выражениям
	/// </summary>
	internal class SimpleEntityResource
	{
		#region [.attributes.]
		/// <summary>
		/// Имя
		/// </summary>
		private const string NameAttribute = "Name";
		/// <summary>
		/// Тип
		/// </summary>
		private const string TypeAttribute = "TYPE";
		/// <summary>
		/// Тип выделения
		/// </summary>
		private const string SelectionTypeAttribute = "Select";
		/// <summary>
		/// Префикс аттрибута словаря
		/// </summary>
		private const string DictionaryAttribute = "dictionary";
		/// <summary>
		/// Имя группы со значением сущности
		/// </summary>
		private const string ValueGroupName = "value";
		#endregion

		/// <summary>
		/// регулярное выражение для выделения регулярных выражений внутри других регулярных выражений
		/// </summary>
		private /*static*/ readonly Regex _usedEntityRegex = new Regex("[(]@(?<type>[0-9A-Za-zА-Яа-я_]{1,})[)]");

		/// <summary>
		/// Регулярные выражения для выделения сущностей
		/// тип - выражение
		/// </summary>
        private readonly Dictionary<string, EntityExpression> _expressions = new Dictionary<string, EntityExpression>();

		#region [.ctor()]
		public SimpleEntityResource(string path, string fileName)
		{
			LoadExpressions(path, fileName);
		}

		/// <summary>
		/// Загрузка регулярных выражений из указанного файла
		/// </summary>
		/// <param name="filePath">путь к файлу с регулярными выражениями</param>
		/// <param name="fileName">имя файла с регулярными выражениями</param>
		private void LoadExpressions(string path, string fileName)
		{
			XElement xml = XElement.Load(Path.Combine(path, fileName));
			foreach (XElement expression in xml.Elements())
			{
				string type = expression.Attribute(TypeAttribute).Value;
				if (_expressions.ContainsKey(type))
					throw new InvalidOperationException(string.Format("type '{0}' is already defined", type));

				EntityExpression entityExpression = new EntityExpression() { EntityType = type };
				StringBuilder regularExpression = new StringBuilder(expression.Element("regex").Value);

				ReplaceAndSaveUsedExpressions(entityExpression, regularExpression);
				ReadAttributes(expression, entityExpression, regularExpression, path);

				entityExpression.SaveRegex(regularExpression.ToString());
				_expressions.Add(type, entityExpression);
			}
		}

		/// <summary>
		/// Чтение атрибутов выражения
		/// </summary>
		/// <param name="expression">выражение</param>
		/// <param name="entityExpression">выражение для выделения сущности</param>
		/// <param name="regularExpression">регулярное выражение для выделения сущности</param>
		private void ReadAttributes(XElement expression, EntityExpression entityExpression, StringBuilder regularExpression, string path)
		{
			foreach (XAttribute attribute in expression.Attributes())
			{
				switch (attribute.Name.ToString())
				{
					case SelectionTypeAttribute:
						entityExpression.SelectionType = (SelectionType)Enum.Parse(typeof(SelectionType), attribute.Value);
						break;
					case NameAttribute:
						entityExpression.EntityType = attribute.Value;
						break;
					default:
						if (IsDictionaryAttribute(attribute))
							regularExpression.Replace(attribute.Name.ToString(), GetRegExVariantsFromDictionary(path, attribute.Value));
						else if (attribute.Name != TypeAttribute)
							throw new InvalidOperationException("Wrong attribute: " + attribute.Name);
						break;
				}
			}
		}

		/// <summary>
		/// Получение регулярного выражения для вариантов из указанного словаря
		/// </summary>
		/// <param name="rootPath">путь к файлу с регулярными выражениями</param>
		/// <param name="relativeDitionaryPath">относительный путь к словарю</param>
		/// <returns>регулярное выражение для вариантов</returns>
		private string GetRegExVariantsFromDictionary(string rootPath, string relativeDitionaryPath)
		{
			var dictionaryLines = Utils.Dictionary.Read(Path.Combine(rootPath, relativeDitionaryPath), Encoding.UTF8);
			return Utils.RegexHelper.GetVariantsFromCollection(dictionaryLines);
		}

		/// <summary>
		/// Замена и сохранение подвыражений
		/// </summary>
		/// <param name="entityExpression">выражение для выделения сущности</param>
		/// <param name="regularExpression">регулярное выражение для выделения сущности</param>
		private void ReplaceAndSaveUsedExpressions(EntityExpression entityExpression, StringBuilder regularExpression)
		{
			MatchCollection matches = _usedEntityRegex.Matches(regularExpression.ToString());
			foreach (Match match in matches)
			{
				string savedExpression = match.Groups["type"].Value;
				if (entityExpression.UsedExpressions.Contains(savedExpression))
					continue;
				entityExpression.UsedExpressions.Add(savedExpression);
				regularExpression.Replace(match.Value, GetChildExpression(_expressions[savedExpression]));
			}
		}

		/// <summary>
		/// Проверка, что данный атрибут - атрибут словаря
		/// </summary>
		/// <param name="attribute">атрибут</param>
		/// <returns>результат проверки</returns>
		private bool IsDictionaryAttribute(XAttribute attribute)
		{
			bool isDictionaryAttribute = false;
			string name = attribute.Name.ToString();
			if (name.StartsWith(DictionaryAttribute) &&
				(name.Length == DictionaryAttribute.Length + 1) &&
				Char.IsDigit(name[DictionaryAttribute.Length]))
			{
				isDictionaryAttribute = true;
			}
			return isDictionaryAttribute;
		}

		/// <summary>
		/// Возврат регулярного выражения для дочерней сущности
		/// </summary>
		/// <param name="expression">выражение для выделения сущности</param>
		/// <returns>регулярное выражение для дочерней сущности</returns>
		private string GetChildExpression(EntityExpression expression)
		{
			return string.Format("({0})", expression.Expression.Replace(string.Format("?<{0}>", ValueGroupName), ""));
		}
		#endregion

		/// <summary>
		/// Получение группы, содержащей значение сущности
		/// </summary>
		/// <param name="match">выделение</param>
		/// <returns>группа, содержащая значение сущности</returns>
		public static Group GetValueGroup(Match match)
		{
			Group result = match.Groups[ValueGroupName];
			if (!result.Success)
				result = match;
			return result;
		}

		/// <summary>
		/// Получение поддерживаемых типов
		/// </summary>
		/// <returns>поддерживаемые типы</returns>
		public IEnumerable<string> GetAllTypes()
		{
			return _expressions.Keys;
		}

		/// <summary>
		/// Получение выражения для выделения сущности по заданному типу
		/// </summary>
		/// <param name="type">тип выражения</param>
		/// <returns>выражение</returns>
		public EntityExpression GetExpression(string type)
		{
			EntityExpression result;
			if (!_expressions.TryGetValue(type, out result))
				throw new ArgumentException("Not supported type");
			return result;
		}
	}
}
