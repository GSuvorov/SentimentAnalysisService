using System;

namespace TextMining.Core
{
    /// <summary>
    /// Типы подпредложения
    /// </summary>
    public enum SubsentenceType
    {
        /// <summary>
        /// неопределенный тип
        /// </summary>
        Default,

        /// <summary>
        /// часть текста в скобках
        /// </summary>
        ApproximatingForceps,

        /// <summary>
        /// деепричастный оборот
        /// </summary>
        DanglingParticiple,

        /// <summary>
        /// равноправное предложение
        /// </summary>
        EqualInRights,

        /// <summary>
        /// причастный оборот
        /// </summary>
        Participial,

        /// <summary>
        /// подчиненное предложение
        /// </summary>
        Subordinate,

        /// <summary>
        /// вводные слова 
        /// </summary>
        Introductory,


        DirectSpeech,

        /// <summary>
        /// June 09, 2012 11:00 AM
        /// Денис,
        /// Мы ввели новый тип подпредложения: AdjunctPhrase
        /// Добавь, пжста. Пока его тип аналогичен Subordinate (на него распространяются те же правила)
        /// </summary>
        AdjunctPhrase = Subordinate,
    }
}




