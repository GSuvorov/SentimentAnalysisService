#include "StdAfx.h"
#include ".\index_container_nodes_s.h"
#include ".\index_container_nodes.h"

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

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

void CCoordinateNode_4s::AddChildNode(unsigned int uiValue, CCoordinateNode_4s* pChild)
{
	if(ISNULL(pChild)) return;
	if(uiValue==EMPTY_VALUE){
		TO_CONSOLE(L"uiValue is empty");
		return;
	}
	AddChild(uiValue, pChild);
}

void CCoordinateNode_4s::ResetNode(void)
{
	DeleteChilds();
}

void CCoordinateNode_4s::ToString(wstring& szValue, wstring szOffset)
{
	szOffset.append(L" ");	
	szValue.append(L"\n");
	szValue.append(szOffset);
	szValue.append(L"-->");
	
	//выводим себя
	ToString(szValue);
	
	//затем детей
	if(MoveFirstChild()){
		do{
			((CCoordinateNode_4s*)GetCurrentChild())->ToString(szValue, szOffset);
		}while(MoveNextChild());
	}
}

CCoordinateNode_4s* CCoordinateNode_4s::MoveUpChildNode(unsigned int uiValue)
{
	SCompositeOffset Offset;
	CCoordinateNode_4s* pCoordinateNode=NULL;
	
	//в начале смотрим среди в коллекции детей
	pCoordinateNode=(CCoordinateNode_4s*)GetChild(uiValue);
	if(pCoordinateNode) return pCoordinateNode;

	//получаем узла смещение
	memset((void*)&Offset, 0xff, sizeof(SCompositeOffset));
	GetChildOffset(uiValue, &Offset);
	if(Offset.uiOffset==EMPTY_OFFSET) return pCoordinateNode;

	//создаем узел
	pCoordinateNode=GetFactory()->CreateCoordinateNode_4s(GetDepth()+1);
	
	if(ISNULL(pCoordinateNode)) return pCoordinateNode;

	//добавляем узел
	AddChildNode(uiValue, pCoordinateNode);

	//читаем узел
	ReadNode(pCoordinateNode, &Offset);
	
	return pCoordinateNode;
}

void CCoordinateNode_4s::ReadNode(CCoordinateNode_4s* pCoordinateNode, SCompositeOffset* pCompositeOffset)
{
	if(ISNULL(pCompositeOffset)) return;
	if(ISNULL(pCoordinateNode)) return;
	
	try{

	START_TICK(L"ReadNode");
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pCoordinatesStorage=GetCoordinatesStorage();
	if(ISNULL(pCoordinatesStorage)) return;
	
	unsigned int uiDataSize=0;
	//читаем узел из хранилища
	if(!pCoordinatesStorage->StartRead(pCompositeOffset, &uiDataSize)){
		TO_CONSOLE(L"StartRead failed!!!");
		return;
	}
	pCoordinateNode->Read(pCoordinatesStorage);
	pCoordinatesStorage->StopRead(&uiDataSize);
	STOP_TICK(L"ReadNode");
	
	//pCoordinateNode->View();

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"CCoordinateNode_4s::ReadNode");
		throw;
	}

}

//--------------------------------------------------------------------//

void CBmCoordinateNode_4s::GetChildOffset(unsigned int uiValue, SCompositeOffset* pCompositeOffset)
{
	if(ISNULL(pCompositeOffset)) return;

	TChildsOffsets::iterator itOffset=m_ChildsOffsets.find(uiValue);
	if(itOffset!=m_ChildsOffsets.end()){
		(*pCompositeOffset)=itOffset->second;
	}
}

void CBmCoordinateNode_4s::ResetNode(void)
{
	CBitMap_4s::Reset();
	CCoordinateNode_4s::ResetNode();
	m_ChildsOffsets.clear();
}

void CBmCoordinateNode_4s::Read(INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return;
	
	START_TICK(L"CBmCoordinateNode_4s::Read");

	//читаем битовую карту
	CBitMap_4s::Read(pNdxStorage);

	//читаем коллекцию смещений на подчиненные узлы
	unsigned int uiChildsDataSize=CCoordinateNode_4s::ReadCompressed(pNdxStorage);
	unsigned char* pChildsData=(unsigned char*)malloc(uiChildsDataSize);
	pNdxStorage->Read(pChildsData, uiChildsDataSize);

	//формируем коллекцию смещений
	SCompositeOffset Offset;
	unsigned char* pCurrPos=pChildsData;
	if(MoveFirstOrt()){
		do{
			//вытаскиваем смещение
			memcpy(&Offset, pCurrPos, sizeof(SCompositeOffset));
			pCurrPos+=sizeof(SCompositeOffset);
			
			//добавляем смещение в коллекцию
			m_ChildsOffsets.insert(TChildsOffsets::value_type(GetCurrentOrt(), Offset));
		}while(MoveNextOrt());
	}

	//освобождаем буфер
	free(pChildsData);
	
	STOP_TICK(L"CBmCoordinateNode_4s::Read");
}

