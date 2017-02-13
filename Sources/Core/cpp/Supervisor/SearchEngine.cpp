#include "stdafx.h"
#include "SearchEngine.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CSearchEngine::CSearchEngine() :
		m_pNdxProcessor(NULL),
		m_pParametrizedQuery(NULL),
		m_pLinguisticProcessor(NULL),
		m_pQueryResult(NULL),
		m_pContainer(NULL)
{
}

CSearchEngine::~CSearchEngine()
{
	delete m_pContainer;
}

/////////////////////////////////////////////////////////////////////////////////////
// CSearchEngine interface

void CSearchEngine::SetLoadManager(ILoadManager* pLoadManager)
{
	SS_TRY;

	if(GetLoadManager())
		return;
	CBaseCoreClass::SetLoadManager(pLoadManager);

	// ISContainer
	if(!m_pContainer)
	{
		m_pContainer = new CISContainerLite;
		m_pContainer->SetLoadManager(pLoadManager);
		m_pLinguisticProcessor = m_pContainer->GetLinguisticProcessor(L"TEXT");
		m_pQueryResult = m_pContainer->GetQueryResult(L"TEXT");
	}

	// Ndx processor
	INdxSearchEngine* pEngine = (INdxSearchEngine*) 
		GetLoadManager()->GetInterface(L"./core/NdxProcessor.dll", CLSID_NdxSearchEngine, IID_NdxSearchEngine);
	m_pNdxProcessor = pEngine->INdxSearchEngineFind();

	m_pParametrizedQuery = (IParametrizedQuery*) 
		GetLoadManager()->GetInterface(L"./core/NdxQuery.dll", CLSID_ParametrizedQuery, IID_ParametrizedQuery);

	SS_CATCH(L"");
}

bool CSearchEngine::Load(LPCWSTR szQuery)
{
	SS_TRY
	{

std::wstring wsMessage;

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin";
OutputDebugString(wsMessage.c_str());
	// настраиваем параметры
	TSearchParams oSearchParams(1);

	CNdxSearchSettings oSearchSettings;
	oSearchSettings.m_SearchSettings.m_EvalRelevanceMode = CNdxSearchSettings::SSearchSettings::erWordProximityWithStatistic;
	oSearchSettings.m_SearchSettings.m_StorageUsingMode = CNdxSearchSettings::SSearchSettings::esumSingleStorage;

	TQueryParams oQueryParams;
	oQueryParams.m_GenerateQueryMode = gqmIndex;
	oQueryParams.SetQuery(szQuery);

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin GenerateQuery()";
OutputDebugString(wsMessage.c_str());
	// запрос
	m_pLinguisticProcessor->GetLinguisticQuery()->GenerateQuery(&oSearchParams, &oQueryParams, m_pQueryResult, true);
wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end GenerateQuery()";
OutputDebugString(wsMessage.c_str());

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin m_pParametrizedQuery->Init";
OutputDebugString(wsMessage.c_str());
	m_pParametrizedQuery->Init(m_pQueryResult, &oSearchParams);
wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end m_pParametrizedQuery->Init";
OutputDebugString(wsMessage.c_str());


wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin NdxProcessor";
OutputDebugString(wsMessage.c_str());
	// поиск в индексном
	m_pNdxProcessor->StartFindSession(m_pContainer);
	HRESULT hr = m_pNdxProcessor->Find(L"TEXT", m_pParametrizedQuery, &oSearchParams, &oSearchSettings);
	m_pNdxProcessor->EndFindSession();
wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end NdxProcessor";
OutputDebugString(wsMessage.c_str());

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end";
OutputDebugString(wsMessage.c_str());
	return true;

	}
	SS_CATCH(L"");
	return false;
}

bool CSearchEngine::Search(LPCWSTR szQuery, ISearchResult** ppResult)
{
	SS_TRY;

std::wstring wsMessage;

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin";
OutputDebugString(wsMessage.c_str());
	// настраиваем параметры
	TSearchParams oSearchParams(1);

	CNdxSearchSettings oSearchSettings;
	oSearchSettings.m_SearchSettings.m_EvalRelevanceMode = CNdxSearchSettings::SSearchSettings::erWordProximityWithStatistic;
	oSearchSettings.m_SearchSettings.m_StorageUsingMode = CNdxSearchSettings::SSearchSettings::esumSingleStorage;

	TQueryParams oQueryParams;
	oQueryParams.m_GenerateQueryMode = gqmIndex;
	oQueryParams.SetQuery(szQuery);

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin GenerateQuery()";
OutputDebugString(wsMessage.c_str());
	// запрос
	m_pLinguisticProcessor->GetLinguisticQuery()->GenerateQuery(&oSearchParams, &oQueryParams, m_pQueryResult, true);
wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end GenerateQuery()";
OutputDebugString(wsMessage.c_str());

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin m_pParametrizedQuery->Init";
OutputDebugString(wsMessage.c_str());
	m_pParametrizedQuery->Init(m_pQueryResult, &oSearchParams);
wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end m_pParametrizedQuery->Init";
OutputDebugString(wsMessage.c_str());


wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin NdxProcessor";
OutputDebugString(wsMessage.c_str());
	// поиск в индексном
	m_pNdxProcessor->StartFindSession(m_pContainer);
	HRESULT hr = m_pNdxProcessor->Find(L"TEXT", m_pParametrizedQuery, &oSearchParams, &oSearchSettings);
	m_pNdxProcessor->EndFindSession();
wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end NdxProcessor";
OutputDebugString(wsMessage.c_str());

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin формирование SearchResult из индексного";
OutputDebugString(wsMessage.c_str());
	// поиск лингвистического процессора
	ISearchResult* pResult = m_pContainer->GetSearchResult(L"TEXT");
wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end формирование SearchResult из индексного";
OutputDebugString(wsMessage.c_str());

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" begin LinguisticProcessor UpdateRelevance";
OutputDebugString(wsMessage.c_str());
	m_pLinguisticProcessor->GetLinguisticSearch()->UpdateRelevance(pResult, &oSearchParams, &oQueryParams, NULL);
wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end LinguisticProcessor UpdateRelevance";
OutputDebugString(wsMessage.c_str());

	*ppResult = pResult;

wsMessage = L"   ---";
wsMessage += __WFUNCTION__;
wsMessage += L" end";
OutputDebugString(wsMessage.c_str());
	return true;

	SS_CATCH(L"");
	return false;
}
