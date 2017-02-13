#pragma once

#include ".\pos_containers.h"

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


///хранилище позиций с возможностями логических операций 
///с коллекциями позиций используется для поиска
class CPosBaCntSearch : public CPosCnt
{
public:
	CPosBaCntSearch(void);
	virtual ~CPosBaCntSearch(void);
	
	///читает содержимое контайнера из файла
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
	///заполняется по буферу
	unsigned int FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize);
	//возвращает бинарный буфер
	CByteAlignCompress::TBytesBuffer* GetBuffer(void){return m_pBuffer;};

	///сбрасывает значение на пустое
	virtual void Reset(void);

	//Обход выставленных составляющих
	///позиционируется на первой составляющей
	bool MoveFirstOrt(void);
	///позиционируется на следущей составляющей
	bool MoveNextOrt(void);

	///возвращает текущую составляющую
	unsigned int GetCurrentOrt(void){return m_uiCurrentPosition;};
	///возвращает количество составляющих значения
	unsigned int GetOrtsNumber(void);
	///проверяет на пустоту
	bool IsEmpty(void){return (unsigned int)m_pBuffer->size()==0?true:false;};

	///возвращает первую позицию
	unsigned int GetFirstPosition(void);
	///возвращает последнюю позицию
	unsigned int GetLastPosition(void){return m_uiLastPosition;};
	///возвращает размер диапозона позиций, которые захватывает контайнер
	virtual unsigned int GetPositionRangeLength(void){return GetLastPosition()-GetFirstPosition();};
	
	///проверяет контайнер на эквивалентность
	bool IsEqual(CPosBaCntSearch* pPosBaCntSearch);

	///отображает содержимое объекта класса
	void View(void);
	///сериализация в строку
	void ToString(wstring& szValue);
	
protected:
	///текущий байт в буфере контейнера
	unsigned int m_uiCurrByte;
	///текущая позиция в контейнере
	unsigned int m_uiCurrentPosition;
	///количество позиций в контейнере
	unsigned int m_uiPositionsNumber;
	///флаг посчета количества позиций
	bool m_bPositionsNumberFixed;
	///буфер с бинарным содержимым
	CByteAlignCompress::TBytesBuffer* m_pBuffer;
};

//--------------------------------------------------------------------//

///хранилище позиций с возможностями логических операций 
///с коллекциями позиций используется для поиска
///позиции хранятся блокам, есть возможность смещения к определенной позиции без полного перебора
class CPosBaCntSearch_Blocked : public CPosBaCntSearch
{
public:
	///информация по блоку
	struct SBlockInfo
	{
		//смещение блока
		unsigned int m_uiBlockOffset;
		//номер блока
		unsigned int m_uiBlockNumber;
	};

	typedef map<unsigned int, SBlockInfo> TBlockOffsets;

	CPosBaCntSearch_Blocked(void)
		:m_uiCurrentPositionIndex(EMPTY_VALUE), m_InLastBlockValuesNumber(0){};

	///сбрасывает значение на пустое
	void Reset(void);

	///читает содержимое контайнера из файла
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
	///заполняется по буферу
	unsigned int FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize);

	//Обход выставленных составляющих
	///позиционируется на первой составляющей
	bool MoveFirstOrt(void);
	///позиционируется на составляющей, которая больше или равна заданной
	bool MoveLowerBoundOrt(unsigned int uiValue);
	///позиционируется на следущей составляющей
	bool MoveNextOrt(void);

	///возвращает количество составляющих значения
	unsigned int GetOrtsNumber(void){return m_uiPositionsNumber;};
	/////возвращает размер диапозона позиций, которые захватывает контайнер
	//unsigned int GetPositionRangeLength(void){return GetOrtsNumber();};

protected:
	///заполняет коллекцию смещений блоков сортированых по первым позициям
	void FillBlockOffsets(unsigned char* pBuffer, unsigned int uiBufferSize);

	///коллекция смещений блоков сортированых по первым позициям
	TBlockOffsets m_BlockOffsets;
	///порядковый номер текущей позиции в контейнере
	unsigned int m_uiCurrentPositionIndex;
	///количество позиций в блоке
	unsigned char m_InLastBlockValuesNumber;
};

//--------------------------------------------------------------------//

}
}
}
}
}