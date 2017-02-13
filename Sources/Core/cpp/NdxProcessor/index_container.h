#pragma once

#include ".\basis_containers.h"
#include ".\cnt_factory.h"
#include ".\index_container_nodes.h"
#include ".\coordinate_convertor.h"

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

///контайнер координат, содержит координаты индекса
///может считывать координаты согласно точке входа, создавать дополнительные контайнер для хранения координат
///координаты храняттся в разложенном виде, в зависимости от заданной структуры,
///части координат хранятся в узлах, сам контайнер содержит массив таких узлов
#pragma pack(push,1)  // Fix packing size 
template <unsigned char LevelsNumber>
class CCoordinateContainer_4i : public CValuesContainer, public CFactoryClient<CContainersFactory>
{
public:
	CCoordinateContainer_4i(CContainersFactory* pContainersFactory, unsigned int uiEntryPoint = EMPTY_VALUE)
	:CValuesContainer(uiEntryPoint), CFactoryClient<CContainersFactory>(pContainersFactory)
	{
		for(unsigned int i=0; i<LevelsNumber; i++){
			m_CoordinateNodes[i]=pContainersFactory->CreateCoordinateNode_4i(i);
		}
	}

	~CCoordinateContainer_4i(void)
	{
		for(unsigned int i=0; i<LevelsNumber; i++){
			delete m_CoordinateNodes[i];
		}
	}

	///добавляет значение к дереву
	bool AddCoordinate(unsigned int* pBuffer, unsigned int uiBufferSize)
	{
		if(ISNULL(GetFactory())) return false;
		if(ISNULL(GetFactory()->GetIndexStorage())) return false;
		if(ISNULL(pBuffer)) return false;

		unsigned int uiSplitCoord=0;
		bool bNewTextAdded=false;

		//в цикле превращаем содержимое буфера в ветвь дерева в зависимости от конфигурации уровней
		SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage::TCoordinateLevelInfo* pLevelCollection=GetFactory()->GetIndexStorage()->GetCoordinateLevelCollection();
		SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage::TCoordinateLevelInfo::iterator itLevelStorage=pLevelCollection->begin();
		SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo const* pLevelInfo=NULL;
		do{
			pLevelInfo=(*itLevelStorage);
			if(ISNULL(pLevelInfo)) break;
			//вытаскиваем из координаты кусочек для уровня
			uiSplitCoord=CCoordinateConvertor::SplitCoordinate(
				*(pBuffer+(unsigned int)(pLevelInfo->m_IndexCoordinateType)), 
				pLevelInfo->m_ucStartBit,
				pLevelInfo->m_ucBitPerLevel);
			
			//START_TICK(L"AddValue");
			//добавляем кусочек координаты в контайнер
			if(m_CoordinateNodes[pLevelInfo->m_ucLevelNumber]->AddCoordinate(uiSplitCoord)){
				//новое значение добавилось необходимо обновить, подчиненые узлы
				UpdateChildNodes(pLevelInfo->m_ucLevelNumber);			

				//если имеем уровень текста устанавливаем флаг
				if(pLevelInfo->m_eControlType==SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo::lctWords){
					bNewTextAdded=true;
				}
			}

			if(pLevelInfo->m_eControlType==SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo::lctWords){
				m_CoordinateNodes[LevelsNumber-1]->AddCoordinate(*(pBuffer+4));
			}
			//STOP_TICK(L"AddValue");
		}while(++itLevelStorage!=pLevelCollection->end());
		
		//View();
		
		return bNewTextAdded;
	}
	///считывает координаты по смещению
	void MoveUp(unsigned int uiOffset)
	{
		if(uiOffset==EMPTY_OFFSET){
			ERR_TO_CONSOLE(erCommon, L"Offset is empty!!!");
			return;
		}
		
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pCoordinatesStorage=GetCoordinatesStorage();
		if(ISNULL(pCoordinatesStorage)) return;
		
		//подымаем узлы от старших к младшим
		SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset CurrentOffset;
		CurrentOffset.ucFileIndex=0;
		CurrentOffset.uiOffset=uiOffset;
		unsigned char i=0; 
		unsigned int uiDataSize=0; 
		unsigned int uiTemp=0;
		do{
			//читаем узел в хранилище
			if(!pCoordinatesStorage->StartRead(&CurrentOffset, &uiDataSize)){
				TO_CONSOLE(L"StartRead failed!!!");
				return;
			}
			m_CoordinateNodes[i]->Read(pCoordinatesStorage);
			pCoordinatesStorage->StopRead(&uiTemp);

			//в хранилище он нам больше не нужен удаляем теперь уже старые данные
			pCoordinatesStorage->Delete(&CurrentOffset);
					
			if(i==LevelsNumber-2){
				//для предпоследнего заполняем последний узел
				m_CoordinateNodes[i]->PopLastChild(m_CoordinateNodes[i+1]);
			}else{
				//для обычного узла получаем смещение на следующий узел
				m_CoordinateNodes[i]->PopLastChildOffset(&CurrentOffset);
			}
		}while(++i<LevelsNumber-1);
		
		//ToConsole(L"Eof-MoveUp");
	}

