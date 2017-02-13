///GlobalSearchData.cpp
#include "stdafx.h"
#include ".\globalsearchdata.h"
#include "..\ASCInterface\ICommonServices.h"
using namespace SS::Interface::Core::CommonServices;

namespace GlobalSearch
{
	CGlobalSearchData::CGlobalSearchData()
		:
		m_pLinguisticSearch(NULL),
		m_pLinguisticQuery(NULL),
		m_pQueryResult(NULL),
		m_pSearchResult(NULL),
		m_pGSPluginManager(NULL),
		m_pGlobalSearchBase(NULL)
	{
	}

	void CGlobalSearchData::SetLoadManager(ILoadManager* pLoadManager)
	{
		SS_TRY
		{
			if (GetLoadManager()) return;
			CBaseCoreClass::SetLoadManager(pLoadManager);

			ISettingsServer* pSettings = (ISettingsServer*) 
			GetLoadManager()->GetInterface(L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);
			bool bMorpho	= pSettings->GetBoolean(L"Linguistic::UseMorpho") == 0 ? false : true;
			bool bSemantic	= pSettings->GetBoolean(L"Linguistic::UseSemantic") == 0 ? false : true;
			bool bSynonims	= pSettings->GetBoolean(L"Linguistic::UseSynonims") == 0 ? false : true;
			bool bSyntax	= pSettings->GetBoolean(L"Linguistic::UseSyntax") == 0 ? false : true;

			TLinguisticProcessorMode oLinguisticMode;
			oLinguisticMode.GetAnalyseDepthParams()->SetIsMorpho(bMorpho);
			oLinguisticMode.GetAnalyseDepthParams()->SetIsSemantic(bSemantic);
			oLinguisticMode.GetAnalyseDepthParams()->SetIsSynonims(bSynonims);
			oLinguisticMode.GetAnalyseDepthParams()->SetIsSyntax(bSyntax);

			ILinguisticProcessor* pLinguisticProcessor = (ILinguisticProcessor*)
			GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, IID_LinguisticProcessor); 
			pLinguisticProcessor->SetMode(&oLinguisticMode);
			m_pLinguisticQuery = pLinguisticProcessor->GetLinguisticQuery();
			m_pLinguisticSearch = pLinguisticProcessor->GetLinguisticSearch();

			m_pQueryResult = (IQueryResult*)
			GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_QueryResult, IID_QueryResult);
			m_pSearchResult = (ISearchResult*)
			GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_SearchResult, IID_SearchResult);
		}
		SS_CATCH(L"")
	}

	void CGlobalSearchData::CreatePluginSearchData()
	{
		SS_TRY
		{
			if (m_pGSPluginManager == NULL)
			{
				m_pGSPluginManager = (IGSPluginManager*)
				GetLoadManager()->GetInterface(L"./GSPlugins/GSPluginManager.dll", CLSID_GSPluginManager, IID_GSPluginManager);
			}
			if (m_pGlobalSearchBase == NULL)
			{
				m_pGlobalSearchBase = (IGlobalSearchBase*)
				GetLoadManager()->GetInterface(L"./Core/CacheModule.dll", CLSID_GlobalSearchBase, IID_GlobalSearchBase);
			}
		}
		SS_CATCH(L"")
	}
}
