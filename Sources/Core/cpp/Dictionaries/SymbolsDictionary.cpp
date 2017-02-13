#include "stdafx.h"

namespace SS{ 
namespace Dictionary{							 
namespace Virtual{

///////////////////////////////////////////////////////////////////////////////
// class CSymbolsDictionary
///////////////////////////////////////////////////////////////////////////////
const unsigned int CSymbolsDictionary::ciBRIndex = 0;
const wchar_t *const CSymbolsDictionary::csBR = _T("<br>");
///////////////////////////////////////////////////////////////////////////////

void CSymbolsDictionary::Init( IAMConverterMorpho *pAMConverter )
{
	SS_TRY
	{
		SS::Core::Features::CMorphoFeature oMorphoFeature;
      oMorphoFeature.m_PartOfSpeechTypes.Assign(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial);
      oMorphoFeature.m_OfficialType.Assign(SS::Core::Features::Values::OfficialType.otSymbol);

		pAMConverter->Code( DATA_TYPE::NAMES::ednSymbols, &oMorphoFeature, &m_MorphoInfo, &m_PartOfSpeech );

	}
	SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

inline bool CSymbolsDictionary::GetWordByDictIndex(
								const TDictionaryIndex &Index,
								wchar_t* wcWord
)
{
	APL_ASSERT( wcWord != 0 );

	if( Index.GetDictionaryNumber() != DATA_TYPE::NAMES::ednSymbols )
		return false;

	if( Index.GetId() == ciBRIndex )
	{
		_tcscpy(wcWord, csBR);
	}
	else
	{
		wcWord[0] = Index.GetId();
		wcWord[1] = _T('\0');
	}

   Detail::CorrectWordCase( wcWord, Index );

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CSymbolsDictionary::ConvertToSymbol( const wchar_t *wcWord, unsigned int &N )
{
	APL_ASSERT( wcWord != 0 );

	if( wcWord[0] == _T('\0') )
		return false;

	if( wcWord[1] == _T('\0') )
	{
		N = wcWord[0];
		return N <= TDictionaryIndex::MaxId();
	}

	if( _tcscmp(wcWord, csBR) == 0 )
	{
		N = ciBRIndex;
		return true;
	}

	return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSymbolsDictionary::IsExistWord(/*in*/const wchar_t *wcWord)
{
	APL_ASSERT( wcWord != 0 );
	SS_TRY
	{
		unsigned int Number;
		
		if( !ConvertToSymbol(wcWord, Number) ) 
			return false;

		APL_ASSERT( Number >= 0 );

	}
	SS_CATCH(L"")

   return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CSymbolsDictionary::GetWordInfo/*ByWord*/(
						   /*in*/const wchar_t* wcWord,
                     /*in*/const wchar_t* wcWordOriginalCase,
                     /*in*/bool IsFirstLetterUpperCase,
						   /*out*/std::list<TWordInfo>* p_l_WordInfo
)
{
	APL_ASSERT( wcWord != 0 );
	
	SS_TRY
	{
		unsigned int Number;

		p_l_WordInfo->clear();

		if( !ConvertToSymbol(wcWord, Number) ) 
			return false;

		APL_ASSERT( Number >= 0 );											   

		//---p_l_WordInfo->clear();

		//Добавляем новый элемент
		p_l_WordInfo->push_back( std::list<TWordInfo>::value_type() );
		p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
		
		FillSWordInfo( wcWordOriginalCase, (unsigned int)Number, p_l_WordInfo->back().back(), IsFirstLetterUpperCase );
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CSymbolsDictionary::GetWordInfo/*ByWordIndex*/(
								/*in*/SWordIndex* pWordIndex, 
								/*out*/SWordInfo* pWordInfo
)
{
	APL_ASSERT( pWordIndex != 0 );
	APL_ASSERT( pWordInfo != 0 );

	SS_TRY
	{
		wchar_t Buf[10];
		
		if( !GetWordByDictIndex(pWordIndex->m_DictIndex, Buf) )
			return false;

      Detail::CorrectWordCase( Buf, pWordIndex->m_DictIndex );
      FillSWordInfo( Buf, pWordIndex->m_DictIndex.GetId(), *pWordInfo, pWordIndex->m_DictIndex.GetFirstLetterUpperCase() );
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CSymbolsDictionary::GetWord(
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

bool CSymbolsDictionary::GetWordSource(
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

bool CSymbolsDictionary::GetWordFormsByIDSource(
							/*in*/TDictionaryIndex* pDictionaryIndex,
							/*out*/std::list<TWordInfo>* p_l_WordInfo
)
{
	APL_ASSERT( pDictionaryIndex != 0 );
	APL_ASSERT( p_l_WordInfo != 0 );

	SS_TRY
	{
		wchar_t Buf[10];

		if( !GetWordByDictIndex(*pDictionaryIndex, Buf) )
			return false;

		p_l_WordInfo->clear();

		//Добавляем новый элемент
		p_l_WordInfo->push_back( std::list<TWordInfo>::value_type() );
		p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );

		Detail::CorrectWordCase( Buf, *pDictionaryIndex );
		FillSWordInfo( Buf, (unsigned int)pDictionaryIndex->GetId(), p_l_WordInfo->back().back(), pDictionaryIndex->GetFirstLetterUpperCase() );
	}
	SS_CATCH(L"")

	return true;
}

} //namespace Virtual
} //namespace Dictionary
} //namespace SS