	///пишет координаты, возвращает смещение 
	unsigned int MoveDown(void)
	{
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pCoordinatesStorage=GetCoordinatesStorage();
		if(ISNULL(pCoordinatesStorage)) return EMPTY_VALUE;

		//ToConsole(L"MoveDown");
		
		//начиная с самого последнего узла кладем их в хранилище
		SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset CurrentOffset;
		unsigned char i=LevelsNumber; 
		unsigned int uiTemp=0;
		do{
			--i;

			if(i==LevelsNumber-1){
				//последний узел сразу кладем в предпоследний
				m_CoordinateNodes[i-1]->AddChild(m_CoordinateNodes[i]);
				//сбрасываем содержимое узла для нового заполнения
				m_CoordinateNodes[i]->ResetNode();
				continue;
			}

			memset((void*)&CurrentOffset, 0xff, sizeof(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset));
			
			if(m_CoordinateNodes[i]->IsEmptyNode()){ 
				ERR_TO_CONSOLE(erCommon, L"Cann't write empty node")		
					return CurrentOffset.uiOffset;
			}

			//кладем узел в хранилище
			pCoordinatesStorage->StartUnCompleteDataAppend(&CurrentOffset);
			m_CoordinateNodes[i]->Write(pCoordinatesStorage);
			pCoordinatesStorage->StopAppend(&uiTemp);

			//смещение передаем родительскому узлу
			if(i!=0) m_CoordinateNodes[i-1]->AddChildOffset(&CurrentOffset);
			
			//сбрасываем содержимое узла для нового заполнения
			m_CoordinateNodes[i]->ResetNode();
		}while(i>0);

		//ToConsole(L"Eof-MoveDown");

		return CurrentOffset.uiOffset;
	}

	///обновляет значение коэффицента увеличения частоты, для конкретной координаты
	bool UpdateFreqIncreaseKoef(unsigned int* pBuffer, unsigned int uiBufferSize, unsigned char ucFreqIncreaseKoef){return false;};

	/////отображает содержимое объекта класса
	//void View(void);
	/////сериализация в строку
	//void ToString(wstring& szValue, wstring szOffset);

protected:
	///обнволяет подчиненный узла начиная с определнного уровня
	void UpdateChildNodes(unsigned char ucParentNodeLevel)
	{
		//ToConsole(L"UpdateChildNodes");
		if(ucParentNodeLevel>=LevelsNumber-1) return;
		
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pCoordinatesStorage=GetCoordinatesStorage();
		if(ISNULL(pCoordinatesStorage)) return;
		
		//начиная с самого последнего узла кладем их в хранилище
		SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset CurrentOffset;
		unsigned char i=LevelsNumber-1; 
		unsigned int uiTemp=0;
		do{
			//если узел пустой это первое добавление, ниче не сбрасываем 
			if(m_CoordinateNodes[i]->IsEmptyNode()) return;

			if(i==LevelsNumber-1){
				//последний узел сразу кладем в предпоследний
				m_CoordinateNodes[i-1]->AddChild(m_CoordinateNodes[i]);
				//сбрасываем содержимое узла для нового заполнения
				m_CoordinateNodes[i]->ResetNode();
				continue;
			}

			memset((void*)&CurrentOffset, 0xff, sizeof(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset));

			//кладем узел в хранилище
			pCoordinatesStorage->StartCompleteDataAppend(&CurrentOffset);
			m_CoordinateNodes[i]->Write(pCoordinatesStorage);
			pCoordinatesStorage->StopAppend(&uiTemp);

			//смещение передаем родительскому узлу
			m_CoordinateNodes[i-1]->AddChildOffset(&CurrentOffset);
			
			//сбрасываем содержимое узла для нового заполнения
			m_CoordinateNodes[i]->ResetNode();
		}while(--i!=ucParentNodeLevel);
		
		//ToConsole(L"Eof-UpdateChildNodes");
	}

	///возвращает хранилище координат
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* GetCoordinatesStorage(void){return GetFactory()->GetIndexStorage()->GetCoordinateStorage();};

	///массив координатных узлов,
	CCoordinateNode_4i* m_CoordinateNodes[LevelsNumber];
};
#pragma pack(pop)

typedef CCoordinateContainer_4i<3> TCoordinateContainer;

//--------------------------------------------------------------------//

}
}
}
}
}