//void CBmCoordinateNode_4s::Read(INdxLevelStorage* pNdxStorage)
//{
//	if(ISNULL(pNdxStorage)) return;
//	
//	START_TICK(L"CBmCoordinateNode_4s::Read");
//
//	//читаем битовую карту
//	CBitMap_4s::Read(pNdxStorage);
//
//	//читаем коллекцию смещений на подчиненные узлы
//	//читаем количество блоков
//	unsigned int uiBinaryBlocksNumber=CCoordinateNode_4s::ReadCompressed(pNdxStorage);
//	
//	//читаем размер крайнего блока
//	unsigned char ucLastBlockFilledSize; 
//	pNdxStorage->Read((unsigned char*)&ucLastBlockFilledSize, sizeof(unsigned char));
//	
//	//определяем общий бинарный размер
//	unsigned int uiTotalSize=(uiBinaryBlocksNumber-1)*cucBmCoordinateNode_BlockBufferSize+ucLastBlockFilledSize;
//	unsigned char* pOffsetsBuffer=(unsigned char*)malloc(uiTotalSize);
//
//	//читаем блоки с боксами, формируем список блоков
//	unsigned char ucBlockFilledSize=ucLastBlockFilledSize; 
//	unsigned char* pCurrPos=pOffsetsBuffer+uiTotalSize;
//	while(uiBinaryBlocksNumber){
//		pCurrPos-=ucBlockFilledSize;
//		//читаем содержимое блока
//		pNdxStorage->Read(pCurrPos, ucBlockFilledSize);
//		ucBlockFilledSize=cucBmCoordinateNode_BlockBufferSize;
//		--uiBinaryBlocksNumber;
//	}
//
//	//формируем коллекцию смещений
//	SCompositeOffset Offset;
//	pCurrPos=pOffsetsBuffer;
//	if(MoveFirstOrt()){
//		do{
//			//вытаскиваем смещение
//			memcpy(&Offset, pCurrPos, sizeof(SCompositeOffset));
//			pCurrPos+=sizeof(SCompositeOffset);
//			
//			//добавляем смещение в коллекцию
//			m_ChildsOffsets.insert(TChildsOffsets::value_type(GetCurrentOrt(), Offset));
//		}while(MoveNextOrt());
//	}
//
//	//освобождаем буфер
//	free(pOffsetsBuffer);
//	
//	STOP_TICK(L"CBmCoordinateNode_4s::Read");
//}

void CBmCoordinateNode_4s::ToString(wstring& szValue)
{
	CBitMap_4s::ToString(szValue);
}

void CBmCoordinateNode_4s::View(void)
{
	wstring szValue;
	szValue.append(L"\n//-------CBmCoordinateNode_4s\n");
	ToString(szValue);
	szValue.append(L"\n//-------Eof-CBmCoordinateNode_4s\n");
	ToConsole(szValue.c_str());
}

//--------------------------------------------------------------------//

void CBmCoordinateNode_4s_Heavy::Read(INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return;
	
	START_TICK(L"CBmCoordinateNode_4s_Heavy::Read");


	//START_TICK(L"CBmCoordinateNode_4s_Heavy::Read0");
	//читаем битовую карту
	CBitMap_4s::Read(pNdxStorage);
	//STOP_TICK(L"CBmCoordinateNode_4s_Heavy::Read0");

	//START_TICK(L"CBmCoordinateNode_4s_Heavy::Read1");
	//читаем подчиненные узлы
	unsigned int uiChildsDataSize=CCoordinateNode_4s::ReadCompressed(pNdxStorage);
	unsigned char* pChildsData=(unsigned char*)malloc(uiChildsDataSize);
	pNdxStorage->Read(pChildsData, uiChildsDataSize);
	//STOP_TICK(L"CBmCoordinateNode_4s_Heavy::Read1");
	
	//ToConsole(L"uiChildsDataSize", uiChildsDataSize);

	//START_TICK(L"CBmCoordinateNode_4s_Heavy::Fill");
	//формируем общую коллекцию узлов
	CCoordinateNode_4s* pCoordinateNode=NULL;
	unsigned char* pCurrPos=pChildsData;
	unsigned int uiReadedSize=0;
	if(MoveFirstOrt()){
		do{
			//создаем узел
			pCoordinateNode=GetFactory()->CreateCoordinateNode_4s(GetDepth()+1);
			if(ISNULL(pCoordinateNode)) return;

			//читаем узел
			uiReadedSize=pCoordinateNode->FromBinaryBuffer(pCurrPos, uiChildsDataSize);
			pCurrPos+=uiReadedSize;
			uiChildsDataSize-=uiReadedSize;
			//pCoordinateNode->View();

			//добавляем узел
			AddChildNode(GetCurrentOrt(), pCoordinateNode);
		}while(MoveNextOrt());
	}
	//STOP_TICK(L"CBmCoordinateNode_4s_Heavy::Fill");

	//освобождаем буфер
	free(pChildsData);
	
	STOP_TICK(L"CBmCoordinateNode_4s_Heavy::Read");
}

//--------------------------------------------------------------------//

}
}
}
}
}