using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core.Morpho;

namespace Linguistics.Core
{
	/// <summary>
	/// Временный класс для передачи информации о сущности из С++ в C#
	/// </summary>
	public class TextEntity
	{
		public string Text { get; set; }
		public string Value { get; set; }
		public string Type { get; set; }
		public int Start { get; set; }
		public int End { get; set; }
		public string MainNoun { get; set; }
		public string MainNounPrefix { get; set; }
		public string FirstName { get; set; }
		public string MiddleName { get; set; }
		public string LastName { get; set; }
		public string Name { get; set; }
        public string Post { get; set; }
		public string Language { get; set; }
		public bool IsPseudoPlace { get; set; }
		public string LinkedPlaces { get; set; }
		public List<string> MorphoInfo { get; set; }
		public List<string> Adjectives { get; set; }
		public List<Entity> SyntaxChildren { get; set; }

		#region [.ctor().]
		public TextEntity()
		{
			MorphoInfo = new List<string>();
			Adjectives = new List<string>();
			SyntaxChildren = new List<Entity>();
		}
		#endregion

		/// <summary>
		/// Создать сущность на основе свойств
		/// </summary>
		/// <returns>созданная сущность</returns>
		public Entity CreateEntity()
		{
			EntityInfoBase info = CreateEntityInfo(Type);
			SetMorphoInformation(info);
			info.Value = Value;
			return new Entity(Text, Start, info) { Language = Language, SyntaxChildren = SyntaxChildren.Any() ? SyntaxChildren : null };
		}

		/// <summary>
		/// Создать информацию о сущности по строкову представлению типа сущности
		/// </summary>
		/// <param name="type">строковое представление типа сущности</param>
		/// <returns>созданная информация о сущности</returns>
		private EntityInfoBase CreateEntityInfo(string type)
		{
			EntityType entityType;
			if (Enum.TryParse<EntityType>(Type, out entityType))
				return CreateEntityInfo(entityType);
			else
				return new AdditionalEntityInfo(Type);
		}

