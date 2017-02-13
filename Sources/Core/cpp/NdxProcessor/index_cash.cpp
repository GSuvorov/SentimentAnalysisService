#include "StdAfx.h"

#include ".\index_cash.h"
#include ".\index_storage.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Core::NdxSE::NdxProcessor::MemoryManagement;

//--------------------------------------------------------------------//

CIndexCash::CIndexCash(CContainersFactory* pContainersFactory, unsigned int uiCashMaxSize, unsigned int uiIndexFreqMinSize)
:CFactoryClient<CContainersFactory>(pContainersFactory),
m_uiCashMaxSize(uiCashMaxSize), m_uiIndexFreqMinSize(uiIndexFreqMinSize)
{
}

CIndexCash::~CIndexCash(void)
{
	if(ISNULL(GetFactory())) return;
	
	Clear();
}

void CIndexCash::Clear(void)
{
	//ToConsole(L"Start cash del");
	for(TIndexCollection::iterator itIndex=m_IndexCash.begin(); itIndex!=m_IndexCash.end(); itIndex++){
		if(itIndex->second!=NULL){
			delete itIndex->second;
		}
	}
	//ToConsole(L"Stop cash del");
	
	//START_TICK(L"m_IndexCash.clear");
	m_IndexCash.clear();
	//STOP_TICK(L"m_IndexCash.clear");
}

void  CIndexCash::View(void)
{
	ToConsole(L"//---IndexCash:");
	ToConsole(L"Indexes count", (unsigned int)m_IndexCash.size());
	ToConsole(L"//---End-of-IndexCash");
}

bool CIndexCash::Update(void)
{
	//ToConsole(L"CIndexCash::Update");

	//флаг переполнения кэша
	bool bCashOverflow=(m_IndexCash.size()>m_uiCashMaxSize);
	
	if(!bCashOverflow) return false;

	//обходим основы в кэше
	TIndexCollection::iterator itIndex=m_IndexCash.begin(); 
	while(itIndex!=m_IndexCash.end()){
		unsigned int freqKoef=itIndex->second->GetFreqKoef();
		itIndex->second->SetFreqKoef(0);
		
		if(bCashOverflow){
			//если кэш переполнился
			//сбрасываем координаты на диск
			//ToConsole(L"Index", itIndex->first);
			//itIndex->second->GetCoordinatesContainer()->View();
			
			if(freqKoef<m_uiIndexFreqMinSize){
				//ToConsole(L"Index", itIndex->first);
				//если индекс редко встречался 
				//сбрасываем все его координаяты на диск
				//START_TICK(L"MoveDownIndexInfo");
				MoveDownIndexInfo(itIndex);
				//STOP_TICK(L"MoveDownIndexInfo");
				//ToConsole(L"MoveDown");

				//и удаляем его из кэша
				delete itIndex->second;
				itIndex=m_IndexCash.erase(itIndex);
			}else{
				//для частого индекса сбрасываем заполненую часть координат на диск
				//и удаляем ее
				//itIndex->second->GetCoordinatesContainer()->MoveDownNotBoundTreePart();
				
				//переходим к следущей основе
				itIndex++;	
			}
		}else{
			//если кэш заполнился не до конца
			//сбрасываем completed-координаты в уровни
			//itIndex->second->GetCoordinatesContainer()->View();
			//itIndex->second->GetCoordinatesContainer()->MoveDownNotBoundTreePart();
			//itIndex->second->GetCoordinatesContainer()->View();

			//переходим к следущей основе
			itIndex++;	
		}
	}

	//ToConsole(L"Eof-CIndexCash::Update");

	return bCashOverflow;
}

bool CIndexCash::Add(unsigned int uiIndex, unsigned int* pCoordBuffer, unsigned int uiCoordBufferSize)
{
	if(ISNULL(pCoordBuffer)) return false;
	
	//ищем
	//START_TICK(L"m_IndexCash.find");
	TIndexCollection::iterator itIndex=m_IndexCash.find(uiIndex);
	if(itIndex==m_IndexCash.end()){
		START_TICK(L"MoveUpIndexInfo");
		itIndex=MoveUpIndexInfo(uiIndex);
		STOP_TICK(L"MoveUpIndexInfo");
	}
	//STOP_TICK(L"m_IndexCash.find");
	
	if(itIndex->second->GetCoordinatesContainer()->AddCoordinate(pCoordBuffer, uiCoordBufferSize)){
		//если вернуло true, значит добавился новый текст увеличиваем счетчик
		itIndex->second->OneMoreText();
	}

	//увеличиваем частотный счетчик
	itIndex->second->OneMoreIndex();
	

	return true;
}

