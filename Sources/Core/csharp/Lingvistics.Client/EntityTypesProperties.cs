using System;
using System.Collections.Generic;

#if WITH_OM_TM
#else
using Linguistics.Core;
#endif

namespace Lingvistics.Client
{
    /// <summary>
    /// Класс для описания лингвистических сем. типов
    /// </summary>
    public static class EntityTypesProperties
    {
        private static Dictionary< EntityType, string > _Dict;

        static EntityTypesProperties()
        {
            _Dict = new Dictionary< EntityType, string >();
            _Dict.Add( EntityType.Address, "Адрес" );
            _Dict.Add( EntityType.Adjective, "Прилагательное" );
            _Dict.Add( EntityType.Adverb, "Наречие" );
            _Dict.Add( EntityType.BankAccount, "Банковский счет" );
            _Dict.Add( EntityType.Building, "Строение" );
            _Dict.Add( EntityType.Conjunction, "Союз" );
            _Dict.Add( EntityType.Corps, "Корпус" );
            _Dict.Add( EntityType.Date, "Дата или диапазон дат" );
            _Dict.Add( EntityType.Deepr, "Деепричастие" );
            _Dict.Add( EntityType.Distance, "Расстояние" );
            _Dict.Add( EntityType.Email, "Электронная почта" );
            _Dict.Add( EntityType.Flat, "Квартира" );
            _Dict.Add( EntityType.Homogeneous, "Цепочка однородных сущностей" );
            _Dict.Add( EntityType.House, "Дом" );
            _Dict.Add( EntityType.Index, "Индекс" );
            _Dict.Add( EntityType.INN, "ИНН" );
            _Dict.Add( EntityType.Money, "Денежные данные" );
            _Dict.Add( EntityType.NounVerb, "Цепочка согласованных существительных, в которой главное слово эвристически определилось как отглагольное существительное" );
            _Dict.Add( EntityType.Numeric, "Число" );
            _Dict.Add( EntityType.OrgAbbr, "Аббревиатура" );
            _Dict.Add( EntityType.OrgName, "Юридическое лицо" );
            _Dict.Add( EntityType.OrgNoInfo, "Тип организации" );
            _Dict.Add( EntityType.Participle, "Причастие" );
            _Dict.Add( EntityType.PersonInfo, "Физическое лицо" );
            _Dict.Add( EntityType.PersonName, "Физическое лицо с фамилией" );
            _Dict.Add( EntityType.PersonNoInfo, "Профессия или должность" );
            _Dict.Add( EntityType.Phone, "Номер телефона" );
            _Dict.Add( EntityType.PlaceName, "Географический объект" );
            _Dict.Add( EntityType.PlaceNoInfo, "Тип географического объекта" );
            _Dict.Add( EntityType.Predicative, "Предикатив" );
            _Dict.Add( EntityType.Pretext, "Предлог" );
            _Dict.Add( EntityType.Pronoun, "Местоимение" );
            _Dict.Add( EntityType.PseudoGeo, "Несловарный географический объект" );
            _Dict.Add( EntityType.Quote, "Кавычка" );
            _Dict.Add( EntityType.ShortAdjective, "Краткое прилагательное" );
            _Dict.Add( EntityType.Slogan, "Лозунг" );
            _Dict.Add( EntityType.Street, "Улица" );
            _Dict.Add( EntityType.Unknown, "Прочие объекты" );
            _Dict.Add( EntityType.UnknownProper, "Неидентифицированные объекты" );
            _Dict.Add( EntityType.URL, "URL" );
            _Dict.Add( EntityType.Verb, "Глагол" );
            _Dict.Add( EntityType.Weight, "Вес" );
            _Dict.Add( EntityType.Event, "Событие" );
            _Dict.Add( EntityType.Product, "Продукт" );
        }

        /// <summary>
        /// Получение описания сем. типа
        /// </summary>
        /// <param name="type">сем. тип</param>
        /// <returns>описание</returns>
        public static string GetTypeDescription( EntityType type )
        {
            string description;
            return (_Dict.TryGetValue( type, out description ) ? description : string.Empty);
        }
    }
}
