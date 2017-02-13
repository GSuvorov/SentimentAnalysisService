#pragma once


namespace SS{ 
namespace Dictionary{
namespace InitData{

/**
   Данные необходимые для инициализации словарей и других объектов в системе
*/
   //HMODULE - этой DLL
   extern HMODULE g_hModuleCurrentDLL;

   //Языки
   namespace Languages
   {
      enum eLanguages
      {
         English,
         Russian,
         
         COUNT,
         Undefined = COUNT
      };
   }

   //Зависимые от языка словари
   namespace LangDepDict
   {
      enum eLangDepDict
      {
         Smart,
         Abbreviations,

         COUNT
      };
   };

   //Идентификаторы инстанцированных объектов словарей и характеристик
   namespace ObjectID
   {
      enum eObjectID
      {
         //Зависмые от языка
         EngSmart,
         EngAbbreviations,
         RusSmart,
         RusAbbreviations,

         //Независимые от языка
		 OrderingRussian,
         ArabicNumbers,
         RomanNumbers,
         NumericOrdering,
         Symbols,

         DICTS_COUNT,
         FIRST_CHARACTERISTIC = DICTS_COUNT,

         //Другие характеристики слова
         ContainHypen = FIRST_CHARACTERISTIC,           //Не содержит дефис
         ContainQuotationS,                             //Не содержит апостроф и s
         ContainQuotationNT,                            //Не содержит апостроф и NT

         FULL_CHARACTERISTIC_COUNT
      };

   }

   //Соотвествие идентификаторов инстанцированных словарей, идентификатору ресурсов со всеми символами словаря
   extern const WORD DictionaryToResId[ObjectID::DICTS_COUNT];

   //Имя группы ресурсов со всеми символами словаря 
   extern const wchar_t DictionaryResClass[];
   
   //Имя группы ресурсов с данными для TConstData
   extern const wchar_t ConstDataResClass[];

   //Конвертирует язык и зависимый от языка словарь в идентификатор словаря
   inline ObjectID::eObjectID Convert( Languages::eLanguages Language, LangDepDict::eLangDepDict LangDepDict )
   {
      APL_ASSERT( Language >= 0 && LangDepDict >= 0 && Language < Languages::COUNT && LangDepDict < LangDepDict::COUNT );

      return static_cast<ObjectID::eObjectID>(Language * Languages::COUNT + LangDepDict);
   }

   //Первый независимый от языка словарь
   //Интервал независимых от языка словарей [ FirstUndepDict(), ObjectID::DICTS_COUNT )
   //Интервал зависимых [ 0, FirstUndepDict() )

   inline size_t FirstUndepDict()
   {
      return LangDepDict::COUNT * Languages::COUNT;
   }

   
  
} //namespace InitData
} //namespace Dictionary
} //namespace SS


