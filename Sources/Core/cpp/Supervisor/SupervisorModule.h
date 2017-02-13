#pragma once

#include "..\ASSInterface\ISupervisor.h"
using namespace SS::Interface::Supervisor;
#include "SearchEngine.h"
#include "IndexationEngine.h"

#include "GlobalSingleSearch.h"
#include "GlobalMultiSearch.h"
using namespace GlobalSearch;

class CSupervisorModule : public ISupervisor
{
public:
	CSupervisorModule();
	virtual ~CSupervisorModule();

// IBase
public:
	virtual HRESULT QueryInterface(REFIID refIID, void** pBase);
	virtual ULONG Release();

// ISupervisor
public:
	virtual IDatabaseList* GetBasesList();
	virtual IPluginList* GetPluginList();
	virtual ILocalSearchEngine* GetLocalSearchEngine();
	virtual IGlobalSingleSearch* GetGlobalSingleSearch();
	virtual IGlobalMultiSearch* GetGlobalMultiSearch();
	virtual IIndexationEngine* GetIndexationEngine();

private:
	CSearchEngine* m_pLocalSearchEngine;
	CGlobalSingleSearch* m_pGlobalSingleSearch;
	CGlobalMultiSearch* m_pGlobalMultiSearch;
	CIndexationEngine* m_pIndexationEngine;
	CBasesList* m_pBases;
};
