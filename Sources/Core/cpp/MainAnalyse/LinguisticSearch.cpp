#include "StdAfx.h"
#include ".\linguisticsearch.h"

#include "..\ASSInterface\IQueryResultEx.h"
#include "..\ASSInterface\ITextBlockEx.h"
#include "..\ASSInterface\Constants.h"
#include "..\ASCInterface\ICommonServices.h"

using namespace SS::Interface::Core::CommonContainers;

namespace SS
{
namespace LinguisticProcessor
{
namespace Search
{

CLinguisticSearch::CLinguisticSearch(void)
{
}


CLinguisticSearch::~CLinguisticSearch(void)
{
}


void CLinguisticSearch::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY
	{
		SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);

		if(!GetAnalyseParams())
		{
			SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams = NULL;
			pAnalyseParams = 
				(SS::Interface::Core::MainAnalyse::IAnalyseParams*)
				GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams);

			SetAnalyseParams(pAnalyseParams);
		}

		m_oGlobalSearch.SetLoadManager(pLoadManager);
		m_oLocalSearch.SetLoadManager(pLoadManager);
	}
	SS_CATCH(L"")
}

void CLinguisticSearch::SetLastQuery(SS::Interface::Core::BlackBox::Query::IQuery* pQuery)
{
	SS_TRY
	{
		m_oGlobalSearch.SetLastQuery(pQuery);
		m_oLocalSearch.SetLastQuery(pQuery);
	}
	SS_CATCH(L"")
}
void CLinguisticSearch::ApplySearchParams(	
	SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
	, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
	, SS::Core::Types::IndexationSearch::TUpdateRelevanceParams* pUpdateRelevanceParams
	)
{
	SS_TRY
	{
		try
		{
			SS::Interface::Core::CommonServices::ISettingsServer* pSettingsServer =
				(SS::Interface::Core::CommonServices::ISettingsServer*)
				GetLoadManager()->GetInterface(L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);

			unsigned int uiUseWordsWeightKoef=pSettingsServer->GetUInt(L"LinguisticProcessor::UseWordsWeightKoef");
			GetAnalyseParams()->SetRelevanceParams((SS::Core::Types::ERelevanceParams)uiUseWordsWeightKoef);
		}
		catch(SS::Interface::Core::CommonServices::IException * pException)
		{
			SAVE_LOG(SS_ERROR AND pException->ToString().c_str() AND __WFUNCTION__);
		}
		catch(...)
		{
			SAVE_LOG(SS_ERROR AND L"Ошибка при обращении к файлу настроек" AND __WFUNCTION__ )
		}

		if(!GetAnalyseParams())
		{
			SS_THROW(L"Не вызван SetLoadManager, или ошибка при создании AnalyseParams");
		}

        
		/* высталение параметров отвечающих за формирование индексов при анализе запроса*/
		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsMorpho	(true/*m_oMode.GetAnalyseDepthParams()->IsMorpho()*/);
		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSemantic	(true/*m_oMode.GetAnalyseDepthParams()->IsSemantic()*/);
		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSyntax	(true/*m_oMode.GetAnalyseDepthParams()->IsSyntax()*/);
		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims	(false/*m_oMode.GetAnalyseDepthParams()->IsSynonims()*/);

		GetAnalyseParams()->m_bIsLiteMode = m_oMode.m_bIsLiteMode;
		GetAnalyseParams()->SetAnalyseMod(SS::Core::Types::eamSearch);
		GetAnalyseParams()->SetAnalyseResultEx(pSearchParams->m_SearchArea);

		if(SS::Core::Types::IndexationSearch::gqmString == pQueryParams->m_GenerateQueryMode)
		{
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims(false);
			GetAnalyseParams()->SetCurrentSystemMode(SS::Core::Types::ecsmGlobal);			
		}else if(SS::Core::Types::IndexationSearch::gqmIndex == pQueryParams->m_GenerateQueryMode)
		{
			GetAnalyseParams()->SetCurrentSystemMode(SS::Core::Types::ecsmLocal);
		}

		/* уточнение ранее выставленных параметров */
		if(pQueryParams->m_SearchDepth == SS::Core::Types::IndexationSearch::sdNormal)
		{
		}else if(pQueryParams->m_SearchDepth == SS::Core::Types::IndexationSearch::sdStrict)
		{
			SS_THROW(L"Неверный парамтр - pQueryParams->m_SearchDepth == SS::Core::Types::IndexationSearch::sdStrict");
		}else if(pQueryParams->m_SearchDepth == SS::Core::Types::IndexationSearch::sdSynonyms)
		{
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims(true);
		}else if(pQueryParams->m_SearchDepth == SS::Core::Types::IndexationSearch::sdRejectUnimportant)
		{
		}

		return;

	}
	SS_CATCH(L"")
}

void CLinguisticSearch::UpdateRelevance(
	SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
	,SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
	, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
	, SS::Core::Types::IndexationSearch::TUpdateRelevanceParams* pUpdateRelevanceParams
	)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);
	
		//wchar_t wzTo[20];
		//SAVE_LOG(SS_MESSAGE AND _itow(pSearchResult->Count(), wzTo, 10) AND __WFUNCTION__);

		ApplySearchParams(	pSearchParams, pQueryParams, pUpdateRelevanceParams);

		if(SS::Core::Types::IndexationSearch::gqmString == pQueryParams->m_GenerateQueryMode)
		{
			m_oGlobalSearch.UpdateRelevance(pSearchResult, pQueryParams->m_AnswerMode);
		}else if(SS::Core::Types::IndexationSearch::gqmIndex == pQueryParams->m_GenerateQueryMode)
		{
			m_oLocalSearch.UpdateRelevance(pSearchResult);
		}

		SL_OUT_DEBUG_STR (__WFUNCTION__);
	}
	SS_CATCH((wchar_t*)pSearchResult->GetFirstTextBlock()->GetFirstBlockCut()->GetTextCut())
}
}
}
}



