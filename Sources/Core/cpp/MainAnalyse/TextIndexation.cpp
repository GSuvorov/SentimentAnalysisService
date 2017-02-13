#include "StdAfx.h"
#include ".\textindexation.h"

#include "./LinguisticIndexation.h"

//#define SYNTAX_ANALYSIS
//#define _SS_UNITTESTS


namespace SS
{
namespace LinguisticProcessor
{
namespace Indexation
{

CTextIndexation::CTextIndexation(void)
{
}

CTextIndexation::~CTextIndexation(void)
{
}

void CTextIndexation::Init()
{
	SS_TRY
	{
		GetAnalyseParams()->m_bIsLiteMode = m_oMode.m_bIsLiteMode;
		GetAnalyseParams()->SetAnalyseMod(SS::Core::Types::eamIndexation);
		GetAnalyseParams()->SetCurrentSystemMode(SS::Core::Types::ecsmLocal);

		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsMorpho	(m_oMode.GetAnalyseDepthParams()->IsMorpho());
		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSemantic	(m_oMode.GetAnalyseDepthParams()->IsSemantic());
		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims	(m_oMode.GetAnalyseDepthParams()->IsSynonims());
		GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSyntax	(m_oMode.GetAnalyseDepthParams()->IsSyntax());

		m_pTextFeatureText->ClearText();
		m_pTextFeatureText->GetTextFeature()->Clear();

		m_pLexica->SetAnalyseParams(GetAnalyseParams());
		m_pSemanticAnalyze->SetAnalyseParams(GetAnalyseParams());

#ifdef SYNTAX_ANALYSIS
		/*	синтаксический анализ	*/
		m_pSyntaxAnalyzer->SetAnalyseParams(GetAnalyseParams());
#endif

	}
	SS_CATCH(L"")
}

void CTextIndexation::Init(
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText
	, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
	, unsigned int uiFistCharPosition 
	)
{
	SS_TRY
	{
		//SaveNativeText(pNativeText) ;
		Init();
		m_pLexica->SetText(pNativeText , uiFistCharPosition);

	}
	SS_CATCH(L"")

}

void CTextIndexation::Clear()
{
	SS_TRY
	{
		m_pText->ClearText();

	}SS_CATCH(L"")
}



unsigned int CTextIndexation::GenerateNext(unsigned int& uiFistCharPosition, unsigned int& uiPosition, SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult)
{
	SS_TRY
	{
SL_OUT_DEBUG_STR (__WFUNCTION__);
		
		m_pText->ClearText();

		SS::Interface::Core::CommonContainers::ITextBlockEx* pTextBlockEx = 
			dynamic_cast<SS::Interface::Core::CommonContainers::ITextBlockEx*>(pIndexationResult->AddTextBlock());
		pTextBlockEx->SetLinguisticProcessorMode(&m_oMode);
		pTextBlockEx->Clear();

		unsigned int uiTempCharPos = m_pLexica->LoadNext();
		if(uiTempCharPos)
		{
			uiFistCharPosition = uiTempCharPos;
			/*	лексический анализ	*/
			m_pLexica->AnalyzeDocStructure(m_pTextFeatureText);
			m_pLexica->AnalyzeNext(m_pText, 0);

			uiPosition = GetLastPositon(m_pText)+1;


#ifdef SYNTAX_ANALYSIS
			/*	синтаксический анализ	*/
			m_pSyntaxAnalyzer->AnalyzeNext(m_pText);
#endif

			/*	семантический анализ	*/
#ifdef _SS_UNITTESTS
			m_pSemanticAnalyze->AnalyseNext(m_pText);
#endif
			m_pLexica->FillSynonyms(m_pText);

			pTextBlockEx->SetIText(m_pText, false);
SL_OUT_DEBUG_STR (__WFUNCTION__);
			return 0;
		}

		pTextBlockEx->SetIText(m_pTextFeatureText, true);
SL_OUT_DEBUG_STR (__WFUNCTION__);
		return 1;		

	}
	SS_CATCH(L"")

	return 0;
}


}
}
}
