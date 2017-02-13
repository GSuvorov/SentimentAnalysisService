#include "stdafx.h"

namespace SS{ 
namespace Dictionary{

namespace HyphenAnalisisPrivate
{

//Набор функторов для анализа

//Сравнение только частей речи
class TPartOfSpeechEqual: public std::unary_function<CMorphoFeature, bool>
{
   PartOfSpeechTypes m_Val;

public:
   TPartOfSpeechEqual(const PartOfSpeechTypes &Val): m_Val(Val) {}

   bool operator()( const CMorphoFeature &MF ) const
   {
      return MF.m_PartOfSpeechTypes.IsContain( m_Val );
   }
};

//Сравнение NumericType
class TNumericTypeEqual: public std::unary_function<CMorphoFeature, bool>
{
   NumericType m_NT;

public:
   TNumericTypeEqual(const NumericType &NT):  m_NT(NT) {}

   bool operator()( const CMorphoFeature &MF ) const
   {
      return 
         MF.m_PartOfSpeechTypes.IsContain( Values::PartOfSpeechTypes.postNumeric )&&
         MF.m_NumericType.IsContain( m_NT );       
   }

   void Set( CMorphoFeature &MF ) const
   {
      MF.m_PartOfSpeechTypes.Assign( Values::PartOfSpeechTypes.postNumeric );
      MF.m_NumericType.Assign( m_NT );       
   }
};


//postVerb, vfParticiple, ?
struct TParticiple:  std::unary_function<CMorphoFeature, bool>
{
   TimeType m_TimeType;

   TParticiple( const TimeType &TimeType ): m_TimeType(TimeType) {}

   bool operator()( const CMorphoFeature &MF ) const
   {
      return 
         MF.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) &&
         MF.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) &&
         MF.m_TimeType.IsContain(m_TimeType);       
   }
};

//postVerb, vfInfinitive
struct TInfinitive:  std::unary_function<CMorphoFeature, bool>
{
   bool operator()( const CMorphoFeature &MF ) const
   {
      return 
         MF.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) &&
         MF.m_VerbForm.IsContain(Values::VerbForm.vfInfinitive);
   }
};

//Pronoun, pngNoun
struct TPronounNoun:  std::unary_function<CMorphoFeature, bool>
{
   bool operator()( const CMorphoFeature &MF ) const
   {
      return 
         MF.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun) &&
         MF.m_PronounGroup.IsContain(Values::PronounGroup.pngNoun);
   }
};

//Pronoun, pngNoun
struct TPronounIndividual:  std::unary_function<CMorphoFeature, bool>
{
   bool operator()( const CMorphoFeature &MF ) const
   {
      return 
         MF.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun) &&
         MF.m_PronounClass.IsContain(Values::PronounClass.pncIndividual);
   }
};

//Сравнение OfficialType
class TOfficialTypeEqual: public std::unary_function<CMorphoFeature, bool>
{
   OfficialType m_OT;

public:
   TOfficialTypeEqual(const OfficialType &OT):  m_OT(OT) {}

   bool operator()( const CMorphoFeature &MF ) const
   {
      return 
         MF.m_PartOfSpeechTypes.IsContain( Values::PartOfSpeechTypes.postOfficial )&&
         MF.m_OfficialType.IsContain( m_OT );       
   }
};

//прил в ед.ч.м.р.Д.п. или мест-прил в ед.ч.м.р.Д.п.
struct TPronounAdjective:  std::unary_function<CMorphoFeature, bool>
{
   bool operator()( const CMorphoFeature &MF ) const
   {
      return 
         MF.m_NumberType.IsContain(Values::NumberType.ntSingle) &&
         MF.m_GenderType.IsContain(Values::GenderType.gtMasculine) &&
         MF.m_CaseType.IsContain(Values::CaseType.ctDative) &&
         (
            MF.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdjective) ||
            (
               MF.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun) &&
               MF.m_PronounGroup.IsContain(Values::PronounGroup.pngAdjective)
            )
         );
   }
};


