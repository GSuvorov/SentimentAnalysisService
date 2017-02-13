#include "StdAfx.h"
#include ".\linguisticprocessor.h"



SS::LinguisticProcessor::CLinguisticProcessor::CLinguisticProcessor(void) : 
	m_pLinguisticIndexation(NULL), 
	m_pLinguisticQuery(NULL), 
	m_pLinguisticSearch(NULL), 
	m_pLinguisticShowResults(NULL)
{
}

SS::LinguisticProcessor::CLinguisticProcessor::~CLinguisticProcessor(void)
{
}

SS::Interface::Core::ILinguisticIndexation* SS::LinguisticProcessor::CLinguisticProcessor::GetLinguisticIndexation()
{
	SS_TRY
	{
//		SL_OUT_DEBUG_STR (__WFUNCTION__);

		if(!m_pLinguisticIndexation)
			m_pLinguisticIndexation = new SS::LinguisticProcessor::Indexation::CLinguisticIndexation();
		m_pLinguisticIndexation->SetLoadManager(GetLoadManager());
		m_pLinguisticIndexation->SetMode(&m_oMode);		

		return (SS::Interface::Core::ILinguisticIndexation*) m_pLinguisticIndexation;
	}
	SS_CATCH(L"")
}
SS::Interface::Core::ILinguisticQuery* SS::LinguisticProcessor::CLinguisticProcessor::GetLinguisticQuery()
{
	SS_TRY
	{
//		SL_OUT_DEBUG_STR (__WFUNCTION__);

		if(!m_pLinguisticQuery)
			m_pLinguisticQuery = new SS::LinguisticProcessor::CLinguisticQuery;
		m_pLinguisticQuery->SetLoadManager(GetLoadManager());
		m_pLinguisticQuery->SetMode(&m_oMode);		

		return (SS::Interface::Core::ILinguisticQuery*)m_pLinguisticQuery;
	}
	SS_CATCH(L"")
}
SS::Interface::Core::ILinguisticSearch* SS::LinguisticProcessor::CLinguisticProcessor::GetLinguisticSearch()
{
	SS_TRY
	{
//		SL_OUT_DEBUG_STR (__WFUNCTION__);

		if(!m_pLinguisticSearch)
			m_pLinguisticSearch = new SS::LinguisticProcessor::Search::CLinguisticSearch;

		m_pLinguisticSearch->SetLoadManager(GetLoadManager());
		m_pLinguisticSearch->SetMode(&m_oMode);		

		if(  m_pLinguisticQuery->GetLastQuery())
		{
			m_pLinguisticSearch->SetLastQuery(m_pLinguisticQuery->GetLastQuery());
		}
		return (SS::Interface::Core::ILinguisticSearch*)m_pLinguisticSearch;
	}
	SS_CATCH(L"")
}
SS::Interface::Core::ILinguisticShowResults* SS::LinguisticProcessor::CLinguisticProcessor::GetLinguisticShowResults()
{
	SS_TRY
	{
//		SL_OUT_DEBUG_STR (__WFUNCTION__);

		if(!m_pLinguisticShowResults)
			m_pLinguisticShowResults = new SS::LinguisticProcessor::CLinguisticShowResults;
		m_pLinguisticShowResults->SetLoadManager(GetLoadManager());
		m_pLinguisticShowResults->SetMode(&m_oMode);		

		return (SS::Interface::Core::ILinguisticShowResults*)m_pLinguisticShowResults;
	}
	SS_CATCH(L"")
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ULONG SS::LinguisticProcessor::CLinguisticProcessor::Release()
{
	delete this;
	return NULL;
}

HRESULT SS::LinguisticProcessor::CLinguisticProcessor::QueryInterface(REFIID pIID, void** pBase)
{
	if(	IID_LinguisticProcessor == pIID)
		*pBase = (SS::Interface::Core::ILinguisticProcessor*) this;
	else if(IID_Base == pIID)
		*pBase = (SS::Interface::IBase*) this;
	else 
		*pBase = NULL;

	return S_OK;
} 

