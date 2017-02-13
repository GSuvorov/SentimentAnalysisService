//TextBlockEx.cpp
#include "StdAfx.h"
#include "textblockex.h"
#include "IndexAndPosition.h"
//для использования IText-а
#include "..\ASSInterface\IBlackBoxDictionaryIndex.h"
#include "..\ASSInterface\IBlackBoxIndex.h"
#include "..\ASSInterface\TDictionaryNames.h"
#include "..\ASSInterface\TDictionaryManager.h"
#include "..\ASSInterface\Constants.h"
#ifdef _SS_UNITTESTS
	#include <iostream>
#endif

namespace SS
{
namespace Core
{
namespace CommonContainers
{

//КОНСТРУКТОРЫ
CTextBlockEx::CTextBlockEx(ILoadManager* pLoadManager) 
	: m_pBlackBoxTextStorage(NULL)
	, m_pLoadManager(NULL)
{
	SS_TRY
	{
		if (pLoadManager)
		{
			m_pLoadManager = pLoadManager;
			InitConnection();
		}		
	}
	SS_CATCH(L"")
}

CTextBlockEx::~CTextBlockEx()
{
}

void CTextBlockEx::InitConnection()		
{
	SS_TRY
	{
		m_pBlackBoxTextStorage = (IBlackBoxTextStorage*) m_pLoadManager->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);
	}
	SS_CATCH(L"")
}

void CTextBlockEx::CopyTextFeature(ITextFeature* ptfTo,ITextFeature* ptfFrom)	// копирует TextFeature	
{
	SS_TRY
	{
		unsigned int uiSize;
		char* pBuff;
		ptfFrom->GetTextFeatureBuff(&pBuff, &uiSize);
		ptfTo->SetTextFeatureBuff(pBuff, uiSize);
	}
	SS_CATCH(L"")
}


void CTextBlockEx::UpdateRelevance()
{
	SS_TRY
	{
		CTextBlock::UpdateRelevance();

		unsigned int iRelevanceMax = 0; 
		for(ITextBlockCut* pTextBlockCut = GetFirstBlockCut(); pTextBlockCut; pTextBlockCut = GetNextBlockCut())
		{
			if(iRelevanceMax < pTextBlockCut->GetRelevance())
			{ iRelevanceMax = pTextBlockCut->GetRelevance(); }			
		}

		iRelevanceMax = ((iRelevanceMax / SS::Constants::RELEVANCE_MULTIPLIER) * SS::Constants::RELEVANCE_MULTIPLIER);
		iRelevanceMax += GetRelevance();
		SetRelevance(iRelevanceMax);
	}
	SS_CATCH(L"")
}

/// устанавливает индексы IndicsationIndex и ServiceIndex
void CTextBlockEx::SetIndexes(ITextBlockCut* pTextBlockCut, IUnit* pUnit)
{
	SS_TRY
	{
		if (UINT_MAX == pUnit->GetPosition())
		{ return; }
		//Получаем IIndexAndPosition
		CIndexAndPosition* pIndexAndPosition = dynamic_cast<CIndexAndPosition*> (pTextBlockCut->AddIndexAndPosition());
		pIndexAndPosition->SetUnit(pUnit);
	}
	SS_CATCH( (wchar_t*)pUnit->GetWord() )
}

//ПЕРЕОПРЕДЕЛЯЮ ИЗ CTextBlock
ITextFeature* CTextBlockEx::GetTextFeature()
{
	SS_TRY
	{
		ITextFeature* pTextFeature = NULL;
		pTextFeature = CTextBlock::GetTextFeature();
		pTextFeature->SetLoadManager(m_pLoadManager);
		return pTextFeature;
	}
	SS_CATCH(L"")
}

//МЕТОДЫ ITextBlockEx
void CTextBlockEx::SetIText(IText* pText, bool bIsCopyTextFeature)// поступление заполненного IText - а (заполняю структуру CommonContainers)
{
	SS_TRY
	{							
		using SS::Interface::Core::BlackBox::IParagraph;
		using SS::Interface::Core::BlackBox::ISentence;
		using SS::Interface::Core::CommonContainers::ITextBlockCut;
							
		IParagraph*		pParagraph = NULL;
		ISentence*		pSentence = NULL;
		IUnit*			pCurrUnit = NULL;
		IUnit*			pRightUnit = NULL;
		IUnit*			pChildUnit = NULL;
		ITextBlockCut*	pTextBlockCut = NULL;
			
		if (bIsCopyTextFeature)
		{ CopyTextFeature(GetTextFeature(), pText->GetTextFeature()); }
							
		//************************* ПЕРЕБОР IText - а *****************************//
		pParagraph = pText->GetFirstParagraph();
		while (pParagraph)								
		{
			pTextBlockCut = AddBlockCut();
			ParagraphToCut(pParagraph, pTextBlockCut);
			pSentence = pParagraph->GetFirstSentence();
			while (pSentence)
			{
				pCurrUnit = pSentence->GetFirstUnit();
				//ПЕРЕБОР Unit - ов
				while (pCurrUnit)
				{
					pChildUnit = pCurrUnit->GetLeftChildUnit();
					if (pChildUnit)
					{
						pCurrUnit = pChildUnit;
						continue;	
					}
					SetIndexes(pTextBlockCut, pCurrUnit);
					pRightUnit = pCurrUnit->GetRightUnit();
					while (pRightUnit == NULL)
					{
						pCurrUnit = pCurrUnit->GetParentUnit();
						if (pCurrUnit == NULL)
						{ break; }
						SetIndexes(pTextBlockCut, pCurrUnit);
						pRightUnit = pCurrUnit->GetRightUnit();
					}
					pCurrUnit = pRightUnit;
				}
				//КОНЕЦ переб Unit - ов
				pSentence = pSentence->GetNextSentence();
				//ViewTextBlockCut(pTextBlockCut);
			}
			pParagraph = pParagraph->GetNextParagraph();
		}
	}
	SS_CATCH(L"")
}

void CTextBlockEx::ViewTextBlockCut(ITextBlockCut* pTextBlockCut)
{
	SS_TRY
	{
		unsigned int uirevPosition = 0, uirevPositionFirstChar = 0;
		IIndexAndPosition * pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition();
		while(pIndexAndPosition)
		{

#ifdef _SS_UNITTESTS

			if(uirevPosition > pIndexAndPosition->GetPosition())
			{
				SAVE_LOG(SS_WARNING AND L"uirevPosition > pCurrUnit->GetPosition()" AND __WFUNCTION__);
			}

			if(uirevPositionFirstChar > pIndexAndPosition->GetFirstCharPosition())
			{
				SAVE_LOG(SS_WARNING AND L"uirevPositionFirstChar > pCurrUnit->GetFirstCharPosition()" AND __WFUNCTION__);
			}

			uirevPositionFirstChar = pIndexAndPosition->GetFirstCharPosition();
			uirevPosition = pIndexAndPosition->GetPosition();
			wchar_t wsFromIToW1[20];
			wchar_t wsToMsg1[1000];
			wcscpy(wsToMsg1, L"Позиция текущего слова = ");
			wcscat(wsToMsg1, _itow(pIndexAndPosition->GetPosition(), wsFromIToW1, 10));		
			wcscat(wsToMsg1, L" Позиция первого символа текущего слова = ");
			wcscat(wsToMsg1, _itow(pIndexAndPosition->GetFirstCharPosition(), wsFromIToW1, 10));		
			wcscpy(wsToMsg1, L" текущее слово : ");					
			wcscat(wsToMsg1, pIndexAndPosition->GetWord());		
			wcscat(wsToMsg1, L" индекс : ");					
			wcscat(wsToMsg1, _itow(pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_DictionaryIndex , wsFromIToW1, 10));		
			wcscat(wsToMsg1, L" номер словаря : ");					
			wcscat(wsToMsg1, _itow((pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_DictionaryIndex >> 24) , wsFromIToW1, 10));				

			SAVE_LOG(SS_MESSAGE AND wsToMsg1 AND __WFUNCTION__);
#endif
			pIndexAndPosition = pTextBlockCut->GetNextIndexAndPosition();
		}
	}
	SS_CATCH(L"")
}

//Возвращение заполненного IText - а (заполняю структуру IText)
IText* CTextBlockEx::GenerateIText()	
{
	SS_TRY
	{
		SS::Interface::Core::CommonContainers::ITextBlockCut*		pTextBlockCut = NULL;
		SS::Interface::Core::CommonContainers::IIndexAndPosition*	pIndexAndPosition = NULL;
		SS::Interface::Core::BlackBox::IText*						pText = NULL;
		SS::Interface::Core::BlackBox::IParagraph*					pParagraph = NULL;
		SS::Interface::Core::BlackBox::ISentence*					pSentence = NULL;
		SS::Interface::Core::BlackBox::IUnit*						pUnit = NULL;
		CIndexAndPosition*														pCIndexAndPosition = NULL;
				
		//создал IText 
		pText = m_pBlackBoxTextStorage->CreateText();						
		
		//*************** копирование TextFeature *******************
		CopyTextFeature(pText->GetTextFeature(), GetTextFeature());

		pTextBlockCut = GetFirstBlockCut();
		while (pTextBlockCut)
		{
			//создал IParagraph
			pParagraph = m_pBlackBoxTextStorage->CreateParagraph();				
			pText->AddParagraph(pParagraph);

			//создал ISentence
			pSentence = m_pBlackBoxTextStorage->CreateSentence();				
			pParagraph->AddSentence(pSentence);

			//Копирование в параграф информации из блок-ката
			CutToParagraph(pTextBlockCut, pParagraph);
			
			pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition();
			while (pIndexAndPosition)
			{
				pCIndexAndPosition = dynamic_cast<CIndexAndPosition*>(pIndexAndPosition);
				if (pUnit = pCIndexAndPosition->GetUnit()) 
				{
					//Не удалять Unit
					pCIndexAndPosition->NotDeleteUnit();
					if (pCIndexAndPosition->IsValid()) 
					{
						pSentence->AddUnit(pUnit);
					}
					else
					{
						pUnit->ReleaseUnitHorizontal();
					}
				}
				pIndexAndPosition = pTextBlockCut->GetNextIndexAndPosition();
			}
			pTextBlockCut = GetNextBlockCut();
		}
		return pText;
	}
	SS_CATCH(L"")
}

///установление режима лингвистического процессора
void CTextBlockEx::SetLinguisticProcessorMode(TLinguisticProcessorMode* pLinguisticProcessorMode)
{
	SS_TRY
	{
		if (pLinguisticProcessorMode)
		{ m_LinguisticProcessorMode = (*pLinguisticProcessorMode); }
	}
	SS_CATCH(L"")
}

///Копирование в блок-кат информации из параграф
void CTextBlockEx::ParagraphToCut( SS::Interface::Core::BlackBox::IParagraph* pParagraph,
								   SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut )
{
	pTextBlockCut->SetIndexTypeName(pParagraph->GetIndexTypeName());
	pTextBlockCut->SetRelevance(pParagraph->GetRelevance());
	pTextBlockCut->SetLevel(pParagraph->GetLevel());
	pTextBlockCut->SetNumber(pParagraph->GetNumber());
	pTextBlockCut->SetType(pParagraph->GetType());
}

///Копирование в параграф информации из блок-ката
void CTextBlockEx::CutToParagraph( SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut,
								   SS::Interface::Core::BlackBox::IParagraph* pParagraph )
{
	pParagraph->SetIndexTypeName(pTextBlockCut->GetIndexTypeName());
	pParagraph->SetRelevance(pTextBlockCut->GetRelevance());
	pParagraph->SetLevel(pTextBlockCut->GetLevel());
	pParagraph->SetNumber(pTextBlockCut->GetNumber());
	pParagraph->SetType(pTextBlockCut->GetType());
}

///Добавит вырезку в хранилище (добавит себе и вернет ссылку)
ITextBlockCut* CTextBlockEx::AddBlockCut()
{
	SS_TRY
	{
		ITextBlockCut *pTextBlockCut = NULL;
		pTextBlockCut = m_pContainersFactory->CreateTextBlockCut(m_pBlackBoxTextStorage, &m_LinguisticProcessorMode);
		CTextBlock::AddCut(pTextBlockCut);
		return pTextBlockCut;
	}
	SS_CATCH(L"")
}

}
}
}