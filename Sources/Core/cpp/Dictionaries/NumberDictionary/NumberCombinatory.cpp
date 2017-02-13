#include "stdafx.h"
#include "NumberCombinatory.h"

#include <assert.h>

#include "..\..\ASSInterface\Constants.h"
#include "../[libs]/_Paths.h"
using namespace SS::Constants;

using namespace std;

namespace SS  
{
namespace Dictionary  
{
namespace NumberCombinatory
{

/////////////////////////////////////////////////////////////////////////////////////
// Static members

const CNumberCombinatory::SLexNumeric CNumberCombinatory::s_rNumeric[] =
{
	//	{ 0, _T("ноль"), unit, false },
	{ 1, _T("один"), unit, false },
	{ 2, _T("два"), unit, false },
	{ 3, _T("три"), unit, false },
	{ 4, _T("четыре"), unit, false },
	{ 5, _T("пять"), unit, false },
	{ 6, _T("шесть"), unit, false },
	{ 7, _T("семь"), unit, false },
	{ 8, _T("восемь"), unit, false },
	{ 9, _T("девять"), unit, false },
	{ 10, _T("десять"), ten, false },
	{ 11, _T("одиннадцать"), unidec, false },
	{ 12, _T("двенадцать"), unidec, false },
	{ 13, _T("тринадцать"), unidec, false },
	{ 14, _T("четырнадцать"), unidec, false },
	{ 15, _T("пятнадцать"), unidec, false },
	{ 16, _T("шестнадцать"), unidec, false },
	{ 17, _T("семнадцать"), unidec, false },
	{ 18, _T("восемнадцать"), unidec, false },
	{ 19, _T("девятнадцать"), unidec, false },
	{ 20, _T("двадцать"), dec, false },
	{ 30, _T("тридцать"), dec, false },
	{ 40, _T("сорок"), dec, false },
	{ 50, _T("пятьдесят"), dec, false },
	{ 60, _T("шестьдесят"), dec, false },
	{ 70, _T("семьдесят"), dec, false },
	{ 80, _T("восемьдесят"), dec, false },
	{ 90, _T("девяносто"), dec, false },
	{ 100, _T("сто"), hundreds, false },
	{ 200, _T("двести"), hundreds, false },
	{ 300, _T("триста"), hundreds, false },
	{ 400, _T("четыреста"), hundreds, false },
	{ 500, _T("пятьсот"), hundreds, false },
	{ 600, _T("шестьсот"), hundreds, false },
	{ 700, _T("семьсот"), hundreds, false },
	{ 800, _T("восемьсот"), hundreds, false },
	{ 900, _T("девятьсот"), hundreds, false },
	{ 1000, _T("тысяча"), thousand, false },
	{ 1000000, _T("миллион"), mln, false },
	{ 1000000000, _T("миллиард"), mlrd, false },
//	{ 1000000000000, _T("триллион"), trln, false },

	{ 1, _T("первый"), unit, true },
	{ 2, _T("второй"), unit, true },
	{ 3, _T("третий"), unit, true },
	{ 4, _T("четвертый"), unit, true },
	{ 5, _T("пятый"), unit, true },
	{ 6, _T("шестой"), unit, true },
	{ 7, _T("седьмой"), unit, true },
	{ 8, _T("восьмой"), unit, true },
	{ 9, _T("девятый"), unit, true },
	{ 10, _T("десятый"), ten, true },
	{ 11, _T("одиннадцатый"), unidec, true },
	{ 12, _T("двенадцатый"), unidec, true },
	{ 13, _T("тринадцатый"), unidec, true },
	{ 14, _T("четырнадцатый"), unidec, true },
	{ 15, _T("пятнадцатый"), unidec, true },
	{ 16, _T("шестнадцатый"), unidec, true },
	{ 17, _T("семнадцатый"), unidec, true },
	{ 18, _T("восемнадцатый"), unidec, true },
	{ 19, _T("девятнадцатый"), unidec, true },
	{ 20, _T("двадцатый"), dec, true },
	{ 30, _T("тридцатый"), dec, true },
	{ 40, _T("сороковой"), dec, true },
	{ 50, _T("пятидесятый"), dec, true },
	{ 60, _T("шестидесятый"), dec, true },
	{ 70, _T("семидесятый"), dec, true },
	{ 80, _T("восьмидесятый"), dec, true },
	{ 90, _T("девяностый"), dec, true },
	{ 100, _T("сотый"), hundred, true },
	{ 200, _T("двухсотый"), hundreds, true },
	{ 300, _T("трехсотый"), hundreds, true },
	{ 400, _T("четырехсотый"), hundreds, true },
	{ 500, _T("пятисотый"), hundreds, true },
	{ 600, _T("шестисотый"), hundreds, true },
	{ 700, _T("семисотый"), hundreds, true },
	{ 800, _T("восьмисотый"), hundreds, true },
	{ 900, _T("девятисотый"), hundreds, true },
	{ 1000, _T("тысячный"), thousand, true },
	{ 1000000, _T("миллионный"), mln, true },
	{ 1000000000, _T("миллиардный"), mlrd, true },
//	{ 1000000000000, _T("триллионный"), trln, true },

//	{ 0, _T("zero"), unit, false },
	{ 1, _T("one"), unit, false },
	{ 2, _T("two"), unit, false },
	{ 3, _T("three"), unit, false },
	{ 4, _T("four"), unit, false },
	{ 5, _T("five"), unit, false },
	{ 6, _T("six"), unit, false },
	{ 7, _T("seven"), unit, false },
	{ 8, _T("eight"), unit, false },
	{ 9, _T("nine"), unit, false },
	{ 10, _T("ten"), ten, false },
	{ 11, _T("eleven"), unidec, false },
	{ 12, _T("twelve"), unidec, false },
	{ 13, _T("thirteen"), unidec, false },
	{ 14, _T("fourteen"), unidec, false },
	{ 15, _T("fifteen"), unidec, false },
	{ 16, _T("sixteen"), unidec, false },
	{ 17, _T("seventeen"), unidec, false },
	{ 18, _T("eighteen"), unidec, false },
	{ 19, _T("nineteen"), unidec, false },
	{ 20, _T("twenty"), dec, false },
	{ 30, _T("thirty"), dec, false },
	{ 40, _T("forty"), dec, false },
	{ 50, _T("fifty"), dec, false },
	{ 60, _T("sixty"), dec, false },
	{ 70, _T("seventy"), dec, false },
	{ 80, _T("eighty"), dec, false },
	{ 90, _T("ninety"), dec, false },
	{ 100, _T("hundred"), hundred, false },
	{ 1000, _T("thousand"), thousand, false },
	{ 1000000, _T("million"), mln, false },
	{ 1000000000, _T("billion"), mlrd, false },
//	{ 1000000000000, _T("trillion"), trln, false },
	{ 0, _T("and"), and, false },

	{ 1, _T("first"), unit, true },
	{ 2, _T("second"), unit, true },
	{ 3, _T("third"), unit, true },
	{ 4, _T("forth"), unit, true },
	{ 5, _T("fifth"), unit, true },
	{ 6, _T("sixth"), unit, true },
	{ 7, _T("seventh"), unit, true },
	{ 8, _T("eighth"), unit, true },
	{ 9, _T("ninth"), unit, true },
	{ 10, _T("tenth"), ten, true },
	{ 11, _T("eleventh"), unidec, true },
	{ 12, _T("twelfth"), unidec, true },
	{ 13, _T("thirteenth"), unidec, true },
	{ 14, _T("fourteenth"), unidec, true },
	{ 15, _T("fifteenth"), unidec, true },
	{ 16, _T("sixteenth"), unidec, true },
	{ 17, _T("seventeenth"), unidec, true },
	{ 18, _T("eighteenth"), unidec, true },
	{ 19, _T("nineteenth"), unidec, true },
	{ 20, _T("twentieth"), dec, true },
	{ 30, _T("thirtieth"), dec, true },
	{ 40, _T("fortieth"), dec, true },
	{ 50, _T("fiftieth"), dec, true },
	{ 60, _T("sixtieth"), dec, true },
	{ 70, _T("seventieth"), dec, true },
	{ 80, _T("eightieth"), dec, true },
	{ 90, _T("ninetieth"), dec, true },
	{ 100, _T("hundredth"), hundred, true },
	{ 1000, _T("thousandth"), thousand, true },
	{ 1000000, _T("millionth"), mln, true },
	{ 1000000000, _T("billionth"), mlrd, true },

	{ 0, 0, 0, 0 },
};


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CNumberCombinatory::CNumberCombinatory() :
		m_pMorphologyAnalyser(NULL),
		m_pConv(NULL),
		m_pvIDSources(NULL)
{
}

CNumberCombinatory::~CNumberCombinatory()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CBaseCoreClass

void CNumberCombinatory::SetLoadManager(ILoadManager* pLoadManager)
{
	if(GetLoadManager())
		return;

	CBaseCoreClass::SetLoadManager(pLoadManager);

	IDictionary *pDictionary = (IDictionary*)
		pLoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);
	if(!pDictionary)
		SS_THROW(L"Failed to create IDictionary");

