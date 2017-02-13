#include "StdAfx.h"
#include ".\index_storage_tbl.h"
#include ".\sr_filler.h"
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

using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CIndexStorageTables::CIndexStorageTables(INdxStorageWithStatistic* pNdxStorage)
	:CIndexStatisticStorage(pNdxStorage), m_uiLastFilledDocumentIndex(EMPTY_VALUE), 
	m_uiLastFilledTableIndex(EMPTY_VALUE), m_uiLastFilledRowIndex(EMPTY_VALUE)
{
	m_pTablesInfoStorage=pNdxStorage->GetTextImagesStorage();

	//выставляем параметры кэша
	m_uiCashMaxSize=10000;
	m_uiCashIndexMinFreq=2;
}

void CIndexStorageTables::AddTableIndexes(IIndexationResult* pIndexationResult)
{
	if(ISNULL(pIndexationResult)) return;

	ITextBlock* pTextBlock=NULL;
	ITextBlockCut* pTextBlockCut=NULL;
	IIndexAndPosition* pIndexAndPosition=NULL;
	unsigned int uiCellsCount=EMPTY_VALUE;
	
	//пустой счетчик устанавливаем в ноль
	if(m_CurrentTrcID.IsEmpty()) m_CurrentTrcID.SetIDByType(Containers::CTrcID::etrcTable, 0);
	
	//добавляем смещения на строки таблицы
	m_TablesOffsets.push_back(Containers::STableInfo());
	m_TablesOffsets.back().m_uiCellsOffset=(unsigned int)m_CellPositions.size();
	
	pTextBlock=pIndexationResult->GetFirstTextBlock();
	while(pTextBlock){
		
		//получаем количество катов
		unsigned int uiTBCCount=GetTextBlockCutCount(pTextBlock);
		if(uiTBCCount==EMPTY_VALUE){
			//если пустая строка переходим к следующему блоку
			pTextBlock=pIndexationResult->GetNextTextBlock();
			continue;
		}

		if(uiCellsCount==EMPTY_VALUE){
			//!!!закладываемся на то, что таблица прямоугольная количество
			//ячеек в строке должно быть постоянным, пустые строки в базу не заносятся

			//для первой строки, устанавливаем значение
			uiCellsCount=uiTBCCount;
			//определяем количество ячеек строки таблицы
			m_TablesOffsets.back().m_uiColumnCount=uiCellsCount;
		}else{
			//если не первая строка, сравниваем на количстов ячеек
			//если оно не совпадает не индексируем эту строку
			if(uiCellsCount!=uiTBCCount){
				//сообщаем в лог
				wchar_t buf[200];
				swprintf(buf, L"Cell count in table not equal. \n\tDocID: %u \n\tTableID: %u \n\tRowID: %u \n\tCellCount: %u \n\tNeeded CellCount: %u", 
					m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictTextAbsNumber), 
					m_CurrentTrcID.GetIDByType(Containers::CTrcID::etrcTable),
					m_CurrentTrcID.GetIDByType(Containers::CTrcID::etrcRow), 
					uiTBCCount, uiCellsCount);
				SAVE_LOG(SS_WARNING AND buf);
				//переходим к следующему блоку
				pTextBlock=pIndexationResult->GetNextTextBlock();
				continue;
			}
		}
		
		//если строка удовлетворяет условиям индексируем ее
		pTextBlockCut=pTextBlock->GetFirstBlockCut();
		while(pTextBlockCut){
			//кладем стартовую позицию ячейки
			m_CellPositions.push_back(EMPTY_VALUE);

			//отображаем блоккат
			//Extracting::CSearchResultFiller::ViewTextBlockCut(pTextBlockCut, true, true);
				
			AddIndexes(pTextBlockCut);
			
			//один TextBlockCut, это одна ячейка увеличиваем счетчик
			m_CurrentTrcID.IncIDByType(Containers::CTrcID::etrcCell);
			//переходим к следующему куску
			pTextBlockCut=pTextBlock->GetNextBlockCut();
		}
		
		//в конец строки добавляем позицию после последнего слова
		m_CellPositions.push_back(m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictWordInTextNumber)+1);

		//один TextBlock, это одна строка увеличиваем счетчик
		m_CurrentTrcID.IncIDByType(Containers::CTrcID::etrcRow);
		//переходим к следующему блоку
		pTextBlock=pIndexationResult->GetNextTextBlock();
	}
	
	if(m_TablesOffsets.back().m_uiColumnCount==EMPTY_VALUE){
		//если в таблице определено количество колонок, не заносим это таблицу	
		//она пустая
		m_TablesOffsets.pop_back();

		wchar_t buf[200];
		swprintf(buf, L"Empty table!!!. \n\tDocID: %u \n\tTableID: %u", 
			m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictTextAbsNumber), 
			m_CurrentTrcID.GetIDByType(Containers::CTrcID::etrcTable));
		SAVE_LOG(SS_WARNING AND buf);
	}else{
		//один IndexationResult это таблица, увеличиваем счетчик
		m_CurrentTrcID.IncIDByType(Containers::CTrcID::etrcTable);
	}
}

