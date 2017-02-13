#pragma once

#include "../[libs]/TestHelper.h"
#include "../[libs]/CoutConvert.h"

namespace SS{ 
namespace Dictionary{

inline bool operator==( const CSmartDictionary::SHypothesisInfo &Ob1, const CSmartDictionary::SHypothesisInfo &Ob2 )
{
   return
      Ob1.m_ConcordanceChars == Ob2.m_ConcordanceChars &&
      Ob1.m_FormsInfoList == Ob2.m_FormsInfoList;
}
///////////////////////////////////////////////////////////////////////////////

inline bool operator==( const CSmartDictionary::SHypothesisInfo::WordInfo &Ob1, const CSmartDictionary::SHypothesisInfo::WordInfo &Ob2 )
{
   return
      Ob1.m_MorphoType.m_IDEnding == Ob2.m_MorphoType.m_IDEnding &&
      Ob1.m_MorphoType.m_IDForm == Ob2.m_MorphoType.m_IDForm &&
      Ob1.m_MorphoType.m_MorphoTypeInfo.m_IDType == Ob2.m_MorphoType.m_MorphoTypeInfo.m_IDType &&
      Ob1.m_MorphoType.m_MorphoTypeInfo.m_MorphoInfo == Ob2.m_MorphoType.m_MorphoTypeInfo.m_MorphoInfo &&
      Ob1.m_MorphoType.m_SpecialMorpho == Ob2.m_MorphoType.m_SpecialMorpho &&
      Ob1.m_DictIndex.GetDictionaryIndex() == Ob2.m_DictIndex.GetDictionaryIndex() &&
      Ob1.m_EndingLen == Ob2.m_EndingLen;
}
///////////////////////////////////////////////////////////////////////////////

struct SWordInfoPredLess: public std::binary_function<TWordInfo, TWordInfo, bool>
{
   bool operator()( const TWordInfo &WI1, const TWordInfo &WI2 ) const
   {
      APL_ASSERT( !WI1.empty() && !WI2.empty() );

      return 
         (WI1.front().m_MorphoTypeInfo.m_IDType < WI2.front().m_MorphoTypeInfo.m_IDType) || 
         (  
            !(WI2.front().m_MorphoTypeInfo.m_IDType < WI1.front().m_MorphoTypeInfo.m_IDType) &&
            (
               WI1.front().m_WordIndex.m_IDForm < WI2.front().m_WordIndex.m_IDForm ||
               (
                  !(WI2.front().m_WordIndex.m_IDForm < WI1.front().m_WordIndex.m_IDForm) &&
                  WI1.front().m_WordIndex.m_DictIndex.GetDictionaryIndex() < WI2.front().m_WordIndex.m_DictIndex.GetDictionaryIndex()
               )
            )
         );
   }
};


void PrepareLingvoBaseManager( SS::Interface::Core::ResourceManagers::ILoadManager *pLoadManager );

//Получить IDictionary у LoadManager'a
IDictionary *GetIDictionary( SS::Interface::Core::ResourceManagers::ILoadManager *pLoadManager );


} //namespace Dictionary
} //namespace SS

namespace SS{ 
namespace UnitTests{ 
namespace Dictionary{

///////////////////////////////////////////////////////////////////////////////
inline bool Compare( const wchar_t *S1, const wchar_t *S2, TDictNumber DN )
{
#if 1  
   if( DN == DATA_TYPE::NAMES::ednOrderingEnglish  )
      return true;
#endif

   std::wstring LowerS1, LowerS2;
   wchar_t C1, C2;

   ToLower( S1, LowerS1 );
   ToLower( S2, LowerS2 );

   if( LowerS1.length() != LowerS2.length() ) 
      return false;

   for( size_t i = 0; i < LowerS1.length(); ++i )
   {
      if( LowerS1[i] == L'ё' ) 
         C1 = L'е';
      else
         C1 = LowerS1[i];
      
      if( LowerS2[i] == L'ё' ) 
         C2 = L'е';
      else
         C2 = LowerS2[i];

      if( C1 != C2 ) return false;
   }

   return true;
}
///////////////////////////////////////////////////////////////////////////////

inline bool CheckMorphoInfo( IAMConverterMorpho *pAMConverter, const SWordInfo &WI, const std::string &Type )
{
   CMorphoFeature MorphoFeature;

   pAMConverter->EnCode( WI.m_WordIndex.m_DictIndex.GetDictionaryNumber(), WI.m_MorphoTypeInfo.m_MorphoInfo, WI.m_SpecialMorpho, &MorphoFeature );

   if( Type == "сущ" )
      return MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun);
   
