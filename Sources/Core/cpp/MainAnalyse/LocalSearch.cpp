#include "StdAfx.h"
#include ".\localsearch.h"

namespace SS
{
namespace LinguisticProcessor
{
namespace Search
{

CLocalSearch::CLocalSearch(void) : m_AnswerMode(SS::Core::Types::IndexationSearch::EAnswerMode::amAuto)
{
}

CLocalSearch::~CLocalSearch(void)
{
}

void CLocalSearch::ClearTexts()
{
	SS_TRY
	{
		for(std::list<SS::Interface::Core::BlackBox::IText*>::iterator iter = m_l_IText.begin(); iter != m_l_IText.end(); iter++)
		{
			(*iter)->ReleaseText();
			(*iter) = NULL;
		}
		m_l_IText.clear();
	}
	SS_CATCH(L"")
}

void CLocalSearch::SetLastQuery(SS::Interface::Core::BlackBox::Query::IQuery* pQuery)
{
	SS_TRY
	{
		m_pQuery = pQuery;
	}
	SS_CATCH(L"")
}

void CLocalSearch::UpdateRelevance(ISearchResult* pSearchResult)
{
	SS_TRY;

	m_pLinguisticRelevance->SetQuery(m_pQuery);

	ClearTexts();

	if(SS::Core::Types::IndexationSearch::amOnlyKeywords == m_AnswerMode)
	{
		return;
	}

	for(ITextBlock* pTextBlock = pSearchResult->GetFirstTextBlock(); pTextBlock; pTextBlock = pSearchResult->GetNextTextBlock())
	{
		ITextBlockEx* pTextBlockEx = dynamic_cast<ITextBlockEx*>(pTextBlock);
		IText* pText = pTextBlockEx->GenerateIText();

		m_l_IText.push_back(pText);

//////////////////////////////////////////////////////////////////////////////////////////////////////		
		m_pLexica->SetAnalyseParams(GetAnalyseParams());
		m_pSemanticAnalyze->SetAnalyseParams(GetAnalyseParams());
		m_pLinguisticRelevance->SetAnalyseParams(GetAnalyseParams());

		m_pLexica->AnalyzeNext(pText, 0);

#ifdef ANAFORA
		using namespace SS::Interface::Core::BlackBox;
		for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
			for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
				pSent->SetRelevance(SS::Constants::RELEVANCE_MULTIPLIER);
		m_pSemanticAnalyze->AnalyseNext(pText);
		m_pLexica->FillSynonyms(pText);
		pText->Normalize();
		m_pLinguisticRelevance->UpdateRelevance(pText, NULL);
#else
		if(m_pLinguisticRelevance->IsExistAnswerWord(pText))
		{
			m_pSemanticAnalyze->AnalyseNext(pText);
			m_pLexica->FillSynonyms(pText);
			pText->Normalize();
			m_pLinguisticRelevance->UpdateRelevance(pText, NULL);
		}
		//m_pLinguisticRelevance->HighlightKeywords(pText);
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////

		pTextBlockEx->Clear();
		pTextBlockEx->SetIText(pText);
		pTextBlockEx->UpdateRelevance();

	}
	SS_CATCH(L"");
}

}
}
}