	m_pMorphologyAnalyser = pDictionary->GetMorphologyAnalyser();

	m_pConv = (IAMConverterMorpho*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);
	if(!m_pConv)
		SS_THROW(L"Failed to create IAMConverterMorpho");

	FillIDSourceTable();
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CNumberCombinatory::QueryInterface(REFIID refIID, void** pBase)
{
	if(refIID == IID_INumberCombinatory)
		*pBase = (INumberCombinatory*) this;
	else if(refIID == IID_Base)
		*pBase = (IBase*) this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CNumberCombinatory::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// INumberCombinatory

void CNumberCombinatory::GetCombinationsInfo(TIDSourceVector* pvIDSources, std::list<SCombinationNewInfo>* plCombinationInfo)
{
	SS_TRY;

	m_pvIDSources = pvIDSources;
	Parse(plCombinationInfo, (int)m_pvIDSources->size());

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CNumericParser functions

int CNumberCombinatory::GetCurrentLexem(int iPosition, YYSTYPE* pValue)
{
	SS_TRY;

	if(iPosition >= (int)m_pvIDSources->size())
		return 0;

	UINT idSource = m_pvIDSources->at(iPosition).uiIDSource & SIGNIFICANT_INDEX_PART;
	if(idSource > 0)
	{
		*pValue = idSource;
		if(idSource < 10)
			return digit;
		else if(idSource < 1000)
			return hundreds_numeric;
	}

	TIDSourceTable::iterator it = m_mIDSources.find(idSource);
	if(it == m_mIDSources.end())
		return unknown;

	*pValue = it->second.uiValue;
	return it->second.iLexemType;

	SS_CATCH(L"");
	return 0;
}

void CNumberCombinatory::AddToOutput(void* pParam, int iFirst, int iLast, YYSTYPE Result)
{
	SS_TRY;

	std::list<SCombinationNewInfo>* plCombinationInfo = (std::list<SCombinationNewInfo>*) pParam;

	SCombinationNewInfo oInfo;
	ZeroMemory(&oInfo, sizeof(SCombinationNewInfo));

	oInfo.PosInClause = m_pvIDSources->at(iFirst).uiPosition;
	oInfo.NumOfMembers = m_pvIDSources->at(iLast).uiPosition - m_pvIDSources->at(iFirst).uiPosition + 1;
	oInfo.CombiType = ctNewLexicalUnit;

	if(Result < MAX_NUMERIC_VALUE)
	{
		oInfo.HashKey = (UINT) Result;

		TCHAR sz[100];
		_stprintf(sz, IsOrdinal(iLast) ? _T("%uth") : _T("%u"), (UINT) Result);
		basic_string<TCHAR> str = sz;

		std::list<SS::Dictionary::Types::TWordInfo> oLst;
		m_pMorphologyAnalyser->GetWordInfo(str.c_str(), &oLst, efzOnlySearch);

		for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
		{
			if(it->empty())
				continue;

			const SWordInfo& mi = it->front();

			oInfo.ID = mi.m_WordIndex.m_DictIndex.GetDictionaryIndex();
			oInfo.uiMorphoInfo = mi.m_MorphoTypeInfo.m_MorphoInfo;
			oInfo.uiPartOfSpeech = mi.m_SpecialMorpho;

			plCombinationInfo->push_back(oInfo);
		}
	}

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CIndexLoader functions

void CNumberCombinatory::FillIDSourceTable()
{
	SS_TRY;

	CMorphoFeature oMorphoFeature;

	for(int i = 0; s_rNumeric[i].sz; i++)
	{
		UINT idSource = GetSourceID(s_rNumeric[i].sz, &oMorphoFeature);
		if(!idSource)
			continue;

		m_mIDSources.insert(TIDSourceTable::value_type(idSource, 
			SNumeric(s_rNumeric[i].uiValue, s_rNumeric[i].iLexemType, s_rNumeric[i].bOrdinal)));
	}

	SS_CATCH(L"");
}

UINT CNumberCombinatory::GetSourceID(LPCTSTR sz, CMorphoFeature* pMorphoFeature)
{
	SS_TRY;

	std::basic_string<TCHAR> str = sz;

	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	m_pMorphologyAnalyser->GetWordInfo(str.c_str(), &oLst, efzOnlySearch);

	pMorphoFeature->Undefine();
	for(list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
	{
		if(!it->empty())
		{
			const SWordInfo* pmi = &it->front();

			m_pConv->EnCode(pmi->m_WordIndex.m_DictIndex.GetDictionaryNumber(), 
							pmi->m_MorphoTypeInfo.m_MorphoInfo, 
							pmi->m_SpecialMorpho, 
							pMorphoFeature);

			if( pMorphoFeature->m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNumeric) ||
				pMorphoFeature->m_OfficialType.IsContain(Values::OfficialType.otConjunction))
			{
				return pmi->m_WordIndex.m_DictIndex.GetDictionaryIndexWithoutFlags();
			}
		}
	}

	SS_CATCH(L"");
	return 0;
}

bool CNumberCombinatory::IsOrdinal(int iPos)
{
	SS_TRY;

	UINT idSource = m_pvIDSources->at(iPos).uiIDSource & SIGNIFICANT_INDEX_PART;
	TIDSourceTable::iterator it = m_mIDSources.find(idSource);
	if(it != m_mIDSources.end())
		return it->second.bOrdinal;

	SS_CATCH(L"");
	return false;
}

}
}
}
