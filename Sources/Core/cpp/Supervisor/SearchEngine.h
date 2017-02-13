#pragma once

#include "..\ASDInterface\INdxSearchEngine.h"
#include "..\ASDInterface\INdxQuery.h"
#include "..\ASDInterface\TNdxSearchEngineTypes.h"
using namespace SS::Interface::Core::NdxSE;

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\ICommonContainers.h"
using namespace SS::Interface::Core::CommonContainers;

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include "..\ASSInterface\ISupervisor.h"
using namespace SS::Interface::Supervisor;

#include "ISContainerLite.h"
#include "BasesList.h"


class CSearchEngine : 
		public CBaseCoreClass,
		public ILocalSearchEngine
{
public:
	CSearchEngine();
	virtual ~CSearchEngine();

public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	///локальный поиск по выбранной базе
	virtual bool Search(LPCWSTR szQuery, ISearchResult** ppSearchResult);
	///загрузка системы и кеширование файлов
	virtual bool Load(LPCWSTR szQuery);

private:
	INdxSearchEngineFind* m_pNdxProcessor;
	IParametrizedQuery* m_pParametrizedQuery;
	ILinguisticProcessor* m_pLinguisticProcessor;
	IQueryResult* m_pQueryResult;
	CISContainerLite* m_pContainer;
};
