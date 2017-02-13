#pragma once

#include "..\..\cpp\ASCInterface\IInterface.h"
#include "..\..\cpp\ASCInterface\ICommoncontainers.h"
#include "..\..\cpp\ASSInterface\ICommoncontainersEx.h"
#include "..\..\cpp\ASCInterface\ILinguisticProcessor.h"

namespace SS
{
	namespace LinguisticProcessor
	{

using namespace SS::Interface::Core::CommonContainers;

public ref class CSearch
{

	SS::Interface::Core::ResourceManagers::ILoadManager*			m_pLoadManager;
	SS::Core::Types::IndexationSearch::TLinguisticProcessorMode*	m_pMode;

	SS::Interface::Core::ILinguisticIndexation*					m_pLinguisticIndexation;
	SS::Interface::Core::ILinguisticQuery*						m_pLinguisticQuery;
	SS::Interface::Core::ILinguisticSearch*						m_pLinguisticSearch;
	SS::Interface::Core::ILinguisticShowResults*					m_pLinguisticShowResults;

public:
	CSearch(void);
	~CSearch(void);

private:
	void GenerateQuery( SS::Core::Types::IndexationSearch::TSearchParams& oSearchParams
							, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
							, System::String^ sQuery);

	void BuildTitles(System::String^ sTextTitle, SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult, SS::Core::Types::IndexationSearch::TIndexationParams& oIndexationParams);

	void RunLocalSearch( 
		System::String^ sQuery
		, System::String^ sText
		, System::String^ sTextTitle
		, SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
		, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams);

	void RunGlobalSearch(
		System::String^ sQuery
		, System::String^ sText
		, System::String^ sTextTitle
		, SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
		, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams);

	void Init()
	{
		SS::Interface::Core::ILinguisticProcessor* pLinguisticProcessor = 
			(SS::Interface::Core::ILinguisticProcessor*)
			m_pLoadManager->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, IID_LinguisticProcessor);

		m_pLinguisticQuery		= pLinguisticProcessor->GetLinguisticQuery();
		m_pLinguisticIndexation = pLinguisticProcessor->GetLinguisticIndexation();
		m_pLinguisticSearch		= pLinguisticProcessor->GetLinguisticSearch();
		m_pLinguisticShowResults= pLinguisticProcessor->GetLinguisticShowResults();

	}

	void GenerateIndexationResult(System::String^ sText
								, SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
								, SS::Core::Types::IndexationSearch::TIndexationParams& oIndexationParams);

public:
	void UpdateRelevance
		( System::String^ sQuery
		, System::String^ sText
		, System::String^ sTextTitle				
		, bool bIsGlobalSearch
		, SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
		, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
		) ;

public:
	virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		m_pLoadManager = pLoadManager;
		Init();
	};
	void SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode)
	{
		m_pMode = pMode;
	}

	
};

	}
}