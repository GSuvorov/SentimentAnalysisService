#include "StdAfx.h"
#include ".\index_storage_hdr.h"
#include "..\ASCInterface\defines.h"

USES_LOAD_MANAGER;

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

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Interface::Core::CommonContainers;

//--------------------------------------------------------------------//

CIndexStorageHeaders::CIndexStorageHeaders(INdxStorageWithStatistic* pNdxStorage)
:CIndexStatisticStorage(pNdxStorage), m_pHeadersInfoStorage(NULL), m_uiLastFilledDocumentIndex(EMPTY_VALUE)
{
	m_pHeadersInfoStorage=pNdxStorage->GetTextImagesStorage();

	//выставляем параметры кэша
	m_uiCashMaxSize=10000;
	m_uiCashIndexMinFreq=2;
}

CIndexStorageHeaders::~CIndexStorageHeaders(void)
{
}

void CIndexStorageHeaders::AddIndexes(ITextBlockCut* pTextBlockCut)
{
	if(ISNULL(pTextBlockCut)) return;
	
	//обновляем текущий идентификаор заголовка
	if(!m_DocumentHeadersInfo.IncCurrentHeaderID(pTextBlockCut->GetLevel()))
	{
		//если не можем увеличить идентификатор заголовка кидаем сообщение 
		//сообщаем в лог
		wchar_t buf[200];
		swprintf(buf, L"Max Headers count exceeded, next header will be lost!!! DocID: %u", 
			m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictTextAbsNumber));
		SAVE_LOG(SS_WARNING AND buf);
		return;
	}

	//дальше стандартно
	CIndexStatisticStorage::AddIndexes(pTextBlockCut);
}

void CIndexStorageHeaders::AddIndexationIndex(IIndexationIndex* pIndexationIndex)
{
	
	///обновляем позицию заголовка
	m_DocumentHeadersInfo.UpdateHeaderPosition(m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictWordInTextNumber));

	//--------------------------
	//добавляем индексную часть
	if(pIndexationIndex!=NULL){
		//кладем координаты в буфер
		unsigned int CoordBuffer[5];
		CoordBuffer[1]=m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictTextAbsNumber);
		CoordBuffer[4]=m_DocumentHeadersInfo.GetCurrentHeaderID()->GetID();

		//ToConsole(L"CoordBuffer[1]", CoordBuffer[1]);
		//ToConsole(L"HeaderID", CoordBuffer[4]);
		
		unsigned int uiIndex=pIndexationIndex->GetFirstIndex();
		if(ISNULL(uiIndex)) return;
		while(uiIndex){

			//ToConsole(L"uiIndex", uiIndex);

			if(!m_pLogIndexCoordinatesFileStream && m_bLogIndexCoordinates){
				//если выставлен флаг открываем лог-файл для индексов
				wstring wsLogFileName(L"./log_icoord_");
				wsLogFileName.append(GetIndexStorageName());
				wsLogFileName.append(L".txt");
				m_pLogIndexCoordinatesFileStream=_wfopen(wsLogFileName.c_str(), L"wt");
				if(ISNULL(m_pLogIndexCoordinatesFileStream)){
					TO_CONSOLE(L"Cann't open log file");
				}
			}

			//пишем в лог: индекс; номер текста; позиция в тексте
			if(m_pLogIndexCoordinatesFileStream)
				fwprintf(m_pLogIndexCoordinatesFileStream, L"%u; %u; %u;\n", uiIndex, CoordBuffer[1], CoordBuffer[4]);

			//если индекс есть добавляем его в кэш
			GetIndexCash()->Add(uiIndex, CoordBuffer, 5);
			//получаем следующий индекс на данной позиции
			uiIndex=pIndexationIndex->GetNextIndex();
		}
	}else{
		ISNULL(pIndexationIndex);
	}
	//--------------------------
}

void CIndexStorageHeaders::CompleteDocumentIndexing(void)
{
	m_DocumentHeadersInfo.View();

	//добавляем информацию по заголовкам
	unsigned int uiTextIndex=EMPTY_VALUE;
	CDocumentHeadersInfo::THeaderInfoCollection* pHeaderInfoCollection=m_DocumentHeadersInfo.GetHeaderInfoCollection();
	m_pHeadersInfoStorage->AppendWholeData((unsigned char*)&pHeaderInfoCollection->front(), 
		(unsigned int)pHeaderInfoCollection->size()*sizeof(SHeaderInfo), &uiTextIndex);
	m_DocumentHeadersInfo.Reset();

	//дальше стандартно
	CIndexStatisticStorage::CompleteDocumentIndexing();
}

