#include "StdAfx.h"
#include ".\globalsearch.h"

namespace SS
{
namespace LinguisticProcessor
{
namespace Search
{

CGlobalSearch::CGlobalSearch(void) : m_AnswerMode(SS::Core::Types::IndexationSearch::EAnswerMode::amAuto)
{
}

CGlobalSearch::~CGlobalSearch(void)
{
}

void CGlobalSearch::ClearTexts()
{
	SS_TRY
	{

		for(std::list< std::list<STextInfo*> >::iterator II = m_l_l_ITextInfo.begin(); II != m_l_l_ITextInfo.end(); II++ )
		{
			for(std::list<STextInfo*>::iterator iter = II->begin(); iter != II->end(); iter++)
			{
				if((*iter))
				{
					(*iter)->pIText->ReleaseText();
				}
				delete (*iter);
				(*iter) = NULL;
			}			
		}
		m_l_l_ITextInfo.clear();
	}
	SS_CATCH(L"")

}

void CGlobalSearch::SetLastQuery(SS::Interface::Core::BlackBox::Query::IQuery* pQuery)
{
	SS_TRY
	{
		m_pQuery = pQuery;
	}
	SS_CATCH(L"")
}

void CGlobalSearch::RunRTFSearchOnlyKeyWords(SS::Interface::Core::BlackBox::IText* pText, const wchar_t *wsText, unsigned int uiBeginPosition)
{
	SS_TRY
	{

		pText->ClearText();
		SS::Core::Types::EAnalyseMods oRememberAnalyseMods;

		oRememberAnalyseMods = GetAnalyseParams()->GetAnalyseMod();
		GetAnalyseParams()->SetAnalyseMod(SS::Core::Types::eamIndexation);			
		
		/// результаты из индексной базы
		m_oNativeText.SetText(wsText);
		m_pLexica->SetAnalyseParams(GetAnalyseParams());
		m_pLexica->SetText(&m_oNativeText, uiBeginPosition);

		if(m_pLexica->LoadNext())
		{
			m_pLexica->AnalyzeNext(pText, 0);

			GetAnalyseParams()->SetAnalyseMod(oRememberAnalyseMods);
			m_pLinguisticRelevance->SetAnalyseParams(GetAnalyseParams());

			pText->Normalize();

			//m_pLinguisticRelevance->HighlightKeywords(pText);
		}
	}
	SS_CATCH(L"")
}

void CGlobalSearch::RunRTFSearch(SS::Interface::Core::BlackBox::IText* pText, const wchar_t *wsText, unsigned int uiBeginPosition)
{
	SS_TRY
	{

		pText->ClearText();
		SS::Core::Types::EAnalyseMods oRememberAnalyseMods;

		oRememberAnalyseMods = GetAnalyseParams()->GetAnalyseMod();
		GetAnalyseParams()->SetAnalyseMod(SS::Core::Types::eamIndexation);			
		
		/// результаты из индексной базы
		m_oNativeText.SetText(wsText);
		m_pLexica->SetAnalyseParams(GetAnalyseParams());
		m_pLexica->SetText(&m_oNativeText, uiBeginPosition);

		if(m_pLexica->LoadNext())
		{
			m_pLexica->AnalyzeNext(pText, 0);

			GetAnalyseParams()->SetAnalyseMod(oRememberAnalyseMods);
			m_pSemanticAnalyze->SetAnalyseParams(GetAnalyseParams());
			m_pLinguisticRelevance->SetAnalyseParams(GetAnalyseParams());

#ifdef ANAFORA
			using namespace SS::Interface::Core::BlackBox;
			for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
				for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
					pSent->SetRelevance(SS::Constants::RELEVANCE_MULTIPLIER);

			m_pSemanticAnalyze->AnalyseNext(pText);
//			m_pLexica->FillSynonyms(pText);
			pText->Normalize();
			m_pLinguisticRelevance->UpdateRelevance(pText, NULL);
#else
			if(m_pLinguisticRelevance->IsExistAnswerWord(pText))
			{
				m_pSemanticAnalyze->SetAnalyseParams(GetAnalyseParams());
				m_pSemanticAnalyze->AnalyseNext(pText);
//				m_pLexica->FillSynonyms(pText);
				pText->Normalize();
				m_pLinguisticRelevance->UpdateRelevance(pText, m_pTextTitle);
			}
#endif

			//m_pLinguisticRelevance->HighlightKeywords(pText);
		}
	}
	SS_CATCH(L"")
}

void CGlobalSearch::UpdateRelevanceInTextCut(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut, std::list<STextInfo*>& p_l_TextInfo)
{
	SS_TRY
	{
		unsigned int uiBeginPosition = 0;

		STextInfo* pTextInfo = new STextInfo;
		pTextInfo->pIText = m_pBlackBoxTextStorage->CreateText();
		pTextInfo->wsPlaginName = pTextBlockCut->GetPluginName();
		p_l_TextInfo.push_back(pTextInfo);

		if(m_AnswerMode == SS::Core::Types::IndexationSearch::amOnlyKeywords)
		{
			RunRTFSearchOnlyKeyWords(pTextInfo->pIText, pTextBlockCut->GetTextCut(), uiBeginPosition);
		}else
		{
			RunRTFSearch(pTextInfo->pIText, pTextBlockCut->GetTextCut(), uiBeginPosition);
		}
		
		if((pTextInfo->pIText->GetFirstParagraph()))
		{
			pTextInfo->pIText->GetFirstParagraph()->SetIndexTypeName(pTextBlockCut->GetIndexTypeName());
			pTextInfo->pIText->GetFirstParagraph()->SetRelevance( (pTextInfo->pIText->GetFirstParagraph()->GetRelevance() + pTextBlockCut->GetRelevance()));
			pTextInfo->pIText->GetFirstParagraph()->SetType(pTextBlockCut->GetType());
		}
	}
	SS_CATCH(L"")
}

void CGlobalSearch::UpdateRelevance(SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult, SS::Core::Types::IndexationSearch::EAnswerMode oAnswerMode)
{
	SS_TRY
	{
		m_AnswerMode = oAnswerMode;
		m_pLinguisticRelevance->SetQuery(m_pQuery);
		ClearTexts();

		for( SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock = pSearchResult->GetFirstTextBlock();
			 pTextBlock; pTextBlock = pSearchResult->GetNextTextBlock() ) 
		{
			try
			{
				std::list<STextInfo*> l_TextInfo;

				for ( SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut = pTextBlock->GetFirstBlockCut();
					  pTextBlockCut; pTextBlockCut = pTextBlock->GetNextBlockCut())
				{
					try
					{
						UpdateRelevanceInTextCut(pTextBlockCut, l_TextInfo);
					}
					catch (SS::Interface::Core::CommonServices::IException* pException)
					{
						SAVE_LOG(SS_ERROR AND __WFUNCTION__ AND pException->ToString().c_str());
						SAVE_LOG(SS_ERROR AND __WFUNCTION__ AND pTextBlockCut->GetTextCut());
					}
				}
				SS::Interface::Core::CommonContainers::ITextBlockEx* pTextBlockEx = 
				dynamic_cast<SS::Interface::Core::CommonContainers::ITextBlockEx*>(pTextBlock);
				pTextBlockEx->SetLinguisticProcessorMode(&m_oMode);
				pTextBlockEx->Clear();
				
				std::list<STextInfo*>::iterator iter(l_TextInfo.begin());
				for (iter; iter != l_TextInfo.end(); ++iter)
				{ pTextBlockEx->SetIText((*iter)->pIText, false); }

				iter = l_TextInfo.begin();
				for ( ITextBlockCut* pTextBlockCut = pTextBlockEx->GetFirstBlockCut();
					  (pTextBlockCut &&  (iter != l_TextInfo.end())); pTextBlockCut = pTextBlockEx->GetNextBlockCut(), ++iter)
				{
					pTextBlockCut->SetPluginName((*iter)->wsPlaginName.c_str());
				}
				pTextBlockEx->UpdateRelevance();
				m_l_l_ITextInfo.push_back(l_TextInfo);
			}
			catch(SS::Interface::Core::CommonServices::IException* pException) 
			{
				SAVE_LOG(SS_ERROR AND __WFUNCTION__ AND pException->ToString().c_str()); 
			}
			catch(...)
			{
				SAVE_LOG(SS_ERROR AND __WFUNCTION__ AND L"Unknown Exeption" ); 
			}
		}
	}
	SS_CATCH(L"")
}

}
}
}