using System;

#if WITH_OM_TM
using EntityRole = Lingvistics.EntityRole;
#else
using EntityRole = Linguistics.Core.EntityRole;
#endif

namespace TextMining.Core
{
    /// <summary>
    /// Роль сущности в предложении
    /// </summary>
    public enum RoleAttributeValue
    {
        //for possible new after [Linguistic] modify
        //Undefined,

        /// <summary>
        /// Обстоятельство
        /// </summary>
		Adj = EntityRole.Adj,

        /// <summary>
        /// 
        /// </summary>
		Adjective = EntityRole.Adjective,

        /// <summary>
        /// Наречие
        /// </summary>
		Adverb = EntityRole.Adverb,

        /// <summary>
        /// Дополнение
        /// </summary>
		Obj = EntityRole.Obj,

        /// <summary>
        /// Другая
        /// </summary>
		Other = EntityRole.Other,

        /// <summary>
        /// Подлежащее
        /// </summary>
		Subj = EntityRole.Subj,


        /// <summary>
        /// self-creating-new-type
        /// </summary>
        Enum,
        ObjAdj,
        AdjObj,
    }
}




