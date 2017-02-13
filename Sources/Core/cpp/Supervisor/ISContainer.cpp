#include "stdafx.h"
#include "ISContainer.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CISContainer::CISContainer()
{
}

CISContainer::~CISContainer()
{
	SetBase(L"");
/*
	for(TQueryResultsMap::iterator itQ = m_QRMap.begin(); itQ != m_QRMap.end(); ++itQ)
		itQ->second->Release();
	for(TSearchResultsMap::iterator itR = m_SRMap.begin(); itR != m_SRMap.end(); ++itR)
		itR->second->Release();
	for(TLinguisticProcessorMap::iterator itL = m_LPMap.begin(); itL != m_LPMap.end(); ++itL)
		itL->second->Release();
	for(TLoadManagerMap::iterator itLM = m_LMMap.begin(); itLM != m_LMMap.end(); ++itLM)
		itLM->second->Release();
*/
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CISContainer::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID == IID_Base)
		*pBase = (SS::Interface::IBase*) this;
	else if(pIID == IID_ISContainer)
		*pBase = (SS::Interface::Core::IISContainerUM*) this;
	else
		*pBase = NULL;
	
	return S_OK;
}

ULONG CISContainer::Release()
{
//	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// IISContainerUM interface

void CISContainer::SetLoadManager(ILoadManager* pLoadManager)
{
	CBaseCoreClass::SetLoadManager(pLoadManager);

	ISettingsServer* pSettings = (ISettingsServer*) GetLoadManager()->GetInterface(
			L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);
	bool bMorpho	= pSettings->GetBoolean(L"Linguistic::UseMorpho") == 0 ? false : true;
	bool bSemantic	= pSettings->GetBoolean(L"Linguistic::UseSemantic") == 0 ? false : true;
	bool bSynonims	= pSettings->GetBoolean(L"Linguistic::UseSynonims") == 0 ? false : true;
	bool bSyntax	= pSettings->GetBoolean(L"Linguistic::UseSyntax") == 0 ? false : true;

	m_oLinguisticMode.GetAnalyseDepthParams()->SetIsMorpho(bMorpho);
	m_oLinguisticMode.GetAnalyseDepthParams()->SetIsSemantic(bSemantic);
	m_oLinguisticMode.GetAnalyseDepthParams()->SetIsSynonims (bSynonims);
	m_oLinguisticMode.GetAnalyseDepthParams()->SetIsSyntax(bSyntax);
}

IQueryResult* CISContainer::GetQueryResult(const wchar_t* wszFieldName)
{
	TQueryResultsMap::iterator it = m_QRMap.find(wszFieldName);
	if(it == m_QRMap.end())
	{
		IQueryResult* pQueryResult = NULL;
		GetNewLoadManager(wszFieldName)->Create(
			L"./core/linguistics/CommonContainers.dll", CLSID_QueryResult, IID_QueryResult, (void**) &pQueryResult);
		it = m_QRMap.insert(TQueryResultsMap::value_type(wszFieldName, pQueryResult)).first;
		return pQueryResult;
	}
	else
		return it->second;
}

ISearchResult* CISContainer::GetSearchResult(const wchar_t* wszFieldName)
{
	TSearchResultsMap::iterator it = m_SRMap.find(wszFieldName);
	if(it == m_SRMap.end())
	{
		ISearchResult* pSearchResult = NULL;
		GetNewLoadManager(wszFieldName)->Create(
			L"./core/linguistics/CommonContainers.dll", CLSID_SearchResult, IID_SearchResult, (void**)&pSearchResult);
		it = m_SRMap.insert(TSearchResultsMap::value_type(wszFieldName, pSearchResult)).first;
		return pSearchResult;
	}
	else
		return it->second;
}

ILinguisticProcessor* CISContainer::GetLinguisticProcessor(const wchar_t* wszFieldName)
{
	TLinguisticProcessorMap::iterator it = m_LPMap.find(wszFieldName);
	if(it == m_LPMap.end())
	{
		ILinguisticProcessor* pLinguisticProcessor = NULL;
		GetNewLoadManager(wszFieldName)->Create(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, 
			IID_LinguisticProcessor, (void**) &pLinguisticProcessor);

		pLinguisticProcessor->SetMode(&m_oLinguisticMode);
		it = m_LPMap.insert(TLinguisticProcessorMap::value_type(wszFieldName, pLinguisticProcessor)).first;
		return pLinguisticProcessor;
	}
	else
		return it->second;
}

void CISContainer::Clear()
{
	for(TSearchResultsMap::iterator it = m_SRMap.begin(); it != m_SRMap.end(); ++it)
	{
		it->second->Clear();
		it++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// CISContainer functions

void CISContainer::SetBase(const wchar_t* wsz)
{
	m_wszBase = wsz;
	for(TLoadManagerMap::iterator itLM = m_LMMap.begin(); itLM != m_LMMap.end(); ++itLM)
	{
		IBasesManager* pBasesManager = (IBasesManager*)
			itLM->second->GetInterface(L"./core/BasesManager.dll", CLSID_BasesManager, IID_BasesManager);

		pBasesManager->CloseStorage();
		if(wsz && wsz[0] != 0)
		{
			pBasesManager->SetActiveStorage(wsz);
			pBasesManager->OpenStorage();
		}
	}
}

ILoadManager* CISContainer::GetNewLoadManager(const wchar_t* wszFieldName)
{
	TLoadManagerMap::iterator it = m_LMMap.find(wszFieldName);
	if(it == m_LMMap.end())
	{
		ILoadManager* pLoadManager = GetLoadManager();
		pLoadManager->Create(
			L"./core/LoadManager.dll", CLSID_LoadManager_IBase, IID_LoadManager, (void**) &pLoadManager);

		IBasesManager* pBasesManager = (IBasesManager*)
			pLoadManager->GetInterface(L"./core/BasesManager.dll", CLSID_BasesManager, IID_BasesManager);

		pBasesManager->CloseStorage();
		pBasesManager->SetActiveStorage(m_wszBase.c_str());
		pBasesManager->OpenStorage();

		it = m_LMMap.insert(TLoadManagerMap::value_type(wszFieldName, pLoadManager)).first;
		return pLoadManager;
	}
	else
		return it->second;
}
