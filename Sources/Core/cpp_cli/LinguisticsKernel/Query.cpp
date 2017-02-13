#include "StdAfx.h"
#include ".\query.h"
#using <mscorlib.dll>

#include "..\..\cpp\ASSInterface\ITextBlockEx.h"
#include "..\..\cpp\ASSInterface\IQueryResultEx.h"

#include "..\..\cpp\ASCInterface\defines.h"

#include "..\..\cpp\ASSInterface\Iterators.h"

using namespace SS::Iterators;


SS::LinguisticProcessor::CQuery::CQuery(void)
{
}

SS::LinguisticProcessor::CQuery::~CQuery(void)
{
}

SS::Interface::Core::BlackBox::IText* SS::LinguisticProcessor::CQuery::GenerateQueryIText(
	SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
	, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
	)
{
	SS_TRY
	{
		if(SS::Core::Types::IndexationSearch::gqmIndex == pQueryParams->m_GenerateQueryMode)
		{
			SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
			m_pText->ClearText();
			/* высталение параметров отвечающих за формирование индексов при анализе запроса*/
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsMorpho	(true/*m_oMode.GetAnalyseDepthParams()->IsMorpho()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSemantic	(true/*m_oMode.GetAnalyseDepthParams()->IsSemantic()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSyntax	(true/*m_oMode.GetAnalyseDepthParams()->IsSyntax()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims	(false/*m_oMode.GetAnalyseDepthParams()->IsSynonims()*/	);

			GetAnalyseParams()->SetAnalyseMod(SS::Core::Types::eamIndexation);
			GetAnalyseParams()->SetCurrentSystemMode(SS::Core::Types::ecsmLocal);

			m_pLexica->SetAnalyseParams(GetAnalyseParams());
			oNativeText.SetText(pQueryParams->GetQuery());
			m_pLexica->SetText(&oNativeText);
			if(m_pLexica->LoadNext())
			{
				m_pLexica->AnalyzeNext(m_pText, 0);
			}
		}		

		//SS::Core::Types::IndexationSearch::TAbsentWords oAbsentWords;
		//CLinguisticQuery::GenerateQuery(pSearchParams, pQueryParams, &oAbsentWords);


		SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult = 
			(SS::Interface::Core::CommonContainers::IQueryResult*)
			GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_QueryResult, IID_QueryResult);

		CLinguisticQuery::GenerateQuery(pSearchParams, pQueryParams, pQueryResult, true);

		return (m_pText);


		//ApplyQueryParams(pSearchParams, pQueryParams);
		///*query analyse*/
		//GetAnalyseParams()->SetAnalyseMod(SS::Core::Types::eamQuery);

		//m_pLexica->SetAnalyseParams(GetAnalyseParams());
		//m_pSyntaxAnalyzer->SetAnalyseParams(GetAnalyseParams());
		//m_pSemanticAnalyze->SetAnalyseParams(GetAnalyseParams());
		//m_pRelevanceEvaluation->SetAnalyseParams(GetAnalyseParams());


		//oNativeText.SetText(pQueryParams->GetQuery());
		//m_pLexica->SetText(&oNativeText);

		//if(m_pLexica->LoadNext())
		//{
		//	m_pLexica->AnalyzeNext(m_pText);

		//	FillAbsentWords(&oAbsentWords);
		//	m_pSemanticAnalyze->AnalyseNext(m_pText);
		//	// TODO изменить
		//	if(
		//		(SS::Core::Types::ealEnglish == m_pText->GetFirstParagraph()->GetFirstSentence()->GetLanguage()) ||
		//		(SS::Core::Types::ealUndefined == m_pText->GetFirstParagraph()->GetFirstSentence()->GetLanguage())
		//		)
		//	{
		//		m_pSyntaxAnalyzer->AnalyzeNext( m_pText);
		//		m_pSemanticAnalyze->SyntaxQuestionPostProcess(m_pText);
		//	}
		//	m_pLexica->FillSynonyms(m_pText);
		//}		

		//AddOneRootWords();

  //  	return m_pText;


	}
	SS_CATCH(L"")

	return (NULL);
}

SS::Interface::Core::BlackBox::Query::IQuery* SS::LinguisticProcessor::CQuery::GenerateQueryIQuery(
	SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
	, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
	)
{
	SS_TRY
	{
		if (!GenerateQueryIText(pSearchParams, pQueryParams)) 
		{
			return (NULL);
		}

		if(m_pRelevanceEvaluation->EvaluateQuery(m_pText, m_pQuery) == E_FAIL)
		{
			return (NULL);
		}
		else
		{
			SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult = 
				(SS::Interface::Core::CommonContainers::IQueryResult*)
				GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_QueryResult, IID_QueryResult);

			SS::Interface::Core::CommonContainers::IQueryResultEx* pQueryResultEx = 
				dynamic_cast<SS::Interface::Core::CommonContainers::IQueryResultEx*>(pQueryResult);

			pQueryResultEx->SetLinguisticProcessorMode(&m_oMode);

#ifdef _SS_UNITTESTS
			SAVE_LOG(SS_MESSAGE AND L"Копирование в из IQuery в результаты разбора запроса" AND __WFUNCTION__);
#endif
			pQueryResultEx->SetIQuery(m_pQuery);

			return (m_pQuery);
		}

	}
	SS_CATCH(L"")

	return (NULL);
}
