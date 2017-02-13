#include "StdAfx.h"
#include ".\search.h"
#using <mscorlib.dll>
#include <vcclr.h>


namespace SS
{
	namespace LinguisticProcessor
	{

CSearch::CSearch(void) : m_pLoadManager(NULL), m_pMode(NULL), m_pLinguisticIndexation(NULL), m_pLinguisticQuery(NULL),
						 m_pLinguisticSearch(NULL), m_pLinguisticShowResults(NULL)
{
}

CSearch::~CSearch(void)
{
}

void CSearch::UpdateRelevance( System::String^ sQuery
							  , System::String^ sText
							  , System::String^ sTextTitle
							  , bool bIsGlobalSearch
							  , SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
							  , SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
							  )
{
	SS_TRY
	{

		if(bIsGlobalSearch)
		{
			RunGlobalSearch(sQuery, sText, sTextTitle, pSearchResult, pQueryParams);
		}else
		{
			RunLocalSearch(sQuery, sText, sTextTitle, pSearchResult, pQueryParams);
		}

		pSearchResult->SetSortedMode(true);

		SS::Core::Types::IndexationSearch::TPrepareForViewParams oPrepareForViewParams;
		oPrepareForViewParams.m_uiFirstProcessedTextBlockNumber = 0;
		oPrepareForViewParams.m_uiProcessedTextBlockCount = 1000;
		m_pLinguisticShowResults->PrepareForView(pSearchResult, &oPrepareForViewParams);


	}
	SS_CATCH(L"")
}


void CSearch::GenerateQuery( SS::Core::Types::IndexationSearch::TSearchParams& oSearchParams
							, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
							, System::String^ sQuery)
{
	SS_TRY
	{
/*	выполн€ем анализ запроса	*/
		pin_ptr< const wchar_t > pwQuery = PtrToStringChars(sQuery);
		pQueryParams->SetQuery(pwQuery);

		SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult = 
			(SS::Interface::Core::CommonContainers::IQueryResult*)
			m_pLoadManager->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_QueryResult, IID_QueryResult);
		pQueryResult->Clear();

		m_pLinguisticQuery->GenerateQuery(&oSearchParams, pQueryParams, pQueryResult, true);

	}
	SS_CATCH(L"")
}

void CSearch::BuildTitles(System::String^ sTextTitle, SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult, SS::Core::Types::IndexationSearch::TIndexationParams& oIndexationParams)
{
	SS_TRY
	{
		pin_ptr< const wchar_t > pwTextTitle = PtrToStringChars(sTextTitle);
		SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeTextTitle;
		oNativeTextTitle.SetText(pwTextTitle);

		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResultTitle = NULL;
		m_pLoadManager->Create(L"./core/linguistics/CommonContainers.dll", CLSID_IndexationResult, IID_IndexationResult, (void**)&pIndexationResultTitle);

		m_pLinguisticIndexation->Init(&oNativeTextTitle, &oIndexationParams);
		m_pLinguisticIndexation->GenerateNext(pIndexationResultTitle);
		
		if(pIndexationResultTitle->GetFirstTextBlock())
		{
			ITextBlock * pTextBlockTitle = pSearchResult->AddTextBlock();

			for(ITextBlockCut* pTextBlockCutIndexation	= pIndexationResultTitle->GetFirstTextBlock()->GetFirstBlockCut();
				pTextBlockCutIndexation; pTextBlockCutIndexation = pIndexationResultTitle->GetFirstTextBlock()->GetNextBlockCut())
			{
				ITextBlockCut* pTextBlockCutSearch		= pTextBlockTitle->AddBlockCut();
				pTextBlockCutSearch->SetType(SS::Core::Types::IndexationSearch::tctHeader);
				
				for(IIndexAndPosition*  pIndexAndPositionIndexation = pTextBlockCutIndexation->GetFirstIndexAndPosition();
					pIndexAndPositionIndexation; pIndexAndPositionIndexation = pTextBlockCutIndexation->GetNextIndexAndPosition())
				{
					IIndexAndPosition*  pIndexAndPositionSearch = pTextBlockCutSearch->AddIndexAndPosition();
					pIndexAndPositionSearch->SetFirstCharPosition( pIndexAndPositionIndexation->GetFirstCharPosition());

					char * buff = new char;
					unsigned int iBuffSize;
					pIndexAndPositionIndexation->GetServiceIndex()->GetInfoBuff( (char**)&buff, &iBuffSize);
					pIndexAndPositionSearch->GetServiceIndex()->SetInfoBuff(buff, iBuffSize);

					pIndexAndPositionSearch->SetPosition(pIndexAndPositionIndexation->GetPosition());					
					pIndexAndPositionSearch->SetFirstCharPosition(pIndexAndPositionIndexation->GetFirstCharPosition());
				}				
			}
		}
	}
	SS_CATCH(L"")
}


void CSearch::GenerateIndexationResult(
									   System::String^ sText
									   , SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
									   , SS::Core::Types::IndexationSearch::TIndexationParams& oIndexationParams)
{
	SS_TRY
	{
/*	выполн€ем подготовку дл€ поиска. ¬ыполн€ем индексацию, с целью разбиени€ на параграфы	*/
		pin_ptr< const wchar_t > pwText = PtrToStringChars(sText);

		SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
		oNativeText.SetText(pwText);

		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult = 
			(SS::Interface::Core::CommonContainers::IIndexationResult*)
			m_pLoadManager->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_IndexationResult, IID_IndexationResult);

		pIndexationResult->Clear();

		m_pLinguisticIndexation->Init( &oNativeText, &oIndexationParams);

		if(m_pLinguisticIndexation->GenerateNext(pIndexationResult) == 0)
		{
			int i = 0 ;
		}	
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		using namespace SS::Interface::Core::CommonContainers;		

		for(ITextBlock* pTextBlockIndexation	= pIndexationResult->GetFirstTextBlock(); 
			pTextBlockIndexation; pTextBlockIndexation = pIndexationResult->GetNextTextBlock())
		{
			ITextBlock* pTextBlockSearch = pSearchResult->GetFirstTextBlock();
			if(!pTextBlockSearch)
				pSearchResult->AddTextBlock();

			for(ITextBlockCut* pTextBlockCutIndexation	= pTextBlockIndexation->GetFirstBlockCut();
				pTextBlockCutIndexation; pTextBlockCutIndexation = pTextBlockIndexation->GetNextBlockCut())
			{
				ITextBlockCut* pTextBlockCutSearch		= pTextBlockSearch->AddBlockCut();
				pTextBlockCutSearch->SetType(SS::Core::Types::IndexationSearch::tctText);

				for(IIndexAndPosition*  pIndexAndPositionIndexation = pTextBlockCutIndexation->GetFirstIndexAndPosition();
					pIndexAndPositionIndexation; pIndexAndPositionIndexation = pTextBlockCutIndexation->GetNextIndexAndPosition())
				{
					IIndexAndPosition*  pIndexAndPositionSearch = pTextBlockCutSearch->AddIndexAndPosition();
					pIndexAndPositionSearch->SetFirstCharPosition( pIndexAndPositionIndexation->GetFirstCharPosition());
					pIndexAndPositionSearch->SetPosition(pIndexAndPositionIndexation->GetPosition());					

					char * buff = new char;
					unsigned int iBuffSize;
					pIndexAndPositionIndexation->GetServiceIndex()->GetInfoBuff( (char**)&buff, &iBuffSize);
					pIndexAndPositionSearch->GetServiceIndex()->SetInfoBuff(buff, iBuffSize);

				}				
			}			
		}


	}
	SS_CATCH(L"")
}

