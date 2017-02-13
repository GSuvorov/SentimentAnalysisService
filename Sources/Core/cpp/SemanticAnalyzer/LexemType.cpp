#include "StdAfx.h"
#include ".\lexemtype.h"
#include "..\ASSInterface\TDictionaryNames.h"

namespace SS
{
	namespace Semantic
	{
CLexemType::CLexemType(void) : m_ppCurentUnit(NULL)
{
}

CLexemType::~CLexemType(void)
{
}

bool CLexemType::WordEqualsTo(const TCharType  pcWord,SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;

	return m_StringService.StrEqual(pUnit->GetWord(),pcWord);
}

bool CLexemType::IsWord(UINT uiLessOrEqual,SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;

	SS::Interface::Core::BlackBox::IIndex * pIndex=pUnit->GetFirstIndex();
	if(!pIndex) return false;

	do{
		if(pIndex->GetDictionaryIndex())
			switch(/*(*/pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/) {
				case SS::Dictionary::DATA_TYPE::NAMES::ednNumeric:
				case SS::Dictionary::DATA_TYPE::NAMES::ednSymbols:
				case SS::Dictionary::DATA_TYPE::NAMES::ednUnknown:
					return false;
				case SS::Dictionary::DATA_TYPE::NAMES::ednSemantic:
				case SS::Dictionary::DATA_TYPE::NAMES::ednSyntax:
				case SS::Dictionary::DATA_TYPE::NAMES::ednDates:
					break;
			default:
				return true;
			}
	}while(pIndex=pUnit->GetNextIndex());

	return false;
}
bool CLexemType::IsUpperCase(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	return (_istupper(*pStr)!=0);
}


bool CLexemType::IsDigits(UINT uiLessOrEqual,SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;

	SS::Interface::Core::BlackBox::IIndex * pIndex=pUnit->GetFirstIndex();
	if(!pIndex) return false;
	do{
		if(pIndex->GetDictionaryIndex())
			switch(/*(*/pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/) {
				case SS::Dictionary::DATA_TYPE::NAMES::ednNumeric:
					return true;
				case SS::Dictionary::DATA_TYPE::NAMES::ednSemantic:
				case SS::Dictionary::DATA_TYPE::NAMES::ednSyntax:
				case SS::Dictionary::DATA_TYPE::NAMES::ednDates:
				case SS::Dictionary::DATA_TYPE::NAMES::ednSymbols:
					break;
				default:
					return false;
			}
	}while(pIndex=pUnit->GetNextIndex());

	return false;
	return false;
}

bool CLexemType::IsSQuotes(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	if(*pStr)
		return (*pStr==_T('\"'));
	return false;
}
bool CLexemType::IsSTire(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	if(*pStr)
		return (*pStr==_T('-'));
	return false;
}
bool CLexemType::IsSDefis(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	if(*pStr)
		return (*pStr==_T('-'));
	return false;
}
bool CLexemType::IsSComma(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	if(*pStr)
		return (*pStr==_T(','));
	return false;
}

bool CLexemType::IsSDot(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	if(*pStr)
		return (*pStr==_T('.'));
	return false;
}

bool CLexemType::IsSApostrophe(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	if(*pStr)
		return (*pStr==_T('\''));
	return false;
}


bool CLexemType::IsSymbol(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;

	SS::Interface::Core::BlackBox::IIndex * pIndex=pUnit->GetFirstIndex();
	if(!pIndex) return false;
	do{
		if(pIndex->GetDictionaryIndex())
			switch(/*(*/pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/) {
				case SS::Dictionary::DATA_TYPE::NAMES::ednSymbols:
					return true;
				case SS::Dictionary::DATA_TYPE::NAMES::ednSemantic:
				case SS::Dictionary::DATA_TYPE::NAMES::ednSyntax:
				case SS::Dictionary::DATA_TYPE::NAMES::ednDates:
					break;
				default:
					return false;
			}
	}while(pIndex=pUnit->GetNextIndex());

	return false;
}
bool CLexemType::SymbolEqualsTo(TCharType pcSymbol,SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	if(*pStr)
		return (*pStr==*pcSymbol);
	return false;
}

UINT CLexemType::GetLexemLength(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=*m_ppCurentUnit;
	if (pUnit==NULL) return false;
	TCHAR * pStr=(TCHAR *)pUnit->GetWord();
	UINT i=0;
	while(pStr[i]) i++;
	return i;
}

	}
}