		/// <summary>
		/// Создать информацию о сущности по типу сущности
		/// </summary>
		/// <param name="type">тип сущности</param>
		/// <returns>созданная информация о сущности</returns>
		private EntityInfoBase CreateEntityInfo(EntityType type)
		{
			EntityInfoBase result = null;
			switch (type)
			{
				case EntityType.Pretext:
					result = new PretextEntityInfo();
					break;
				case EntityType.Numeric:
					result = new NumericEntityInfo();
					break;
				case EntityType.Pronoun:
					result = new PronounEntityInfo();
					break;
				case EntityType.Quote:
					result = new QuoteEntityInfo();
					break;
				case EntityType.Verb:
					result = new VerbEntityInfo();
					break;
				case EntityType.Participle:
					result = new ParticipleEntityInfo();
					break;
				case EntityType.Deepr:
					result = new DeeprEntityInfo();
					break;
                case EntityType.Infinitive:
                    result = new InfinitiveEntityInfo();
                    break;
				case EntityType.Adverb:
					result = new AdverbEntityInfo();
					break;
				case EntityType.Predicative:
					result = new PredicativeEntityInfo();
					break;
				case EntityType.Conjunction:
					result = new ConjunctionEntityInfo(SubSentenceType.Default);
					break;
				case EntityType.QuestionWord:
					result = new QuestionWordEntityInfo();
					break;
				case EntityType.Adjective:
					result = new AdjectiveEntityInfo();
					break;
				case EntityType.ShortAdjective:
					result = new ShortAdjectiveEntityInfo();
					break;
                case EntityType.Comparative:
                    result = new ComparativeEntityInfo();
                    break;
                case EntityType.Superlative:
                    result = new SuperlativeEntityInfo();
                    break;
				case EntityType.Date:
					result = new DateEntityInfo();
					break;
				case EntityType.Homogeneous:
					result = new HomogeneousEntityInfo();
					break;
				case EntityType.Unknown:
					result = SetNamedEntityProperties(new UnknownEntityInfo());
					break;
				case EntityType.UnknownProper:
					result = SetNamedEntityProperties(new UnknownProperEntityInfo());
					break;
				case EntityType.NounVerb:
					result = SetNamedEntityProperties(new NounVerbEntityInfo());
					break;
				case EntityType.PersonName:
					result = SetPersonEntityProperties(new PersonNameEntityInfo());
					break;
				case EntityType.PersonInfo:
					result = SetPersonEntityProperties(new PersonInfoEntityInfo());
					break;
				case EntityType.PersonNoInfo:
					result = SetPersonEntityProperties(new PersonNoInfoEntityInfo());
					break;
				case EntityType.OrgName:
					result = SetOrgEntityProperties(new OrgNameEntityInfo());
					break;
				case EntityType.OrgAbbr:
					result = SetNamedEntityProperties(new OrgAbbrEntityInfo());
					break;
				case EntityType.OrgNoInfo:
					result = SetNamedEntityProperties(new OrgNoInfoEntityInfo());
					break;
				case EntityType.Product:
					result = SetNamedEntityProperties(new ProductEntityInfo());
					break;
				case EntityType.Event:
					result = SetNamedEntityProperties(new EventEntityInfo());
					break;
				case EntityType.PlaceName:
				{
					var entityInfo = new PlaceNameEntityInfo();
					entityInfo.LinkedPlaces = LinkedPlaces;
					result = SetNamedEntityProperties(entityInfo);
					break;
				}
				case EntityType.PlaceNoInfo:
					result = SetNamedEntityProperties(new PlaceNoInfoEntityInfo());
					break;
				case EntityType.Address:
				case EntityType.BankAccount:
				case EntityType.Building:
				case EntityType.Corps:
				case EntityType.Email:
				case EntityType.Flat:
				case EntityType.House:
				case EntityType.Index:
				case EntityType.INN:
				case EntityType.Money:
				case EntityType.Phone:
				case EntityType.Slogan:
				case EntityType.Street:
				case EntityType.URL:
					result = new AdditionalEntityInfo(type.ToString());
					break;
				default:
					throw new InvalidOperationException("No EntityInfo for type: " + type.ToString());
			}
			return result;
		}

		/// <summary>
		/// Установка свойств в заданную информацию о персоне
		/// </summary>
		/// <param name="info">информация о персоне</param>
		/// <returns>информация о персоне</returns>
		private PersonEntityInfoBase SetPersonEntityProperties(PersonEntityInfoBase info)
		{
			SetNamedEntityProperties(info);
			info.FirstName = FirstName;
			info.LastName = LastName;
			info.MiddleName = MiddleName;
            info.Post = Post;
			return info;
		}

		/// <summary>
		/// Установка свойств в заданную информацию об организации
		/// </summary>
		/// <param name="info">информация об организации</param>
		/// <returns>информация об организации</returns>
		private OrgNameEntityInfo SetOrgEntityProperties(OrgNameEntityInfo info)
		{
			SetNamedEntityProperties(info);
			if (!string.IsNullOrEmpty(Name))
				info.Name = Name.Trim();
			return info;
		}

		/// <summary>
		/// Установка свойств в заданную информацию об именованной сущности
		/// </summary>
		/// <param name="info">информация об именованной сущности</param>
		/// <returns>информация об именованной сущности</returns>
		private NamedEntityInfoBase SetNamedEntityProperties(NamedEntityInfoBase info)
		{
			info.MainNoun = MainNoun;
			info.Prefix = MainNounPrefix;
			info.SetAdjectives(Adjectives);
			info.IsPseudoPlace = IsPseudoPlace;
			return info;
		}

		/// <summary>
		/// Задание морфологической информации для заданной информации о сущности
		/// </summary>
		/// <param name="info">информация о сущности</param>
		private void SetMorphoInformation(EntityInfoBase info)
		{
			info.Morpho = MorphoInfo.Select(_ => info.ParseMorphoInfo(_)).ToArray();
		}
	}
}
