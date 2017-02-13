using System;

namespace OpinionMining
{
    internal enum DictionaryType
    {
        //Verbs
        VerbsOpinion1,
        VerbsOpinion2,
        VerbsOpinion3,
        VerbsOpinion4,
        VerbsOpinion5,
        VerbsOpinion6,
        VerbsOpinion7,

        //Phrasis
        Phrasis1,
        Phrasis2,

        //First-words
        FirstWords1, //single-words

        //
        IntrWords1, //multi-words
        IntrWords2, //single-words

        //
        EntityOpinion, //multi-words
    }

    internal enum DictionaryTypeVerbs
    {
        VerbsOpinion1 = DictionaryType.VerbsOpinion1,
        VerbsOpinion2 = DictionaryType.VerbsOpinion2,
        VerbsOpinion3 = DictionaryType.VerbsOpinion3,
        VerbsOpinion4 = DictionaryType.VerbsOpinion4,
        VerbsOpinion5 = DictionaryType.VerbsOpinion5,
        VerbsOpinion6 = DictionaryType.VerbsOpinion6,
        VerbsOpinion7 = DictionaryType.VerbsOpinion7,
    }

    internal enum DictionaryTypePhrasis1
    {
        Phrasis1 = DictionaryType.Phrasis1,
    }

    internal enum DictionaryTypePhrasis2
    {
        Phrasis2 = DictionaryType.Phrasis2,
    }

    internal enum DictionaryTypeSingleWords
    {
        FirstWords1 = DictionaryType.FirstWords1,
        IntrWords2  = DictionaryType.IntrWords2,
    }

    internal enum DictionaryTypeMultiWords
    {
        IntrWords1    = DictionaryType.IntrWords1,
        EntityOpinion = DictionaryType.EntityOpinion,
    }
}




