#include "StdAfx.h"

#include ".\pos_ba_containers_s.h"

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

CPosBaCntSearch::CPosBaCntSearch()
:m_uiCurrByte(EMPTY_VALUE), 
m_uiCurrentPosition(EMPTY_VALUE), m_uiPositionsNumber(0), m_bPositionsNumberFixed(false)
{
	m_pBuffer=new CByteAlignCompress::TBytesBuffer;
}

CPosBaCntSearch::~CPosBaCntSearch(void)
{
	Reset();
	if(m_pBuffer) delete m_pBuffer;
}

void CPosBaCntSearch::Reset(void)
{
	m_pBuffer->clear();
	m_uiCurrByte=EMPTY_VALUE;
	m_uiCurrentPosition=EMPTY_VALUE;
	m_uiPositionsNumber=0;
}

unsigned int CPosBaCntSearch::FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize)
{
	if(ISNULL(pBuffer)) return 0;
	if(ISNULL(uiBufferSize)) return 0;
	
	unsigned int uiReadedBytesNumber=0;

	//читаем последнюю позицию
	unsigned int uiWorkedBytesNumber;
	m_uiLastPosition=m_pByteAlignCompress->Decode(pBuffer, uiBufferSize, 0, &uiWorkedBytesNumber);
	pBuffer+=uiWorkedBytesNumber;	
	uiReadedBytesNumber+=uiWorkedBytesNumber;

	//читаем количество блоков
	unsigned int uiBinaryBlocksNumber=m_pByteAlignCompress->Decode(pBuffer, uiBufferSize, 0, &uiWorkedBytesNumber);
	pBuffer+=uiWorkedBytesNumber;	
	uiReadedBytesNumber+=uiWorkedBytesNumber;

	//читаем размер крайнего блока
	unsigned char ucLastBlockFilledSize; 
	memcpy((unsigned char*)&ucLastBlockFilledSize, pBuffer, sizeof(unsigned char));
	pBuffer+=sizeof(unsigned char);
	uiReadedBytesNumber+=sizeof(unsigned char);

	//определяем общий бинарный размер
	unsigned int uiTotalSize=(uiBinaryBlocksNumber-1)*cucBaCoordinateNode_BlockBufferSize+ucLastBlockFilledSize;
	m_pBuffer->assign(uiTotalSize, 0);

	//читаем блоки с боксами, формируем список блоков
	unsigned char ucBlockFilledSize=ucLastBlockFilledSize; 
	unsigned char* pCurrPos=((unsigned char*)(&m_pBuffer->front()))+m_pBuffer->size();
	while(uiBinaryBlocksNumber){
		pCurrPos-=ucBlockFilledSize;

		//читаем содержимое блока
		memcpy(pCurrPos, pBuffer, ucBlockFilledSize);
		pBuffer+=ucBlockFilledSize;
		uiReadedBytesNumber+=ucBlockFilledSize;

		ucBlockFilledSize=cucBaCoordinateNode_BlockBufferSize;
		--uiBinaryBlocksNumber;
	}

	////читаем частотный коэффициент
	//unsigned char ucTemp=0;
	//memcpy(&ucTemp, pBuffer, sizeof(unsigned char));
	//SetFreqIncreaseKoef(ucTemp);
	//pBuffer+=sizeof(unsigned char);
	//uiReadedBytesNumber+=sizeof(unsigned char);
	
	return uiReadedBytesNumber;
}

void CPosBaCntSearch::Read(INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return;
	
	//читаем последнюю позицию
	m_uiLastPosition=ReadCompressed(pNdxStorage);

	//читаем количество блоков
	unsigned int usBinaryBlocksNumber=ReadCompressed(pNdxStorage);
	
	//читаем размер крайнего блока
	unsigned char ucLastBlockFilledSize; 
	pNdxStorage->Read((unsigned char*)&ucLastBlockFilledSize, sizeof(unsigned char));
	
	//определяем общий бинарный размер
	unsigned int uiTotalSize=(usBinaryBlocksNumber-1)*cucBaCoordinateNode_BlockBufferSize+ucLastBlockFilledSize;
	m_pBuffer->assign(uiTotalSize, 0);
	//ToConsole(L"uiTotalSize", uiTotalSize+5);

	//читаем блоки с боксами, формируем список блоков
	unsigned char ucBlockFilledSize=ucLastBlockFilledSize; 
	unsigned char* pCurrPos=((unsigned char*)(&m_pBuffer->front()))+m_pBuffer->size();
	while(usBinaryBlocksNumber){
		pCurrPos-=ucBlockFilledSize;
		//читаем содержимое блока
		pNdxStorage->Read(pCurrPos, ucBlockFilledSize);
		ucBlockFilledSize=cucBaCoordinateNode_BlockBufferSize;
		--usBinaryBlocksNumber;
	}

	////читаем частотный коэффициент
	//unsigned char ucTemp=0;
	//pNdxStorage->Read((unsigned char*)&ucTemp, sizeof(unsigned char));
	//SetFreqIncreaseKoef(ucTemp);
}

