#pragma once

#include "../ASCInterface/ILinguisticProcessor.h"

#include ".\linguisticindexation.h"
#include ".\linguisticquery.h"
#include ".\linguisticsearch.h"
#include ".\linguisticshowresults.h"

namespace SS
{
namespace LinguisticProcessor
{

	class CLinguisticProcessor : public SS::Interface::Core::ILinguisticProcessor
	{
		SS::LinguisticProcessor::Indexation::CLinguisticIndexation*	m_pLinguisticIndexation;
		SS::LinguisticProcessor::CLinguisticQuery*		m_pLinguisticQuery;
		SS::LinguisticProcessor::Search::CLinguisticSearch*		m_pLinguisticSearch;
		SS::LinguisticProcessor::CLinguisticShowResults*	m_pLinguisticShowResults;

		SS::Core::Types::IndexationSearch::TLinguisticProcessorMode m_oMode;
	public:
		CLinguisticProcessor(void);
		virtual ~CLinguisticProcessor(void);
	public: // from IBase
		///освобождение
		ULONG Release();
		///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
		HRESULT QueryInterface(REFIID pIID, void** pBase); 
	public:// from ILinguisticProcessor
		/// возвращает интерфейс для выполнения лингвистической индексации
		SS::Interface::Core::ILinguisticIndexation* GetLinguisticIndexation();
		/// возвращает интерфейс для выполнения лингвистического анализа запроса пользователя
		SS::Interface::Core::ILinguisticQuery* GetLinguisticQuery();
		/// возвращает интерфейс для выполнения лингвистического поиска
		SS::Interface::Core::ILinguisticSearch* GetLinguisticSearch();
		/// возвращает интерфейс для отображения результатов поиска
		SS::Interface::Core::ILinguisticShowResults* GetLinguisticShowResults();
		/// служит для выставления режима работы лингвистического процессора
	public:
		void SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode)
		{
			m_oMode = *pMode;
			if(m_pLinguisticIndexation)
				m_pLinguisticIndexation->SetMode(&m_oMode);
			if(m_pLinguisticQuery)
				m_pLinguisticQuery->SetMode(&m_oMode);
			if(m_pLinguisticSearch)
				m_pLinguisticSearch->SetMode(&m_oMode);
			if(m_pLinguisticShowResults)
				m_pLinguisticShowResults->SetMode(&m_oMode);
		}
	};
}
}