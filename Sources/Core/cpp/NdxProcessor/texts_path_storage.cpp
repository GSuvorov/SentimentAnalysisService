#include "StdAfx.h"
#include ".\texts_path_storage.h"
#include ".\console.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

bool CStringService::StrEqual(const wchar_t* sz1, const wchar_t* sz2)
{
	if(ISNULL(sz1)) return false;
	if(ISNULL(sz2)) return false;

	int i = 0;

	wchar_t chbuf1[2];
	wchar_t chbuf2[2];

	chbuf1[1] = 0;
	chbuf2[1] = 0;

	do{
		chbuf1[0] = sz1[i];
		chbuf2[0] = sz2[i];
		_wcslwr(chbuf1);
		_wcslwr(chbuf2);


		if(chbuf1[0] != chbuf2[0]) return false;
		if(chbuf1[0] == 0) break;
		i++;
	}while(chbuf1[0] != 0);

	return true;
}

//--------------------------------------------------------------------//

CTextPathNode::CTextPathNode(const wchar_t* pMyPathChunk, unsigned int uiMyIndex)
:CVecTreeNode<STextPathChunk*>
(new STextPathChunk(pMyPathChunk, uiMyIndex))
{
}

CTextPathNode::~CTextPathNode(void){

}

CTextPathNode* CTextPathNode::AddChild(const wchar_t* pMyPathChunk, unsigned int uiMyIndex)
{
	TChilds::iterator itChild=m_Childs.begin();
	while(itChild!=m_Childs.end()){
		if(*itChild){
			//сравниваем строки
			if(CStringService::StrEqual((*itChild)->GetMyself()->m_pMyPathChunk, pMyPathChunk)){
				return static_cast<CTextPathNode*>(*itChild);
			}
		}
		itChild++;
	}
	
	//если совпадающих строк не было добавляем нового ребенка
	return static_cast<CTextPathNode*>(CVecTreeNode<STextPathChunk*>::AddChild(new CTextPathNode(pMyPathChunk, uiMyIndex)));
}

void CTextPathNode::MoveDown(SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* pNdxFile)
{
	if(ISNULL(pNdxFile)) return;

	//затем кладаем свое бренное тело
	STextPathChunk* pTextPathChunk=GetMyself();
	if(ISNULL(pTextPathChunk)) return;
	
	//запоминаем свое смещение
	pTextPathChunk->m_uiMyOffset=pNdxFile->GetDataUnitSize();
	
	
	//добавляем флаг удаления
	bool bDeleted=false;
	pNdxFile->Append((unsigned char*)&bDeleted, sizeof(unsigned char));

	unsigned int uiTemp=EMPTY_VALUE;
	//добавляем смещение на родителя
	if(GetParent())
		uiTemp=GetParent()->GetMyself()->m_uiMyOffset;
	pNdxFile->Append((unsigned char*)&uiTemp, sizeof(unsigned int));

	//добавляем смещение на детей
	uiTemp=EMPTY_VALUE;
	pNdxFile->Append((unsigned char*)&uiTemp, sizeof(unsigned int));

	//добавляем индекс куска
	pNdxFile->Append((unsigned char*)&pTextPathChunk->m_uiMyIndex, sizeof(unsigned int));

	//добавляем длину куска пути в байтах 
	uiTemp=(unsigned int)(wcslen(pTextPathChunk->m_pMyPathChunk)*sizeof(wchar_t));
	pNdxFile->Append((unsigned char*)&uiTemp, sizeof(unsigned int));
	//добавляем кусок пути
	pNdxFile->Append((unsigned char*)pTextPathChunk->m_pMyPathChunk, uiTemp);
	
	//сбрасываем детей
	vector<unsigned int> ChildsOffsets;
	ChildsOffsets.reserve(m_Childs.size());
	TChilds::iterator itChild=m_Childs.begin();
	while(itChild!=m_Childs.end()){
		if((*itChild)){
			((CTextPathNode*)(*itChild))->MoveDown(pNdxFile);
			//кладем в буфер смещение ребенка
			ChildsOffsets.push_back((*itChild)->GetMyself()->m_uiMyOffset);
		}
		itChild++;
	}
	
	if(!ChildsOffsets.empty()){
		//если есть валидные дети 

		//запоминаем смещение на детей
		unsigned int uiChildsOffset=pNdxFile->GetDataUnitSize();

		//добавляем количество детей
		uiTemp=(unsigned int)ChildsOffsets.size();
		pNdxFile->Append((unsigned char*)&uiTemp, sizeof(unsigned int));
		//добавляем буфер смещений детей
		pNdxFile->Append((unsigned char*)(&ChildsOffsets.front()), ((unsigned int)ChildsOffsets.size()*sizeof(unsigned int)));
		
		//пишем себе смещение на детей
		pNdxFile->SeekPos(pTextPathChunk->m_uiMyOffset+sizeof(unsigned int)+sizeof(unsigned char), SEEK_SET);
		pNdxFile->Write((unsigned char*)&uiChildsOffset, sizeof(unsigned int));
	}
}	

