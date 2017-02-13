#include "StdAfx.h"
#include ".\tableindexation.h"

#include ".\LinguisticIndexation.h"

#include ".\resultsex.h"

//#define SS_UNITTESTS

namespace SS
{
namespace LinguisticProcessor
{
namespace Indexation
{

CTableIndexation::CTableIndexation(void) : m_p_l_l_NativeText(NULL)
{
}

CTableIndexation::~CTableIndexation(void)
{
	Clear();
}

void CTableIndexation::Init(
	std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> > * p_l_l_NativeText
	, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
	)
{
	SS_TRY
	{

#ifdef SS_UNITTESTS
		CResultsEx oResultsEx;
		oResultsEx.ShowTable(p_l_l_NativeText);
#endif

		m_p_l_l_NativeText = p_l_l_NativeText;
		m_II = m_p_l_l_NativeText->begin();
	}
	SS_CATCH(L"")
}

void CTableIndexation::Clear()
{
	SS_TRY
	{
		for(std::list<SS::Interface::Core::BlackBox::IText*>::iterator iter = m_l_TextForTables.begin(); 
			iter != m_l_TextForTables.end(); iter++)
		{
			(*iter)->ReleaseText();
			(*iter) = NULL;
		}
		m_l_TextForTables.clear();

	}SS_CATCH(L"")
}


unsigned int CTableIndexation::GenerateNext(unsigned int& uiFistCharPosition, unsigned int& uiPosition, SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult)
{
	SS_TRY
	{

SL_OUT_DEBUG_STR (__WFUNCTION__);

		Clear();
		unsigned int uiStartPosition = uiPosition;

		for(; m_II != m_p_l_l_NativeText->end(); m_II++)
		{

			if(uiPosition > (uiStartPosition + SS::Constants::MAX_WORDS_IN_BLOCK))
			{
#ifdef SS_UNITTESTS
				CResultsEx oResultsEx;
				oResultsEx.ShowTable(pIndexationResult);
#endif

SL_OUT_DEBUG_STR (__WFUNCTION__);
				return 0;
			}

			SS::Interface::Core::CommonContainers::ITextBlockEx* pTextBlockEx = 
				dynamic_cast<SS::Interface::Core::CommonContainers::ITextBlockEx*>(pIndexationResult->AddTextBlock());
			pTextBlockEx->SetLinguisticProcessorMode(&m_oMode);
			pTextBlockEx->Clear();

			SS::Interface::Core::BlackBox::IText* pText = m_pBlackBoxTextStorage->CreateText();
			m_l_TextForTables.push_back(pText);

			for(std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*>::iterator I = m_II->begin(); 
				I != m_II->end(); I++)
			{
				m_pLexica->SetText((*I), uiFistCharPosition);

				if(0 == uiFistCharPosition)
					uiFistCharPosition = 1;					

				if(m_pLexica->LoadNext(true))
				{
					m_pLexica->AnalyzeNext(pText, 0);
				}
				unsigned int uiLastPosition = GetLastPositon(pText);
				if((uiLastPosition == (uiPosition-1)) || (uiLastPosition == 0))
				{					
					SS::Interface::Core::BlackBox::IParagraph* pParagraph = m_pBlackBoxTextStorage->CreateParagraph();
					pText->AddParagraph(pParagraph);
				}else
				{
					uiPosition = uiLastPosition + 1;
				}
			}

			m_pLexica->FillSynonyms(pText);
			pTextBlockEx->SetIText(pText, false);

		}

#ifdef SS_UNITTESTS
		CResultsEx oResultsEx;
		oResultsEx.ShowTable(pIndexationResult);
#endif

SL_OUT_DEBUG_STR (__WFUNCTION__);
		return 1;	



	}
	SS_CATCH(L"")
}

}
}
}