unsigned int CIndexStorageTables::GetTextBlockCutCount(ITextBlock* pTextBlock)
{
	if(ISNULL(pTextBlock)) return 0;
	ITextBlockCut* pTextBlockCut=pTextBlock->GetFirstBlockCut();
	unsigned int uiInc=0;
	bool bEmptyRow=true;
	while(pTextBlockCut){
		uiInc++;
		if(pTextBlockCut->GetFirstIndexAndPosition()!=NULL) bEmptyRow=false;
		pTextBlockCut=pTextBlock->GetNextBlockCut();
	}

	//для пустой строки устнавливаем пустое количество.
	if(bEmptyRow) uiInc=EMPTY_VALUE;
	return uiInc;		
}

void CIndexStorageTables::AddIndexationIndex(IIndexationIndex* pIndexationIndex)
{
	if(m_CellPositions.back()==EMPTY_VALUE){
		m_CellPositions.back()=m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictWordInTextNumber);
	}

	//ToConsole(L"m_CellPositions.back()", m_CellPositions.back());

	//--------------------------
	//добавляем индексную часть
	if(pIndexationIndex!=NULL){
		//кладем координаты в буфер
		unsigned int CoordBuffer[5];
		CoordBuffer[1]=m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictTextAbsNumber);
		CoordBuffer[4]=m_CurrentTrcID.GetTrcID();

		//ToConsole(L"CoordBuffer[1]", CoordBuffer[1]);
		//m_CurrentTrcID.View();
		
		unsigned int uiIndex=pIndexationIndex->GetFirstIndex();
		if(ISNULL(uiIndex)) return;
		while(uiIndex){

			//ToConsole(L"uiIndex", uiIndex);

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

void CIndexStorageTables::CompleteDocumentIndexing(void)
{
	//кладем в хранилище позиции таблиц

	//добавляем смещение на несуществущую таблицу 
	//на чтобы можно было посчитать размер крайне таблицы
	m_TablesOffsets.push_back(Containers::STableInfo());
	m_TablesOffsets.back().m_uiCellsOffset=(unsigned int)m_CellPositions.size();

	//пишем смещения на таблицы
	unsigned int uiTablesOffsetsSize=(unsigned int)m_TablesOffsets.size()*sizeof(Containers::STableInfo);
	m_pTablesInfoStorage->AppendDataChunk((unsigned char*)&uiTablesOffsetsSize, sizeof(unsigned int));
	m_pTablesInfoStorage->AppendDataChunk((unsigned char*)&m_TablesOffsets.front(), uiTablesOffsetsSize);
	
	//пишем позиции строк таблиц
	m_pTablesInfoStorage->AppendDataChunk((unsigned char*)&m_CellPositions.front(), (unsigned int)m_CellPositions.size()*sizeof(unsigned int));

	//завершаем добавление
	unsigned int uiTextIndex=EMPTY_VALUE;
	m_pTablesInfoStorage->AppendDataChunkComplete(&uiTextIndex);
	m_TablesOffsets.clear();
	m_CellPositions.clear();

	//сбрасываем счетчик
	m_CurrentTrcID.Reset();
	//дальше стандартно
	CIndexStatisticStorage::CompleteDocumentIndexing();
}

void CIndexStorageTables::FillTablePositions(unsigned int uiDocumentIndex, unsigned int uiTableIndex, unsigned int uiRowIndex)
{
	if(ISNULL(m_pTablesInfoStorage)) return;
	
	unsigned int uiTablesOffsetsSize=0;

	if(m_uiLastFilledDocumentIndex!=uiDocumentIndex){ 
		//если индекс документа изменился, читаем базовые данные

		//смещаемся к данным узнаем требуемый размер буфера
		unsigned int uiBufferSize;
		m_pTablesInfoStorage->MoveToAndGetDataSize(uiDocumentIndex, &uiBufferSize);
		
		//резервируем буфер читаем смещения на таблицы
		m_pTablesInfoStorage->ReadDataChunk((unsigned char*)&uiTablesOffsetsSize, sizeof(unsigned int));
		m_TablesOffsets.resize(uiTablesOffsetsSize/sizeof(Containers::STableInfo));
		m_pTablesInfoStorage->ReadDataChunk((unsigned char*)&m_TablesOffsets.front(), uiTablesOffsetsSize);
		
		//обновляем индекс документа
		m_uiLastFilledDocumentIndex=uiDocumentIndex;
		m_uiLastFilledTableIndex=EMPTY_VALUE;
		m_uiLastFilledRowIndex=EMPTY_VALUE;
	}

	if(m_uiLastFilledTableIndex!=uiTableIndex){
		//если индекс таблицы изменился, читаем 
		//резервируем буфер согласно числу колонок таблицы

		if(uiTableIndex>=m_TablesOffsets.size()-1){
			TO_CONSOLE(L"Invalid table index!!!");
			ToConsole(L"uiTableIndex", uiTableIndex);
			ToConsole(L"DocumentIndex", m_uiLastFilledDocumentIndex);
			return;
		}

		//резевируем буфер под строку + 1, чтобы определить конец
		m_CellPositions.resize(m_TablesOffsets[uiTableIndex].m_uiColumnCount+1);
		
		//обновляем индекс таблицы
		m_uiLastFilledTableIndex=uiTableIndex;
		m_uiLastFilledRowIndex=EMPTY_VALUE;
	}

	if(m_uiLastFilledRowIndex!=uiRowIndex){
		//если индекс строки изменился, читаем позиции ячеек заново
		//m_TablesOffsets[uiTableIndex].View();
		
		unsigned int uiRowsCount=m_TablesOffsets[uiTableIndex].GetRowsCount(m_TablesOffsets[uiTableIndex+1].m_uiCellsOffset);

		if(uiRowIndex>=uiRowsCount){
			TO_CONSOLE(L"Invalid uiRowIndex index!!!");
			ToConsole(L"uiTableIndex", uiTableIndex);
			ToConsole(L"DocumentIndex", m_uiLastFilledDocumentIndex);
			ToConsole(L"uiRowIndex", uiRowIndex);
			ToConsole(L"uiRowsCount", uiRowsCount);
			return;
		}

		//смещаем к позициям требуемой строки
		m_pTablesInfoStorage->SeekInData(sizeof(unsigned int)+
			((unsigned int)m_TablesOffsets.size()*sizeof(Containers::STableInfo))+
			(m_TablesOffsets[uiTableIndex].m_uiCellsOffset*sizeof(unsigned int))+
			(uiRowIndex*(m_TablesOffsets[uiTableIndex].m_uiColumnCount+1)*sizeof(unsigned int)));

		//читаем позиции строк таблицы
		m_pTablesInfoStorage->ReadDataChunk((unsigned char*)&m_CellPositions.front(), (unsigned int)m_CellPositions.size()*sizeof(unsigned int));
		
		//обновляем индекс строки
		m_uiLastFilledRowIndex=uiRowIndex;
	}
}

unsigned int CIndexStorageTables::GetCellTailPosition(unsigned int uiCellIndex)
{
	unsigned int uiCellPosition=EMPTY_VALUE;
	if(uiCellIndex<m_CellPositions.size()-1 || uiCellIndex==EMPTY_VALUE){
		//переходим к следующей ячейке и ищем валидную позицию
		uiCellIndex++;
		do{
			if(m_CellPositions[uiCellIndex]!=EMPTY_VALUE){
				uiCellPosition=m_CellPositions[uiCellIndex]-1;
				break;
			}
		}while(++uiCellIndex<m_CellPositions.size());
	}
	return uiCellPosition;
}

void CIndexStorageTables::FillTablePositions(unsigned int uiDocumentIndex, 
	unsigned int uiTableIndex, Containers::STablePositions* pOutTablePositions, unsigned int uiHeadRowIndex, unsigned int uiTailRowIndex)
{
	if(ISNULL(m_pTablesInfoStorage)) return;
	if(ISNULL(pOutTablePositions)) return;
	
	//проверяем на пустый координаты таблицы
	if(uiTableIndex==EMPTY_VALUE || uiHeadRowIndex==EMPTY_VALUE){
		pOutTablePositions->m_uiHeadPosition=EMPTY_VALUE;
		pOutTablePositions->m_uiTailPosition=EMPTY_VALUE;
		return;
	}

	if(uiTailRowIndex==EMPTY_VALUE) uiTailRowIndex=(unsigned int)m_CellPositions.size()-1;
	
	if(uiHeadRowIndex>uiTailRowIndex){
		TO_CONSOLE(L"Invalid rows indexes!!!");
		ToConsole(L"uiHeadRowIndex", uiHeadRowIndex);
		ToConsole(L"uiTailRowIndex", uiTailRowIndex);
		return;
	}

	//заполняем позицию головной строки
	FillTablePositions(uiDocumentIndex, uiTableIndex, uiHeadRowIndex);
	//определяем позицию первого слова первой ячейки строки
	pOutTablePositions->m_uiHeadPosition=GetCellTailPosition(EMPTY_VALUE);
	
	//заполняем хвостовую позицию
	if(uiHeadRowIndex!=uiTailRowIndex){
		//если строка другая заполняем буфера заново
		FillTablePositions(uiDocumentIndex, uiTableIndex, uiTailRowIndex);
	}

	//определяем позицию последнего слова последней ячейки строки
	pOutTablePositions->m_uiTailPosition=GetCellTailPosition(m_TablesOffsets[m_uiLastFilledTableIndex].m_uiColumnCount-1);
}

void CIndexStorageTables::FillTablesToTextBlock(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock, 
		Containers::CInterval* pInterval, unsigned int uiGroupID)
{
	if(ISNULL(pTextBlock)) return;
	if(ISNULL(pInterval)) return;
	if(ISNULL(m_pTablesInfoStorage)) return;

	//!!!результатом поиска по таблице являются индекс таблицы и строки расположенные в контайнере интервала

	try{

	ITextBlockCut* pTextBlockCut=NULL;
	
	//получаем индекс документа
	unsigned int uiDocumentIndex=pTextBlock->GetTextFeature()->GetTextNumber();

	//проверяем на пустый координаты таблицы
	if(pInterval->m_uiHeadPos==EMPTY_VALUE){
		return;
	}

	//ToConsole(L"uiDocumentIndex", uiDocumentIndex);
	//ToConsole(L"TableIndex", pInterval->m_uiHeadPos);
	//ToConsole(L"ColumnCount", (unsigned int)m_CellPositions.size()-1);

	//int a=0;
	//if(uiDocumentIndex==30921){
	//	a=30921;
	//}

	//в начале достаем шапку таблицы
	//шапку таблицы кладем как подзагловок 2-го уровня
	unsigned int uiHeadRowIndex=0;
	FillTablePositions(uiDocumentIndex, pInterval->m_uiHeadPos, uiHeadRowIndex);
	wstring wsIndexStorageName(L"HEADERS");
	for(unsigned int i=0; i<m_TablesOffsets[m_uiLastFilledTableIndex].m_uiColumnCount; i++){
		
		Containers::CInterval Interval(EMPTY_VALUE, EMPTY_VALUE, pInterval->m_uiWeight);
		Interval.m_uiHeadPos=m_CellPositions[i];
		if(Interval.m_uiHeadPos!=EMPTY_VALUE){
			//если ячейка не пустая определяем позицию последнего слова
			Interval.m_uiTailPos=GetCellTailPosition(i+1);
			//заполняем блокат
			pTextBlockCut=GetTextsBinaryStorage()->FillTextBlock(pTextBlock, &Interval, &wsIndexStorageName);	
		}else{
			//для пустых ячеек заполнение не производим
			pTextBlockCut=pTextBlock->AddBlockCut();
		}

		//устанавливаем вес блока
		pTextBlockCut->SetRelevance(pInterval->m_uiWeight);
		//устанавливаем имя индексного хранилища в котором найден блок
		pTextBlockCut->SetIndexTypeName(wsIndexStorageName.c_str());
		pTextBlockCut->SetNumber(uiGroupID);
		pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
		pTextBlockCut->SetLevel(2);
	}
	
	if(pInterval->m_uiTailPos!=0){
		//если строка результата не является шапкой
		//получаем позиции строки
		FillTablePositions(uiDocumentIndex, pInterval->m_uiHeadPos, pInterval->m_uiTailPos);
		wsIndexStorageName.assign(GetIndexStorageName());
		for(unsigned int i=0; i<m_TablesOffsets[m_uiLastFilledTableIndex].m_uiColumnCount; i++){
			
			Containers::CInterval Interval(EMPTY_VALUE, EMPTY_VALUE, pInterval->m_uiWeight);
			Interval.m_uiHeadPos=m_CellPositions[i];
			if(Interval.m_uiHeadPos!=EMPTY_VALUE){
				//если ячейка не пустая определяем позицию последнего слова
				Interval.m_uiTailPos=GetCellTailPosition(i+1);
				//заполняем блокат
				pTextBlockCut=GetTextsBinaryStorage()->FillTextBlock(pTextBlock, &Interval, &wsIndexStorageName);	
			}else{
				//для пустых ячеек заполнение не производим
				pTextBlockCut=pTextBlock->AddBlockCut();
			}

			//устанавливаем вес блока
			pTextBlockCut->SetRelevance(pInterval->m_uiWeight);
			//устанавливаем имя индексного хранилища в котором найден блок
			pTextBlockCut->SetIndexTypeName(wsIndexStorageName.c_str());
			pTextBlockCut->SetNumber(uiGroupID);
			pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctTable);
		}
	}

	}
	catch(...)
	{
		ERR_TO_CONSOLE(erTryCatch, L"CIndexStorageTables::FillTablesToTextBlock failed!!!");
		throw;
	}

}