   if( Type == "прил" )
      return MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective);
   
   if( Type == "глаг" )
      return MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postVerb);

   APL_ERROR( false );
};
///////////////////////////////////////////////////////////////////////////////

inline bool Cmp( const SWordInfo &WI1, const SWordInfo &WI2, bool CaseInsensitive )
{
   bool WordRezult;
   
   if(!CaseInsensitive)
   {
      WordRezult = WI1.m_wWord == WI2.m_wWord;
   }
   else
   {
      WordRezult = Compare( WI1.m_wWord.c_str(), WI2.m_wWord.c_str(), WI1.m_WordIndex.m_DictIndex.GetDictionaryNumber());
   }
   
   return
      WordRezult &&	
      WI1.m_SpecialMorpho == WI2.m_SpecialMorpho &&
      WI1.m_WordIndex.m_DictIndex.GetId() == WI2.m_WordIndex.m_DictIndex.GetId() &&
      WI1.m_WordIndex.m_DictIndex.GetDictionaryNumber() == WI2.m_WordIndex.m_DictIndex.GetDictionaryNumber() &&	
      WI1.m_WordIndex.m_IDForm == WI2.m_WordIndex.m_IDForm &&	
      WI1.m_MorphoTypeInfo.m_IDType == WI2.m_MorphoTypeInfo.m_IDType &&	
      WI1.m_MorphoTypeInfo.m_MorphoInfo == WI2.m_MorphoTypeInfo.m_MorphoInfo;	
}
///////////////////////////////////////////////////////////////////////////////

inline bool Cmp( const SSource &ob1, const SSource &ob2 )
{
   return 
      ob1.m_DictIndex.GetDictionaryIndex() == ob2.m_DictIndex.GetDictionaryIndex() && 
      ob1.m_MorphoTypeInfo.m_IDType == ob2.m_MorphoTypeInfo.m_IDType	&&
      ob1.m_MorphoTypeInfo.m_MorphoInfo == ob2.m_MorphoTypeInfo.m_MorphoInfo	&&
      ob1.m_wSource == ob2.m_wSource;
}
///////////////////////////////////////////////////////////////////////////////
template <unsigned char nBitSize, unsigned char nValues, class TType>
inline std::string FeatureData( CMapFeature<nBitSize, nValues, TType> &Feature )
{
   std::string Out;
   unsigned int Value = Feature.GetValue();
   TType Type;
   int n = 0;
   
   Out += '[';
   Out += Type.GetTypeDescription();
   Out += ": ";

   for( unsigned int i = 0; i < Type.GetValueCount(); ++i )
   {
      if( Value & (1 << i) )
      {
         if( n != 0 ) Out += ", ";
         Out += Type.GetValueDescription(i); 
         ++n;
      }
   }
   
   Out += ']';

   return Out; 
}
///////////////////////////////////////////////////////////////////////////////

template <unsigned char nBitSize, unsigned char nValues, class TType>
inline std::string FeatureDataNameOnly( const CMapFeature<nBitSize, nValues, TType> &Feature )
{
   std::string Out;
   unsigned int Value = Feature.GetValue();
   TType Type;
   int n = 0;

   for( unsigned int i = 1; i < Type.GetValueCount(); ++i )
   {
      if( Value & (1 << i) )
      {
         if( n != 0 ) Out += ", ";
         Out += Type.GetValueName(i);
         ++n;
      }
   }
   
   if( n > 0 ) Out += ';';

   return Out; 
}
///////////////////////////////////////////////////////////////////////////////

inline std::string FeatureDataNameOnly( const CMorphoFeature &MF )
{
   return MF.GetPrintableDataDescription();
}