bool CPosBaCntSearch::IsEqual(CPosBaCntSearch* pPosBaCntSearch)
{
	//сначала проверяем по косвенным признакам
	if(GetFirstPosition()==pPosBaCntSearch->GetFirstPosition() &&
		GetLastPosition()==pPosBaCntSearch->GetLastPosition() &&
		m_pBuffer->size()==pPosBaCntSearch->m_pBuffer->size()){
		//если они сошлись сравниваем буфера		
		if(memcmp(&m_pBuffer->front(), &pPosBaCntSearch->m_pBuffer->front(), m_pBuffer->size())==0){
			return true;
		}
	}
	
	return false;
}

unsigned int CPosBaCntSearch::GetFirstPosition(void)
{
	if(IsEmpty()) return EMPTY_VALUE;

	unsigned int uiWorkedBytesNumber=0;
	return m_pByteAlignCompress->Decode(&m_pBuffer->front(), (unsigned int)m_pBuffer->size(), 0, &uiWorkedBytesNumber);
}

bool CPosBaCntSearch::MoveFirstOrt(void)
{
	if(IsEmpty()){
		m_uiCurrByte=EMPTY_VALUE;
		m_uiCurrentPosition=EMPTY_VALUE;
		m_uiPositionsNumber=0;
		return false;
	}else{
		if(!m_bPositionsNumberFixed) m_uiPositionsNumber=0;
		unsigned int uiWorkedBytesNumber=0;
		m_uiCurrByte=0;
		m_uiCurrentPosition=m_pByteAlignCompress->Decode(&m_pBuffer->front(), (unsigned int)m_pBuffer->size(), m_uiCurrByte, &uiWorkedBytesNumber);
		m_uiCurrByte+=uiWorkedBytesNumber;
		if(!m_bPositionsNumberFixed) ++m_uiPositionsNumber;
		return true;
	}
}

bool CPosBaCntSearch::MoveNextOrt(void)
{
	if(m_uiCurrByte>(unsigned int)m_pBuffer->size()-1){
		m_uiCurrByte=EMPTY_VALUE;
		m_uiCurrentPosition=EMPTY_VALUE;
		m_bPositionsNumberFixed=true;
		return false;
	}else{
		unsigned int uiWorkedBytesNumber=0;
		m_uiCurrentPosition+=m_pByteAlignCompress->Decode(&m_pBuffer->front(), (unsigned int)m_pBuffer->size(), m_uiCurrByte, &uiWorkedBytesNumber);
		m_uiCurrByte+=uiWorkedBytesNumber;
		if(!m_bPositionsNumberFixed) ++m_uiPositionsNumber;
		return true;
	}
}

unsigned int CPosBaCntSearch::GetOrtsNumber(void)
{
	if(!m_bPositionsNumberFixed){
		if(MoveFirstOrt()) do{		
		}while(MoveNextOrt());
	}

	return m_uiPositionsNumber;
};

void CPosBaCntSearch::ToString(wstring& szValue)
{
	wchar_t buf[50];
	if(IsEmpty()){
		szValue.append(L"--Empty-- ");
	}else{
		if(MoveFirstOrt())
		do{
			wsprintf(buf, L"%u ", GetCurrentOrt());
			szValue.append(buf);
		}while(MoveNextOrt());
	}

	wsprintf(buf, L"k:%u ", (unsigned int)GetFreqIncreaseKoef());
	szValue.append(buf);
}

void CPosBaCntSearch::View(void)
{
	ToConsole(L"//------PosBaCntSearch");

	wstring szValue;
	ToString(szValue);
	ToConsole(szValue.c_str());

	ToConsole(L"//------Eof-PosBaCntSearch");
}

//--------------------------------------------------------------------//

void CPosBaCntSearch_Blocked::Reset(void)
{
	CPosBaCntSearch::Reset();
	m_BlockOffsets.clear();
}