//--------------------------------------------------------------------//

CTextsPathsTreeStorage::CTextsPathsTreeStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* pNdxCashedFile)
:m_pNdxCashedFile(pNdxCashedFile), m_RootPathNode(L"Root")
{
}

CTextsPathsTreeStorage::~CTextsPathsTreeStorage(void)
{
}

void CTextsPathsTreeStorage::BuildTree(CTextsFieldsStorage* pTextsInfo)
{
	if(ISNULL(pTextsInfo)) return;

	if(m_pNdxCashedFile->IsOpen()){
		unsigned int uiTextsNumber=pTextsInfo->GetTextsNumber();
		wstring wszTextPath(L"");

		SS::Interface::Core::NdxSE::CNdxMetaField NdxMetaField(
			L"FileName", 
			SS::Interface::Core::DBMS::TYPE_WSTRING, 
			SS::Interface::Core::DBMS::MOD_COMMON);

		for(unsigned int uiTextIndex=0; uiTextIndex<uiTextsNumber; uiTextIndex++){
			//достаем путь по индексу текста
			pTextsInfo->GetTextInfo(uiTextIndex, &NdxMetaField);
			wszTextPath.assign(((wstring*)NdxMetaField.GetValue())->c_str());
			//ToConsole(wszTextPath.c_str());
			if(wszTextPath.size()!=0){
				//если путь есть добавляем его к дереву
				//рубим путь на куски, по которым потом можно будет проверять узлы
				vector<wstring> ChunksCollection;
				HackTextPathOnChunks((wchar_t*)wszTextPath.c_str(), &ChunksCollection);
				CTextPathNode* pCurrNode=&m_RootPathNode;
				
				//в цикле обходим куски путей, создаем узлы дерева путей
				for(unsigned int uiCurrChunkIndex=0; uiCurrChunkIndex<ChunksCollection.size(); uiCurrChunkIndex++){
					if(uiCurrChunkIndex==ChunksCollection.size()-1){
						//если кусок пути последний, создаем узел с индексом файла
						pCurrNode=pCurrNode->AddChild(ChunksCollection[uiCurrChunkIndex].c_str(), uiTextIndex);
					}else{
						pCurrNode=pCurrNode->AddChild(ChunksCollection[uiCurrChunkIndex].c_str());
					}
				}


			}
		}
		
		//бросаем созданное дерево на диск
		MoveDown();	
	}else{
		TO_CONSOLE(L"TextsPathsTree not opened!!!");
	}
}