inline void Print( std::ostream &OutStream, const SWordInfo &WordInfo, IAMConverterMorpho *pAMConverter = 0  )
{
   OutStream << "{"; 
   OutStream <<	"m_wWord: " << WordInfo.m_wWord;	
   OutStream <<	" m_DictionaryNumber: " << DictionaryIndexDescription( WordInfo.m_WordIndex.m_DictIndex );	
   OutStream <<	" m_IDSource: " << WordInfo.m_WordIndex.m_DictIndex.GetId();	
   OutStream <<	" m_IDType: " << WordInfo.m_MorphoTypeInfo.m_IDType;	
   OutStream <<	" m_IDForm: " << WordInfo.m_WordIndex.m_IDForm;	
   OutStream <<	" m_MorphoInfo: " << WordInfo.m_MorphoTypeInfo.m_MorphoInfo;	
   OutStream <<	" m_SpecialMorpho: " << WordInfo.m_SpecialMorpho;	
   
   if( pAMConverter != 0 )
   {
      using namespace SS::Core::Features::Types;
      OutStream << std::endl;

      CMorphoFeature MF;
      pAMConverter->EnCode( WordInfo.m_WordIndex.m_DictIndex.GetDictionaryNumber(), WordInfo.m_MorphoTypeInfo.m_MorphoInfo, WordInfo.m_SpecialMorpho, &MF );

#if 0
      OutStream << FeatureData(MF.m_PartOfSpeechTypes);
      OutStream << " " << FeatureData(MF.m_GenderType);
      OutStream << " " << FeatureData(MF.m_CaseType);
      OutStream << " " << FeatureData(MF.m_PersonType);
      OutStream << " " << FeatureData(MF.m_CommonType);
      OutStream << " " << FeatureData(MF.m_NumericType);
#else
      OutStream << "[";
      OutStream << FeatureDataNameOnly(MF);
      OutStream << "]";
#endif
  }

   OutStream << "}\n"; 
}
///////////////////////////////////////////////////////////////////////////////

inline void Print( std::ostream &OutStream, const std::list<TWordInfo> &WordInfoList, IAMConverterMorpho *pAMConverter = 0 )
{
   typedef std::list<TWordInfo> TList;
   int ni, nj;

   TList::const_iterator I;
   TWordInfo::const_iterator J;

   OutStream << "{"; 

   for( ni = 0, I = WordInfoList.begin(); I != WordInfoList.end();  ++I, ++ni )
   {
      OutStream << "{"; 

      for( nj = 0, J = I->begin(); J != I->end();  ++J, ++nj )
      {
         Print( OutStream, *J, pAMConverter );		
      }

      OutStream << "}:" << nj; 
   }

   OutStream << "}:" << ni;
}
///////////////////////////////////////////////////////////////////////////////

inline void Print( std::ostream &OutStream, const TWordInfo &WordInfoList, IAMConverterMorpho *pAMConverter = 0 )
{
   int ni;

   TWordInfo::const_iterator I;

   OutStream << "{"; 

   for( ni = 0, I = WordInfoList.begin(); I != WordInfoList.end();  ++I, ++ni )
   {
      Print( OutStream, *I, pAMConverter );		
   }

   OutStream << "}:" << ni;
}
///////////////////////////////////////////////////////////////////////////////

