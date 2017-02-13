#pragma once

#include ".\const.h"
#include ".\console.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{
namespace FileCash
{

//--------------------------------------------------------------------//

///буфер с данными
class CBuffer
{
public:
	CBuffer(unsigned int uiBufferIndex, unsigned int uiBufferSize)
		:m_uiBufferIndex(uiBufferIndex), 
		m_pBegin((unsigned char*)malloc(uiBufferSize)), 
		m_pFree(m_pBegin), m_pEnd(m_pBegin+uiBufferSize), m_pCurrent(NULL){};

	virtual ~CBuffer(void){if(m_pBegin) free(m_pBegin);};
	
	///возвращает индекс буфера
	unsigned int GetBufferId(void){return m_uiBufferIndex;};
	///возвращает размер заполненного в буфере
	unsigned int GetFilledSize(void)
	{
		return (unsigned int)(m_pFree-m_pBegin);
	};
	///возвращает размер буфера
	unsigned int	GetBufferSize(void){return (unsigned int)(m_pEnd-m_pBegin);};
	///возвращает указатель на начало буфера
	unsigned char*	GetBuffer(void){return m_pBegin;};
	
	///изменяет размер буфера
	void Resize(unsigned int uiNewSize);
	///устанавливает позицию в буфере
	void SeekFromFront(unsigned int uiOffset);
	///устанавливает позицию в буфере
	void SeekFromBack(unsigned int uiOffset);
	///пишет данные в буфер, возвращает размер записанного
	unsigned int Write(unsigned char* pucData, unsigned int uiSize);
	///добавляет данные в буфер, возвращает размер добавленного
	unsigned int Append(unsigned char* pucData, unsigned int uiSize);
	///читает данные из буфер, возвращает размер прочитанного
	unsigned int Read(unsigned char* pucData, unsigned int uiSize);
	
	///сериализует объект в строку
	void ToString(wstring& wsValue);

protected:
	///индекс буфера
	const unsigned int m_uiBufferIndex;

	///указатель на начало
	unsigned char* const m_pBegin;
	///указатель на свободное место
	unsigned char*	m_pFree;
	///указатель на текущее место
	unsigned char*	m_pCurrent;
	///указатель на свободное место
	unsigned char* const m_pEnd;
};

//--------------------------------------------------------------------//

///кольцевой буфер
template <class T>
class CCircularBuffer
{
public:
	CCircularBuffer(unsigned int uiBufferSize)
		:m_uiBufferSize(uiBufferSize), m_pHead(NULL), m_pTail(NULL), m_uiCurrentSize(0)
	{
		m_pBuffer=new T*[uiBufferSize];
		memset((void*)m_pBuffer, 0, sizeof(T*)*uiBufferSize);
	};
	
	virtual ~CCircularBuffer(void)
	{
		delete[] m_pBuffer;
	};
	
	///возвращает текущее количество элементов
	unsigned int GetSize(void){return m_uiCurrentSize;};
	///возвращает элемент по индексу
	T* GetByIndex(unsigned int uiIndex)
	{
		if(uiIndex>=m_uiCurrentSize) return NULL;
		if(m_pHead+uiIndex>m_pBuffer+m_uiBufferSize-1)
			return *(m_pTail-(m_uiCurrentSize-uiIndex-1));
		else
			return *(m_pHead+uiIndex);
	};
	///возвращает головной элемент
	T* GetHead(void){return m_pHead?*m_pHead:NULL;};
	///возвращает хвостовой элемент
	T* GetTail(void){return m_pTail?*m_pTail:NULL;};

	///добавляет элемент в хвост, если "забивает другой элемент" возвращает указатель на него
	T* PushBack(T* pT)
	{
		if(!m_uiBufferSize) return NULL;
		if(!m_pHead){
			//для первого раза взводим указатели
			InitPtrs();
		}
		
		//переходим к очередному элементу
		m_pTail=NextPtr(m_pTail);
		T* pPoped=NULL;
		if(m_pHead==m_pTail && GetSize()!=0){
			//если дошли до головы, выбрасываем голову
			pPoped=PopFront();
		}
		//заносим элемент
		(*m_pTail)=pT;
		//увеличиваем размер
		++m_uiCurrentSize;
		//возвращаем выбрашенное
		return pPoped;
	};

	///убирает элемент с головы
	T* PopFront(void)
	{
		if(!m_pHead) return NULL;
		T* pPoped=(*m_pHead);
		(*m_pHead)=NULL;
		
		//переходим к следущему если его нет сбрасываем укзазатели
		if(m_pHead==m_pTail && GetSize()==1)
			m_pHead=m_pTail=NULL;
		else
			m_pHead=NextPtr(m_pHead);

		--m_uiCurrentSize;
		return pPoped;
	};
	
	///убирает элемент с хвоста
	T* PopBack(void)
	{
		if(!m_pTail) return NULL;
		//достаем элемет
		T* pPoped=(*m_pTail);
		(*m_pTail)=NULL;
		
		//переходим к предыдущему если его нет сбрасываем укзазатели
		if(m_pHead==m_pTail)
			m_pHead=m_pTail=NULL;
		else
			m_pTail=PrevPtr(m_pTail);

		--m_uiCurrentSize;
		return pPoped;
	};
	
protected:
	///инициализирует указатели
	void InitPtrs(void)
	{
		m_pHead=m_pBuffer;
		m_pTail=m_pBuffer+m_uiBufferSize-1;
	};

	///передвигает к очередному указателю
	T** NextPtr(T** pT)
	{
		if(pT==m_pBuffer+m_uiBufferSize-1){
			return m_pBuffer;
		}else{
			return ++pT;
		}
	};

	///передвигает к преддущему указателю
	T** PrevPtr(T** pT)
	{
		if(pT<=m_pBuffer){
			return m_pBuffer+m_uiBufferSize-1;
		}else{
			return --pT;
		}
	};

	///указатель на буфер
	T** m_pBuffer;
	///указатель на голову
	T** m_pHead;
	///указатель на хвост
	T** m_pTail;
	///текущее количество элементов
	unsigned int m_uiCurrentSize;

	///размер выделенного буфера
	const unsigned int m_uiBufferSize;
};

//--------------------------------------------------------------------//

///позиция в кэше
class CCashPosition
{
public:
	///текущий буфер
	unsigned int m_uiBufferIndex;
	///текущее смещение в текущем буфере
	unsigned int m_uiBufferOffset;

	CCashPosition(void)
		:m_uiBufferIndex(SS::Core::NdxSE::NdxDataStorage::EMPTY_VALUE), 
		m_uiBufferOffset(SS::Core::NdxSE::NdxDataStorage::EMPTY_VALUE){};
	
	///сбрасывает позицию в пустую
	void Clear(void)
	{
		m_uiBufferIndex=SS::Core::NdxSE::NdxDataStorage::EMPTY_VALUE;
		m_uiBufferOffset=SS::Core::NdxSE::NdxDataStorage::EMPTY_VALUE;
	}

	///проверка на пустую позицию 
	bool IsEmpty(void)
	{
		return (m_uiBufferIndex==SS::Core::NdxSE::NdxDataStorage::EMPTY_VALUE ||
			m_uiBufferOffset==SS::Core::NdxSE::NdxDataStorage::EMPTY_VALUE)?true:false;
	}

	///выводит позицию на консоль
	void View(void)
	{
		ToConsole(L"CCashPosition:");
		ToConsole(L"m_uiBufferIndex", m_uiBufferIndex);
		ToConsole(L"m_uiBufferOffset", m_uiBufferOffset);
	}

};

//--------------------------------------------------------------------//

}
}
}
}
}