void CTextsPathsTreeStorage::AddTextPath(unsigned int uiTextIndex, wchar_t* pszTextPath)
{
	if(ISNULL(pszTextPath)) return;
	if(uiTextIndex==EMPTY_VALUE){ 
		TO_CONSOLE(L"TextIndex is empty!!!");	
		return;
	}

	if(wcslen(pszTextPath)!=0){
		//если путь есть добавляем его к дереву
		//рубим путь на куски, по которым потом можно будет проверять узлы
		vector<wstring> ChunksCollection;
		HackTextPathOnChunks(pszTextPath, &ChunksCollection);
		CTextPathNode* pCurrNode=&m_RootPathNode;
		
		//в цикле обходим куски путей, создаем узлы дерева путей
		for(unsigned int uiCurrChunkIndex=0; uiCurrChunkIndex<ChunksCollection.size(); uiCurrChunkIndex++){
			if(uiCurrChunkIndex==ChunksCollection.size()-1){
				//если кусок пути последний, создаем узел с индексом файла
				pCurrNode=pCurrNode->AddChild(ChunksCollection[uiCurrChunkIndex].c_str(), uiTextIndex);
			}else{
				pCurrNode=pCurrNode->AddChild(ChunksCollection[uiCurrChunkIndex].c_str());
			}
		}


	}
}

void CTextsPathsTreeStorage::HackTextPathOnChunks(wchar_t* pszTextPath, vector<wstring>* pChunksCollection)
{
	bool bEmptyFileName=false;
	//определяем конец строки пути
	unsigned int ln = (unsigned int)wcslen(pszTextPath) - 1;
	if(pszTextPath[ln] == '/' || pszTextPath[ln] == '\\'){
		pszTextPath[ln]= 0 ;
		//устанавливаем флаг пустого имени файла
		bEmptyFileName=true;
	}

	wchar_t* buff=new wchar_t[wcslen(pszTextPath)+1];
	bool bCanBreakPath = true;
	unsigned int i=0;
	unsigned int k=0;

	//в цикле вытаскиваем куски пути и строим ветку дерева путей
	while(pszTextPath[i] != 0){
		if((pszTextPath[i] == '\\' || pszTextPath[i] == '/' /*|| pszTextPath[i] == '?' || pszTextPath[i] == '|'*/) && bCanBreakPath){

			//if(pszTextPath[i] == '?'){
			//	bCanBreakPath = false;
			//}

			//если есть признаки кусок пути собран
			if(k>0){
				//обрезаем строку
				buff[k] = 0;
				//добавляем очередной кусок
				pChunksCollection->push_back(buff);
				//инитим строку
				k = 0;
			}
		}
		else{
			//если нет признаков собираем кусок пути дальше
			buff[k] = pszTextPath[i];
			k++;
		}
		i++;
	}

	//обрезаем строку
	buff[k] = 0;
	
	//добавляем последний кусок
	pChunksCollection->push_back(buff);
	
	if(bEmptyFileName){
		//если установлен флаг пустого имени файла, ставим ему пробел
		pChunksCollection->push_back(L" ");
	}

	delete[] buff;
}

void CTextsPathsTreeStorage::MoveDown(void)
{
	if(m_pNdxCashedFile->IsOpen()){
		//очищаем то что было
		m_pNdxCashedFile->Clear();
		//сбрасываем на диск
		m_RootPathNode.MoveDown(m_pNdxCashedFile);	
		//сбрасываем буфера на диск
		m_pNdxCashedFile->Flush();
	}else{
		TO_CONSOLE(L"TextsPathsTree not opened!!!");
	}
}