inline void Print( std::ostream &OutStream, const CSmartDictionary::SHypothesisInfo &HypothesisInfo, CSmartDictionary *pSD, IAMConverterMorpho *pAMConverter = 0 )
{
   CSmartDictionary::SHypothesisInfo::TFormsInfoList::const_iterator I;
   wchar_t Word[SS::Constants::MAX_CHARS_IN_WORD];
   SWordIndex WI;

   for( 
      I = HypothesisInfo.m_FormsInfoList.begin();
      I != HypothesisInfo.m_FormsInfoList.end();
      ++I
   )
   {
      WI.m_DictIndex = I->m_DictIndex;
      WI.m_IDForm = I->m_MorphoType.m_IDForm;

      if( !pSD->GetWord(&WI, Word) )
         StrCpy( Word, L"<!ERROR!>" );

      OutStream << "{"; 
      OutStream <<   "WORD: " << Word;
      OutStream <<   " m_EndingLen: " << I->m_EndingLen;
      OutStream <<   " DictionaryNumber: " << DictionaryIndexDescription( I->m_DictIndex );
      OutStream <<   " Id: " << I->m_DictIndex.GetId();
      OutStream <<   " IDType: " << I->m_MorphoType.m_MorphoTypeInfo.m_IDType;
      OutStream <<   " IDForm: " << I->m_MorphoType.m_IDForm;
      OutStream <<   " IDEnding: " << I->m_MorphoType.m_IDEnding;
      OutStream <<   " MorphoInfo: " << I->m_MorphoType.m_MorphoTypeInfo.m_MorphoInfo;
      OutStream <<   " SpecialMorpho: " << I->m_MorphoType.m_SpecialMorpho;
      
      if( pAMConverter != 0 )
      {
         OutStream << std::endl;

         using namespace SS::Core::Features::Types;

         CMorphoFeature MF;
         pAMConverter->EnCode( I->m_DictIndex.GetDictionaryNumber(), I->m_MorphoType.m_MorphoTypeInfo.m_MorphoInfo, I->m_MorphoType.m_SpecialMorpho, &MF );

         OutStream << "[";
         OutStream << FeatureDataNameOnly(MF);
         OutStream << "]";
         //OutStream << FeatureData(MF.m_PartOfSpeechTypes);
         //OutStream << " " << FeatureData(MF.m_GenderType);
         //OutStream << " " << FeatureData(MF.m_CaseType);
         //OutStream << " " << FeatureData(MF.m_PersonType);
         //OutStream << " " << FeatureData(MF.m_CommonType);
      }

      OutStream << "}" << std::endl;
   }
}
///////////////////////////////////////////////////////////////////////////////

struct SWordByIDSource 
{
   TDictionaryIndex DI;
   std::list<TWordInfo> WIList;

   SWordByIDSource(){ DI.SetDictionaryIndex(0); }
};

inline bool operator==( const SWordByIDSource &Ob1, const SWordByIDSource &Ob2 )
{
   return 
      Ob1.DI.GetDictionaryIndex() == Ob2.DI.GetDictionaryIndex() &&
      Ob1.WIList == Ob2.WIList;
}

///////////////////////////////////////////////////////////////////////////////

struct SWordInfoMorphoCmp: public std::binary_function<SWordInfo, SWordInfo, bool> 
{
   bool operator()(const SWordInfo &WI1, const SWordInfo &WI2 ) const
   {
      return MultipleCompare( 
         WI1.m_MorphoTypeInfo.m_IDType, WI2.m_MorphoTypeInfo.m_IDType,
         WI1.m_WordIndex.m_IDForm, WI2.m_WordIndex.m_IDForm,
         WI1.m_MorphoTypeInfo.m_MorphoInfo, WI2.m_MorphoTypeInfo.m_MorphoInfo,
         WI1.m_SpecialMorpho, WI2.m_SpecialMorpho,
         std::less<TIDType>(),
         std::less<TIDForm>(),
         std::less<TMorphoInfo>(),
         std::less<TSpecialMorpho>()
         );

   }
};

struct STWordInfoMorphoCmp: public std::binary_function<TWordInfo, TWordInfo, bool> 
{
   bool operator()(const TWordInfo &WI1, const TWordInfo &WI2 ) const
   {
      APL_ASSERT(WI1.size() == 1);
      APL_ASSERT(WI2.size() == 1);

      return SWordInfoMorphoCmp()(WI1.front(), WI2.front());   
   }
};


struct STWordInfoMorphoCmpEqual: public std::binary_function<TWordInfo, TWordInfo, bool> 
{
   bool operator()(const TWordInfo &WI1, const TWordInfo &WI2 ) const
   {
      return !STWordInfoMorphoCmp()(WI1, WI2) && !STWordInfoMorphoCmp()(WI2, WI1);   
   }
};


} //namespace Dictionary
} //namespace UnitTests
} //namespace SS


namespace SS {
namespace Dictionary {
namespace Types {

inline bool operator==( const SWordInfo &WI1, const SWordInfo &WI2 )
{
   return UnitTests::Dictionary::Cmp( WI1, WI2, false );
}

}//namespace SS 
}//namespace Dictionary 
}//namespace Types 