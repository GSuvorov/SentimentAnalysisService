#include "stdafx.h"

namespace SS{ 
namespace Dictionary{
namespace Virtual{

///////////////////////////////////////////////////////////////////////////////
// class CRomanNumberDictionary
///////////////////////////////////////////////////////////////////////////////
void CRomanNumberDictionary::Init( IAMConverterMorpho *pAMConverter )
{
	SS_TRY
	{
      SS::Core::Features::CMorphoFeature oMorphoFeature;
      oMorphoFeature.m_PartOfSpeechTypes.Assign(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);   
      oMorphoFeature.m_NumericType.Assign(SS::Core::Features::Values::NumericType.ntCount);
      pAMConverter->Code( DATA_TYPE::NAMES::ednNumeric, &oMorphoFeature, &m_rgMorphoParams[0].MorphoInfo, &m_rgMorphoParams[0].PartOfSpeech );

      oMorphoFeature.m_NumericType.Assign(SS::Core::Features::Values::NumericType.ntOrdering);
      pAMConverter->Code( DATA_TYPE::NAMES::ednNumeric, &oMorphoFeature, &m_rgMorphoParams[1].MorphoInfo, &m_rgMorphoParams[1].PartOfSpeech );
	}
	SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

inline bool CRomanNumberDictionary::GetWordByDictIndex(
								const TDictionaryIndex &Index,
								wchar_t* wcWord
)
{
	APL_ASSERT( wcWord != 0 );

	if( Index.GetDictionaryNumber() != DATA_TYPE::NAMES::ednRomanNumeric )
		return false;

   bool RetVal = Detail::CRomanArabicConvertor::Arabic2Roman( Index.GetId(), wcWord, Constants::MAX_CHARS_IN_WORD ) != 0;
   Detail::CorrectWordCase( wcWord, Index );

   return RetVal;
}
///////////////////////////////////////////////////////////////////////////////

bool CRomanNumberDictionary::IsExistWord(/*in*/const wchar_t* wcWord)
{
	APL_ASSERT( wcWord != 0 );
	SS_TRY
	{
      if( IsUnacceptedWord(wcWord) ) return false;

		const wchar_t *pLast = wcWord + std::wcslen(wcWord);
		unsigned int Number;
		
		if( 
			wcWord == pLast ||
			Detail::CRomanArabicConvertor::Roman2Arabic( wcWord, pLast, Number ) != pLast ||
			Number > TDictionaryIndex::MaxId()
		) 
			return false;

		APL_ASSERT( Number >= 0 );

	}
	SS_CATCH(L"")

   return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CRomanNumberDictionary::GetWordInfo/*ByWord*/(
						   /*in*/const wchar_t* wcWord, 
                     /*in*/const wchar_t* wcWordOriginalCase,
                     /*in*/bool IsFirstLetterUpperCase,
						   /*out*/std::list<TWordInfo>* p_l_WordInfo
						   )
{
	APL_ASSERT( wcWord != 0 );
	
	SS_TRY
	{
		if( IsUnacceptedWord(wcWord) ) return false;
      
        const wchar_t *pLast = wcWord + std::wcslen(wcWord);
		unsigned int Number;

		p_l_WordInfo->clear();

		if( wcWord == pLast ||
			Detail::CRomanArabicConvertor::Roman2Arabic( wcWord, pLast, Number ) != pLast ||
			Number > TDictionaryIndex::MaxId()
		  ) 
		{
			return false;
		}

		APL_ASSERT( Number >= 0 );											   

		//---p_l_WordInfo->clear();
		
		for( TIDForm IDForm = 0; IDForm < APL_ARRSIZE(m_rgMorphoParams); ++IDForm )
		{
			p_l_WordInfo->push_back( std::list<TWordInfo>::value_type() );
			p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
		
			FillSWordInfo( wcWordOriginalCase, (unsigned int)Number, IDForm, p_l_WordInfo->back().back(), IsFirstLetterUpperCase );
		}
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CRomanNumberDictionary::GetWordInfo/*ByWordIndex*/(
								/*in*/SWordIndex* pWordIndex, 
								/*out*/SWordInfo* pWordInfo
)
{
	APL_ASSERT( pWordIndex != 0 );
	APL_ASSERT( pWordInfo != 0 );

	SS_TRY
	{
		wchar_t Buf[Constants::MAX_CHARS_IN_WORD];
		
		if( !GetWordByDictIndex(pWordIndex->m_DictIndex, Buf) )
			return false;

      Detail::CorrectWordCase( Buf, pWordIndex->m_DictIndex );
		FillSWordInfo( Buf, pWordIndex->m_DictIndex.GetId(), pWordIndex->m_IDForm, *pWordInfo, pWordIndex->m_DictIndex.GetFirstLetterUpperCase() );
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CRomanNumberDictionary::GetWord(
			 /*in*/SWordIndex* pWordIndex,
			 /*out*/wchar_t* wcWord
)
{
	APL_ASSERT( pWordIndex != 0 );
	APL_ASSERT( wcWord != 0 );

	SS_TRY
	{
		if( !GetWordByDictIndex(pWordIndex->m_DictIndex, wcWord) )
			return false;
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CRomanNumberDictionary::GetWordSource(
			 /*in*/const TDictionaryIndex &DictIndex,
			 /*out*/wchar_t* wcWordSource
)
{
	APL_ASSERT( wcWordSource != 0 );

	SS_TRY
	{
		if( !GetWordByDictIndex(DictIndex, wcWordSource) )
			return false;
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CRomanNumberDictionary::GetWordFormsByIDSource(
							/*in*/TDictionaryIndex* pDictionaryIndex,
							/*out*/std::list<TWordInfo>* p_l_WordInfo
)
{
	APL_ASSERT( pDictionaryIndex != 0 );
	APL_ASSERT( p_l_WordInfo != 0 );

	SS_TRY
	{
		wchar_t Buf[Constants::MAX_CHARS_IN_WORD];

		if( !GetWordByDictIndex(*pDictionaryIndex, Buf) )
			return false;

		p_l_WordInfo->clear();

		for( TIDForm IDForm = 0; IDForm < APL_ARRSIZE(m_rgMorphoParams); ++IDForm )
		{
			p_l_WordInfo->push_back( std::list<TWordInfo>::value_type() );
			p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );

			Detail::CorrectWordCase( Buf, *pDictionaryIndex );
			FillSWordInfo( Buf, (unsigned int)pDictionaryIndex->GetId(), IDForm, p_l_WordInfo->back().back(), pDictionaryIndex->GetFirstLetterUpperCase() );
		}
	}
	SS_CATCH(L"")

	return true;
}

} //namespace Virtual
} //namespace Dictionary
} //namespace SS
