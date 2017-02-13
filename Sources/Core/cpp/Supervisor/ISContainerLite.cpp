#include "stdafx.h"
#include "ISContainerLite.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CISContainerLite::CISContainerLite() :
		m_pQueryResult(NULL),
		m_pSearchResult(NULL),
		m_pLinguisticProcessor(NULL)
{
}

CISContainerLite::~CISContainerLite()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CISContainerLite::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID == IID_Base)
		*pBase = (SS::Interface::IBase*) this;
	else if(pIID == IID_ISContainer)
		*pBase = (SS::Interface::Core::IISContainerUM*) this;
	else
		*pBase = NULL;
	
	return S_OK;
}

ULONG CISContainerLite::Release()
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// IISContainerUM interface

void CISContainerLite::SetLoadManager(ILoadManager* pLoadManager)
{
	SS_TRY;

	CBaseCoreClass::SetLoadManager(pLoadManager);

	ISettingsServer* pSettings = (ISettingsServer*) GetLoadManager()->GetInterface(
			L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);
	bool bMorpho	= pSettings->GetBoolean(L"Linguistic::UseMorpho") == 0 ? false : true;
	bool bSemantic	= pSettings->GetBoolean(L"Linguistic::UseSemantic") == 0 ? false : true;
	bool bSynonims	= pSettings->GetBoolean(L"Linguistic::UseSynonims") == 0 ? false : true;
	bool bSyntax	= pSettings->GetBoolean(L"Linguistic::UseSyntax") == 0 ? false : true;

	TLinguisticProcessorMode oLinguisticMode;
	oLinguisticMode.GetAnalyseDepthParams()->SetIsMorpho(bMorpho);
	oLinguisticMode.GetAnalyseDepthParams()->SetIsSemantic(bSemantic);
	oLinguisticMode.GetAnalyseDepthParams()->SetIsSynonims (bSynonims);
	oLinguisticMode.GetAnalyseDepthParams()->SetIsSyntax(bSyntax);

	m_pQueryResult = (IQueryResult*) 
		GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_QueryResult, IID_QueryResult);
	m_pSearchResult = (ISearchResult*)
		GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_SearchResult, IID_SearchResult);
	m_pLinguisticProcessor = (ILinguisticProcessor*)
		GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, IID_LinguisticProcessor); 
	m_pLinguisticProcessor->SetMode(&oLinguisticMode);

	SS_CATCH(L"");
}

IQueryResult* CISContainerLite::GetQueryResult(const wchar_t* wszFieldName)
{
	return m_pQueryResult;
}

ISearchResult* CISContainerLite::GetSearchResult(const wchar_t* wszFieldName)
{
	return m_pSearchResult;
}

ILinguisticProcessor* CISContainerLite::GetLinguisticProcessor(const wchar_t* wszFieldName)
{
	return m_pLinguisticProcessor;
}

void CISContainerLite::Clear()
{
	m_pSearchResult->Clear();
}