CIndexCash::TIndexCollection::iterator CIndexCash::MoveUpIndexInfo(unsigned int uiIndex)
{
	if(uiIndex==EMPTY_SOURCE){ 
		TO_CONSOLE(L"Empty source index!");	
		return m_NullCollection.end();
	}

	//вставляем основу в кэш
	TIndexCollection::iterator itIndex=m_IndexCash.insert(TIndexCollection::value_type(uiIndex, (CIndexAttributes*)0)).first;
	itIndex->second=new CIndexAttributes(GetFactory());

	unsigned int uiIndexEntryPoint=EMPTY_OFFSET;
	unsigned int uiIndexFrequency=0;
	//достаем смещение основы
	GetFactory()->GetIndexStorage()->GetIndexEntryPointTable()->GetIndexData(uiIndex, &uiIndexEntryPoint, &uiIndexFrequency);

	//если есть смещение подымаем дерево основы с диска
	if(uiIndexEntryPoint!=EMPTY_OFFSET){
		//выставляем смещение на верхний уровень
		itIndex->second->SetIndexEntryPoint(uiIndexEntryPoint);
		//количество текстов в которых уже встречалась данная основа
		//itIndex->second->SetAppearedTextsNumber(uiIndexFrequency);
		itIndex->second->SetFreqInCollection(uiIndexFrequency);
		//подымаем край координатного дерева

		START_TICK(L"MoveUpTreeBoundPart");
		itIndex->second->GetCoordinatesContainer()->MoveUp(uiIndexEntryPoint);
		STOP_TICK(L"MoveUpTreeBoundPart");
		//ToConsole(L"MoveUp");
		//ToConsole(L"Index Index", itIndex->first);
		//ToConsole(L"Index Offset", uiIndexEntryPoint);
		//itIndex->second->GetCoordinatesContainer()->View();
	}
	
	return itIndex;
}

void CIndexCash::MoveDownIndexInfo(CIndexCash::TIndexCollection::iterator itIndex)
{
	//ToConsole(L"MoveDownIndexInfo Index", itIndex->first);

	//кладем дерево на диск по смещению 
	unsigned int uiIndexEntryPoint=itIndex->second->GetIndexEntryPoint();
	//itIndex->second->GetCoordinatesContainer()->View();
	//START_TICK(L"MoveDownTree");
	uiIndexEntryPoint=itIndex->second->GetCoordinatesContainer()->MoveDown();

	//STOP_TICK(L"MoveDownTree");
	//itIndex->second->GetCoordinatesContainer()->View();

	if(itIndex->second->GetIndexEntryPoint()==EMPTY_OFFSET && uiIndexEntryPoint!=EMPTY_OFFSET){
		//если смещения не было добавляем его
		GetFactory()->GetIndexStorage()->GetIndexEntryPointTable()->
			SetIndexEntryPoint(itIndex->first, uiIndexEntryPoint, 
				itIndex->second->GetFreqInCollection()/*GetAppearedTextsNumber()*/);
		itIndex->second->SetIndexEntryPoint(uiIndexEntryPoint);
	}else{
		//если смещение уже в базе надо только обновить частотный кэффициент индекса
		GetFactory()->GetIndexStorage()->
			GetIndexEntryPointTable()->ModifyIndexDataRecord(itIndex->first, uiIndexEntryPoint, 
				itIndex->second->GetFreqInCollection()/*GetAppearedTextsNumber()*/);
	}
}

void CIndexCash::Flush(void)
{
	//ToConsole(L"CIndexCash::Flush");

	for(TIndexCollection::iterator itIndex=m_IndexCash.begin(); itIndex!=m_IndexCash.end(); itIndex++){
		//ToConsole(L"Index", itIndex->first);
		//itIndex->second->GetCoordinatesContainer()->View();
		MoveDownIndexInfo(itIndex);
	}

	Clear();
	//ToConsole(L"Eof-CIndexCash::Flush");

}

//--------------------------------------------------------------------//

}
}
}
}
}