//порядковое числительное мн.ч. В.п.
struct TOrdinalNumber:  std::unary_function<CMorphoFeature, bool>
{
   bool operator()( const CMorphoFeature &MF ) const
   {
      return 
         MF.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNumeric) &&
         MF.m_NumberType.IsContain(Values::NumberType.ntPlural) &&
         MF.m_NumericType.IsContain(Values::NumericType.ntOrdering);       
   }
};

///////////////////////////////////////////////////////////////////////////////
void FillFeatureList( const TWIList &WIList, TFeatureList &FeatureList, IAMConverterMorpho *pAMConverter )
{
   APL_ASSERT_PTR(pAMConverter);

   TWIList::const_iterator I;
   TWordInfo::const_iterator J;

   FeatureList.clear();

   for( I = WIList.begin(); I != WIList.end(); ++I )
      for( J = I->begin(); J != I->end(); ++J )
      {
         FeatureList.push_back( TFeatureList::value_type() );
         pAMConverter->EnCode( J->m_WordIndex.m_DictIndex.GetDictionaryNumber(), J->m_MorphoTypeInfo.m_MorphoInfo, J->m_SpecialMorpho, &FeatureList.back() );
      }
}

///////////////////////////////////////////////////////////////////////////////
// Составные слова РУССКИЙ
///////////////////////////////////////////////////////////////////////////////

//1. Наречие + -то, -либо, -нибудь -> Наречие
struct TAdverb_To :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
   ) const
   {
      TFeatureList::const_iterator I = std::find_if(LeftMorpho.begin(), LeftMorpho.end(), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postAdverb) );

      if( I != LeftMorpho.end() && GetType(WCS, RightWord) == WCToLiboNibud )
      {
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
            "' по правилу '1. Наречие + -то, -либо, -нибудь -> Наречие'"
         );

         MorphoFeature = *I;
         return true;
      }

      return false;     
   }
};

//2. Кое-, кой- + наречие -> Наречие
struct TKoe_Adverb :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
      ) const
   {
      TFeatureList::const_iterator I = std::find_if(RightMorpho.begin(), RightMorpho.end(), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postAdverb) );

      if( I != RightMorpho.end() && GetType(WCS, LeftWord) == WCKoeKoy )
      {
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
            "' по правилу '2. Кое-, кой- + наречие -> Наречие'"
         );
         
         MorphoFeature = *I;
         return true;
      }

      return false;     
   }
};
                            
//3.По- + прил в ед.ч.м.р.Д.п. или мест-прил в ед.ч.м.р.Д.п. -> Наречие
struct TPo_PronounAdjective :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
      ) const
   {
      TFeatureList::const_iterator I = std::find_if(RightMorpho.begin(), RightMorpho.end(), TPronounAdjective() );

      if( I != RightMorpho.end() && GetType(WCS, LeftWord) == WCPo )
      {
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
            "' по правилу '3. По- + прил в ед.ч.м.р.Д.п. или мест-прил в ед.ч.м.р.Д.п. -> Наречие'"
            );

         //MorphoFeature = *I;
         MorphoFeature.m_PartOfSpeechTypes.Assign( Values::PartOfSpeechTypes.postAdverb );
         return true;
      }

      return false;     
   }
};

//4. По- + прилагательное, заканчивающееся на –ски -> Наречие
struct TPo_AdjectiveSki :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
      ) const
   {
      static const WCHAR FindStr[] = L"ски";

      if( RightWord.size() < APL_ARRSIZE(FindStr) - 1 || GetType(WCS, LeftWord) != WCPo )
         return false;
      
      TFeatureList::const_iterator I = std::find_if(RightMorpho.begin(), RightMorpho.end(), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postAdjective) );
     
      if( I == RightMorpho.end()  )
         return false;

      if( std::mismatch(RightWord.begin() + RightWord.size() - (APL_ARRSIZE(FindStr) - 1), RightWord.end(), FindStr).first == RightWord.end() )
      {
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
            "' по правилу '4. По- + прилагательное, заканчивающееся на –ски -> Наречие'"
            );

         //APL_ASSERT( !RightMorpho.empty() );
         //MorphoFeature = RightMorpho.front();
         MorphoFeature.m_PartOfSpeechTypes.Assign( Values::PartOfSpeechTypes.postAdverb );
         return true;
      }

      return false;     
   }
};


