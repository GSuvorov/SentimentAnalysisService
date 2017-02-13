#include "StdAfx.h"
#include ".\resultsex.h"
#include "..\ASSInterface\Constants.h"
#include "..\[libs]\_Paths.h"

namespace SS
{
namespace LinguisticProcessor
{

CResultsEx::CResultsEx(void) : m_pDictionary( NULL)
{	
	
}

CResultsEx::~CResultsEx(void)
{
}

void CResultsEx::Init()
{
	SS_TRY
	{
		m_pDictionary = 
			(SS::Interface::Core::Dictionary::IDictionary*)
			GetLoadManager()->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);	

		if(!m_pDictionary->GetLoader()->Load())
		{
			SS_THROW(L"");
		}

		if(!GetAnalyseParams())
		{
			SetAnalyseParams( 
				(SS::Interface::Core::MainAnalyse::IAnalyseParams*)
				GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams));
			//GetAnalyseParams()->SetAnalyseMod(eamIndexation);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsMorpho(true);
		}

	}
	SS_CATCH(L"")
}

unsigned int CResultsEx::IsExistWord(const wchar_t* psWord)
{
	SS_TRY
	{
//		SL_OUT_DEBUG_STR (__WFUNCTION__);

		GetAnalyseParams()->SetCurrentSystemMode(SS::Core::Types::ecsmGlobal);
		m_pDictionary->GetMorphologyAnalyser()->SetAnalyseParams(GetAnalyseParams());

		bool bExist = false;
		bExist = m_pDictionary->GetMorphologyAnalyser()->IsExistWord(/*in*/psWord);
		if(bExist)
			return 1;
		else
			return 0;
	}
	SS_CATCH(L"")
}

void CResultsEx::FillIndexationIndexes(SS::Interface::Core::CommonContainers::ITextBlock * pTextBlock)
{
	SS_TRY
	{
		std::list<SS::Dictionary::Types::TWordInfo> l_WordInfo;

		for(SS::Interface::Core::CommonContainers::ITextBlockCut * pTextBlockCut = pTextBlock->GetFirstBlockCut();
			pTextBlockCut; pTextBlockCut = pTextBlock->GetNextBlockCut())
		{
			for(SS::Interface::Core::CommonContainers::IIndexAndPosition * pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition();
				pIndexAndPosition; pIndexAndPosition = pTextBlockCut->GetNextIndexAndPosition())
			{

				wchar_t wcWord[SS::Constants::MAX_CHARS_IN_WORD];
				SS::Dictionary::Types::SWordIndex oWordIndex;

				oWordIndex.m_DictIndex.SetDictionaryIndex(pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_DictionaryIndex);
				oWordIndex.m_IDForm = pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_IDForm;

				m_pDictionary->GetMorphologyAnalyser()->GetWord(&oWordIndex, wcWord);
 
				pIndexAndPosition->SetWord(wcWord);

//SAVE_LOG(SS_MESSAGE AND pIndexAndPosition->GetWord() AND __WFUNCTION__);

				m_pDictionary->GetMorphologyAnalyser()->GetWordInfo(
					pIndexAndPosition->GetWord(), &l_WordInfo, SS::Dictionary::Types::efzOnlySearch
					);

				for(std::list<SS::Dictionary::Types::TWordInfo>::iterator iter = l_WordInfo.begin();
					iter != l_WordInfo.end(); iter++)
				{
					pIndexAndPosition->GetIndexationIndex()->AddIndex(iter->begin()->m_WordIndex.m_DictIndex.GetDictionaryIndex());
				}
				
			}			
		}

	}
	SS_CATCH(L"")
}


void CResultsEx::ShowTable(std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> > * p_l_l_NativeText)
{
	SS_TRY
	{
wchar_t wz[SS::Constants::MAX_WORDS_IN_BLOCK + 1000];
SL_OUT_DEBUG_STR(L"ITT In<Table>");
		for(std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> >::iterator II = p_l_l_NativeText->begin();
			II != p_l_l_NativeText->end(); II++)
		{
SL_OUT_DEBUG_STR(L"ITT  <Row>");
			for(std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*>::iterator I = II->begin(); 
				I != II->end(); I++)
			{
wcscpy(wz, L"ITT   <Cell> ");					
wcscat(wz, (*I)->GetText());					
SL_OUT_DEBUG_STR(wz);
			}			
		}
SL_OUT_DEBUG_STR(L"ITT In<Table>");
	}SS_CATCH(L"")
}