void CIndexStorageHeaders::FillHeaderPositions(unsigned int uiDocumentIndex, 
	CLevelHeaderID* pLevelHeaderID, SHeaderPositions* pOutHeaderPositions)
{
	if(ISNULL(m_pHeadersInfoStorage)) return;
	if(ISNULL(pLevelHeaderID)) return;
	if(ISNULL(pOutHeaderPositions)) return;
	
	CDocumentHeadersInfo::THeaderInfoCollection* pHeaderInfoCollection=NULL;

	if(m_uiLastFilledDocumentIndex!=uiDocumentIndex){
		//смещаемся к данным узнаем требуемый размер буфера
		unsigned int uiHeadersBufferSize;
		m_pHeadersInfoStorage->MoveToAndGetDataSize(uiDocumentIndex, &uiHeadersBufferSize);
		
		//резервируем буфер читаем данные
		pHeaderInfoCollection=m_DocumentHeadersInfo.GetHeaderInfoCollection();
		pHeaderInfoCollection->resize(uiHeadersBufferSize/sizeof(SHeaderInfo));
		m_pHeadersInfoStorage->ReadWholeData((unsigned char*)&pHeaderInfoCollection->front(), uiHeadersBufferSize);	
		//обновляем индекс документа
		m_uiLastFilledDocumentIndex=uiDocumentIndex;
	}else{
		//данные уже в буфере считвать их не нужно
		pHeaderInfoCollection=m_DocumentHeadersInfo.GetHeaderInfoCollection();
	}

	//обходим коллекцию ищем нужный заголовок
	for(unsigned int i=0; i<pHeaderInfoCollection->size(); i++){
		if(pHeaderInfoCollection->at(i).m_HeaderID==(*pLevelHeaderID)){
			memcpy(pOutHeaderPositions, &pHeaderInfoCollection->at(i).m_HeaderPositions, sizeof(SHeaderPositions));
			return;
		}
	}

}

CDocumentHeadersInfo* CIndexStorageHeaders::FillHeadersInfo(unsigned int uiDocumentIndex)
{
	if(ISNULL(m_pHeadersInfoStorage)) return NULL;
	
	//смещаемся к данным узнаем требуемый размер буфера
	unsigned int uiHeadersBufferSize;
	m_pHeadersInfoStorage->MoveToAndGetDataSize(uiDocumentIndex, &uiHeadersBufferSize);
	
	//резервируем буфер читаем данные
	CDocumentHeadersInfo::THeaderInfoCollection* pHeaderInfoCollection=m_DocumentHeadersInfo.GetHeaderInfoCollection();
	pHeaderInfoCollection->resize(uiHeadersBufferSize/sizeof(SHeaderInfo));
	m_pHeadersInfoStorage->ReadWholeData((unsigned char*)&pHeaderInfoCollection->front(), uiHeadersBufferSize);	
	
	return &m_DocumentHeadersInfo;
}

void CIndexStorageHeaders::FillHeadersToTextBlock(ITextBlock* pTextBlock, 
		Containers::CIntervalAndHeader* pInterval, unsigned int uiGroupID)
{
	if(ISNULL(pTextBlock)) return;
	if(ISNULL(pInterval)) return;
	if(ISNULL(m_pHeadersInfoStorage)) return;
	
	unsigned int uiDocumentIndex=pTextBlock->GetTextFeature()->GetTextNumber();
	
	//int a=0;
	//if(uiDocumentIndex==17383){
	//	a=17383;
	//}

	CLevelHeaderID HeaderID(pInterval->m_HeaderID.GetID());
	list<CLevelHeaderID> HeaderIDList;
	while(!HeaderID.IsEmpty()){
		HeaderIDList.push_front(HeaderID);
		//получаем родительский заголовок
		HeaderID=HeaderID.MakeParentHeader();
	}

	list<CLevelHeaderID>::iterator itHeaderID=HeaderIDList.begin();
	while(itHeaderID!=HeaderIDList.end()){
		//получаем позиции заголовка
		SHeaderPositions HeaderPositions;
		FillHeaderPositions(uiDocumentIndex, &(*itHeaderID), &HeaderPositions);
		
		//наполняем его содержимым
		CInterval Interval(HeaderPositions.m_uiHeadPosition, HeaderPositions.m_uiTailPosition, pInterval->m_uiWeight);
		wstring wsIndexStorageName(GetIndexStorageName());
		ITextBlockCut* pTextBlockCut=GetTextsBinaryStorage()->FillTextBlock(pTextBlock, &Interval, &wsIndexStorageName);	
		pTextBlockCut->SetNumber(uiGroupID);
		pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
		pTextBlockCut->SetLevel(itHeaderID->GetHeaderLevel());

		itHeaderID++;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}