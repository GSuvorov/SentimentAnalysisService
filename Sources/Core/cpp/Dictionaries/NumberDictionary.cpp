#include "stdafx.h"


namespace SS{ 
namespace Dictionary{
namespace Virtual{

///////////////////////////////////////////////////////////////////////////////
// class CNumberDictionary
///////////////////////////////////////////////////////////////////////////////
void CNumberDictionary::Init( IAMConverterMorpho *pAMConverter )
{
	SS_TRY
	{
		SS::Core::Features::CMorphoFeature oMorphoFeature;
      oMorphoFeature.m_PartOfSpeechTypes.Assign(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);   
      oMorphoFeature.m_NumericType.Assign(SS::Core::Features::Values::NumericType.ntCount);
		pAMConverter->Code( DATA_TYPE::NAMES::ednNumeric, &oMorphoFeature, &m_rgMorphoParamsNormal[0].MorphoInfo, &m_rgMorphoParamsNormal[0].PartOfSpeech );
      pAMConverter->Code( DATA_TYPE::NAMES::ednFloatPointNumeric, &oMorphoFeature, &m_rgMorphoParamsFloating[0].MorphoInfo, &m_rgMorphoParamsFloating[0].PartOfSpeech );

      oMorphoFeature.m_NumericType.Assign(SS::Core::Features::Values::NumericType.ntOrdering);
      pAMConverter->Code( DATA_TYPE::NAMES::ednNumeric, &oMorphoFeature, &m_rgMorphoParamsNormal[1].MorphoInfo, &m_rgMorphoParamsNormal[1].PartOfSpeech );
	}
	SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

inline bool CNumberDictionary::GetWordByDictIndex(
								const TDictionaryIndex &Index,
								wchar_t* wcWord
)
{
	APL_ASSERT( wcWord != 0 );

	if( Index.GetDictionaryNumber() == DATA_TYPE::NAMES::ednNumeric )
   {
	   //Я не знаю почему _ltow не лижит в std, но так жизнь сложилась!
	   _ltow( Index.GetId(), wcWord, 10 );
   }
   else if( Index.GetDictionaryNumber() == DATA_TYPE::NAMES::ednFloatPointNumeric )
   {
      int ZeroCount = Index.GetId() >> 21;

      while(ZeroCount--) *(wcWord++) = L'0';
      
      long Num = Index.GetId() & 0xFFFF;

      if( Num != 0 )
         _ltow( Num, wcWord, 10 );
      else
        *wcWord = L'\0';
   }
   else 
      return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CNumberDictionary::IsExistWord(/*in*/const wchar_t* wcWord)
{
	APL_ASSERT( wcWord != 0 );
	SS_TRY
	{
		const wchar_t *pLast = wcWord + std::_tcslen(wcWord);
		INT32 Number;
		
		if( 
			wcWord == pLast ||
			ConvertStringToInteger( wcWord, pLast, Number ) != pLast ||
			Number > (INT32)TDictionaryIndex::MaxId()
		) 
			return false;

		APL_ASSERT( Number >= 0 );

	}
	SS_CATCH(L"")

   return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CNumberDictionary::GetWordInfo/*ByWord*/(
						   /*in*/const wchar_t* wcWord, 
                     /*in*/const wchar_t* wcWordOriginalCase,
                     /*in*/bool IsFirstLetterUpperCase,
						   /*out*/std::list<TWordInfo>* p_l_WordInfo
)
{
	APL_ASSERT( wcWord != 0 );
	
	SS_TRY
	{
		const wchar_t *pLast = wcWord + std::wcslen(wcWord);
        const wchar_t *pFirstNonNull = wcWord; //Первый ненулевой символ
		INT32 Number;

		p_l_WordInfo->clear();

		while( pFirstNonNull != pLast && *pFirstNonNull == L'0' ) 
		{
			++pFirstNonNull;
		}

		if( wcWord == pLast ||
			ConvertStringToInteger( pFirstNonNull, pLast, Number ) != pLast ||
			Number < 0 
		  ) 
		{
			return false;
		}

		if( pFirstNonNull == wcWord )
		{
			if( Number > (INT32)TDictionaryIndex::MaxId() ) return false;
		}
		else
		{
			if( pFirstNonNull - wcWord > 0x7 || Number > 0xFFFFF ) return false;                                    
		}

		//---p_l_WordInfo->clear();
	
		unsigned int ZeroCount = (unsigned int)(pFirstNonNull - wcWord);
      
		if( ZeroCount == 0 )
		{
			for( TIDForm IDForm = 0; IDForm < APL_ARRSIZE(m_rgMorphoParamsNormal); ++IDForm )
			{
			    p_l_WordInfo->push_back( std::list<TWordInfo>::value_type() );
			    p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
				FillSWordInfoNormal( wcWordOriginalCase, (unsigned int)Number, ZeroCount, IDForm, p_l_WordInfo->back().back() );
			}
		}
		else
		{
			for( TIDForm IDForm = 0; IDForm < APL_ARRSIZE(m_rgMorphoParamsFloating); ++IDForm )
			{
				p_l_WordInfo->push_back( std::list<TWordInfo>::value_type() );
				p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
				FillSWordInfoFloating( wcWordOriginalCase, (unsigned int)Number, ZeroCount, IDForm, p_l_WordInfo->back().back() );
			}
		}
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CNumberDictionary::GetWordInfo/*ByWordIndex*/(
								/*in*/SWordIndex* pWordIndex, 
								/*out*/SWordInfo* pWordInfo
)
{
	APL_ASSERT( pWordIndex != 0 );
	APL_ASSERT( pWordInfo != 0 );

	SS_TRY
	{
		wchar_t Buf[20];
		
		if( !GetWordByDictIndex(pWordIndex->m_DictIndex, Buf) )
			return false;
      if( pWordIndex->m_DictIndex.GetDictionaryNumber() == DATA_TYPE::NAMES::ednNumeric )
      {
		   FillSWordInfoNormal( Buf, pWordIndex->m_DictIndex, pWordIndex->m_IDForm, *pWordInfo );
      }
      else
      {
         APL_ASSERT(pWordIndex->m_DictIndex.GetDictionaryNumber() == DATA_TYPE::NAMES::ednFloatPointNumeric);
         FillSWordInfoFloating( Buf, pWordIndex->m_DictIndex, pWordIndex->m_IDForm, *pWordInfo );
      }
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CNumberDictionary::GetWord(
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

bool CNumberDictionary::GetWordSource(
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

bool CNumberDictionary::GetWordFormsByIDSource(
							/*in*/TDictionaryIndex* pDictionaryIndex,
							/*out*/std::list<TWordInfo>* p_l_WordInfo
)
{
	APL_ASSERT( pDictionaryIndex != 0 );
	APL_ASSERT( p_l_WordInfo != 0 );

	SS_TRY
	{
		wchar_t Buf[20];

		if( !GetWordByDictIndex(*pDictionaryIndex, Buf) )
			return false;

		p_l_WordInfo->clear();

		if( pDictionaryIndex->GetDictionaryNumber() == DATA_TYPE::NAMES::ednNumeric )
		{
			for( TIDForm IDForm = 0; IDForm < APL_ARRSIZE(m_rgMorphoParamsNormal); ++IDForm )
			{
				p_l_WordInfo->push_back( std::list<TWordInfo>::value_type() );
				p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );

				FillSWordInfoNormal( Buf, *pDictionaryIndex, IDForm, p_l_WordInfo->back().back() );
			}
		}
		else
		{
			APL_ASSERT( pDictionaryIndex->GetDictionaryNumber() == DATA_TYPE::NAMES::ednFloatPointNumeric );
         
			for( TIDForm IDForm = 0; IDForm < APL_ARRSIZE(m_rgMorphoParamsFloating); ++IDForm )
			{
				p_l_WordInfo->push_back( std::list<TWordInfo>::value_type() );
				p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );

				FillSWordInfoFloating( Buf, *pDictionaryIndex, IDForm, p_l_WordInfo->back().back() );
			}
		}
	}
	SS_CATCH(L"")

	return true;
}

} //namespace Virtual
} //namespace Dictionary
} //namespace SS
