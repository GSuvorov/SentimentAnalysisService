#include "StdAfx.h"
#include ".\hdr_containers.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

void CDocumentHeadersInfo::UpdateHeaderPosition(unsigned int uiPosition)
{
	//!!!пока не сделано отслеживание границы влияния заголовка

	SHeaderInfo HeaderInfo;
	HeaderInfo.m_HeaderID=m_CurrentHeaderID;
	if(m_HeaderInfoCollection.empty()){
		//если добавляем первый заголовок
		//добавляем новый заголовок, обновдяем его стартовую позицию
		
		if(HeaderInfo.m_HeaderID.GetValueAtLevel(0)==0){
			//проверяем первый заголовок должен быть рутовым
			TO_CONSOLE(L"First header not Root header");
			HeaderInfo.m_HeaderID.View();
			//устанавливаем рутовый заголовок
			m_CurrentHeaderID.Reset();
			m_CurrentHeaderID.IncAtLevel(0);
			HeaderInfo.m_HeaderID=m_CurrentHeaderID;
		}

		m_HeaderInfoCollection.push_back(HeaderInfo);
		m_HeaderInfoCollection.back().m_HeaderPositions.m_uiHeadPosition=uiPosition;
		m_HeaderInfoCollection.back().m_HeaderPositions.m_uiTailPosition=uiPosition;

	}else if(m_HeaderInfoCollection.back().m_HeaderID.GetID()!=m_CurrentHeaderID.GetID()){
		//если идентификатор последний добавленный заголовок не равен входному
		//добавляем новый заголовок, обновдяем его стартовую позицию

		//все предыдущим загловкам более низкого уровня и ОДНОМУ такого же, проставляем границу действия
		THeaderInfoCollection::iterator itHeaderInfo=(--m_HeaderInfoCollection.end());
		do{
			//все предыдущим загловкам такого же или более низкого уровня, 
			//проставляем границу
			if(itHeaderInfo->m_HeaderID.IsAtSameLevel(&m_CurrentHeaderID)){
				//если заголовок равный определяем границу и выходим
				itHeaderInfo->m_HeaderPositions.m_uiBoundPosition=uiPosition;
				break;
			}else if(itHeaderInfo->m_HeaderID<m_CurrentHeaderID){
				//если заголовок младше, определяем границу и продолжаем
				itHeaderInfo->m_HeaderPositions.m_uiBoundPosition=uiPosition;
				itHeaderInfo--;
			}else{
				break;
			}
		}while(itHeaderInfo!=m_HeaderInfoCollection.begin());
		
		//кладем новый заголовок, устанавливаем ему головную позицию
		m_HeaderInfoCollection.push_back(HeaderInfo);
		m_HeaderInfoCollection.back().m_HeaderPositions.m_uiHeadPosition=uiPosition;
	
	}else{
		//если идентификатор последний добавленный заголовок равен входному
		//обновляем последнюю позицию
		m_HeaderInfoCollection.back().m_HeaderPositions.m_uiTailPosition=uiPosition;
	}
}

bool CDocumentHeadersInfo::IncCurrentHeaderID(unsigned int uiHeaderLevel)
{
	//!!!принудительно устанавливаем уровень
	if(m_HeaderInfoCollection.empty()){
		uiHeaderLevel=0;
	}else{
		uiHeaderLevel=1;
	}

	if(GetCurrentHeaderID()->GetValueAtLevel(uiHeaderLevel)>=0xffff) return false;

	//обновляем текущий идентификаор заголовка
    GetCurrentHeaderID()->IncAtLevel(uiHeaderLevel);
	return true;
}

void CDocumentHeadersInfo::ToString(wstring& wszValue)
{
	wszValue.assign(L"\n//--------Bof-CDocumentHeadersInfo:\n");
	wstring wszTemp;
	for(unsigned int i=0; i<m_HeaderInfoCollection.size(); i++){
		wszValue.append(L"\n");
		m_HeaderInfoCollection[i].ToString(wszTemp);
		wszValue.append(wszTemp);
	}
	wszValue.append(L"\n\n//--------Eof-CDocumentHeadersInfo:\n");
}

