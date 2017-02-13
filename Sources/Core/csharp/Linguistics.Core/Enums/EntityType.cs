using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if WITH_OM_TM
namespace Lingvistics
#else
namespace Linguistics.Core
#endif
{
	/// <summary>
	/// Тип сущности
	/// </summary>
	public enum EntityType
	{
		/// <summary>
		/// Наречие
		/// </summary>
		Adverb = 0,
		/// <summary>
		/// Союз
		/// </summary>
		Conjunction = 1,
		/// <summary>
		/// значение по умолчанию
		/// </summary>
		Default = 2,
		/// <summary>
		/// Диапазон времени
		/// </summary>
		Date = 3,
		/// <summary>
		/// Деепричастие
		/// </summary>
		Deepr = 4,
		/// <summary>
		/// Цепочка однородных сущностей
		/// </summary>
		Homogeneous = 5,
		/// <summary>
		/// Числительные и числа
		/// </summary>
		Numeric = 6,
		/// <summary>
		/// Отглагольное существительное
		/// </summary>
		NounVerb = 7,
		/// <summary>
		/// Причастие
		/// </summary>
		Participle = 8,
		/// <summary>
		/// Персона с фамилией
		/// </summary>
		PersonName = 9,
		/// <summary>
		/// Персона
		/// </summary>
		PersonInfo = 10,
		/// <summary>
		/// Профессия или должность
		/// </summary>
		PersonNoInfo = 11,
		/// <summary>
		/// Географическое название
		/// </summary>
		PlaceName = 12,
		/// <summary>
		/// Тип географического объекта
		/// </summary>
		PlaceNoInfo = 13,
		/// <summary>
		/// Предикатив
		/// </summary>
		Predicative = 14,
		/// <summary>
		/// Предлог
		/// </summary>
		Pretext = 15,
		/// <summary>
		/// Местоимение
		/// </summary>
		Pronoun = 16,
		/// <summary>
		/// Ирюдическое лицо
		/// </summary>
		OrgName = 17,
		/// <summary>
		/// Аббревиатура
		/// </summary>
		OrgAbbr = 18,
		/// <summary>
		/// Тип организации
		/// </summary>
		OrgNoInfo = 19,
		/// <summary>
		/// Согласованная цепочка слов
		/// </summary>
		Unknown = 20,
		/// <summary>
		/// Неидентифицированные объекты
		/// </summary>
		UnknownProper = 21,
		/// <summary>
		/// Глагол
		/// </summary>
		Verb = 22,
		/// <summary>
		/// Географический объект, не прошедший проверки по словарю
		/// </summary>
		PseudoGeo = 23,
		/// <summary>
		/// Кавычка
		/// </summary>
		Quote = 24,
		/// <summary>
		/// Прилагательное
		/// </summary>
		Adjective = 25,
		/// <summary>
		/// Краткое прилагательное
		/// </summary>
		ShortAdjective = 26,
		/// <summary>
		/// Адрес
		/// </summary>
		Address = 27,
		/// <summary>
		/// Электронная почта
		/// </summary>
		Email = 28,
		/// <summary>
		/// Банковский счет
		/// </summary>
		BankAccount = 29,
		/// <summary>
		/// url
		/// </summary>
		URL = 30,
		/// <summary>
		/// Номер телефона
		/// </summary>
		Phone = 31,
		/// <summary>
		/// ИНН
		/// </summary>
		INN = 32,
		/// <summary>
		/// Денежные данные
		/// </summary>
		Money = 33,
		/// <summary>
		/// Лозунг
		/// </summary>
		Slogan = 34,
		/// <summary>
		/// Индекс
		/// </summary>
		Index = 35,
		/// <summary>
		/// Улица
		/// </summary>
		Street = 36,
		/// <summary>
		/// Дом
		/// </summary>
		House = 37,
		/// <summary>
		/// Корпус
		/// </summary>
		Corps = 38,
		/// <summary>
		/// Строение
		/// </summary>
		Building = 39,
		/// <summary>
		/// Квартира
		/// </summary>
		Flat = 40,
		/// <summary>
		/// Расстояние
		/// </summary>
		Distance = 41,
		/// <summary>
		/// Вес
		/// </summary>
		Weight = 42,
		/// <summary>
		/// Событие
		/// </summary>
		Event = 43,
		/// <summary>
		/// Продукт
		/// </summary>
		Product = 44,
		/// <summary>
		/// Вопросительное слово
		/// </summary>
		QuestionWord = 45,
        /// <summary>
        /// Инфинитив
        /// </summary>
        Infinitive = 46,
        /// <summary>
        /// Сравнительная степень прилагательного
        /// </summary>
        Comparative = 47,
        /// <summary>
        /// Превосходная степень прилагательного
        /// </summary>
        Superlative = 48,
	}
}