//5.	В- или во- + порядковое числительное мн.ч. В.п. -> Наречие
struct TVo_Numeral :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
      ) const
   {
      TFeatureList::const_iterator I = std::find_if(RightMorpho.begin(), RightMorpho.end(), TOrdinalNumber() );

      if( I != RightMorpho.end() && GetType(WCS, LeftWord) == WCVVo )
      {
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
            "' по правилу '5. В- или во- + порядковое числительное мн.ч. В.п. -> Наречие'"
            );

         //MorphoFeature = *I;
         MorphoFeature.m_PartOfSpeechTypes.Assign( Values::PartOfSpeechTypes.postAdverb );
         return true;
      }

      return false;     
   }
};


//6. Местоимение + -то, -либо, -нибудь -> Местоимение
struct TPronoun_To :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
      ) const
   {
      TFeatureList::const_iterator I = std::find_if(LeftMorpho.begin(), LeftMorpho.end(), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postPronoun) );

      if( I != LeftMorpho.end() && GetType(WCS, RightWord) == WCToLiboNibud )
      {
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
            "' по правилу '6. Местоимение + -то, -либо, -нибудь -> Местоимение'"
            );

         MorphoFeature = *I;
         return true;
      }

      return false;     
   }
};

//7.	Кое-, кой- + местоимение -> Местоимение
struct TKoe_Pronoun :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
      ) const
   {
      TFeatureList::const_iterator I = std::find_if(RightMorpho.begin(), RightMorpho.end(), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postPronoun) );

      if( I != RightMorpho.end() && GetType(WCS, LeftWord) == WCKoeKoy )
      {
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
            "' по правилу '7. Кое-, кой- + местоимение -> Местоимение'"
            );

         MorphoFeature = *I;
         return true;
      }

      return false;     
   }
};

//8.	часть речи + –де, -ка, -ко, -кось, -с -> часть речи первой половины слова
struct TAny_De :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
      ) const
   {
      if( GetType(WCS, RightWord) == WCDeKaKoKosS )
      {
         
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
            "' по правилу '8. часть речи + –де, -ка, -ко, -кось, -с -> Часть речи первой половины слова'"
            );

         APL_ASSERT( !LeftMorpho.empty() );
         MorphoFeature = LeftMorpho.front();
         return true;
      }

      return false;     
   }
};

//9. Часть речи по второй половине слова
struct TLastWord :IRusRuleCheck
{
   virtual bool Check( 
      const std::wstring &LeftWord, const std::wstring &RightWord, 
      const TFeatureList &LeftMorpho, const TFeatureList &RightMorpho, 
      const TWordClassificationStorage &WCS,  
      CMorphoFeature &MorphoFeature
      ) const
   {    
      DEBUG_MSG_LOG4( 
         "Опредиление дефиса '" << Convert(LeftWord, std::string()) << '-' << Convert(RightWord, std::string()) <<
         "' по правилу '9. Часть речи по второй половине слова'"
         );

      APL_ASSERT( !LeftMorpho.empty() );
      MorphoFeature = RightMorpho.front();
      return true;

      return false;     
   }
};


