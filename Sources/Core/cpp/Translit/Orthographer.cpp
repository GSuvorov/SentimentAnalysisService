#include "stdafx.h"
#include "Orthographer.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "ConfigReader.h"
#include "../[libs]/_Paths.h"

/////////////////////////////////////////////////////////////////////////////////////
// Static variables

static const LPCWSTR s_rgOrtho[] = 
{
	L"а", L"а", L"о", 0,
	L"е", L"е", L"и", 0,
	L"и", L"и", L"е", 0,
	L"о", L"о", L"а", 0,
	L"ъ", L"ъ", L"ь", 0,
	L"я", L"я", L"и", L"е", 0,
	0,
};

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

COrthographer::COrthographer() :
		m_pMorphoAnalyzer(NULL),
		m_pTable(NULL),
		m_pHypotesis(NULL),
		m_pBreaker(NULL)
{
}

COrthographer::~COrthographer()
{
	delete m_pTable;
	delete m_pHypotesis;
	delete m_pBreaker;
}

/////////////////////////////////////////////////////////////////////////////////////
// COrthographer interface

void COrthographer::SetLoadManager(ILoadManager* pLoadManager)
{
	SS_TRY;

	if(GetLoadManager())
		return;

	CBaseCoreClass::SetLoadManager(pLoadManager);

	// automaton
	if(!m_pTable)
	{
		m_pTable = new CTransitionTable;

		wchar_t szDir[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, szDir);
		PathAppendW(szDir, L"Config");
		PathAppendW(szDir, L"Orthography.dat");

		CConfigReader oReader;
		if(!oReader.FillTable(szDir, m_pTable, false))
		{
			m_pTable->Clear();
			if(!m_pTable->InitByDefault((LPCWSTR*) s_rgOrtho))
				SS_THROW(L"Невозможно инициализировать таблицу орфографии по умолчанию");
		}
	}

	// storage
	if(!m_pHypotesis)
		m_pHypotesis = new CHypotesisStorage(1000);

	// word breaker
	if(!m_pBreaker)
		m_pBreaker = new CWordBreaker(pmOrtho);

	// analyse params
	IAnalyseParams* pAnalyseParams = (IAnalyseParams*)
		pLoadManager->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams);
	pAnalyseParams->SetCurrentSystemMode(ecsmGlobal);
	pAnalyseParams->SetAnalyseMod(eamIndexation);
	pAnalyseParams->GetAnalyseDepthParams()->SetIsMorpho(true);
	pAnalyseParams->GetAnalyseDepthParams()->SetIsSemantic(true);
	pAnalyseParams->GetAnalyseDepthParams()->SetIsSyntax(true);
	pAnalyseParams->GetAnalyseDepthParams()->SetIsSynonims(false);

	// morphology
	IDictionary* pDict = (IDictionary*) 
		pLoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);
	m_pMorphoAnalyzer = pDict->GetMorphologyAnalyser();
	m_pMorphoAnalyzer->SetAnalyseParams(pAnalyseParams);

	SS_CATCH(L"");
}

void COrthographer::CheckOrthography(LPCWSTR szIncorrect, wstring* pCorrect)
{
	SS_TRY;

	int iLen = lstrlenW(szIncorrect);
	pCorrect->reserve(iLen);

	for(ELexemType lt = m_pBreaker->GetFirstLexem(szIncorrect); lt; lt = m_pBreaker->GetNextLexem())
	{
		if(lt == ltRus)
			AddWord((LPWSTR) m_pBreaker->GetCurrLexemString(), pCorrect);
		else
			*pCorrect += m_pBreaker->GetCurrLexemString();
	}

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// COrthographer functions

void COrthographer::AddWord(LPWSTR szWord, wstring* pResult)
{
	SS_TRY;

	if(m_pMorphoAnalyzer->IsExistWord(szWord))
	{
		*pResult += szWord;
		return;
	}

	m_pTable->Process(szWord, m_pHypotesis);
	for(LPCWSTR sz = m_pHypotesis->GetFirstHypotesis(); sz; sz = m_pHypotesis->GetNextHypotesis())
	{
		if(m_pMorphoAnalyzer->IsExistWord(sz))
		{
			*pResult += sz;
			return;
		}
	}

	*pResult += szWord;

	SS_CATCH(L"Failed on word" AND szWord);
}
