using System;

namespace TextMining.Core
{
    /// <summary>
    /// 
    /// </summary>
    public enum ObjectAllocateMethod
    {
        PhysicalOrJuridicalPersons = 0x1, //physical-or-juridical-persons
        FirstEntityWithRoleSubj    = (PhysicalOrJuridicalPersons << 1) + 1, //0x3, //first-entity-with-role-subj
        FirstEntityWithTypePronoun = (FirstEntityWithRoleSubj    << 1) + 1, //0x7, //first-entity-with-type-pronoun
        FirstVerbEntityWithRoleObj = (FirstEntityWithTypePronoun << 1) + 1, //0xf, //В случае, если ничего не сработало, т.е. ОТ по правилам не найден, 
                                                                                   //но в подпредложении есть VERB[ @TYPE=VERB ], то смотрим у него шестое значение MI=*****А/P. 
                                                                                   //Если MI==P, то в качестве ОТ брать первое ENTITY[ @ROLE=OBJ ]
    }
}