///////////////////////////////////////////////////////////////////////////////
// Составные слова АНГЛИЙСКИЙ
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Шаблон предоставляет общий интерфейс для всех правил поиска
///////////////////////////////////////////////////////////////////////////////
template<class T1, class T2 = NullType, class T3 = NullType, class T4 = NullType>
class TGenericRule: public IEngRuleCheck 
{  
   TFeatureList m_FeatureList;
   T1 m_T1;
   T2 m_T2;
   T3 m_T3;
   T4 m_T4;

private:
   //Добавить данные в результат
   void AppendResult( TFeatureList &Result, const TFeatureList &NewData ) const
   {
      //Необходимо исключить дублирование частей речи
      TFeatureList::const_iterator I, J;

      for( I = NewData.begin(); I != NewData.end(); ++I )
      {
         for( J = Result.begin(); J != Result.end() && !J->m_PartOfSpeechTypes.IsIntersectsWith(&I->m_PartOfSpeechTypes); ++J ) { ; }

         if( J == Result.end() )
            Result.push_back(*I);
      }
   }

   bool CheckImpl( 
      const TFeatureListForWord &FeatureListForWord, TFeatureList &FeatureList,
      Int2Type<false>, Int2Type<false>, Int2Type<false>, Int2Type<false>  
   ) const
   {
      if( FeatureListForWord.size() != 4 )
         return false;
      
      if( 
         std::find_if(FeatureListForWord[0].begin(), FeatureListForWord[0].end(), m_T1) !=  FeatureListForWord[0].end() &&
         std::find_if(FeatureListForWord[1].begin(), FeatureListForWord[1].end(), m_T2) !=  FeatureListForWord[1].end() &&
         std::find_if(FeatureListForWord[2].begin(), FeatureListForWord[2].end(), m_T3) !=  FeatureListForWord[2].end() &&
         std::find_if(FeatureListForWord[3].begin(), FeatureListForWord[3].end(), m_T4) !=  FeatureListForWord[3].end()
      )
      {
         AppendResult( FeatureList, m_FeatureList );
         return true;
      }

      return false;
   }

   bool CheckImpl( 
      const TFeatureListForWord &FeatureListForWord, TFeatureList &FeatureList,
      Int2Type<false>, Int2Type<false>, Int2Type<false>, Int2Type<true>  
   ) const
   {
      if( FeatureListForWord.size() != 3 )
         return false;
      
      if( 
         std::find_if(FeatureListForWord[0].begin(), FeatureListForWord[0].end(), m_T1) !=  FeatureListForWord[0].end() &&
         std::find_if(FeatureListForWord[1].begin(), FeatureListForWord[1].end(), m_T2) !=  FeatureListForWord[1].end() &&
         std::find_if(FeatureListForWord[2].begin(), FeatureListForWord[2].end(), m_T3) !=  FeatureListForWord[2].end()
      )
      {
         AppendResult( FeatureList, m_FeatureList );
         return true;
      }

      return false;
   }

   bool CheckImpl( 
      const TFeatureListForWord &FeatureListForWord, TFeatureList &FeatureList,
      Int2Type<false>, Int2Type<false>, Int2Type<true>, Int2Type<true>  
   ) const
   {
      if( FeatureListForWord.size() != 2 )
         return false;
      
      if( 
         std::find_if(FeatureListForWord[0].begin(), FeatureListForWord[0].end(), m_T1) !=  FeatureListForWord[0].end() &&
         std::find_if(FeatureListForWord[1].begin(), FeatureListForWord[1].end(), m_T2) !=  FeatureListForWord[1].end()
      )
      {
         AppendResult( FeatureList, m_FeatureList );
         return true;
      }

      return false;
   }

public:
   ///////////////////////////////////////////////////////////////////////////////
   // FeatureList - Результирующий список морфоинформации, если все проверки завершились успешно
   // T1 - TN     - Функтор, Правила для каждой части слова разделённого дефисами
   TGenericRule( const TFeatureList &FeatureList, const T1 &ob1 = T1(), const T2 &ob2 = T2(), const T3 &ob3 = T3(), const T4 &ob4 = T4() ):
      m_FeatureList(FeatureList), m_T1(ob1), m_T2(ob2), m_T3(ob3), m_T4(ob4)
   {}
   