void CSearch::RunGlobalSearch(System::String^ sQuery
					, System::String^ sText
					, System::String^ sTextTitle
					, SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
					, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams)
{
	SS_TRY
	{
		pSearchResult->Clear();

////////////////////////////////////////////////////////////////////////////////////////////////////
		SS::Core::Types::IndexationSearch::TIndexationParams			oIndexationParams;
		SS::Core::Types::IndexationSearch::TSearchParams				oSearchParams;
		SS::Core::Types::IndexationSearch::TUpdateRelevanceParams	oUpdateRelevanceParams;
////////////////////////////////////////////////////////////////////////////////////////////////////
/* формируем заголовки */
		BuildTitles(sTextTitle, pSearchResult, oIndexationParams);
/*	выполн€ем подготовку дл€ поиска*/
		pin_ptr< const wchar_t > pwText = PtrToStringChars(sText);
		ITextBlock* pTextBlock = pSearchResult->GetFirstTextBlock();
		if(!pTextBlock)
			pSearchResult->AddTextBlock();
		pTextBlock->AddBlockCut()->SetTextCut(pwText);
////////////////////////////////////////////////////////////////////////////////////////////////////
		
		pQueryParams->m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmString;
///*	выполн€ем анализ запроса	*/
		GenerateQuery(oSearchParams, pQueryParams, sQuery);
/*	выполн€ем поиск	*/
		m_pLinguisticSearch->UpdateRelevance(pSearchResult, &oSearchParams, pQueryParams, &oUpdateRelevanceParams);
	}
	SS_CATCH(L"")
}

void CSearch::RunLocalSearch( System::String^ sQuery
					, System::String^ sText
					, System::String^ sTextTitle
					, SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
					, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams)
{
	SS_TRY
	{
		pSearchResult->Clear();

		SS::Core::Types::IndexationSearch::TIndexationParams			oIndexationParams;
		SS::Core::Types::IndexationSearch::TSearchParams				oSearchParams;
		SS::Core::Types::IndexationSearch::TUpdateRelevanceParams	oUpdateRelevanceParams;

/* формируем заголовки */
		BuildTitles(sTextTitle, pSearchResult, oIndexationParams);
/*	выполн€ем подготовку дл€ поиска. ¬ыполн€ем индексацию, с целью разбиени€ на параграфы	*/
		GenerateIndexationResult(sText, pSearchResult, oIndexationParams);

/*	выполн€ем анализ запроса	*/
		pQueryParams->m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmIndex;
		GenerateQuery(oSearchParams, pQueryParams, sQuery);
/*	выполн€ем поиск	*/
		m_pLinguisticSearch->UpdateRelevance(pSearchResult, &oSearchParams, pQueryParams, &oUpdateRelevanceParams);

	}SS_CATCH(L"")

}


	}
}