void CPosBaCntSearch_Blocked::Read(INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return;
	
	//читаем базовый контайнер
	CPosBaCntSearch::Read(pNdxStorage);

	//читаем количество блоков
	unsigned int BinaryBlocksNumber=ReadCompressed(pNdxStorage);
	//читаем размер крайнего блока
	unsigned char ucLastBlockFilledSize; 
	pNdxStorage->Read((unsigned char*)&ucLastBlockFilledSize, sizeof(unsigned char));
	//определяем общий бинарный размер
	unsigned int uiTotalSize=(BinaryBlocksNumber-1)*cucBaCoordinateNode_BlockBufferSize+ucLastBlockFilledSize;
	unsigned char* pBlockOffsets=(unsigned char*)malloc(uiTotalSize);
	//ToConsole(L"uiTotalSize", uiTotalSize+5);

	//читаем блоки, формируем, буфер
	unsigned char ucBlockFilledSize=ucLastBlockFilledSize; 
	unsigned char* pCurrPos=pBlockOffsets+uiTotalSize;
	while(BinaryBlocksNumber){
		pCurrPos-=ucBlockFilledSize;
		//читаем содержимое блока
		pNdxStorage->Read(pCurrPos, ucBlockFilledSize);
		ucBlockFilledSize=cucBaCoordinateNode_BlockBufferSize;
		--BinaryBlocksNumber;
	}
	
	//заполняем коллекцию смещений блоков сортированых по первым позициям
	FillBlockOffsets(pBlockOffsets, uiTotalSize);

	//освобождаем буфер
	free(pBlockOffsets);

	//читаем количество позиций в последнем неоконченном блоке
	pNdxStorage->Read(&m_InLastBlockValuesNumber, sizeof(unsigned char));
	//определяем общее количество позиций в контайнере
	m_uiPositionsNumber=((unsigned int)m_BlockOffsets.size()-1)*cuiPositionBlockSize+m_InLastBlockValuesNumber;
	m_bPositionsNumberFixed=true;

}

unsigned int CPosBaCntSearch_Blocked::FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize)
{
	if(ISNULL(pBuffer)) return 0;
	if(ISNULL(uiBufferSize)) return 0;
	
	unsigned int uiReadedBytesNumber=0;

	//читаем базовый контайнер
	uiReadedBytesNumber+=CPosBaCntSearch::FromBinaryBuffer(pBuffer, uiBufferSize);
	pBuffer+=uiReadedBytesNumber;
	
	//читаем количество блоков
	unsigned int uiWorkedBytesNumber=0;
	unsigned int BinaryBlocksNumber=m_pByteAlignCompress->Decode(pBuffer, uiBufferSize-uiReadedBytesNumber, 0, &uiWorkedBytesNumber);
	pBuffer+=uiWorkedBytesNumber;	
	uiReadedBytesNumber+=uiWorkedBytesNumber;

	//читаем размер крайнего блока
	unsigned char ucLastBlockFilledSize; 
	memcpy((unsigned char*)&ucLastBlockFilledSize, pBuffer, sizeof(unsigned char));
	pBuffer+=sizeof(unsigned char);
	uiReadedBytesNumber+=sizeof(unsigned char);

	//определяем общий бинарный размер
	unsigned int uiTotalSize=(BinaryBlocksNumber-1)*cucBaCoordinateNode_BlockBufferSize+ucLastBlockFilledSize;
	unsigned char* pBlockOffsets=(unsigned char*)malloc(uiTotalSize);

	//читаем блоки, формируем, буфер
	unsigned char ucBlockFilledSize=ucLastBlockFilledSize; 
	unsigned char* pCurrPos=pBlockOffsets+uiTotalSize;
	while(BinaryBlocksNumber){
		pCurrPos-=ucBlockFilledSize;

		//читаем содержимое блока
		memcpy(pCurrPos, pBuffer, ucBlockFilledSize);
		pBuffer+=ucBlockFilledSize;
		uiReadedBytesNumber+=ucBlockFilledSize;

		ucBlockFilledSize=cucBaCoordinateNode_BlockBufferSize;
		--BinaryBlocksNumber;
	}
	
	//заполняем коллекцию смещений блоков сортированых по первым позициям
	FillBlockOffsets(pBlockOffsets, uiTotalSize);

	//освобождаем буфер
	free(pBlockOffsets);

	//читаем количество позиций в последнем неоконченном блоке
	memcpy(&m_InLastBlockValuesNumber, pBuffer, sizeof(unsigned char));
	uiReadedBytesNumber+=sizeof(unsigned char);

	//определяем общее количество позиций в контайнере
	m_uiPositionsNumber=((unsigned int)m_BlockOffsets.size()-1)*cuiPositionBlockSize+m_InLastBlockValuesNumber;
	m_bPositionsNumberFixed=true;

	//ToConsole(L"m_InLastBlockValuesNumber", (unsigned int)m_InLastBlockValuesNumber);
	//ToConsole(L"m_uiPositionsNumber", m_uiPositionsNumber);
	//ToConsole(L"m_BlockOffsets.size", (unsigned int)m_BlockOffsets.size());
	//View();

	return uiReadedBytesNumber;
}

