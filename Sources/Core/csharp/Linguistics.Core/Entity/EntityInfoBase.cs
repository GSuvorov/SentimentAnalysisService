using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core.Morpho;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о сущности
	/// </summary>
    [Serializable]
	public abstract class EntityInfoBase : ISerializerToRDF
	{
		/// <summary>
		/// Тип сущности
		/// </summary>
		public EntityTypeStructure Type { get; private set; }
		/// <summary>
		/// Нормализованное значение
		/// </summary>
		public string Value { get; set; }

		/// <summary>
		/// Имя тега
		/// </summary>
		public abstract UnitTextType UnitTextType { get; }

		/// <summary>
		/// Морфологическая информация
		/// </summary>
		/// не может быть пустой массив
		public MorphoInformation[] Morpho { get; set; }

		#region [.morpho.]
		/// <summary>
		/// Преобразует заданное строковое представление в его эквивалент MorphoInformation
		/// </summary>
		/// <param name="morpho">строковое представление морфологической информации</param>
		/// <returns>бъект, эквивалентный строковому представлению</returns>
		public abstract MorphoInformation ParseMorphoInfo(string morpho);

		/// <summary>
		/// Преобразует заданный объект MorphoInformation в его строковый эквивалент
		/// </summary>
		/// <param name="morpho">объект MorphoInformation</param>
		/// <returns>строковый эквивалент</returns>
		public abstract string MorphoInfoToString(MorphoInformation morpho);
		#endregion

		#region [.ctor().]
		public EntityInfoBase(EntityTypeStructure type)
		{
			Type = type;
		}
		#endregion

		#region [ISerializerToRDF]
		public abstract XElement ToXElement();

		/// <summary>
		/// Добавление к заданном элементу атрибута морфологической информации
		/// </summary>
		/// <param name="element">элемент</param>
		protected void AddMorphoInfoAttribute(XElement element)
		{
			if (Morpho != null)
				element.SetAttributeValue(RDF.Attribute.MorphoInformation, GetMorphoString());
		}

		/// <summary>
		/// Получение строкового представления морфологической информации
		/// </summary>
		/// <returns>строковое представление морфологической информации</returns>
		private string GetMorphoString()
		{
			StringBuilder result = new StringBuilder();
			foreach (var morpho in Morpho)
			{
				if (!result.IsEmpty())
					result.Append(RDF.ValueSeparator);
				result.Append(MorphoInfoToString(morpho));
			}
			return result.ToString();
		}
		#endregion
	}
}
