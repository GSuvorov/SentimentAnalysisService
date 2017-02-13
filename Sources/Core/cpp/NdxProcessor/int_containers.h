#pragma once

#include ".\const.h"
#include ".\consoleclient.h"

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

///хранилище интервала
class CInterval : public CConsoleClient
{
public:
	CInterval(void):m_uiHeadPos(EMPTY_VALUE), m_uiTailPos(EMPTY_VALUE), m_uiWeight(0){};
	CInterval(unsigned int uiHeadPos, unsigned int uiTailPos, unsigned int uiWeight = 0)
		:m_uiHeadPos(uiHeadPos), m_uiTailPos(uiTailPos), m_uiWeight(uiWeight){};

	///возвращает длинну интервала
	unsigned int GetLength(void){return (m_uiTailPos-m_uiHeadPos+1);};
	///сбрасывает параметры интервала
	void Reset(void){m_uiTailPos=EMPTY_VALUE;m_uiHeadPos=EMPTY_VALUE;m_uiWeight=0;};
	///проверка на пустоту
	bool IsEmpty(void){return (m_uiTailPos==EMPTY_VALUE && m_uiHeadPos==EMPTY_VALUE)?true:false;};
	///расширяет интервал, изменяет его длину
	void Expand(unsigned int uiExpandedValue)
	{
		//расширяем интервал от головы и хвоста на нужное расстояние
		int iTemp=((int)m_uiHeadPos-uiExpandedValue);
		m_uiHeadPos=iTemp<0?0:iTemp;
		m_uiTailPos+=uiExpandedValue;
	};

	///сериализация в строку
	void ToString(wstring& wszValue)
	{
		wchar_t buf[50]=L"";
		wsprintf(buf, L"L:%u-R:%u D:%u W:%u\n", m_uiHeadPos, m_uiTailPos, GetLength(), (unsigned int)m_uiWeight);
		wszValue.append(buf);
	};
	///отображает содержимое объекта класса
	void View(void)
	{
		wstring wszValue;
		wszValue.append(L"//-------CInterval\n");
		ToString(wszValue);
		wszValue.append(L"//-----Eof-CInterval\n");
		ToConsole(wszValue.c_str());
	};
	
	///создает клона объекта класса
	virtual CInterval* Clone(void)
	{
		return new CInterval(m_uiHeadPos, m_uiTailPos, m_uiWeight);
	}

	///начальная позиция интервала
	unsigned int m_uiHeadPos;
	///конечная позиция интервала
	unsigned int m_uiTailPos;
	///вес интервала
	unsigned int m_uiWeight;
};


///коллекция интервалов
typedef vector<CInterval*> TIntervalCollection; 

//--------------------------------------------------------------------//

class CWordInSequence;

///позиция интервала
struct SIntervalPos
{
	SIntervalPos(void):m_uiIndexPosition(EMPTY_VALUE), m_pCommonPositions(NULL){};

	///позиция индекса в интервале
	unsigned int m_uiIndexPosition;
	///колекция всех позиций индекса
	CWordInSequence* m_pCommonPositions;

	///оператор сравнения для сортировки
	bool operator <(const SIntervalPos& IntervalPos) const
	{
		return (m_uiIndexPosition<IntervalPos.m_uiIndexPosition)?true:false;
	};
};

///коллекция позиций интервалов
//typedef list<SIntervalPos> TIntervalPositions; 
typedef map<unsigned int, SIntervalPos> TIntervalPositions; 

//--------------------------------------------------------------------//

}
}
}
}
}