void CPosBaCntSearch_Blocked::FillBlockOffsets(unsigned char* pBuffer, unsigned int uiBufferSize)
{
	if(ISNULL(pBuffer)) return;
	if(ISNULL(uiBufferSize)) return;
	
	//по буферу формируем коллекцию указателей на блоки
	m_BlockOffsets.clear();
	unsigned int uiCurrPos=0;
	unsigned int uiTemp=0;
	unsigned int uiWorkedBytesNumber=0;
	unsigned int uiPosition=0;
	SBlockInfo BlockInfo;
	BlockInfo.m_uiBlockNumber=0;
	while(uiCurrPos<uiBufferSize){
		//получаем смещение
		uiTemp=m_pByteAlignCompress->Decode(pBuffer, uiBufferSize, uiCurrPos, &uiWorkedBytesNumber);
		uiCurrPos+=uiWorkedBytesNumber;	
		BlockInfo.m_uiBlockOffset=uiTemp;

		//читаем позицию		
		uiPosition=m_pByteAlignCompress->Decode(&m_pBuffer->front(), (unsigned int)m_pBuffer->size(), uiTemp, &uiWorkedBytesNumber);

		//кладем в коллекцию
		m_BlockOffsets.insert(TBlockOffsets::value_type(uiPosition, BlockInfo));
		BlockInfo.m_uiBlockNumber++;
	}

}

bool CPosBaCntSearch_Blocked::MoveLowerBoundOrt(unsigned int uiValue)
{
	if(uiValue>GetLastPosition()) return false;
	if(m_BlockOffsets.empty()) return false;

	//определяем блок в котором находится лежит требуемое значение
	TBlockOffsets::iterator itBlock=m_BlockOffsets.lower_bound(uiValue);

	if(itBlock==m_BlockOffsets.end() || itBlock->first>uiValue){
		//если первое значение блока больше заданного и блок не первый, переходим к предыдущему
		if(itBlock!=m_BlockOffsets.begin()) --itBlock;
	}

	//смещаемся в буфере к блоку
	m_uiCurrByte=itBlock->second.m_uiBlockOffset;
	unsigned int uiWorkedBytesNumber=0;
	m_uiCurrentPosition=m_pByteAlignCompress->Decode(&m_pBuffer->front(), (unsigned int)m_pBuffer->size(), m_uiCurrByte, &uiWorkedBytesNumber);
	m_uiCurrByte+=uiWorkedBytesNumber;
	m_uiCurrentPositionIndex=itBlock->second.m_uiBlockNumber*cuiPositionBlockSize;

	//позиционируемся на позиции большей или равной входной
	while(GetCurrentOrt()<uiValue){	
		if(!MoveNextOrt()) return false;
	}

	return true;
}

bool CPosBaCntSearch_Blocked::MoveFirstOrt(void)
{
	m_uiCurrentPositionIndex=0;
	return CPosBaCntSearch::MoveFirstOrt();
}

bool CPosBaCntSearch_Blocked::MoveNextOrt(void)
{
	//по текущей позиции определяем:
	if((m_uiCurrentPositionIndex+1)%cuiPositionBlockSize!=0){
		//если она не является первой в очередном блоке двигаемся базовым методом
		++m_uiCurrentPositionIndex;
		return CPosBaCntSearch::MoveNextOrt();
	}
	
	//если она первая в очередном блоке она является абсолютной
	if(m_uiCurrByte>(unsigned int)m_pBuffer->size()-1){
		m_uiCurrByte=EMPTY_VALUE;
		m_uiCurrentPosition=EMPTY_VALUE;
		m_bPositionsNumberFixed=true;
		m_uiCurrentPositionIndex=EMPTY_VALUE;
		return false;
	}else{
		unsigned int uiWorkedBytesNumber=0;
		m_uiCurrentPosition=m_pByteAlignCompress->Decode(&m_pBuffer->front(), (unsigned int)m_pBuffer->size(), m_uiCurrByte, &uiWorkedBytesNumber);
		m_uiCurrByte+=uiWorkedBytesNumber;
		++m_uiCurrentPositionIndex;
		return true;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}