   ///////////////////////////////////////////////////////////////////////////////
   // Проверить подходит ли слово 
   // FeatureListForWord - морфоинформацияпо каждой части дефисного слова
   // FeatureList - Результирующая морфоиформация, если все проверки завершились успешно
   bool Check( 
      const TFeatureListForWord &FeatureListForWord,  
      TFeatureList &FeatureList
   ) const
   {
      return CheckImpl( FeatureListForWord, FeatureList, 
         Int2Type<IsSameType<T1, NullType>::value>(), 
         Int2Type<IsSameType<T2, NullType>::value>(),
         Int2Type<IsSameType<T3, NullType>::value>(),
         Int2Type<IsSameType<T4, NullType>::value>()
     );
   }
};

///////////////////////////////////////////////////////////////////////////////
// Класс предоставляет удобный механизм для заполнения TFeatureList
///////////////////////////////////////////////////////////////////////////////
class TMakeFeatureList
{
   TFeatureList m_FeatureList;

public:
   operator const TFeatureList&() const { return m_FeatureList; }
   
   TMakeFeatureList &operator%( const PartOfSpeechTypes &Val )
   {
      m_FeatureList.push_back( TFeatureList::value_type() );
      m_FeatureList.back().m_PartOfSpeechTypes.Assign( Val );
      return *this;
   }

   template<class T>
   TMakeFeatureList &operator%( const T &Val )
   {
      m_FeatureList.push_back( TFeatureList::value_type() );
      Val.Set( m_FeatureList.back() );
      return *this;
   }
};


} //namespace HyphenAnalisisPrivate

using namespace HyphenAnalisisPrivate;