void CTextsPathsTreeStorage::FillTextPathChunk(STextPathChunk* pTextPathChunk)
{
	if(ISNULL(pTextPathChunk)) return;

	if(m_pNdxCashedFile->IsOpen() && m_pNdxCashedFile->GetDataUnitSize()){
		if(pTextPathChunk->m_uiMyOffset==EMPTY_OFFSET){
			TO_CONSOLE(L"TextPathChunk offset not filled!!!");		
			return;
		}

		if(pTextPathChunk->m_pMyPathChunk){
			//если кусок путь был удаляем его
			delete[] pTextPathChunk->m_pMyPathChunk;
			pTextPathChunk->m_pMyPathChunk=NULL;
		}
		
		if(IsTextPathChunkDeleted(pTextPathChunk)){ 
			//если узел удален ничего не заполняем по нему
			return;
		}

		//смещаемся к месту где записаны данные
		unsigned int uiDataOffset=pTextPathChunk->m_uiMyOffset+(sizeof(unsigned int)*2)+sizeof(unsigned char);
		m_pNdxCashedFile->SeekPos(uiDataOffset, SEEK_SET);

		//читаем идентификатор узла
		m_pNdxCashedFile->Read((unsigned char*)&pTextPathChunk->m_uiMyIndex, sizeof(unsigned int));

		//читаем длину куска пути в байтах
		unsigned int uiTemp=0;
		m_pNdxCashedFile->Read((unsigned char*)&uiTemp, sizeof(unsigned int));

		//читаем кусок пути
		unsigned int uiTextPathChunkSize=uiTemp/sizeof(wchar_t);
		pTextPathChunk->m_pMyPathChunk=new wchar_t[uiTextPathChunkSize+1];
		m_pNdxCashedFile->Read((unsigned char*)pTextPathChunk->m_pMyPathChunk, uiTemp);
		*((wchar_t*)(pTextPathChunk->m_pMyPathChunk)+uiTextPathChunkSize)=0;
	}else{
		TO_CONSOLE(L"TextsPathsTree not opened or empty!!!");
	}
}

void CTextsPathsTreeStorage::GetChildsOffsets(STextPathChunk* pTextPathChunk, vector<unsigned int>* pChildsOffsetsCollection)
{
	if(ISNULL(pTextPathChunk)) return;
	if(ISNULL(pChildsOffsetsCollection)) return;

	if(m_pNdxCashedFile->IsOpen() && m_pNdxCashedFile->GetDataUnitSize()){
		if(pTextPathChunk->m_uiMyOffset==EMPTY_OFFSET){
			TO_CONSOLE(L"TextPathChunk offset not filled!!!");		
			return;
		}

		//смещаемся к месту где записаны данные
		unsigned int uiChildsOffsetPos=pTextPathChunk->m_uiMyOffset+sizeof(unsigned int)+sizeof(unsigned char);
		m_pNdxCashedFile->SeekPos(uiChildsOffsetPos, SEEK_SET);

		//читаем смещение на смещения детей
		unsigned int uiChildsOffset=EMPTY_OFFSET;
		m_pNdxCashedFile->Read((unsigned char*)&uiChildsOffset, sizeof(unsigned int));
		if(uiChildsOffset!=EMPTY_OFFSET){
			//смещаемся к месту где записаны смещения на детей
			m_pNdxCashedFile->SeekPos(uiChildsOffset, SEEK_SET);
					
			//читаем количество детей
			unsigned int uiChildsNumber=0;
			m_pNdxCashedFile->Read((unsigned char*)&uiChildsNumber, sizeof(unsigned int));
			
			pChildsOffsetsCollection->assign(uiChildsNumber, EMPTY_OFFSET);
			//читаем смещения на детей
			m_pNdxCashedFile->Read((unsigned char*)(&(pChildsOffsetsCollection->front())), (uiChildsNumber*sizeof(unsigned int)));
		}

	}else{
		TO_CONSOLE(L"TextsPathsTree not opened or empty!!!");
	}
}

void CTextsPathsTreeStorage::GetParentOffset(STextPathChunk* pTextPathChunk, unsigned int* pParentOffset)
{
	if(ISNULL(pTextPathChunk)) return;
	if(ISNULL(pParentOffset)) return;

	if(m_pNdxCashedFile->IsOpen() && m_pNdxCashedFile->GetDataUnitSize()){
		if(pTextPathChunk->m_uiMyOffset==EMPTY_OFFSET){
			TO_CONSOLE(L"TextPathChunk offset not filled!!!");		
			return;
		}

		//смещаемся к месту где записаны данные
		unsigned int uiParentOffset=pTextPathChunk->m_uiMyOffset+sizeof(unsigned char);
		m_pNdxCashedFile->SeekPos(uiParentOffset, SEEK_SET);
		//читаем смещение на родителя		
		m_pNdxCashedFile->Read((unsigned char*)pParentOffset, sizeof(unsigned int));

	}else{
		TO_CONSOLE(L"TextsPathsTree not opened or empty!!!");
	}
}