void CResultsEx::ShowTable(SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult)
{
	SS_TRY
	{
wchar_t wz[SS::Constants::MAX_WORDS_IN_BLOCK + 1000];
SL_OUT_DEBUG_STR(L"ITT <Table>");
		for(SS::Interface::Core::CommonContainers::ITextBlock * pTextBlock = pIndexationResult->GetFirstTextBlock();
			pTextBlock; pTextBlock = pIndexationResult->GetNextTextBlock())
		{
SL_OUT_DEBUG_STR(L"ITT  <Row>");
			for(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut = pTextBlock->GetFirstBlockCut();pTextBlockCut;
				pTextBlockCut = pTextBlock->GetNextBlockCut())
			{
wcscpy(wz, L"ITT   <Cell> ");					
				for(SS::Interface::Core::CommonContainers::IIndexAndPosition * pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition();
					pIndexAndPosition; pIndexAndPosition = pTextBlockCut->GetNextIndexAndPosition())
				{
wcscat(wz, pIndexAndPosition->GetWord());					
wcscat(wz, L" ");
				}
SL_OUT_DEBUG_STR(wz);
			}			
		}
SL_OUT_DEBUG_STR(L"ITT <Table>");
	}SS_CATCH(L"")
}

void CResultsEx::PrepareForView( SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult	, SS::Core::Types::IndexationSearch::TPrepareForViewParams* pPrepareForViewParams)
{
	SS_TRY
	{		
		int iShowedTextBlockCount = 0;
		int iCurTextBlockNumber = 0;
		bool bStartShowResult = false;
		for(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock = pSearchResult->GetFirstTextBlock(); pTextBlock;
			pTextBlock = pSearchResult->GetNextTextBlock())
		{				
			if(pPrepareForViewParams->m_uiFirstProcessedTextBlockNumber == iCurTextBlockNumber)
			{
				bStartShowResult = true;
			}

			iCurTextBlockNumber++;
			if(!bStartShowResult)
				continue;

			if(pPrepareForViewParams->m_uiProcessedTextBlockCount == iShowedTextBlockCount)
				break;

			iShowedTextBlockCount++;

			PrepareForView(pTextBlock);

		}

	}
	SS_CATCH(L"")
}


void CResultsEx::PrepareForView( SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock)
{
	SS_TRY
	{		
		for(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut = pTextBlock->GetFirstBlockCut();pTextBlockCut;
			pTextBlockCut = pTextBlock->GetNextBlockCut())
		{
			PrepareForView(pTextBlockCut);		
		}			

	}
	SS_CATCH(L"")

}

void CResultsEx::PrepareForView( SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut)
{
	SS_TRY
	{		

		std::wstring wsTextCut ;

		wchar_t    wsWord[SS::Constants::MAX_CHARS_IN_WORD];

		for(SS::Interface::Core::CommonContainers::IIndexAndPosition * pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition();
			pIndexAndPosition; pIndexAndPosition = pTextBlockCut->GetNextIndexAndPosition())
		{
#ifdef _DEBUG
			std::wstring ws = pIndexAndPosition->GetWord();
#endif
			if(*(pIndexAndPosition->GetWord()))
			{
				continue;
			}

			if(!pIndexAndPosition->GetServiceIndex())
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"!pIndexAndPosition->GetServiceIndex()");
				continue;
			}
			if(!pIndexAndPosition->GetServiceIndex()->GetViewIndex())
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"!pIndexAndPosition->GetServiceIndex()->GetViewIndex()");
				continue;
			}
			SS::Dictionary::Types::SWordIndex oWordIndex;
			oWordIndex.m_DictIndex.SetDictionaryIndex(pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_DictionaryIndex);
			oWordIndex.m_IDForm	= pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_IDForm;
			m_pDictionary->GetMorphologyAnalyser()->GetWord(&oWordIndex, wsWord);
			pIndexAndPosition->SetWord(wsWord);
		}


	}
	SS_CATCH(L"")

}



}
}