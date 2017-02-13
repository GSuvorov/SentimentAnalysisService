#pragma once

#include ".\box_containers.h"

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

///битовая карта содержит коллекцию SBitBox (используется при поиске)
class CBitMap_4s : public CCompositeValue, public CBinaryContainer
{
public:
	typedef vector<SBitBox> TBitBoxCollection;

	///енумератор состояний битовой карты
	enum EBitMapState{
		//промежуточное
		esIntermediate,
		//карта полная (все биты выставлены)
		esFull,
		//карты пустая (ни один бит не выставлен)
		esEmpty
	};

	CBitMap_4s(unsigned int uiBitMapSize, EBitMapState eBitMapState = esEmpty);
	
	///возвращает бинарный буфер
	unsigned char* GetBinaryBuffer(void){return (unsigned char*)&(m_BitMapBoxes.front());};	
	///возвращает размер бинарного буфера
	unsigned int GetBinaryBufferSize(void){return (unsigned int)m_BitMapBoxes.size();};	
	///устанавливает новый размер бинарного буфера, буфер очищает
	void InitBinaryBuffer(unsigned int uiNewSize)
	{
		SBitBox BitBox;
		memset((void*)&BitBox, 0, 1);
		m_BitMapBoxes.assign(uiNewSize, BitBox);
		if(!m_BitMapBoxes.empty()) SetNotEmpty();
	};
	
	///заполняет бинарный буфер, по исходному буферу
	void FillBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize)
	{
		if(ISNULL(pBuffer)) return;
		if(ISNULL(uiBufferSize)) return;

		InitBinaryBuffer(uiBufferSize);
		memcpy(GetBinaryBuffer(), pBuffer, GetBinaryBufferSize());
	};

	///возвращает размер битовой карты в битах
	unsigned int GetBitMapSize(void){return m_uiBitMapSizeInBits;};	
	///возвращает указатель на буфер боксов
	TBitBoxCollection* GetBoxBuffer(void){return &m_BitMapBoxes;};	
	///возвращает максимальный размер битовой карты в боксах
	unsigned int GetBitMapBoxSize(void);
	
	///делает карту полной
	void Fill(void);
	///проверяет карту на заполненность
	bool IsFull(void){return GetState()==esFull?true:false;};
	///возвращает текущее состояние
	EBitMapState GetState(void){return m_eBitMapState;};	
	///устанавливает значение последнего бита карты
	void SetLastBitValue(unsigned int uiValue){m_uiLastBitValue=uiValue; if(m_uiLastBitValue!=EMPTY_VALUE) SetNotEmpty();};
	///возвращает значение последнего бита карты
	unsigned int GetLastBitValue(void){return m_uiLastBitValue;};


	//-----------CValue
	///выполняет операцию AND 
	void MakeAnd(CValue* pValue);
	///выполняет операцию AND со сглаживанием
	void MakeSmoothAnd(CValue* pValue, unsigned int uiAdjacentSize);
	///выполняет операцию OR
	void MakeOr(CValue* pValue);
	///проверяет на пустоту
	bool IsEmpty(void){return GetState()==esEmpty?true:false;};
	///сбрасывает значение на пустое
	void Reset(void);
	//-----------Eof-CValue

	//-----------CCompositeValue
	///выставляет нужный бит в карте
	bool SetOrt(unsigned int uiBitPos);	
	///проверяет выставлен ли нужный бит в карте
	bool HasOrt(unsigned int uiBitPos);
	///позиционируется на первой составляющей
	bool MoveFirstOrt(void);
	///позиционируется на следущей составляющей
	bool MoveNextOrt(void);
	///позиционируется на последней составляющей
	bool MoveLastOrt(void);
	///возвращает текущую составляющую
	unsigned int GetCurrentOrt(void){return m_uiCurrBitValue;};
	///возвращает количество составляющих
	unsigned int GetOrtsMaxNumber(void){return GetBitMapSize();};
	//-----------Eof-CCompositeValue

	///читает содержимое контайнера из файла
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);

	//-----------CConsoleClient
	///сериализация в строку
	void ToString(wstring& szValue);
	///отображает содержимое объекта класса
	void View(void);
	//-----------Eof-CConsoleClient

protected:
	///режим сливания
	enum EMergeMode{
		///сливание по И
		mmAND,
		///сливание по ИЛИ
		mmOR
	};
	
	///информация при передивжении по карте
	struct SMoveInBitMap
	{
		///текущий бокс
		SBitBox m_BitBox; 
		///количестов шагов на которые надо сдвинуться
		unsigned int m_uiSteps;
		///количестов шагов на которые не сдвинулись
		unsigned int m_uiNotDoneSteps;
	};
	
	///приравнивает данное карте входную
	void Assign(CBitMap_4s* pBitMap);
	///пред (частные случаи и прочее) сливает данную битвую карту со входной в соответствии с режимом
	bool PreMerge(CBitMap_4s* pBitMap, EMergeMode MergeMode);
	///сливает данную битвую карту со входной в соответствии с режимом
	void Merge(CBitMap_4s* pBitMap, EMergeMode MergeMode);
	///добавляет бокс результат в коллекцю
	void AddResultBox(TBitBoxCollection* pBitBoxCollection, SBitBox ResultBox);
	///сливает боксы по И возвращает результат, обновляет смещения
	SBitBox MakeBoxAnd(SMoveInBitMap* pThisMoveInBitMap, SMoveInBitMap* pThatMoveInBitMap);
	///сливает боксы по ИЛИ возвращает результат, обновляет смещения
	SBitBox MakeBoxOr(SMoveInBitMap* pThisMoveInBitMap, SMoveInBitMap* pThatMoveInBitMap);
	///переходит к очередному боксу в коллекции согласно SMoveInBitMap
	void GetNextBox(TBitBoxCollection* pBitBoxCollection, 
		TBitBoxCollection::iterator*  pitCurrBox, SMoveInBitMap* pMoveInBitMap);


	///сливает битвую карту с учётом смежности
	void MergeAdjacent(CBitMap_4s* pBitMap, int m_iSizeAdjacent);

	///устанавливает текущее состояние
	void SetState(EBitMapState eBitMapState){m_eBitMapState=eBitMapState;};	
	///сбрасывает флаг пустой карты
	void SetNotEmpty(void){m_eBitMapState=esIntermediate;};
	///добавляет бит в конец карты
	bool AddBit(unsigned int uiBitValue);
	
	///вектор хранилищ битов
	TBitBoxCollection m_BitMapBoxes;
	///текущее хранилище при обходе
	TBitBoxCollection::iterator m_itCurrBox;
	///коллекция для инициализации итераторов
	TBitBoxCollection m_NullCollection;
	///размер битовой карты в битах
	unsigned int m_uiBitMapSizeInBits;
	///абсолютная позиция текущего хранилища при обходе
	unsigned int m_uiCurrBoxPos;
	///номер текущего выcтавленного бита в хранилище при обходе
	unsigned char m_ucCurrBitNumber;
	///значение текущего выcтавленного бита в хранилище при обходе
	unsigned int m_uiCurrBitValue;
	///абсолютное значение последнего бита в карте
	unsigned int m_uiLastBitValue;
	///состояние  карты
	EBitMapState m_eBitMapState;	
};

//--------------------------------------------------------------------//

}
}
}
}
}