///////////////////////////////////////////////////////////////////////////////
// class TCompoundWordAnalysis
///////////////////////////////////////////////////////////////////////////////
TCompoundWordAnalysis::TCompoundWordAnalysis()
{
#ifdef REGISTER_HELPER
   #error "REGISTER_HELPER already defined"
#endif

#define REGISTER_HELPER( arg1, arg2 ) m_WordClassification.insert( TWordClassificationStorage::value_type(arg1, arg2) )

   REGISTER_HELPER( L"то", WCToLiboNibud );
   REGISTER_HELPER( L"либо", WCToLiboNibud );
   REGISTER_HELPER( L"нибудь", WCToLiboNibud );
   REGISTER_HELPER( L"кое", WCKoeKoy );
   REGISTER_HELPER( L"кой", WCKoeKoy );
   REGISTER_HELPER( L"по", WCPo );
   REGISTER_HELPER( L"в", WCVVo );
   REGISTER_HELPER( L"во", WCVVo );
   REGISTER_HELPER( L"де", WCDeKaKoKosS );
   REGISTER_HELPER( L"ка", WCDeKaKoKosS );
   REGISTER_HELPER( L"ко", WCDeKaKoKosS );
   REGISTER_HELPER( L"кось", WCDeKaKoKosS );
   REGISTER_HELPER( L"с", WCDeKaKoKosS );

#undef REGISTER_HELPER

   ///////////////////////////////////////////////////////////////////////////////
   // РУССКИЙ
   
   //1. Наречие + -то, -либо, -нибудь -> Наречие
   m_RussianRules.push_back(new TAdverb_To );
   
   //2. Кое-, кой- + наречие -> Наречие
   m_RussianRules.push_back(new TKoe_Adverb );

   //3. По- + мест или мест-прил в ед.ч.м.р.Д.п. -> Наречие
   m_RussianRules.push_back(new TPo_PronounAdjective );
   
   //4. По- + прилагательное, заканчивающееся на –ски -> Наречие
   m_RussianRules.push_back(new TPo_AdjectiveSki );

   //5. В- или во- + порядковое числительное мн.ч. В.п. -> Наречие
   m_RussianRules.push_back(new TVo_Numeral );

   //6. Местоимение + -то, -либо, -нибудь -> Местоимение
   m_RussianRules.push_back(new TPronoun_To );

   //7. Кое-, кой- + местоимение -> Местоимение
   m_RussianRules.push_back(new TKoe_Pronoun );
   
   //8. Часть речи + частица –де, -ка, -ко, -кось, -с -> часть речи первой половины слова
   m_RussianRules.push_back(new TAny_De );

   //9. Часть речи по второй половине слова
   m_RussianRules.push_back(new TLastWord );

   ///////////////////////////////////////////////////////////////////////////////
   // АНГЛИЙСКИЙ
   
   m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective % Values::PartOfSpeechTypes.postAdverb,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postAdverb), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postAdjective)
      ),
      "1. Adverb + Adjective = Adj / Adv"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postAdjective)
      ),
      "2. Noun + Adjective = Adj"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postNoun % Values::PartOfSpeechTypes.postAdjective % Values::PartOfSpeechTypes.postAdverb,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postAdverb), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun)
      ),
      "3. Adverb + Noun = N  / Adj / Adv"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TNumericTypeEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective % Values::PartOfSpeechTypes.postAdverb,
         TNumericTypeEqual(Values::NumericType.ntCount), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun)
      ),
      "4. postNumeric, ntCount + Noun = Adj / Verb"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TNumericTypeEqual, TNumericTypeEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postNoun,
         TNumericTypeEqual(Values::NumericType.ntCount), TNumericTypeEqual(Values::NumericType.ntCount)
      ),
      "5. postNumeric, ntCount + postNumeric, ntCount = Noun"
    ));
    
    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TNumericTypeEqual, TNumericTypeEqual>( 
         TMakeFeatureList() % TNumericTypeEqual(Values::NumericType.ntOrdering),
         TNumericTypeEqual(Values::NumericType.ntCount), TNumericTypeEqual(Values::NumericType.ntOrdering)
      ),
      "6. postNumeric, ntCount + postNumeric, ntOrdering = postNumeric, ntOrdering"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TParticiple, TOfficialTypeEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective,
         TParticiple(Values::TimeType.tmtPast), TOfficialTypeEqual(Values::OfficialType.otPretext)
      ),
      "7. vfParticiple, tmtPast + Pretext = Adj"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TOfficialTypeEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postNoun,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun), TOfficialTypeEqual(Values::OfficialType.otConjunction), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun)
      ),
      "8. Noun + Conjunction + Noun = Noun"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TOfficialTypeEqual, TOfficialTypeEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postNoun,
         TOfficialTypeEqual(Values::OfficialType.otPretext), TOfficialTypeEqual(Values::OfficialType.otConjunction), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun)
      ),
      "9. Pretext + Conjunction + Noun = Noun"
    ));
    
    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postNoun % Values::PartOfSpeechTypes.postAdjective % Values::PartOfSpeechTypes.postVerb,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun)
      ),
      "10. Noun + Noun = Noun / Adj / Verb"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postVerb,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postVerb)
      ),
      "11. Noun + Verb = Verb"
    ));
    
    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TParticiple>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun), TParticiple(Values::TimeType.tmtPresent)
      ),
      "12. Noun + vfParticiple, tmtPresent = Adj"
    ));
    
    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TOfficialTypeEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun), TOfficialTypeEqual(Values::OfficialType.otPretext)
      ),
      "13. Noun + Pretext = Adj"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPartOfSpeechEqual, TOfficialTypeEqual, TOfficialTypeEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective % Values::PartOfSpeechTypes.postNoun,
         TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun), TOfficialTypeEqual(Values::OfficialType.otPretext), TOfficialTypeEqual(Values::OfficialType.otDeterminers), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun)
      ),
      "14. Noun + Pretext + Determiner + Noun = Adj / Noun"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TOfficialTypeEqual, TOfficialTypeEqual, TOfficialTypeEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective,
         TOfficialTypeEqual(Values::OfficialType.otPretext), TOfficialTypeEqual(Values::OfficialType.otPretext), TOfficialTypeEqual(Values::OfficialType.otDeterminers), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun)
      ),
      "15. Pretext + Pretext + Determiner + Noun = Adj"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TInfinitive, TPartOfSpeechEqual, TPronounNoun>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postNoun,
         TInfinitive(), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postPronoun), TPronounNoun()
      ),
      "16. Verb, vfInfinitive + Pronoun + Pronoun, pngNoun = Noun"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TInfinitive, TOfficialTypeEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postNoun % Values::PartOfSpeechTypes.postAdjective,
         TInfinitive(), TOfficialTypeEqual(Values::OfficialType.otPretext)
      ),
      "17. Verb, vfInfinitive + Pretext = Noun / Adj"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TOfficialTypeEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective,
         TOfficialTypeEqual(Values::OfficialType.otPretext), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postAdverb)
      ),
      "18. Pretext + Adverb = Adj"
    ));

    m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TOfficialTypeEqual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postAdjective,
         TOfficialTypeEqual(Values::OfficialType.otDeterminers), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun) 
      ),
      "19. Determiner + Noun = Adj"
    ));

     m_EnglishRules.push_back( std::make_pair(
      new TGenericRule<TPronounIndividual, TPartOfSpeechEqual>( 
         TMakeFeatureList() % Values::PartOfSpeechTypes.postNoun,
         TPronounIndividual(), TPartOfSpeechEqual(Values::PartOfSpeechTypes.postNoun) 
      ),
      "20. Pronoun, pncIndividual + Noun = Noun"
    ));
}
///////////////////////////////////////////////////////////////////////////////