void CIndexStorageTables::FillTablesStructure(unsigned int uiTextIndex, std::wstring* pwsTablesStructure)
{
	if(ISNULL(m_pTablesInfoStorage)) return;
	if(ISNULL(pwsTablesStructure)) return;

	pwsTablesStructure->append(L"//---------Bof-Tables-Structure\n");
	wchar_t buf[1000];
	swprintf(buf, L"Document: %u\n", uiTextIndex);
	pwsTablesStructure->append(buf);
	
	//смещаемся к данным узнаем требуемый размер буфера
	unsigned int uiBufferSize;
	m_pTablesInfoStorage->MoveToAndGetDataSize(uiTextIndex, &uiBufferSize);
	
	//резервируем буфер читаем смещения на таблицы
	unsigned int uiTablesOffsetsSize=0;
	m_pTablesInfoStorage->ReadDataChunk((unsigned char*)&uiTablesOffsetsSize, sizeof(unsigned int));
	m_TablesOffsets.resize(uiTablesOffsetsSize/sizeof(Containers::STableInfo));
	m_pTablesInfoStorage->ReadDataChunk((unsigned char*)&m_TablesOffsets.front(), uiTablesOffsetsSize);

	pwsTablesStructure->append(L"//------Bof-Tables-Offsets\n");
	wstring wsValue;

	for(unsigned int i=0; i<m_TablesOffsets.size()-1; i++){
		m_TablesOffsets[i].ToString(wsValue);
		pwsTablesStructure->append(L"\t");
		pwsTablesStructure->append(wsValue.c_str());
		swprintf(buf, L" RowsCount:%u", m_TablesOffsets[i].GetRowsCount(m_TablesOffsets[i+1].m_uiCellsOffset));
		pwsTablesStructure->append(buf);
		pwsTablesStructure->append(L"\n");
	}	
	pwsTablesStructure->append(L"//------Eof-Tables-Offsets\n");


	//читаем позиции строк
	uiBufferSize-=sizeof(unsigned int)+uiTablesOffsetsSize;
	m_CellPositions.resize(uiBufferSize/sizeof(unsigned int));

	//ToConsole(L"uiBufferSize", uiBufferSize);

	//читаем позиции строк таблицы
	m_pTablesInfoStorage->ReadDataChunk((unsigned char*)&m_CellPositions.front(), uiBufferSize);
	
	//обходим смещения на строки таблиц
	unsigned int uiCurentRow=0;
	for(unsigned int i=0; i<m_TablesOffsets.size()-1; i++){
		swprintf(buf, L"//-------Table: %u\n", i);
		pwsTablesStructure->append(buf);
		//определяем количество строк таблицы
		unsigned int uiRowsCount=m_TablesOffsets[i].GetRowsCount(m_TablesOffsets[i+1].m_uiCellsOffset);
		//ToConsole(L"uiRowsCount", uiRowsCount);
		for(unsigned int r=0; r<uiRowsCount; r++){
			//заполняем строку
			FillTablePositions(uiTextIndex, i, r);
			swprintf(buf, L"R_%u:---------\n\t", r);
			pwsTablesStructure->append(buf);
			int a=0;
			for(unsigned int c=0; c<m_TablesOffsets[i].m_uiColumnCount; c++){
				//результат содержимым строки

				//определяем позицию начала и конца строки
				if(m_CellPositions[c]!=EMPTY_VALUE){
					swprintf(buf, L"<%u-%u> ", m_CellPositions[c], GetCellTailPosition(c));
					pwsTablesStructure->append(buf);
				}else{
					pwsTablesStructure->append(L"<empty> ");
				}

				if(a==5){
					a=0; pwsTablesStructure->append(L"\n\t");
				}else a++;

			}
			pwsTablesStructure->append(L"\n");
		}
		pwsTablesStructure->append(L"//-------\n");
	}
	pwsTablesStructure->append(L"//---------Eof-Tables-Structure\n");
}

//--------------------------------------------------------------------//

}
}
}
}
}