void CTextsPathsTreeStorage::DeleteTextPathChunk(STextPathChunk* pTextPathChunk, std::vector<unsigned int>* pDeletedTextsNumbers)
{
	if(ISNULL(pTextPathChunk)) return;

	if(m_pNdxCashedFile->IsOpen() && m_pNdxCashedFile->GetDataUnitSize()){
		if(pTextPathChunk->m_uiMyOffset==EMPTY_OFFSET){
			TO_CONSOLE(L"TextPathChunk offset not filled!!!");		
			return;
		}

		//теперь тоже самое с детьми

		//получаем смещения детей
		vector<unsigned int> ChildsOffsets;
		STextPathChunk TextPathChunk(NULL, EMPTY_VALUE);
		GetChildsOffsets(pTextPathChunk, &ChildsOffsets);

		if(!ChildsOffsets.empty()){
			//если есть дети в цикле обходим и удаляем детей по смещениям
			for(unsigned int i=0; i<ChildsOffsets.size(); i++){
				TextPathChunk.m_uiMyOffset=ChildsOffsets[i];
				//удаляем кусок текста
				DeleteTextPathChunk(&TextPathChunk, pDeletedTextsNumbers);
			}
		}else{
			//если детей нет получаем инфу по узлу
			FillTextPathChunk(pTextPathChunk);
			if(pTextPathChunk->m_uiMyIndex!=EMPTY_VALUE && pDeletedTextsNumbers){
				//если это текст вносим его в массив удаленных текстов
				pDeletedTextsNumbers->push_back(pTextPathChunk->m_uiMyIndex);
			}
		}
		
		//в любом случае удаляем себя
		//смещаемся к месту где записаны данные
		m_pNdxCashedFile->SeekPos(pTextPathChunk->m_uiMyOffset, SEEK_SET);
		//удаляем типа (выставляем флаг удаления)
		bool bDeleted=true;
		m_pNdxCashedFile->Write((unsigned char*)&bDeleted, sizeof(unsigned char));

	}else{
		TO_CONSOLE(L"TextsPathsTree not opened or empty!!!");
	}
}

bool CTextsPathsTreeStorage::IsTextPathChunkDeleted(STextPathChunk* pTextPathChunk)
{
	if(ISNULL(pTextPathChunk)) return false;

	if(m_pNdxCashedFile->IsOpen() && m_pNdxCashedFile->GetDataUnitSize()){
		if(pTextPathChunk->m_uiMyOffset==EMPTY_OFFSET){
			TO_CONSOLE(L"TextPathChunk offset not filled!!!");		
			return false;
		}

		//смещаемся к месту где записаны данные
		m_pNdxCashedFile->SeekPos(pTextPathChunk->m_uiMyOffset, SEEK_SET);
		//читаем флаг удаления
		bool bDeleted=true;
		m_pNdxCashedFile->Read((unsigned char*)&bDeleted, sizeof(unsigned char));
		//возвращаем его
		return bDeleted;
	}else{
		TO_CONSOLE(L"TextsPathsTree not opened or empty!!!");
		return false;
	}
}

