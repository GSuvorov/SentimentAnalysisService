#include "stdafx.h"
#include "SupervisorModule.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CSupervisorModule::CSupervisorModule() :
		m_pIndexationEngine(NULL),
		m_pLocalSearchEngine(NULL),
		m_pGlobalSingleSearch(NULL),
		m_pGlobalMultiSearch(NULL),
		m_pBases(NULL)
{
}

CSupervisorModule::~CSupervisorModule()
{
	delete m_pLocalSearchEngine;
	delete m_pGlobalSingleSearch;
	delete m_pGlobalMultiSearch;
	delete m_pIndexationEngine;
	delete m_pBases;
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CSupervisorModule::QueryInterface(REFIID refIID, void** pBase)
{
	if(refIID == IID_Supervisor)
		*pBase = (ISupervisor*) this;
	else if(refIID == IID_Base)
		*pBase = (IBase*) this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CSupervisorModule::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// CSupervisorModule interface

IDatabaseList* CSupervisorModule::GetBasesList()
{
	SS_TRY;

	if(!m_pBases)
	{
		m_pBases = new CBasesList;
		m_pBases->SetLoadManager(GetLoadManager());
		m_pBases->FillDatabaseList();
	}
	return m_pBases;

	SS_CATCH(L"");
	return NULL;
}

IPluginList* CSupervisorModule::GetPluginList()
{
	return NULL;
}

ILocalSearchEngine* CSupervisorModule::GetLocalSearchEngine()
{
	SS_TRY;

	if(!m_pLocalSearchEngine)
	{
		m_pLocalSearchEngine = new CSearchEngine;
		m_pLocalSearchEngine->SetLoadManager(GetLoadManager());
	}
	return m_pLocalSearchEngine;

	SS_CATCH(L"");
	return NULL;
}

IGlobalSingleSearch* CSupervisorModule::GetGlobalSingleSearch()
{
	SS_TRY
	{
		if (!m_pGlobalSingleSearch) 
		{
			m_pGlobalSingleSearch = new CGlobalSingleSearch;
			m_pGlobalSingleSearch->SetLoadManager(GetLoadManager());
		}
		return m_pGlobalSingleSearch;
	}
	SS_CATCH(L"")
	return NULL;
}

IGlobalMultiSearch* CSupervisorModule::GetGlobalMultiSearch()
{
	SS_TRY
	{
		if (!m_pGlobalMultiSearch) 
		{
			m_pGlobalMultiSearch = new CGlobalMultiSearch;
			m_pGlobalMultiSearch->SetLoadManager(GetLoadManager());
		}
		return m_pGlobalMultiSearch;
	}
	SS_CATCH(L"")
	return NULL;
}

IIndexationEngine* CSupervisorModule::GetIndexationEngine()
{
	SS_TRY;

	if(!m_pIndexationEngine)
	{
		m_pIndexationEngine = new CIndexationEngine;
		m_pIndexationEngine->SetLoadManager(GetLoadManager());
	}
	return m_pIndexationEngine;

	SS_CATCH(L"");
	return NULL;
}
