#include "StdAfx.h"
#include ".\linguisticshowresults.h"
#include "..\ASSInterface\Constants.h"
#include "..\ASSInterface\IBlackBoxIndex.h"
#include "..\[libs]\_Paths.h"

using namespace SS::Interface::Core::BlackBox;

SS::LinguisticProcessor::CLinguisticShowResults::CLinguisticShowResults(void) : 
		m_pLexicaPrimary(NULL),
		m_pMorphoAnalyzer(NULL)
{
}

SS::LinguisticProcessor::CLinguisticShowResults::~CLinguisticShowResults(void)
{
}

SS::Interface::Core::ILexicaPrimary* SS::LinguisticProcessor::CLinguisticShowResults::GetLexicaPrimary()
{
	SS_TRY
	{
		//SL_OUT_DEBUG_STR (__WFUNCTION__);
		if(m_pLexicaPrimary)
			return (SS::Interface::Core::ILexicaPrimary*)m_pLexicaPrimary;
		else
			return NULL;
	}
	SS_CATCH(L"")	
}

unsigned int SS::LinguisticProcessor::CLinguisticShowResults::IsExistWord(const wchar_t* psWord)
{
	SS_TRY
	{
		//SL_OUT_DEBUG_STR (__WFUNCTION__);
		return m_ResultsEx.IsExistWord(psWord);
	}
	SS_CATCH(L"")
}

void SS::LinguisticProcessor::CLinguisticShowResults::PrepareForView(
	/// контейнер с результатами поиска
	SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
	/// параметры подготовки к выводу (кол-во подготавливаемых результатов и т.п.)
	, SS::Core::Types::IndexationSearch::TPrepareForViewParams* pPrepareForViewParams
	)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);
		m_ResultsEx.PrepareForView(pSearchResult, pPrepareForViewParams);
		SL_OUT_DEBUG_STR (__WFUNCTION__);
	}
	SS_CATCH(L"")

}

void SS::LinguisticProcessor::CLinguisticShowResults::GetSourceIndices(const wchar_t* wszWord, std::vector<unsigned int>* pvIndices)
{
	SS_TRY;

	pvIndices->clear();
	if(!InitDictionary())
		return;

	std::list<TWordInfo> oLst;
	m_pMorphoAnalyzer->GetWordInfo(wszWord, &oLst, efzOnlySearch);
	if(oLst.empty())
		return;

	long idPrev = -1;
	for(std::list<TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
	{
		SWordInfo* pwi = &(it->front());

		// if we have the same source id as in previous cycle, use previous pIndex
		long idSource = pwi->m_WordIndex.m_DictIndex.GetDictionaryIndexWithoutFlags();
		if(idPrev == -1 || idPrev != idSource)
		{
			pvIndices->push_back(idSource);
			idPrev = idSource;
		}
	}

	SS_CATCH(L"");
}

void SS::LinguisticProcessor::CLinguisticShowResults::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY
	{
		SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);

		m_ResultsEx.SetLoadManager(GetLoadManager());
		m_pLexicaPrimary= 
			(SS::Interface::Core::ILexicaPrimary*)
			GetLoadManager()->GetInterface(L"./core/linguistics/LexicalAnalyzer.dll", CLSID_LexicaPrimary, IID_LexicaPrimary);

		InitDictionary();

	}
	SS_CATCH(L"");
}

bool SS::LinguisticProcessor::CLinguisticShowResults::InitDictionary()
{
	SS_TRY;

	if(!GetLoadManager())
		return false;

	if(!GetAnalyseParams())
	{
		SetAnalyseParams( 
			(SS::Interface::Core::MainAnalyse::IAnalyseParams*)
			GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams));
		//GetAnalyseParams()->SetAnalyseMod(eamIndexation);
		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsMorpho(true);
	}

	if(!m_pMorphoAnalyzer)
	{
		IDictionary* pDict = (IDictionary*) GetLoadManager()->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);
		m_pMorphoAnalyzer = pDict->GetMorphologyAnalyser();
		m_pMorphoAnalyzer->SetAnalyseParams(GetAnalyseParams());
	}

	SS_CATCH(L"");
	return m_pMorphoAnalyzer != NULL;
}
