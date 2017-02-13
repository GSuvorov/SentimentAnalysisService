#pragma once

namespace SS{ 
namespace Dictionary{

//Общие типя для анализа
namespace HyphenAnalisisPrivate
{
   //Тип слова
   enum EWordClassification
   {
      WCUndefined,
      WCToLiboNibud,
      WCKoeKoy,
      WCPo,
      WCVVo,
      WCDeKaKoKosS
   };
   
   typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;

   //Классификация частей слов
   typedef AssocVector<std::wstring, EWordClassification> TWordClassificationStorage;

   //Список морфоинформации
   typedef Detail::TFeatureList TFeatureList;
   typedef std::vector<TFeatureList> TFeatureListForWord;

   //Интерфейс для всех правил анализа составных слов
   struct IRusRuleCheck
   {
      virtual ~IRusRuleCheck() {}
      
      ///////////////////////////////////////////////////////////////////////////////
      // Анализировать слово содержащие дефис
      // LeftWord, RightWord - Слово стоящее слева и справа от дефиса
      // LeftMorpho, RightMorpho - Морфоинформация левого и правого слова
      // MorphoFeature - Результирующая морфоинформация составного слова
      // WCS - Классификация частей слов
      virtual bool Check( 
         const std::wstring &LeftWord, const std::wstring &RightWord, 
         const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
         const TWordClassificationStorage &WCS,  
         CMorphoFeature &MorphoFeature
      ) const = 0;
   };

   //Информация об отдельном слове
   struct TWordPartInfo
   {
      std::wstring WordLower;   //Нижний регистр
      std::wstring WordOrigin;  //Оригинальный регистр
      TWIList MorphoInfo;       //Морфологическая информация слова
   };

   typedef std::vector<TWordPartInfo> TWordPartInfoList;

   //Интерфейс для всех правил анализа составных слов
   struct IEngRuleCheck
   {
      virtual ~IEngRuleCheck() {}

      ///////////////////////////////////////////////////////////////////////////////
      // Анализировать слово содержащие дефис
      // TFeatureListForWord - список морфоинформации о каждой часте слова, т.е. подстрок слова разделённых дефисами
      // MorphoFeature - Выходная морфоинформация
      virtual bool Check( 
         const TFeatureListForWord &FeatureListForWord,  
         TFeatureList &FeatureList
         ) const = 0;
   };


   //Запнить список FeatureList из WIList 
   void FillFeatureList( const TWIList &WIList, HyphenAnalisisPrivate::TFeatureList &FeatureList, IAMConverterMorpho *pAMConverter );

   inline EWordClassification GetType(const TWordClassificationStorage &WCS, const std::wstring &Str )
   {
      TWordClassificationStorage::const_iterator I = WCS.find( Str );

      return I != WCS.end() ? I->second : WCUndefined;
   }

} //namespace HyphenAnalisisPrivate

///////////////////////////////////////////////////////////////////////////////
// Класс проводит дополнительный анализ составных слов из словаря
///////////////////////////////////////////////////////////////////////////////
class TCompoundWordAnalysis: public NonCopyable
{ 
public:
   typedef HyphenAnalisisPrivate::TWordPartInfoList TWordPartInfoList;
   typedef HyphenAnalisisPrivate::TFeatureList TFeatureList;
   

private:
   typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;
   typedef std::vector<HyphenAnalisisPrivate::IRusRuleCheck *> TRussianRules;
   typedef std::vector<std::pair<HyphenAnalisisPrivate::IEngRuleCheck *, const char *> > TEnglishRules; //Список английский правил с описанием


private:
   HyphenAnalisisPrivate::TWordClassificationStorage m_WordClassification;
   TRussianRules m_RussianRules;
   TEnglishRules m_EnglishRules;

public:
   TCompoundWordAnalysis();
   ~TCompoundWordAnalysis();

   ///////////////////////////////////////////////////////////////////////////////
   // Анализировать слово на русском языке содержащие дефис
   // LeftWord, RightWord - Слово стоящее слева и справа от дефиса
   // LeftMorpho, RightMorpho - Морфоинформация левого и правого слова
   // MorphoFeature - Результирующая морфоинформация составного слова
   // 
   // ВОЗВР - удалось ли определить морфоинформацию результирующего слова
   // Правила см. в конструкторе
   ///////////////////////////////////////////////////////////////////////////////
   bool HyphenAnalisisRussian(
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TWIList &LeftMorpho, const TWIList &RightMorpho, 
      IAMConverterMorpho *pAMConverter,
      CMorphoFeature &MorphoFeature
   ) const;

   ///////////////////////////////////////////////////////////////////////////////
   // Анализировать слово на английском языке содержащие дефис
   // wcWord - Слово содержащие дефисы
   // PartInfoList - Морфоинформация о каждой части слова
   // FeatureList  - Результирующая морфоинформация составного слова
   // 
   // ВОЗВР - удалось ли определить морфоинформацию результирующего слова
   // Правила см. в конструкторе
   ///////////////////////////////////////////////////////////////////////////////
   bool HyphenAnalisisEnglish(
      const wchar_t* wcWord,
      const TWordPartInfoList &PartInfoList, 
      IAMConverterMorpho *pAMConverter,
      TFeatureList &FeatureList
   ) const;


};

} //namespace Dictionary
} //namespace SS
