#include "stdafx.h"
#include "OrderingRussian.h"

using namespace SS::Dictionary::Types;
using namespace SS::Core::Features;
using namespace SS::Constants;
using namespace std;


namespace SS
{
namespace Dictionary
{
namespace Virtual
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

COrderingRussian::COrderingRussian()
{
}

COrderingRussian::COrderingRussian(IAMConverterMorpho* pAMConverter)
{
	m_oHelper.Init(pAMConverter);
}

COrderingRussian::~COrderingRussian()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// COrderingRussian interface

bool COrderingRussian::IsExistWord(const wchar_t* wcWord)
{
	SS_TRY;

	TIDForm idForm;
	UINT uiValue;
	if(GetWordValues(wcWord, uiValue, idForm))
		return true;

	SS_CATCH(L"");
	return false;
}

bool COrderingRussian::GetWordInfo(const wchar_t* wcWord, 
								   const wchar_t* wcWordOriginalCase, 
								   bool IsFirstLetterUpperCase,
								   std::list<TWordInfo>* pWordInfo)
{
	SS_TRY;

	TIDForm idForm;
	UINT uiValue;
	if(!GetWordValues(wcWord, uiValue, idForm))
		return false;

	TConvFeatureList* pList = m_oHelper.GetConvertedFeature(idForm, uiValue);
	for(TConvFeatureList::const_iterator it = pList->begin(); it != pList->end(); ++it)
	{
		pWordInfo->push_back(std::list<TWordInfo>::value_type());
		pWordInfo->back().push_back(SWordInfo());
		FillWordInfo(pWordInfo->back().back(), wcWord, uiValue, idForm, it->iMorphoInfo, it->iSpecialMorpho);
	}

	SS_CATCH(L"");
	return true;
}

bool COrderingRussian::GetWordInfo(SWordIndex* pWordIndex, SWordInfo* pWordInfo)
{
	SS_TRY;

	wchar_t szWord[MAX_CHARS_IN_WORD];
	if(!GetWordByIndex(pWordIndex->m_DictIndex, pWordIndex->m_IDForm, szWord))
		return false;

	TConvFeatureList* pList = m_oHelper.GetConvertedFeature(pWordIndex->m_IDForm, pWordIndex->m_DictIndex.GetId());
	TConvFeatureList::const_iterator it = pList->begin();
	if(it != pList->end())
	{
		FillWordInfo(*pWordInfo, szWord, pWordIndex->m_DictIndex.GetId(), pWordIndex->m_IDForm, 
			it->iMorphoInfo, it->iSpecialMorpho);
	}

	SS_CATCH(L"");
	return true;
}

bool COrderingRussian::GetWord(SWordIndex* pWordIndex, wchar_t* wcWord)
{
	SS_TRY;

	if(GetWordByIndex(pWordIndex->m_DictIndex, pWordIndex->m_IDForm, wcWord))
		return true;

	SS_CATCH(L"");
	return false;
}

bool COrderingRussian::GetWordSource(const TDictionaryIndex &DictIndex, wchar_t* wcWordSource)
{
	SS_TRY;

	if(GetWordByIndex(DictIndex, -1, wcWordSource))
		return true;

	SS_CATCH(L"");
	return false;
}

bool COrderingRussian::GetWordFormsByIDSource(TDictionaryIndex* pDictionaryIndex, list<TWordInfo>* pWordInfo)
{
	SS_TRY;

	wchar_t szWord[MAX_CHARS_IN_WORD];
	if(!GetWordByIndex(*pDictionaryIndex, -1, szWord))
		return false;

	pWordInfo->clear();

	TConvFeatureList* pList = m_oHelper.GetConvertedFeature(0, pDictionaryIndex->GetId());
	for(TConvFeatureList::const_iterator it = pList->begin(); it != pList->end(); ++it)
	{
		pWordInfo->push_back(std::list<TWordInfo>::value_type());
		pWordInfo->back().push_back(SWordInfo());
		FillWordInfo(pWordInfo->back().back(), szWord, pDictionaryIndex->GetId(), 0, it->iMorphoInfo, it->iSpecialMorpho);
	}

	SS_CATCH(L"");
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// COrderingRussian functions

bool COrderingRussian::GetWordByIndex(const TDictionaryIndex &oIndex, TIDForm idForm, wchar_t* wcWord)
{
	SS_TRY;

	LPCWSTR szSuffix = m_oHelper.GetSuffix(idForm);
	if(!szSuffix)
		return false;

	if(idForm == 0xffff)
		swprintf(wcWord, L"%d", oIndex.GetId());
	else
		swprintf(wcWord, L"%d-%s", oIndex.GetId(), szSuffix);

	SS_CATCH(L"");
	return true;
}

void COrderingRussian::FillWordInfo(SWordInfo& oWordInfo, LPCWSTR szWord, UINT uiValue, TIDForm idForm, int iMorphoInfo, int iSpecialMorpho)
{
	SS_TRY;

	oWordInfo.m_wWord = szWord;
	oWordInfo.m_SpecialMorpho = iSpecialMorpho;
	oWordInfo.m_WordIndex.m_DictIndex.SetId(uiValue);
	oWordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(DATA_TYPE::NAMES::ednOrderingRussian);
	oWordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase(false);
	oWordInfo.m_WordIndex.m_IDForm = idForm;
	oWordInfo.m_MorphoTypeInfo.m_IDType = 0;
	oWordInfo.m_MorphoTypeInfo.m_MorphoInfo = iMorphoInfo;

	SS_CATCH(L"");
}

TIDForm COrderingRussian::GetIDForm(LPCWSTR szWord)
{
	SS_TRY;

	LPCWSTR szSuffix = GetWordSuffix(szWord);
	if(!szSuffix)
		return -1;
	return m_oHelper.GetIDForm(szSuffix);

	SS_CATCH(L"");
	return -1;
}

LPCWSTR COrderingRussian::GetWordSuffix(LPCWSTR szWord)
{
	SS_TRY;

	for(LPCWSTR sz = szWord; *sz; sz++)
	{
		if(IsCharAlphaW(*sz))
			return sz;
	}

	SS_CATCH(L"");
	return NULL;
}

bool COrderingRussian::GetWordValues(const wchar_t* wcWord, UINT& uiValue, TIDForm& idForm)
{
	SS_TRY;

	if(!iswdigit(*wcWord))
		return false;

	int iLen = (int) wcslen(wcWord);
	if(iLen == 0 || iLen > 8)
		return false;

	uiValue = _wtoi(wcWord);
	if(uiValue > TDictionaryIndex::MaxId())
		return false;

	idForm = GetIDForm(wcWord);
	if(idForm == 0xffff)
		return false;

	SS_CATCH(L"");
	return true;
}

}
}
}