TCompoundWordAnalysis::~TCompoundWordAnalysis()
{
   for( TRussianRules::iterator I = m_RussianRules.begin(); I != m_RussianRules.end(); ++I )
      delete *I;

   for( TEnglishRules::iterator I = m_EnglishRules.begin(); I != m_EnglishRules.end(); ++I )
      delete I->first;
}
///////////////////////////////////////////////////////////////////////////////

bool TCompoundWordAnalysis::HyphenAnalisisRussian(
   const std::wstring &LeftWord, const std::wstring &RightWord, 
   const TWIList &LeftMorpho, const TWIList &RightMorpho, 
   IAMConverterMorpho *pAMConverter,
   CMorphoFeature &MorphoFeature
) const
{
   APL_ASSERT_PTR(pAMConverter);
   
   TFeatureList LeftFeatureList, RightFeatureList;
   
   FillFeatureList( LeftMorpho, LeftFeatureList, pAMConverter );
   FillFeatureList( RightMorpho, RightFeatureList, pAMConverter );

   for( TRussianRules::const_iterator I = m_RussianRules.begin(); I != m_RussianRules.end(); ++I )
      if( (*I)->Check(LeftWord, RightWord, LeftFeatureList, RightFeatureList, m_WordClassification, MorphoFeature) )
         return true;

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool TCompoundWordAnalysis::HyphenAnalisisEnglish(
   const wchar_t* wcWord,
   const TWordPartInfoList &PartInfoList, 
   IAMConverterMorpho *pAMConverter,
   TFeatureList &FeatureList
) const
{
   APL_ASSERT_PTR(pAMConverter);
   
   TFeatureListForWord FeatureListForWord( PartInfoList.size() );
   bool Result = false;

   for( size_t i = 0; i < PartInfoList.size(); ++i )
      FillFeatureList( PartInfoList[i].MorphoInfo, FeatureListForWord[i], pAMConverter );

   for( TEnglishRules::const_iterator I = m_EnglishRules.begin(); I != m_EnglishRules.end(); ++I )
      if( I->first->Check(FeatureListForWord, FeatureList) )
      {
         DEBUG_MSG_LOG4( 
            "Опредиление дефиса '" << Convert(wcWord, std::string()) << "' по правилу '" << I->second << "'"
            );

         Result = true;
      }

   return Result;
}

} //namespace Dictionary
} //namespace SS
