#pragma once
#include ".\list_containers.h"
#include ".\pool_client.h"
#include ".\consoleclient.h"
#include ".\basis_containers.h"

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

///массив масок для выделения битов и бокса
const unsigned char carrBitBoxMasks[]={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};
///максимальное значение бита в боксе
const unsigned char cucBitMaxBitValue=0x06;
///максимальное число хранимое в боксе
const unsigned char cucBitMaxNumberValue=0x7f;
///максимальное количество битов в боксе
const unsigned char cucBitBoxValueSize=0x07;
///тип NullBox
const unsigned char cucNullBoxType=0x00;
///тип BitBox
const unsigned char cucBitBoxType=0x01;
///маска полного битбокса
const unsigned char cucFullBitBox=0xff;

/// хранилище битов.
/// формат хранения: 
///	8-бит: 
///	1. первый бит-флаг определяет тип : ValBox или NullBox
/// 2. остальный 7 бит значащие
/// ValBox: содержит 7 значащих битов - 7-битовый кусочек полной битовой карты
/// бит-флаг равен 1
/// NullBox: содержит 7 значащих битов, они определяют от 0-128 количество 7-биток у которых нет выставленых битов
/// бит-флаг равен 0
/// Если Value и Type равны нулю SBitBox неопределен
#pragma pack(push,1)  // Fix packing size 
struct SBitBox
{
	///содержимое бокса
	unsigned char
	///младшие биты определяют наполнение
	Value:7,
	///старший бит определяет тип
	Type:1;
	
	///возвращает значение последнего бита в боксе
	unsigned char GetLastBitValue(void)
	{
		for(/*unsigned*/ char i = cucBitMaxBitValue; i>=0; i--)
			if(Value&carrBitBoxMasks[i]) return i;
		return 0xff;
	}

	///возвращает значение первого бита в боксе
	unsigned char GetFirstBitValue(void)
	{
		for(unsigned char i=0; i<=cucBitMaxBitValue; i++)
			if(Value&carrBitBoxMasks[i]) return i;
		return 0xff;
	}

	///возвращает значение бита в боксе после заданного
	unsigned char GetNextBitValue(unsigned char ucFrom)
	{
		for(unsigned char i=ucFrom; i<=cucBitMaxBitValue; i++)
			if(Value&carrBitBoxMasks[i]) return i;
		return 0xff;
	}

	///выставляет бит в боксе (значения могут быть 0-6)
	void SetBit(unsigned char ucBitValue)
	{
		if(ucBitValue>cucBitMaxBitValue) return;
		Value|=carrBitBoxMasks[ucBitValue];
	}

	///убирает бит в боксе (значения могут быть 0-6)
	void UnSetBit(unsigned char ucBitValue)
	{
		if(ucBitValue>cucBitMaxBitValue) return;
		Value&=(~carrBitBoxMasks[ucBitValue]);
	}

	///проверяет выставляет ли бит в боксе (значения могут быть 0-6)
	bool IsSetBit(unsigned char ucBitValue)
	{
		if(ucBitValue>cucBitMaxBitValue) return false;
		return (Value&(carrBitBoxMasks[ucBitValue]))==0?false:true;
	}

	///выставляет число в боксе
	void SetNumber(unsigned char ucValue)
	{
		Value=ucValue&cucBitMaxNumberValue;
	}

