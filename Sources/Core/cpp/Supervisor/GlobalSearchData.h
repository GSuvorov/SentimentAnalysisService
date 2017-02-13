///GlobalSearchData.h
#pragma once

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\ICommonContainers.h"
using namespace SS::Interface::Core::CommonContainers;

#include "..\ASCInterface\ILinguisticProcessor.h"
using namespace SS::Interface::Core;

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;

#include "..\ASSInterface\IGSPluginManager.h"
using namespace SS::Interface::GSPlugins;

#include "..\ASSInterface\ICacheModule.h"
using namespace SS::Interface::Cache;

namespace GlobalSearch
{
	///Данные для глобального поиска
	class CGlobalSearchData : public CBaseCoreClass
	{
	protected:
		ILinguisticSearch*	m_pLinguisticSearch;
		ILinguisticQuery*	m_pLinguisticQuery;
		IQueryResult*		m_pQueryResult;
		ISearchResult*		m_pSearchResult;
		IGSPluginManager*	m_pGSPluginManager;
		IGlobalSearchBase*	m_pGlobalSearchBase;

	public:
		CGlobalSearchData();
		~CGlobalSearchData(){};
		void SetLoadManager(ILoadManager* pLoadManager);
		void CreatePluginSearchData();
	};
}