bool CTextsPathsTreeStorage::IsTextPathExist(const wchar_t* pszTextPath)
{
	if(ISNULL(pszTextPath)) return false;

	if(m_pNdxCashedFile->IsOpen() && m_pNdxCashedFile->GetDataUnitSize()){
		//рубим путь на куски, по которым потом можно будет проверять узлы
		vector<wstring> ChunksCollection;
		HackTextPathOnChunks((wchar_t*)pszTextPath, &ChunksCollection);

		vector<unsigned int> ChildsOffsets;
		STextPathChunk RootTextPathChunk(NULL, 0);
		STextPathChunk CurrTextPathChunk(NULL, 0);
		RootTextPathChunk.m_uiMyOffset=0;
		bool bStopChecking=true;

		for(unsigned int uiCurrChunkIndex=0; uiCurrChunkIndex<ChunksCollection.size(); uiCurrChunkIndex++){
			//получаем смещения детей для текущего узла
			GetChildsOffsets(&RootTextPathChunk, &ChildsOffsets);
			//устанавливаем флаг прерывания проверки
			bStopChecking=true;
			for(vector<unsigned int>::iterator itCurrChildOffset=ChildsOffsets.begin(); itCurrChildOffset<ChildsOffsets.end(); itCurrChildOffset++){
				//устанавливаем смещение
				CurrTextPathChunk.m_uiMyOffset=(*itCurrChildOffset);
				//читаем кусок пути ребенка
				FillTextPathChunk(&CurrTextPathChunk);
				//проверяем куски путей на равенство
				if(CurrTextPathChunk.m_pMyPathChunk && CStringService::StrEqual(ChunksCollection[uiCurrChunkIndex].c_str(), CurrTextPathChunk.m_pMyPathChunk)){
					//если пути равну переходим к слудующему куску
					//делаем текущий кусок рутовым
					RootTextPathChunk.m_uiMyOffset=CurrTextPathChunk.m_uiMyOffset;
					//сбрасываем флаг прерывания проверки
					bStopChecking=false;
					break;
				}
			}
			if(bStopChecking){
				//если проверка прошла неудачно выходим
				return false;
			}
		}

		//путь совпал по всем кускам
		return true;

	}else{
		TO_CONSOLE(L"TextsPathsTree not opened or empty!!!");
		return false;
	}
}

void CTextsPathsTreeStorage::DeleteTextByPath(const wchar_t* pszTextPath, std::vector<unsigned int>* pDeletedTextsNumbers)
{
	if(ISNULL(pszTextPath)) return;

	if(m_pNdxCashedFile->IsOpen() && m_pNdxCashedFile->GetDataUnitSize()){
		//рубим путь на куски, по которым потом можно будет проверять узлы
		vector<wstring> ChunksCollection;
		HackTextPathOnChunks((wchar_t*)pszTextPath, &ChunksCollection);

		vector<unsigned int> ChildsOffsets;
		STextPathChunk RootTextPathChunk(NULL, 0);
		STextPathChunk CurrTextPathChunk(NULL, 0);
		RootTextPathChunk.m_uiMyOffset=0;
		bool bStopChecking=true;

		for(unsigned int uiCurrChunkIndex=0; uiCurrChunkIndex<ChunksCollection.size(); uiCurrChunkIndex++){
			//получаем смещения детей для текущего узла
			GetChildsOffsets(&RootTextPathChunk, &ChildsOffsets);
			//устанавливаем флаг прерывания проверки
			bStopChecking=true;
			for(vector<unsigned int>::iterator itCurrChildOffset=ChildsOffsets.begin(); itCurrChildOffset<ChildsOffsets.end(); itCurrChildOffset++){
				//устанавливаем смещение
				CurrTextPathChunk.m_uiMyOffset=(*itCurrChildOffset);
				//читаем кусок пути ребенка
				FillTextPathChunk(&CurrTextPathChunk);
				//проверяем куски путей на равенство
				if(CurrTextPathChunk.m_pMyPathChunk && CStringService::StrEqual(ChunksCollection[uiCurrChunkIndex].c_str(), CurrTextPathChunk.m_pMyPathChunk)){
					//если пути равну переходим к слудующему куску
					//делаем текущий кусок рутовым
					RootTextPathChunk.m_uiMyOffset=CurrTextPathChunk.m_uiMyOffset;
					//сбрасываем флаг прерывания проверки
					bStopChecking=false;
					break;
				}
			}

			if(bStopChecking){
				//если проверка прошла неудачно выходим
				return;
			}
		}

		//путь совпал по всем кускам, удаляем его
		DeleteTextPathChunk(&CurrTextPathChunk, pDeletedTextsNumbers);

	}else{
		TO_CONSOLE(L"TextsPathsTree not opened or empty!!!");
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}