//--------------------------------------------------------------------//

CHeaderNode::CHeaderNode(CLevelHeaderID* pHeaderID)
:CTreeNode< CHeaderWords<Extracting::Logics::CHeaderLogic::MaxBlockCount>* >
	(new CHeaderWords<Extracting::Logics::CHeaderLogic::MaxBlockCount>)
{
	//устанавливаем узлу ео идентификатор
	CLevelHeaderID* pHID=GetMyself()->GetHeaderID();
	(*pHID)=(*pHeaderID);
}

void CHeaderNode::InsertHeaderNode(unsigned int uiWordID, CLevelHeaderID* pHeaderID)
{
	if(ISNULL(pHeaderID)) return;

	//проверяем идентификатор заголовка
	CLevelHeaderID* pMyHID=GetMyself()->GetHeaderID();
	if((*pMyHID)==(*pHeaderID)){

		//если попали но свой узел заносим слово	
		GetMyself()->SetWordID(uiWordID);

	}else if(pMyHID->GetValueAtLevel(GetHeaderLevel())==pHeaderID->GetValueAtLevel(GetHeaderLevel())){

		//если это более старший узел, то есть удовлетворяется равнетсво на данном уровне
		//ищем ребенка с нужным значением, или создаем его и отправляем все ему
		unsigned int uiTemp=pHeaderID->GetValueAtLevel(GetHeaderLevel()+1);
		if(!uiTemp){
			TO_CONSOLE(L"InsertHeaderNode error, imposible value!!!");
			return;
		}
		
		CHeaderNode* pChildNode=static_cast<CHeaderNode*>(GetChild(uiTemp));
		if(!pChildNode){
			//создаем соответсвующий идентификатор до данного уровня
			CLevelHeaderID ChildHeaderID(pHeaderID->GetIDTillLevel(GetHeaderLevel()+1));
			//создаем и добавляем ребенка
			pChildNode=new CHeaderNode(&ChildHeaderID);
			AddChild(uiTemp, pChildNode);
		}
		
		//продолжаем вставку
		pChildNode->InsertHeaderNode(uiWordID, pHeaderID);
	}
}

void CHeaderNode::ToString(wstring& szValue, wstring szOffset)
{
	szOffset.append(L" ");	
	szValue.append(L"\n");
	szValue.append(szOffset);
	szValue.append(L"|-->");
	wstring wszTemp;
	GetMyself()->ToString(wszTemp);
	szValue.append(wszTemp);

	CHeaderNode* pCurrentNode=NULL;
	if(MoveFirstChild())
	do{
		pCurrentNode=static_cast<CHeaderNode*>(GetCurrentChild());
		pCurrentNode->ToString(szValue, szOffset);
	}while(MoveNextChild());
}

//--------------------------------------------------------------------//

CHeadersTree::CHeadersTree(unsigned int uiDocumentIndex)
:m_uiDocumentIndex(uiDocumentIndex), m_pRootNode(NULL)
{
	//создаем рутовый узел
	CLevelHeaderID RootHeaderID;
	RootHeaderID.IncAtLevel(0);
	m_pRootNode=new CHeaderNode(&RootHeaderID);
	//m_pRootNode->View();

	//строим дерево
	//BuildTree(pBlockHIDCollection);
}

CHeadersTree::~CHeadersTree(void)
{
	if(m_pRootNode) delete m_pRootNode;
}

void CHeadersTree::AddNodes(vector<unsigned int>* pWordIDCollection, CFindResult::THIDCollection* pHIDCollection)
{
	if(ISNULL(pWordIDCollection)) return;
	if(ISNULL(pHIDCollection)) return;
	
	//обходим коллекцию вставляем узлы
	for(unsigned int i=0; i<pWordIDCollection->size(); i++){
		for(unsigned int k=0; k<pHIDCollection->size(); k++){
			//вставляем узел
			CLevelHeaderID HeaderID(pHIDCollection->at(k));
			//HeaderID.View();
			m_pRootNode->InsertHeaderNode(pWordIDCollection->at(i), &HeaderID);
		}
	}
}

