#pragma once

#include ".\const.h"
#include ".\console.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace MemoryManagement
{

//--------------------------------------------------------------------//

///обертка системной кучи
class CNdxHeap
{
public:
	CNdxHeap(void);
	virtual ~CNdxHeap(void);
	
	///аллокирует блок
	void*	Alloc(size_t uiSize);
	///освобождает блок
	void	Free(void* pBuffer);

protected:
	///описатель кучи
	HANDLE m_hHeap;	
};

//--------------------------------------------------------------------//
class CPool;
///блок памяти, размер определен в пуле
struct SPoolBlock
{
	///указатель на следующий блок
	SPoolBlock* m_pNextBlock;
	///указатель на пул блока
	CPool* m_pBlockPool;
};

//--------------------------------------------------------------------//

///экстент - непрерывный массив блоков памяти, выделяется в пуле
struct SPoolExtent
{
	///указатель на следующий екстент
	SPoolExtent* m_pNextExtent;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4200)
#endif
	///виртуальный массив блоков экстента
	SPoolBlock	m_arrBlocks[0];
#ifdef _MSC_VER
#pragma warning(pop)
#endif
};

//--------------------------------------------------------------------//

///пул - содержит список экстентов, для объектов определенного размера
class CPool
{
public:
	CPool(CNdxHeap* pHeap, size_t uiBlockSize, unsigned int uiInitBlocksCount, const wchar_t* pwszObjectTypeName);
	virtual ~CPool(void);
	
	///выделяет память под объект
	void* Alloc(void);
	///освобождает память объекта
	void Free(void* p);
	///возвращает размер объектов пула	
	size_t GetPoolBlockSize(void){return m_cuiBlockSize;};
	///сбрасывает пул очищает занимаемую им память
	void Reset(void);

	///сериализует в строку
	void ToString(wstring& szValue, unsigned int* puiSize);

protected:
	///добавляет новый экстент
	void AddExtent(unsigned int uiBlocksCount);
	///создает экстент
	SPoolExtent* AllocExtent(unsigned int uiBlocksCount);
	///удаляет экстент
	void FreeExtent(SPoolExtent* pExtent);
	///очищает спиоск экстентов
	void ClearExtents(void);
	
	///создает блок памяти в экстенте
	SPoolBlock* AllocBlock(void);
	///освобождает блок памяти в экстенте
	void FreeBlock(SPoolBlock* pBlock);
	///получает блок памяти из экстента
	SPoolBlock* GetBlock(void);
	
	///рабочая куча пула
	CNdxHeap* const m_cpHeap;
	///количестов блоков текущем экстенсте
	size_t m_uiCurrExtentSize;
	///размер блока данных для данного пула
	const size_t m_cuiBlockSize;
	///текущий экстент пула
	SPoolExtent* m_pCurrExtent;
	///текущий блок текущего экстента
	SPoolBlock*	m_pCurrBlock;
	///список освобожденных блоков
	SPoolBlock*	m_pFreeBlockList;

	///количество живых объектов
	unsigned int m_uiCurrBlocksNumber;
	///количество освобожденных объектов
	unsigned int m_uiFreeBlocksNumber;
	///количество аллокирванных экстентов
	unsigned int m_uiCurrExtentsNumber;
	///количество блоков экстента по умолчанию
	unsigned int m_uiInitBlocksNumber;
	
	///описание типа объекта
	wstring m_wsObjectTypeName;
};

//--------------------------------------------------------------------//

///буфер с данными
class CBuffer
{
public:
	CBuffer(unsigned int uiBufferSize)
	:m_pBegin((unsigned char*)malloc(uiBufferSize)), m_pFree(m_pBegin), m_pEnd(m_pBegin+uiBufferSize){};
	virtual ~CBuffer(void){if(m_pBegin) free(m_pBegin);};
	
	///возвращает размер заполненного в буфере
	unsigned int GetFilledSize(void)
	{
		return (unsigned int)(m_pFree-m_pBegin);
	};
	///возвращает указатель на начало
	unsigned char*	GetHeadPtr(void){return m_pBegin;};
	///возвращает указатель на свободное место в буфере
	unsigned char*	GetFreePtr(void){return m_pFree;};
	///устанавливает указатель на свободное место в буфере
	void SetFreePtr(unsigned char* pFree)
	{
		if(pFree<m_pBegin && pFree>m_pEnd){
			TO_CONSOLE(L"Invalid Free Pointer");
		}else{
			m_pFree=pFree;
		}
	};
	///возвращает размер буфера
	unsigned int	GetBufferSize(void){return (unsigned int)(m_pEnd-m_pBegin);};
	///копирует в буфер данные
	bool PutData(unsigned char* pData, unsigned int uiDataSize)
	{
		if(!pData || !uiDataSize) return false;
		
		//проверяем есть ли места для данных
		if(m_pEnd-m_pFree<(int)uiDataSize) return false;
		
		//копируем данные сдвигаем указатель
		memcpy(m_pFree, pData, uiDataSize);
		m_pFree+=uiDataSize;
		return true;
	}
	///сбрасывает буфер новые данные будт класться с головы
	void Reset(void){m_pFree=m_pBegin;}

protected:
	///указатель на начало
	unsigned char* const m_pBegin;
	///указатель на свободное место
	unsigned char*	m_pFree;
	///указатель на свободное место
	unsigned char* const m_pEnd;
};

//--------------------------------------------------------------------//

}
}
}
}
}