	///проверяет неопределен ли бокс
	bool IsUndefined(void){return (Value==0 && Type==0)?true:false;};
	///проверяет тип бокса
	bool IsBitBox(void){return Type==cucBitBoxType?true:false;};
	///проверяет тип бокса
	bool IsNullBox(void){return Type==cucNullBoxType?true:false;};
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///максимальное количество битбоксов в блоке
const unsigned char cucBoxInBitChunk=0x08;
///маска полного блока
const unsigned char carrFullNullBoxChunk[]={0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f};
///маска пустого блока
const unsigned char carrFullBitBoxChunk[]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

///блок SBitBox-ов 
#pragma pack(push,1)  // Fix packing size 
class CBitChunk : 
	public SS::Core::NdxSE::NdxProcessor::MemoryManagement::CPoolClient
{
public:
	CBitChunk():m_ucLastBox(cucBoxInBitChunk), m_pNextItem(NULL){memset(m_BitBoxes, 0, cucBoxInBitChunk);};

	///заполняем блок полными нулбоксами
	void FillByNullBox(void){memcpy(m_BitBoxes, carrFullNullBoxChunk, cucBoxInBitChunk);m_ucLastBox=cucBoxInBitChunk-1;};
	///заполняем блок полными битбоксами
	void FillByBitBox(void){memcpy(m_BitBoxes, carrFullBitBoxChunk, cucBoxInBitChunk);m_ucLastBox=cucBoxInBitChunk-1;};
	///добавляем к блоку нулбокс
	void AddNullBox(unsigned char ucValue)
	{
		if(IsFull()) return;
		m_ucLastBox==cucBoxInBitChunk?m_ucLastBox=0:m_ucLastBox++;

		m_BitBoxes[m_ucLastBox].Type=cucNullBoxType;
		m_BitBoxes[m_ucLastBox].SetNumber(ucValue);
	};
	///добавляем к блоку битбокс
	void AddBitBox()
	{
		if(IsFull()) return;
		m_ucLastBox==cucBoxInBitChunk?m_ucLastBox=0:m_ucLastBox++;

		m_BitBoxes[m_ucLastBox].Type=cucBitBoxType;
	};
	///проверка на пустоту
	bool IsEmpty(void){return m_ucLastBox<cucBoxInBitChunk?false:true;};
	///проверка на полноту
	bool IsFull(void){return m_ucLastBox==(cucBoxInBitChunk-1)?true:false;};
	///возвращает крайний битбокс
	SBitBox* GetLastBitBox(void){return IsEmpty()?NULL:&(m_BitBoxes[m_ucLastBox]);};
	///конвертирует в массив битов
	unsigned int ToArray(vector<unsigned int>* pBitArray, unsigned int uiStartValue);

	///возвращает следующий узел в списке
	CBitChunk* GetNextItem(void){return m_pNextItem;};
	///устанавливает следующий узел в списке
	void SetNextItem(CBitChunk* pNextItem){m_pNextItem=pNextItem;};

	///возвращает указатель на буфер с боксами
	unsigned char* GetBitBoxBuffer(void){return (unsigned char*)m_BitBoxes;};
	///возвращает текщий размер хранилище боксов в байтах
	unsigned char GetBitBoxesNumber(void){return m_ucLastBox+1;}; 
	///устанавливает текщий размер хранилище боксов в байтах
	void SetBitBoxesNumber(unsigned char ucValue){ucValue==0?m_ucLastBox=cucBoxInBitChunk:m_ucLastBox=ucValue-1;}; 
	///заполняет бинарный буфер, по исходному буферу
	void FillBitBoxBuffer(unsigned char* pBoxBuffer, unsigned char ucBoxNumber = cucBoxInBitChunk)
	{
		if(ISNULL(pBoxBuffer)) return;
		if(ISNULL(ucBoxNumber)) return;

		memcpy(m_BitBoxes, pBoxBuffer, ucBoxNumber);
	};

protected:
	///блок SBitBox
	SBitBox m_BitBoxes[cucBoxInBitChunk];
	///следующий итем
	CBitChunk* m_pNextItem;
	///индекс последней SBitBox, 
	///если равен cucBoxInBitChunk-1 блок полный, если равен cucBoxInBitChunk блок пустой
	unsigned char m_ucLastBox;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///битовая карта содержит список CBitChunk-ов (используется при индексации)
#pragma pack(push,1)  // Fix packing size 
class CBitMap : public CConsoleClient, public CBinaryContainer
{
public:
	CBitMap(void);
	virtual ~CBitMap(void);

	///возвращает значение последнего бита карты
	unsigned int GetLastBitValue(void){return m_uiLastBitValue;};
	///добавляет бит к битовой карте
	bool AddBit(unsigned int uiBitValue);	

	///возвращает хвостовой блок CBitChunk в карте
	CBitChunk* GetTailBitChunk(void){return m_pTailBitChunk;};
	///возвращает колчество CBitChunk блоков которые содержит карта
	unsigned short GetBitChunkNumber(void){return m_usBitChunkNumber;};
	///устанавливает колчество CBitChunk блоков которые содержит карта
	void SetBitChunkNumber(unsigned short usValue){m_usBitChunkNumber=usValue;};
	///устанавливает значение последнего бита карты
	void SetLastBitValue(unsigned int uiValue){m_uiLastBitValue=uiValue;};
	///сбрасывает битовую карту
	void Reset(void);	
	///возвращает флаг пустой битовой карты
	bool IsEmpty(void){return m_pTailBitChunk==NULL?true:false;};	

	///пишет содержимое контайнера в файл
	unsigned int Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
	///читает содержимое контайнера из файла
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);

	///сериализация в строку
	void ToString(wstring& szValue);
	///отображает содержимое объекта класса
	void View(void);

protected:
	///добавляет CBitChunk
	void AddBitChunk(void);

	///хвостовой блок CBitChunk в карте
	CBitChunk* m_pTailBitChunk;
	///абсолютное значение последнего бита в карте
	unsigned int m_uiLastBitValue;
	///колчество CBitChunk блоков которые содержит карта
	unsigned short m_usBitChunkNumber;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

}
}
}
}
}