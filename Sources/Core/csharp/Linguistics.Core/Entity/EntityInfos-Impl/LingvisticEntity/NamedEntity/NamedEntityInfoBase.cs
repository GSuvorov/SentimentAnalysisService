using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация об именованной сущности
	/// </summary>
    [Serializable]
    public abstract class NamedEntityInfoBase : LingvisticEntityInfoBase
	{
		/// <summary>
		/// Главное существительное сущности
		/// </summary>
		public string MainNoun { get; set; }

		/// <summary>
		/// Префикс сущности
		/// </summary>
		public string Prefix { get; set; }

		/// <summary>
		/// Флаг географии, определенной по геобъектному предлогу
		/// </summary>
		public bool IsPseudoPlace { get; set; }

        /// <summary>
        /// Название
        /// </summary>
        public string Name { get; set; }

		/// <summary>
		/// Прилагательные, которые могут быть отфильтрованы
		/// </summary>
		private List<string> _adjectivesForFilter;

		#region [.ctor().]
		protected NamedEntityInfoBase(EntityType type)
			: base(type)
		{
			_adjectivesForFilter = new List<string>();
		}
		#endregion

		/// <summary>
		/// Установить прилагательные
		/// </summary>
		/// <param name="adjectives">коллекция прилагательных</param>
		public void SetAdjectives(IEnumerable<string> adjectives)
		{
			_adjectivesForFilter = adjectives.ToList();
		}

		#region [ISerializerToRDF]
		public override XElement ToXElement()
		{
			XElement result = base.ToXElement();
			AddMorphoInfoAttribute(result);
			AddMainNounAttribute(result);
			AddPrefixAttribute(result);
			AddAdjectivesAttribute(result);
            AddNotEmptyAttribute(result, RDF.Attribute.Name, Name);
			return result;
		}

		/// <summary>
		/// Добавление к заданному элементу главного существительного цепочки, если это необходимо
		/// </summary>
		/// <param name="element">элемент</param>
		private void AddMainNounAttribute(XElement element)
		{
			if (MainNoun != Value)
				AddNotEmptyAttribute(element, RDF.Attribute.MainNoun, MainNoun);
		}

		/// <summary>
		/// Добавление к заданному элементу префикса, если он не пуст
		/// </summary>
		/// <param name="element">элемент</param>
		private void AddPrefixAttribute(XElement element)
		{
			AddNotEmptyAttribute(element, RDF.Attribute.Prefix, Prefix);
		}

		/// <summary>
		/// Добавление к заданному элементу всех прилагательных цепочки, если они есть
		/// </summary>
		/// <param name="element">элемент</param>
		private void AddAdjectivesAttribute(XElement element)
		{
			if (_adjectivesForFilter.Any())
			{
				StringBuilder builder = new StringBuilder();
				foreach (string adjective in _adjectivesForFilter)
				{
					if (!builder.IsEmpty())
						builder.Append(RDF.ValueSeparator);
					builder.Append(adjective);
				}
				AddNotEmptyAttribute(element, RDF.Attribute.Adjectives, builder.ToString());
			}
		}

		/// <summary>
		/// Добавление к заданному элементу заданного атрибута, если его значение не пусто
		/// </summary>
		/// <param name="element">элемент</param>
		/// <param name="attributeName">имя атрибута</param>
		/// <param name="attributeValue">значение атрибута</param>
		protected void AddNotEmptyAttribute(XElement element, string attributeName, string attributeValue)
		{
			if (!string.IsNullOrEmpty(attributeValue))
				element.SetAttributeValue(attributeName, attributeValue);
		}
		#endregion
	}
}
