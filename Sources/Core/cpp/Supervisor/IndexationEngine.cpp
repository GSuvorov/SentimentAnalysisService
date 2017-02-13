#include "stdafx.h"
#include "IndexationEngine.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;

#include "..\ASCInterface\ICommonServices.h"
using namespace SS::Interface::Core::CommonServices;


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CIndexationEngine::CIndexationEngine() :
		m_pNdxProcessor(NULL),
		m_pLinguisticIndexation(NULL),
		m_pResult(NULL),
		m_pPluginManager(NULL)
{
}

CIndexationEngine::~CIndexationEngine()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CSupervisorModule interface

void CIndexationEngine::SetLoadManager(ILoadManager* pLoadManager)
{
	SS_TRY;

	CBaseCoreClass::SetLoadManager(pLoadManager);

	INdxSearchEngine* pEngine = (INdxSearchEngine*) 
		GetLoadManager()->GetInterface(L"./core/NdxProcessor.dll", CLSID_NdxSearchEngine, IID_NdxSearchEngine);
	m_pNdxProcessor = pEngine->INdxSearchEngineIndexation();

	ILinguisticProcessor* pProcessor = (ILinguisticProcessor*)
		GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, IID_LinguisticProcessor);
	TLinguisticProcessorMode oMode;
	oMode.GetAnalyseDepthParams()->SetIsMorpho(true);
	pProcessor->SetMode(&oMode);
	m_pLinguisticIndexation = pProcessor->GetLinguisticIndexation();

	m_pResult = (IIndexationResult*)
		GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_IndexationResult, IID_IndexationResult);

	m_pPluginManager = (IPluginManager*) 
		GetLoadManager()->GetInterface(L"DocPlugins\\PluginManager.dll", CLSID_PluginManager, IID_PluginManager);
	LoadPlugins();

	SS_CATCH(L"");
}

bool CIndexationEngine::Indexate(LPCWSTR szPath)
{
	SS_TRY;

	m_pNdxProcessor->StartIndexationSession();
	IndexateFile(szPath);
	m_pNdxProcessor->EndIndexationSession();

	SS_CATCH(L"");
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////
// CSupervisorModule functions

bool CIndexationEngine::IndexateFile(LPCWSTR szFile)
{
	SS_TRY;

	IDocParser* pParser = m_pPluginManager->GetParser();
	if(!pParser->Parse(szFile))
		return false;

	m_pLinguisticIndexation->StartDocumentIndexation();
	m_pNdxProcessor->StartDocumentIndexation(m_pResult);

	for(EPluginBlockType eType = pParser->GetFirstBlock(&m_strText); eType != pbtEnd; eType = pParser->GetNextBlock(&m_strText))
	{
		LPCWSTR szBlockType = GetFieldName(eType);
		IndexateFileBlock(m_strText.c_str(), szBlockType);
	}

	TNdxMetaFieldCollection oFieldColl;
	m_pNdxProcessor->EndDocumentIndexation(&oFieldColl, m_pResult);
	return true;

	SS_CATCH(L"");
	return false;
}

void CIndexationEngine::IndexateFileBlock(LPCWSTR szText, LPCWSTR szBlockType)
{
	SS_TRY;

	TNativeTextForIndexation oText;
	oText.SetText(m_strText.c_str());
	m_pLinguisticIndexation->Init(&oText, NULL);

	while(m_pLinguisticIndexation->GenerateNext(m_pResult) == 0)
	{
		for(ITextBlock* pBlock = m_pResult->GetFirstTextBlock(); pBlock; pBlock = m_pResult->GetNextTextBlock())
		{
			for(ITextBlockCut* pCut = pBlock->GetFirstBlockCut(); pCut; pCut = pBlock->GetNextBlockCut())
			{
				pCut->m_IndexesType = itNormal;
				pCut->SetIndexTypeName(szBlockType);
			}
		}
		m_pNdxProcessor->IndexateDocumentChunk(m_pResult);
	}

	SS_CATCH(L"");
}

void CIndexationEngine::LoadPlugins()
{
	SS_TRY;

	IWorkRegister* pWorkRegister = (IWorkRegister*)
		GetLoadManager()->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);
	wstring strPath = pWorkRegister->GetParameter(L"DocPluginsPath");
	m_pPluginManager->LoadPlugins(strPath.c_str());

	SS_CATCH(L"");
}

LPCWSTR CIndexationEngine::GetFieldName(EPluginBlockType eType)
{
	switch(eType)
	{
		case pbtText:
			return L"TEXT";
		case pbtTable:
			return L"TABLES";
		case pbtTitle:
			return L"HEADERS";
		default:
			return L"";
	}
}