//void CHeadersTree::BuildTree(CFindResult::TBlockHIDCollection* pBlockHIDCollection)
//{
//	if(ISNULL(pBlockHIDCollection)) return;
//	
//	//обходим коллекцию вставляем узлы
//	for(unsigned int i=0; i<pBlockHIDCollection->size(); i++){
//		if(pBlockHIDCollection->at(i)){
//			for(unsigned int k=0; k<pBlockHIDCollection->at(i)->size(); k++){
//				//вставляем узел
//				CLevelHeaderID HeaderID(pBlockHIDCollection->at(i)->at(k));
//				//HeaderID.View();
//				m_pRootNode->InsertHeaderNode(i, &HeaderID);
//			}
//		}
//	}
//}

bool CHeadersTree::MoveFirstBranch(void)
{
	m_CurrentBranch.clear();
	if(ISNULL(m_pRootNode)) return false;
	
	//позиционируемся на крайней ветке дерева
	CHeaderNode* pCurrentNode=m_pRootNode;
	while(pCurrentNode){
		pCurrentNode->MoveFirstChild();
		m_CurrentBranch.push_back(pCurrentNode);
		pCurrentNode=static_cast<CHeaderNode*>(pCurrentNode->GetCurrentChild());
	}

	return true;
}

bool CHeadersTree::MoveNextBranch(void)
{
	if(ISNULL(m_pRootNode)) return false;

	if(m_CurrentBranch.empty()) return false;
	//выкидываем последний узел
	m_CurrentBranch.pop_back();
	if(m_CurrentBranch.empty()) return false;

	//получаем бывший предпоследний узел
	CHeaderNode* pCurrentNode=*(--m_CurrentBranch.end());
	if(pCurrentNode->MoveNextChild()){
		//переходим к следущему ребенку, взводим его
		pCurrentNode=static_cast<CHeaderNode*>(pCurrentNode->GetCurrentChild());
		pCurrentNode->MoveFirstChild();
		m_CurrentBranch.push_back(pCurrentNode);
	}

	return true;
}

CHeaderNode* CHeadersTree::GetCurrentLastHeaderAndWordsIDs(
	CHeaderWords<Extracting::Logics::CHeaderLogic::MaxBlockCount>* pHeaderBranchWords)
{
	if(ISNULL(m_pRootNode)) return NULL;
	if(ISNULL(pHeaderBranchWords)) return NULL;
	if(m_CurrentBranch.empty()) return NULL;

	pHeaderBranchWords->Reset();
	//формируем коллекцию идентификаторов слов
	//обходим ветку и вытаскиваем идентификаторы слов
	for(unsigned int i=0; i<m_CurrentBranch.size(); i++){
		for(unsigned int k=0; k<m_CurrentBranch[i]->GetMyself()->GetMaxWordsNumber(); k++){
			if(m_CurrentBranch[i]->GetMyself()->IsWordExist(k)){
				pHeaderBranchWords->SetWordID(k);
			}
		}
	}	
	

	return m_CurrentBranch.back();
}

void CHeadersTree::ToString(wstring& wszValue)
{
	wszValue.assign(L"\n//--------Bof-CHeadersTree:\n");

	wchar_t buf[100];
	wsprintf(buf, L"DocumentIndex: %u\n", m_uiDocumentIndex);
	wszValue.append(buf);

	wstring wszTemp;
	wstring wszOffset;
	m_pRootNode->ToString(wszTemp, wszOffset);
	wszValue.append(wszTemp);

	wszValue.append(L"\n\n//--------Eof-CHeadersTree:\n");
}

//--------------------------------------------------------------------//


}
}
}
}
}
