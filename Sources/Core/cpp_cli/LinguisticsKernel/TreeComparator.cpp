#include "StdAfx.h"
#include ".\TreeComparator.h"
#using <mscorlib.dll>
#using <System.Drawing.dll>

#include "..\..\cpp\ASSInterface\TDictionaryManager.h"

#include "..\..\cpp\ASSInterface\Constants.h"
using namespace SS::Constants;

namespace LinguisticsKernel
{

using namespace SS::Interface::Core::BlackBox;
using namespace SS::Interface::Core::BlackBox::Query;

//---------------------------------------------------------------------------------//

CWord::CWord(SS::Interface::Core::BlackBox::IUnit* pUnit, CStringService^ pStringService)
:m_psWord(gcnew System::String("")), m_parrIndexes(gcnew System::Collections::ArrayList()), 
m_bQuestion(false), m_parrSameWords(gcnew System::Collections::ArrayList()), m_pStringService(pStringService)
{
	m_parrIndexes->Add(gcnew System::Collections::ArrayList());
	m_parrIndexes->Add(gcnew System::Collections::ArrayList());
	m_parrIndexes->Add(gcnew System::Collections::ArrayList());
	m_parrIndexes->Add(gcnew System::Collections::ArrayList());

	if(!pUnit) return;

	m_psWord=gcnew System::String(pUnit->GetWord());

	if(pUnit->GetSyntaxFeature())
		m_bQuestion=(pUnit->GetSyntaxFeature()->IsQuestion()==0)?false:true;
	
	if(m_bQuestion){
		m_WordColor=System::Drawing::Color::Green;
	}else{
		m_WordColor=System::Drawing::Color::Red;
	}

	IIndex* pIndex;
	IDictionaryIndex* pDictionaryIndex;

	for(pIndex = pUnit->GetFirstIndex();pIndex; pIndex = pUnit->GetNextIndex())
	{
		pDictionaryIndex = pIndex->GetDictionaryIndex();
		if(pDictionaryIndex)
		{
			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();

			switch(oDictionaryIndex.GetIndexType())
			{
			case SS::Interface::Core::BlackBox::eitMorphoIndex:
				if(!m_bQuestion)
					if(!GetIndexesByType(EIndexType::eitMorpho)->Contains(oDictionaryIndex.GetDictionaryIndexWithoutFlags())) 
						GetIndexesByType(EIndexType::eitMorpho)->Add(oDictionaryIndex.GetDictionaryIndexWithoutFlags());
				break;
			case SS::Interface::Core::BlackBox::eitSyntaxIndex:
				if(!GetIndexesByType(EIndexType::eitSyntax)->Contains(oDictionaryIndex.GetDictionaryIndexWithoutFlags())) 
					GetIndexesByType(EIndexType::eitSyntax)->Add(oDictionaryIndex.GetDictionaryIndexWithoutFlags());
				break;
			case SS::Interface::Core::BlackBox::eitSemanticIndex:
				if(!GetIndexesByType(EIndexType::eitSemantic)->Contains(oDictionaryIndex.GetDictionaryIndexWithoutFlags())) 
					GetIndexesByType(EIndexType::eitSemantic)->Add(oDictionaryIndex.GetDictionaryIndexWithoutFlags());
				break;
			case SS::Interface::Core::BlackBox::eitSynonymIndex:
				if(!m_bQuestion)
					if(!GetIndexesByType(EIndexType::eitMorpho)->Contains(oDictionaryIndex.GetDictionaryIndexWithoutFlags())) 
						GetIndexesByType(EIndexType::eitMorpho)->Add(oDictionaryIndex.GetDictionaryIndexWithoutFlags());
				break;
			default:
				break;
			}
		}		
	}

}

CWord::CWord(SS::Interface::Core::BlackBox::Query::IQueryUnit* pQueryUnit, bool bQuestion, CStringService^ pStringService)
:m_psWord(gcnew System::String("")), m_parrIndexes(gcnew System::Collections::ArrayList()), 
m_bQuestion(bQuestion), m_parrSameWords(gcnew System::Collections::ArrayList()), m_pStringService(pStringService)
{
	m_parrIndexes->Add(gcnew System::Collections::ArrayList());
	m_parrIndexes->Add(gcnew System::Collections::ArrayList());
	m_parrIndexes->Add(gcnew System::Collections::ArrayList());
	m_parrIndexes->Add(gcnew System::Collections::ArrayList());

	if(!pQueryUnit) return;

	m_psWord=gcnew System::String(pQueryUnit->GetWord());

	if(m_bQuestion){
		m_WordColor=System::Drawing::Color::Green;
	}else{
		m_WordColor=System::Drawing::Color::Red;
	}

	System::UInt32 uiIndex;
	IQueryUnitIndexCollection* pQueryUnitIndexCollection=pQueryUnit->GetFirstQueryUnitIndexCollection();
	while(pQueryUnitIndexCollection){
		System::Collections::ArrayList^ pCurrIndexes=nullptr;
		//тип коллекции
		switch(pQueryUnitIndexCollection->GetIndexCollectionType()){
		case SS::Interface::Core::BlackBox::eitMorphoIndex:
			if(!m_bQuestion)
				pCurrIndexes=GetIndexesByType(EIndexType::eitMorpho);
			break;
		case SS::Interface::Core::BlackBox::eitSyntaxIndex:
			pCurrIndexes=GetIndexesByType(EIndexType::eitSyntax);
			break;
		case SS::Interface::Core::BlackBox::eitSemanticIndex:
			pCurrIndexes=GetIndexesByType(EIndexType::eitSemantic);
			break;
		case SS::Interface::Core::BlackBox::eitSynonymIndex:
			if(!m_bQuestion)
				pCurrIndexes=GetIndexesByType(EIndexType::eitMorpho);
			break;
		default:
			pCurrIndexes=nullptr;
			break;
		}

		IQueryUnitIndex* pQueryUnitIndex=pQueryUnitIndexCollection->GetFirstQueryUnitIndex();
		while(pQueryUnitIndex && pCurrIndexes)
		{
			uiIndex=pQueryUnitIndex->GetIndex().GetDictionaryIndex();

			if(!pCurrIndexes->Contains(uiIndex)) pCurrIndexes->Add(uiIndex);

			pQueryUnitIndex=pQueryUnitIndex->GetNextQueryUnitIndex();
		}
		pQueryUnitIndexCollection=pQueryUnitIndexCollection->GetNextQueryUnitIndexCollection();
	}

}

CWord::~CWord(void)
{
}

void CWord::ExtractIndexes(CWord^ pWord)
{
	if(!pWord) return;

	GetIndexesByType(EIndexType::eitMorpho)->AddRange(pWord->GetIndexesByType(EIndexType::eitMorpho));
	GetIndexesByType(EIndexType::eitSyntax)->AddRange(pWord->GetIndexesByType(EIndexType::eitSyntax));
	GetIndexesByType(EIndexType::eitSemantic)->AddRange(pWord->GetIndexesByType(EIndexType::eitSemantic));
	GetIndexesByType(EIndexType::eitSynonim)->AddRange(pWord->GetIndexesByType(EIndexType::eitSynonim));
	
	//m_bQuestion=pWord->IsQuestion;
}

void CWord::Compare(System::Collections::ArrayList^ pLeftIndexes, System::Collections::ArrayList^ pRightIndexes, System::Collections::ArrayList^ pSameIndexes, System::Collections::ArrayList^ pDifIndexes)
{
	if(!pSameIndexes) return;
	if(!pDifIndexes) return;
	if(!pLeftIndexes) return;
	if(!pRightIndexes) return;


	pSameIndexes->Clear();
	pDifIndexes->Clear();

	for(int i = 0; i < pRightIndexes->Count; i++)
	{
		for(int j = 0; j < pLeftIndexes->Count; j++)
		{
			int iRightIndex = (int) pRightIndexes[i],
				iLeftIndex = (int) pLeftIndexes[j];

			if((iRightIndex & SIGNIFICANT_INDEX_PART) == (iLeftIndex & SIGNIFICANT_INDEX_PART))
				pSameIndexes->Add(pRightIndexes[i]);
			else
				pDifIndexes->Add(pRightIndexes[i]);
		}
	}
}

void CWord::Compare(CWord^ pWord, System::Windows::Forms::TreeNode^ pTNWord, System::Windows::Forms::TreeNode^ pCommonTNWord)
{
	System::Windows::Forms::TreeNode^ pTNChildWord=nullptr;
	System::Windows::Forms::TreeNode^ pCommonTNChildWord=nullptr;
	bool bFlag=true;
	System::Collections::ArrayList^ pSameIndexes=gcnew System::Collections::ArrayList(); 
	System::Collections::ArrayList^ pDifIndexes=gcnew System::Collections::ArrayList();

	pTNChildWord=pTNWord->Nodes->Add(pWord->Word);
	pCommonTNChildWord=pCommonTNWord->Nodes->Add(pWord->Word);
	
	if(GetIndexesByType(EIndexType::eitMorpho)->Count){
		//если у слова есть индексы данного типа, проводим сравнение
		Compare(GetIndexesByType(EIndexType::eitMorpho), 
				pWord->GetIndexesByType(EIndexType::eitMorpho), pSameIndexes, pDifIndexes);

		if(pSameIndexes->Count || pDifIndexes->Count){
			//если есть совпавшие индексы, добавл€ем результаты к узлу
			AddToNode(pSameIndexes, pDifIndexes, pTNChildWord->Nodes->Add("Morpho"), EIndexType::eitMorpho);
		}

		if(pSameIndexes->Count){
		}else{
			//если по морфологии нет совпадени€, дальше не сравниваем
			pTNChildWord->Remove();
			pCommonTNChildWord->Remove();
			return;
		}
	}

	if(GetIndexesByType(EIndexType::eitSyntax)->Count){
		//если у слова есть индексы данного типа, проводим сравнение
		Compare(GetIndexesByType(EIndexType::eitSyntax), 
				pWord->GetIndexesByType(EIndexType::eitSyntax), pSameIndexes, pDifIndexes);

		if(pSameIndexes->Count || pDifIndexes->Count){
			//если есть совпавшие индексы, добавл€ем результаты к узлу
			AddToNode(pSameIndexes, pDifIndexes, pTNChildWord->Nodes->Add("Syntax"), EIndexType::eitSyntax);
		}		

		if(!pSameIndexes->Count){
			pCommonTNChildWord->Nodes->Add("Syntax - нет совпадений");
			bFlag=false;
		}
	}

	if(GetIndexesByType(EIndexType::eitSemantic)->Count){
		//если у слова есть индексы данного типа, проводим сравнение
		Compare(GetIndexesByType(EIndexType::eitSemantic), 
				pWord->GetIndexesByType(EIndexType::eitSemantic), pSameIndexes, pDifIndexes);

		if(pSameIndexes->Count || pDifIndexes->Count){
			//если есть совпавшие индексы, добавл€ем результаты к узлу
			AddToNode(pSameIndexes, pDifIndexes, pTNChildWord->Nodes->Add("Semantic"), EIndexType::eitSemantic);
		}		
		
		if(!pSameIndexes->Count){
			pCommonTNChildWord->Nodes->Add("Semantic - нет совпадений");
			bFlag=false;
		}
	}
	
	if(bFlag){
		//слово прошло сравнени€ выставл€ем ему цвет родител€
		pTNChildWord->ForeColor = m_WordColor;
		pCommonTNChildWord->ForeColor = m_WordColor;
		//заносим его в коллекцию совпавших слов
		m_parrSameWords->Add(pWord->Word);
	}

	if(!pTNChildWord->Nodes->Count){
		pTNChildWord->Remove();
	}
}

void CWord::AddToNode(System::Collections::ArrayList^ pSameIndexes, System::Collections::ArrayList^ pDifIndexes, System::Windows::Forms::TreeNode^ pTN, CWord::EIndexType IndexType)
{
	if(!pSameIndexes) return;
	if(!pDifIndexes) return;
	if(!pTN) return;

	for(int i=0; i<pSameIndexes->Count; i++)
		AddIndexToNode(System::Convert::ToUInt32(pSameIndexes[i]), IndexType, m_WordColor, pTN);

	for(int i=0; i<pDifIndexes->Count; i++)
		AddIndexToNode(System::Convert::ToUInt32(pDifIndexes[i]), IndexType, System::Drawing::Color::Blue, pTN);

}

void CWord::AddIndexToNode(System::UInt32 Index, CWord::EIndexType IndexType, System::Drawing::Color ForeColor, System::Windows::Forms::TreeNode^ pTN)
{
	if(!pTN) return;
	if(!m_pStringService) return;

	if(IndexType==EIndexType::eitSyntax){
		pTN->Nodes->Add(m_pStringService->SyntaxIndexToString(Index))->ForeColor = ForeColor;
	}else if(IndexType==EIndexType::eitSemantic){
		pTN->Nodes->Add(m_pStringService->SemanticIndexToString(Index))->ForeColor = ForeColor;
	}else{
		pTN->Nodes->Add(m_pStringService->MorphoIndexToString(Index))->ForeColor = ForeColor;
	}

}

void CWord::AddToNodeIndexes(System::Windows::Forms::TreeNode^ pTN)
{
	if(!pTN) return;
	if(!m_pStringService) return;
	
	System::Windows::Forms::TreeNode^ pTempNode;
	if(GetIndexesByType(EIndexType::eitMorpho)->Count){
		pTempNode=pTN->Nodes->Add("Morpho");
		pTempNode->ForeColor = pTN->ForeColor;
		for(int i=0; i<GetIndexesByType(EIndexType::eitMorpho)->Count; i++)
		{
			AddIndexToNode(
				System::Convert::ToUInt32(GetIndexesByType(EIndexType::eitMorpho)[i]), 
				EIndexType::eitMorpho, pTempNode->ForeColor, pTempNode);
		}
	}

	if(GetIndexesByType(EIndexType::eitSyntax)->Count){
		pTempNode=pTN->Nodes->Add("Syntax");
		pTempNode->ForeColor = pTN->ForeColor;
		for(int i=0; i<GetIndexesByType(EIndexType::eitSyntax)->Count; i++){
			AddIndexToNode(
				System::Convert::ToUInt32(GetIndexesByType(EIndexType::eitSyntax)[i]), 
				EIndexType::eitSyntax, pTempNode->ForeColor, pTempNode);
		}
	}

	if(GetIndexesByType(EIndexType::eitSemantic)->Count){
		pTempNode=pTN->Nodes->Add("Semantic");
		pTempNode->ForeColor = pTN->ForeColor;
		for(int i=0; i<GetIndexesByType(EIndexType::eitSemantic)->Count; i++){
			AddIndexToNode(
				System::Convert::ToUInt32(GetIndexesByType(EIndexType::eitSemantic)[i]), 
				EIndexType::eitSemantic, pTempNode->ForeColor, pTempNode);
		}
	}

}

System::Windows::Forms::TreeNode^ CWord::AddWordToNode(System::Windows::Forms::TreeNode^ pTN)
{
	if(!pTN) return nullptr;

	System::Windows::Forms::TreeNode^ pTNWord=pTN->Nodes->Add(m_psWord);
	pTNWord->ForeColor = m_WordColor;

	return pTNWord;
}

//---------------------------------------------------------------------------------//

CTreeComparator::CTreeComparator(void)
{
	m_pLeftWords=gcnew System::Collections::ArrayList();
	m_pRightWords=gcnew System::Collections::ArrayList();
	m_pStringService=gcnew CStringService();
}

CTreeComparator::~CTreeComparator(void)
{
}

void CTreeComparator::Convert(SS::Interface::Core::BlackBox::IText* pText, System::Collections::ArrayList^ pWords)
{
	if(!pText) SS_THROW(L"pText");	
	if(!pWords) SS_THROW(L"pWords");	

	SS::Interface::Core::BlackBox::IParagraph* pParagraph=NULL;
	SS::Interface::Core::BlackBox::ISentence* pSentence=NULL;
	SS::Interface::Core::BlackBox::IUnit* pUnit=NULL;
	
	pParagraph=pText->GetFirstParagraph();
	pSentence=pParagraph->GetFirstSentence();
	System::Collections::ArrayList^ pSentenceWords=gcnew System::Collections::ArrayList();
	pWords->Add(pSentenceWords);

	//в цикле обходим юниты предложени€, создаем по ним группы
	pUnit=pSentence->GetFirstUnit();
	while(pUnit){
		//если группа создалась проходим по дет€м юнита пихаем их в группу
		if(!pUnit->HasChilds()){
			//если у юнита нет добавл€ем слово в массив
			pSentenceWords->Add(gcnew CWord(pUnit, m_pStringService));
		}else{
			//если у юнита дети есть пихаем их в группу
			CWord^ pParentWord=gcnew CWord(pUnit, m_pStringService);
			SS::Interface::Core::BlackBox::IUnit* pChildUnit=pUnit;
			while(pChildUnit){
				
				if(!pChildUnit->HasChilds()){
					//создаем юнит группы и добавл€ем к группе
					CWord^ pChildWord=gcnew CWord(pChildUnit, m_pStringService);
					pChildWord->ExtractIndexes(pParentWord);
					pSentenceWords->Add(pChildWord);

					if(!pChildUnit->GetRightUnit()){
						//если соседа нет переходим наверх
						pChildUnit=pChildUnit->GetParentUnit();
						if(pChildUnit==pUnit){
							//если дошли до верхнего юнит
							break;
						}
						if(pChildUnit){
							//и сразу сдвигаемс€ на соседа
							pChildUnit=pChildUnit->GetRightUnit();
						}
					}else{
						//если есть сосед переходим к следующему соседу
						pChildUnit=pChildUnit->GetRightUnit();
					}
				}else{
					pChildUnit=pChildUnit->GetLeftChildUnit();
				}
			}
		}
		
		//переходим к следующему юниту предложени€
		pUnit=pUnit->GetRightUnit();
	}

}

void CTreeComparator::Convert(SS::Interface::Core::BlackBox::Query::IQuery* pQuery, System::Collections::ArrayList^ pWords)
{
	if(!pQuery) return;
	if(!pWords) return;
	
	ITransformation* pTransformation=pQuery->GetFirstTransformation();
	while(pTransformation){
		System::Collections::ArrayList^ pTransformationWords=gcnew System::Collections::ArrayList();
		pWords->Add(pTransformationWords);
		IGroup* pGroup=pTransformation->GetFirstGroup();
		while(pGroup){
			IQueryUnit* pQueryUnit=pGroup->GetFirstQueryUnit();
			while(pQueryUnit){
				//добавл€ем слово в коллекцию
				CWord^ pWord=gcnew CWord(
					pQueryUnit, 
					(pGroup->GetSyntaxFeature()->IsQuestion()==0)?false:true, 
					m_pStringService);
				pTransformationWords->Add(pWord);
				pQueryUnit=pQueryUnit->GetNextQueryUnit();
			}
			pGroup=pGroup->GetNextGroup();
		}
		pTransformation=pTransformation->GetNextTransformation();
	}

}

void CTreeComparator::Compare(System::String^ pQuery, System::Windows::Forms::TreeView^ pComparationTree, System::Windows::Forms::TreeView^ pCoincidenceTree)
{
	if(!pComparationTree) SS_THROW(L"pComparationTree");	
	if(!pCoincidenceTree) SS_THROW(L"pCoincidenceTree");	
	if(!pQuery) SS_THROW(L"pQuery");	
	if(!m_pRightWords->Count) return;
	if(!m_pLeftWords->Count) return;

	//показываем сам запрос
	pComparationTree->Nodes->Clear();
	pComparationTree->Nodes->Add(pQuery)->ForeColor = System::Drawing::Color::SteelBlue;
	pCoincidenceTree->Nodes->Clear();
	pCoincidenceTree->Nodes->Add(pQuery)->ForeColor = System::Drawing::Color::SteelBlue;
	

	//выполн€ем сравнение
	CWord^ pLeftWord=nullptr;
	CWord^ pRightWord=nullptr;
	for(int iLeftWords=0; iLeftWords<m_pLeftWords->Count; iLeftWords++){
		System::Collections::ArrayList^ pWords=safe_cast< System::Collections::ArrayList^> (m_pLeftWords[iLeftWords]);
		System::String^ pszRez=System::String::Concat("–езультаты є", iLeftWords.ToString());
		System::Windows::Forms::TreeNode^ pCompCollectionNode=pComparationTree->Nodes->Add(pszRez);
		pCompCollectionNode->ForeColor = System::Drawing::Color::SteelBlue;
		System::Windows::Forms::TreeNode^ pCoinCollectionNode=pCoincidenceTree->Nodes->Add(pszRez);
		pCoinCollectionNode->ForeColor = System::Drawing::Color::SteelBlue;
		
		bool bFlag=false;
		for(int iLeft=0; iLeft<pWords->Count; iLeft++){
			pLeftWord=safe_cast<CWord^>(pWords[iLeft]);
			System::Windows::Forms::TreeNode^ pCompTNWord=pLeftWord->AddWordToNode(pCompCollectionNode);
			System::Windows::Forms::TreeNode^ pCoinTNWord=pLeftWord->AddWordToNode(pCoinCollectionNode);
			
			//отображаем индексы слова
			pLeftWord->AddToNodeIndexes(pCompTNWord);
			pLeftWord->SameWords->Clear();

			//сравниваем данное слово с другими
			System::Collections::ArrayList^ pTempWords=safe_cast<System::Collections::ArrayList^>(m_pRightWords[0]);
			for(int iRight=0; iRight<pTempWords->Count; iRight++){
				pRightWord=safe_cast<CWord^>(pTempWords[iRight]);
				pLeftWord->Compare(pRightWord, pCompTNWord, pCoinTNWord);
			}

			//провер€ем на общее совпадение
			if(!pLeftWord->SameWords->Count){
				bFlag=true;
				pCoinTNWord->Text=pCoinTNWord->Text->Concat(pCoinTNWord->Text, gcnew System::String(" -нет совпадений-"));
			}

		}

		//если флаг не выставлен, имеем полное совпадение
		if(!bFlag){
			pCoinCollectionNode->Text=pCoinCollectionNode->Text->Concat(pCoinCollectionNode->Text, gcnew System::String(" есть совпадени€ дл€ каждого слова"));
		}
	}

	
}

//---------------------------------------------------------------------------------//

}