#pragma once

#include ".\const.h"
#include ".\console.h"
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

///информаци€ по расположению заголовка
#pragma pack(push,1)  // Fix packing size 
struct STablePositions
{
	STablePositions(void):m_uiHeadPosition(EMPTY_VALUE), m_uiTailPosition(EMPTY_VALUE){};

	///возвращает длину
	unsigned int GetTableLength(void){return m_uiTailPosition-m_uiHeadPosition;};

	///сериализаци€ в строку
	void ToString(wstring& wszValue)
	{
		wchar_t buf[100];
		wsprintf(buf, L"Head: %u, Tail: %u", m_uiHeadPosition, m_uiTailPosition);
		wszValue.assign(buf);
	};

	///головна€ позици€ (начало)
	unsigned int m_uiHeadPosition;
	///хвостова€ позици€ (конец)
	unsigned int m_uiTailPosition;
};
#pragma pack(pop)
//--------------------------------------------------------------------//

///информаци€ по расположению заголовка
#pragma pack(push,1)  // Fix packing size 
struct STableInfo
{
	STableInfo(void):m_uiCellsOffset(EMPTY_VALUE), m_uiColumnCount(EMPTY_VALUE){};
	
	///возвращает количество строк в таблице
	unsigned int GetRowsCount(unsigned int uiNextCellsOffset)
	{
		//оно определ€етс€ как разница между смещени€ на таблицы, в интегерах, деленой на количестов €чеек в строке+1
		return (uiNextCellsOffset-m_uiCellsOffset)/(m_uiColumnCount+1);
	}

	///сериализаци€ в строку
	void ToString(wstring& wszValue)
	{
		wchar_t buf[100];
		wsprintf(buf, L"CellsOffset: %u, ColumnCount: %u", m_uiCellsOffset, m_uiColumnCount);
		wszValue.assign(buf);
	};

	///отображает содержимое объекта класса
	void View(void)
	{
		wstring wszValue;
		ToString(wszValue);
		ToConsole(wszValue.c_str());
	};

	///смещение на €чейки
	unsigned int m_uiCellsOffset;
	///количество колонок в таблице
	unsigned int m_uiColumnCount;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///смещени€ дл€ выделени€ из буфера идентификаторов таблицы, строки, €чейки
const unsigned int arrTrcOffsets[3]={0, 8, 24};
///маски дл€ выделени€ из буфера идентификаторов таблицы, строки, €чейки
const unsigned int arrTrcMasks[3]={0x000000ff, 0x00ffff00, 0xff000000};

///TRC идентификатор, содержит в себе совокупность
///идентификаторов таблицы, строки, €чейки, 
//по нему можно однозначно идентифицировать €чейку в документе, строке таблице
class CTrcID : public CConsoleClient
{
public:
	CTrcID(void):m_uiValues(EMPTY_VALUE){};
	CTrcID(unsigned int uiTableID):m_uiValues(uiTableID){};
	
	enum ETrcIDType
	{
		etrcCell,
		etrcRow,
		etrcTable,
	};

	///возвращает значение TRC идентификатора
	unsigned int GetTrcID(void){return m_uiValues;};

	///возвращает идентификатор по типу
	unsigned int GetIDByType(CTrcID::ETrcIDType Type){
		if((unsigned int)Type>(unsigned int)etrcTable){
			TO_CONSOLE(L"Too big TRC Type!!!");
			return EMPTY_VALUE;
		}	
		return (m_uiValues&arrTrcMasks[Type])>>arrTrcOffsets[Type];
	};
	
	///устанавливает идентификатор по типу, сбрасывает младшие
	void SetIDByType(CTrcID::ETrcIDType Type, unsigned int uiID){
		if((unsigned int)Type>(unsigned int)etrcTable){
			TO_CONSOLE(L"Too big TRC Type!!!");
			return;
		}	

		m_uiValues&=(~arrTrcMasks[Type]);
		m_uiValues|=((uiID<<arrTrcOffsets[Type])&arrTrcMasks[Type]);

		if(Type==etrcCell) return;
		unsigned int i=(unsigned int)Type;
		do{
			--i;
			SetIDByType((CTrcID::ETrcIDType)i, 0);
		}while(i!=0);
	};

	///инкрементирует идентификатор по типу
	void IncIDByType(CTrcID::ETrcIDType Type){SetIDByType(Type, GetIDByType(Type)+1);};

	///сбрасыает идентификатор
	void Reset(void){m_uiValues=EMPTY_VALUE;};
	///провер€ет на пустоту
	bool IsEmpty(void){return m_uiValues==EMPTY_VALUE?true:false;};

	///сериализаци€ в строку
	void ToString(wstring& wszValue);

protected:
	///буфер содержит идентификаторы таблицы, строки, €чейки
	unsigned int m_uiValues;
};

//--------------------------------------------------------------------//

}
}
}
}
}