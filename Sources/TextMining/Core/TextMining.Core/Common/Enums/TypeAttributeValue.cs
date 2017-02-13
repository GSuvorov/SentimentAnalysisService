using System;

#if WITH_OM_TM
using EntityType = Lingvistics.EntityType;
#else
using EntityType = Linguistics.Core.EntityType;
#endif

namespace TextMining.Core
{
    /// <summary>
    /// Тип сущности
    /// </summary>
    public enum TypeAttributeValue
    {
        Adverb         = EntityType.Adverb,
        Conjunction    = EntityType.Conjunction,
        Default        = EntityType.Default,
        Date           = EntityType.Date,
        Deepr          = EntityType.Deepr,
        Homogeneous    = EntityType.Homogeneous,
        Numeric        = EntityType.Numeric,
        NounVerb       = EntityType.NounVerb,
        Participle     = EntityType.Participle,
        PersonName     = EntityType.PersonName,
        PersonInfo     = EntityType.PersonInfo,
        PersonNoInfo   = EntityType.PersonNoInfo,
        PlaceName      = EntityType.PlaceName,
        PlaceNoInfo    = EntityType.PlaceNoInfo,
        Predicative    = EntityType.Predicative,
        Pretext        = EntityType.Pretext,
        Pronoun        = EntityType.Pronoun,
        OrgName        = EntityType.OrgName,
        OrgAbbr        = EntityType.OrgAbbr,
        OrgNoInfo      = EntityType.OrgNoInfo,
        Unknown        = EntityType.Unknown,
        UnknownProper  = EntityType.UnknownProper,
        Verb           = EntityType.Verb,
        PseudoGeo      = EntityType.PseudoGeo,
        Quote          = EntityType.Quote,
        Adjective      = EntityType.Adjective,
        ShortAdjective = EntityType.ShortAdjective,
        Address        = EntityType.Address,
        Email          = EntityType.Email,
        BankAccount    = EntityType.BankAccount,
        URL            = EntityType.URL,
        Phone          = EntityType.Phone,
        INN            = EntityType.INN,
        Money          = EntityType.Money,
        Slogan         = EntityType.Slogan,
        Index          = EntityType.Index,
        Street         = EntityType.Street,
        House          = EntityType.House,
        Corps          = EntityType.Corps,
        Building       = EntityType.Building,
        Flat           = EntityType.Flat,
        Distance       = EntityType.Distance,
        Weight         = EntityType.Weight,
        Event          = EntityType.Event,
        Product        = EntityType.Product,
        QuestionWord   = EntityType.QuestionWord,
        Infinitive     = EntityType.Infinitive,
        Comparative    = EntityType.Comparative,
        Superlative    = EntityType.Superlative,

        //Будут у созданных элементов 'ENTITY' для колокаций
        Collocation,

        //Будут у созданных фиктивных элементов 'ENTITIES'
        Participial,
        DanglingParticiple,
        Subordinate,

        //Объект мониторинга
        Inquiry,
    }
}




