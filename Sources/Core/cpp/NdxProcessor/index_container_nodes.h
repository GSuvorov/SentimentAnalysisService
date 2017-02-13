#pragma once

#include "..\ASDInterface\INdxDataStorage.h"
#include ".\box_containers.h"
#include ".\block_container.h"

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

///контайнер - базовый координатный узел, с возможность хранения
///содержит смещения на подчиненные узлы
class CCoordinateNode_4i : public CBinaryContainer
{
public:
	///добавляет значение в контайнер, возвращает true если значение добавлено
	virtual bool AddCoordinate(unsigned int uiValue) = 0;	
	///добавляет смещение на подчиненный узел
	virtual void AddChildOffset(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset){};	
	///возвращает и выбрасывает из своей коллекции смещение на крайний подчиненный узел
	virtual void PopLastChildOffset(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset){};	
	///добавляет подчиненный узел
	virtual void AddChild(CCoordinateNode_4i* pCoordinateNode){};	
	///заполняет и выбрасывает из своей коллекции крайний подчиненный узел
	virtual void PopLastChild(CCoordinateNode_4i* pCoordinateNode){};	
	///сбрасывает узел
	virtual void ResetNode(void) = 0;	
	///возвращает флаг пустого узла
	virtual bool IsEmptyNode(void) = 0;	

protected:
	///сжимает 4 байта и пишет в файл
	unsigned int WriteCompressed(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage, unsigned int uiValue)
	{
		return CBinaryContainer::WriteCompressed(pNdxStorage, uiValue);
	}
	///читает и разжимает в 4 байта из файл
	unsigned int ReadCompressed(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage)
	{
		return CBinaryContainer::ReadCompressed(pNdxStorage);
	}
};

//--------------------------------------------------------------------//

///контайнер - координатный узел 
///значения хранит в битовой карте
///содержит смещения на подчиненные узлы
///обладает возможностью чтения записи в хранилище
#pragma pack(push,1)  // Fix packing size 
class CBmCoordinateNode_4i : 
	public CBitMap,
	public CBlockBufferPusher<cucBmCoordinateNode_BlockBufferSize>,
	public CCoordinateNode_4i
	,
	public SS::Core::NdxSE::NdxProcessor::MemoryManagement::CPoolClient
{
public:
	CBmCoordinateNode_4i(void):m_pucChildData(NULL), m_uiChildDataSize(0){};
	~CBmCoordinateNode_4i(void){ResetNode();};

	///добавляет значение в контайнер, возвращает true если значение добавлено
	bool AddCoordinate(unsigned int uiValue);	
	///добавляет смещение на подчиненный узел
	void AddChildOffset(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset);	
	///возвращает и выбрасывает из своей коллекции смещение на крайний подчиненный узел
	void PopLastChildOffset(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset);	
	///сбрасывает узел
	void ResetNode(void);	
	///возвращает флаг пустого узла
	bool IsEmptyNode(void);	

	///пишет содержимое контайнера в хранилище
	unsigned int Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
	///читает содержимое контайнера из хранилища
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);

	///сериализация в строку
	void ToString(wstring& szValue, wstring szOffset);

protected:
	///буфер с данными по детям	
	unsigned char* m_pucChildData;
	///размер буфера с данными по детям	
	unsigned int m_uiChildDataSize;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///контайнер - координатный узел 
///значения хранит в битовой карте
///содержит подчиненные узлы
///обладает возможностью чтения записи в хранилище
#pragma pack(push,1)  // Fix packing size 
class CBmCoordinateNode_4i_Heavy : 
	public CBmCoordinateNode_4i
{
public:
	CBmCoordinateNode_4i_Heavy(void):m_uiLastChildSize(0){};

	///добавляет смещение на подчиненный узел
	void AddChildOffset(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset){};	
	///возвращает и выбрасывает из своей коллекции смещение на крайний подчиненный узел
	void PopLastChildOffset(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset){};	
	///добавляет подчиненный узел
	void AddChild(CCoordinateNode_4i* pCoordinateNode);	
	///заполняет и выбрасывает из своей коллекции крайний подчиненный узел
	void PopLastChild(CCoordinateNode_4i* pCoordinateNode);	

	///пишет содержимое контайнера в хранилище
	unsigned int Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
	///читает содержимое контайнера из хранилища
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);

protected:
	unsigned int m_uiLastChildSize;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///контайнер - координатный узел 
///значения хранит сжатием ByteAlign
///обладает возможностью чтения записи в хранилище
#pragma pack(push,1)  // Fix packing size 
class CBaCoordinateNode_4i : 
	public CBlockBuffer<cucBaCoordinateNode_BlockBufferSize>,
	public CCoordinateNode_4i
	,
	public SS::Core::NdxSE::NdxProcessor::MemoryManagement::CPoolClient
{
public:
	CBaCoordinateNode_4i(void):m_uiLastValue(EMPTY_VALUE){};

	///добавляет значение в контайнер, возвращает true если значение добавлено
	bool AddCoordinate(unsigned int uiValue);	
	///сбрасывает узел
	void ResetNode(void);	
	///возвращает флаг пустого узла
	bool IsEmptyNode(void);	

	///пишет содержимое контайнера в хранилище
	unsigned int Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
	///читает содержимое контайнера из хранилища
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);

	///заполняется по буферу
	unsigned int FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize);
	///пишется в буфер
	void ToBinaryBuffer(CByteAlignCompress::TBytesBuffer* pBuffer);

	///сериализация в строку
	void ToString(wstring& szValue, wstring szOffset);
	///отображает содержимое объекта класса
	void View(void);
	///сериализация в строку
	void ToString(wstring& szValue);

protected:
	///последнее добавленное значение
	unsigned int m_uiLastValue;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///контайнер - координатный узел 
///значения хранит сжатием ByteAlign
///обладает возможностью чтения записи в хранилище
///значения хранит блочно, дополнительно собирает массив ссылок на блоки
#pragma pack(push,1)  // Fix packing size 
class CBaCoordinateNode_4i_Blocked : 
	public CBaCoordinateNode_4i
{
public:
	CBaCoordinateNode_4i_Blocked(void):m_InBlockValuesNumber(cuiPositionBlockSize){};

	///добавляет значение в контайнер, возвращает true если значение добавлено
	bool AddCoordinate(unsigned int uiValue);	
	///сбрасывает узел
	void ResetNode(void);	

	///пишет содержимое контайнера в хранилище
	unsigned int Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
	///читает содержимое контайнера из хранилища
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);

	///заполняется по буферу
	unsigned int FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize);
	///пишется в буфер
	void ToBinaryBuffer(CByteAlignCompress::TBytesBuffer* pBuffer);

	///отображает содержимое объекта класса
	void View(void);
	///сериализация в строку
	void ToString(wstring& szValue);

protected:
	///блочный буфер для хранения смещений на позиции
	CBlockBuffer<cucBaCoordinateNode_BlockBufferSize> m_OffsetsBuffer;
	///количество позиций в блоке
	unsigned char m_InBlockValuesNumber;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

}
}
}
}
}