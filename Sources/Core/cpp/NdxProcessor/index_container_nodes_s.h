#pragma once

#include "..\ASDInterface\INdxDataStorage.h"
#include ".\box_containers_s.h"
#include ".\pos_ba_containers_s.h"
#include ".\node_containers.h"
#include ".\cnt_factory.h"

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

///контайнер - базовый координатный узел, может читать себя из хранилища 
///содержит смещения на подчиненные узлы
class CCoordinateNode_4s : 
	public CTreeNode<CCoordinateNode_4s*> , public CBinaryContainer, 
	public CConsoleClient, public CFactoryClient<CContainersFactory>
	
{
public:
	CCoordinateNode_4s(CContainersFactory* pContainersFactory)
		:CFactoryClient<CContainersFactory>(pContainersFactory){};
	~CCoordinateNode_4s(void){ResetNode();};
	
	using CConsoleClient::ToString;

	///возвращает смещение на подчиненный узел по значению узла
	virtual void GetChildOffset(unsigned int uiValue, SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset){};	
	///добавляет узел по значению в коллекцию детей
	virtual void AddChildNode(unsigned int uiValue, CCoordinateNode_4s* pChild);	
	///сбрасывает узел
	virtual void ResetNode(void);	
	///подымает подчиненный узел, добавляет себе и возвравщает
	virtual CCoordinateNode_4s* MoveUpChildNode(unsigned int uiValue);
	///читает узел из хранилища
	void ReadNode(CCoordinateNode_4s* pCoordinateNode, SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset);

	///сериализация в строку
	void ToString(wstring& szValue, wstring szOffset);

protected:
	///возвращает хранилище координат
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* GetCoordinatesStorage(void){return GetFactory()->GetIndexStorage()->GetCoordinateStorage();};
};

//--------------------------------------------------------------------//

///контайнер - координатный узел 
///значения хранит в битовой карте
///содержит смещения на подчиненные узлы в сортированной коллекции по значениям бит в карте
///обладает возможностью чтения из хранилища
class CBmCoordinateNode_4s : 
	public CBitMap_4s,
	public CCoordinateNode_4s
{
public:
	typedef map<unsigned int, SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset> TChildsOffsets;

	CBmCoordinateNode_4s(unsigned int uiBitMapSize, CContainersFactory* pContainersFactory)
		:CBitMap_4s(uiBitMapSize), CCoordinateNode_4s(pContainersFactory){};
	~CBmCoordinateNode_4s(void){ResetNode();};

	///возвращает смещение на подчиненный узел по значению узла
	void GetChildOffset(unsigned int uiValue, SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pCompositeOffset);	
	///сбрасывает узел
	void ResetNode(void);	

	///читает содержимое контайнера из хранилища
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);

	///сериализация в строку
	void ToString(wstring& szValue);
	///отображает содержимое объекта класса
	void View(void);
protected:
	///коллекция смещений подчиненных узлов по значениям
	TChildsOffsets m_ChildsOffsets;
};

//--------------------------------------------------------------------//

///контайнер - координатный узел 
///значения хранит в битовой карте
///содержит смещения на подчиненные узлы в сортированной коллекции по значениям бит в карте
///обладает возможностью чтения из хранилища
class CBmCoordinateNode_4s_Heavy : 
	public CBmCoordinateNode_4s 
{
public:
	CBmCoordinateNode_4s_Heavy(unsigned int uiBitMapSize, CContainersFactory* pContainersFactory)
		:CBmCoordinateNode_4s(uiBitMapSize, pContainersFactory){};

	///читает содержимое контайнера из хранилища
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
};

//--------------------------------------------------------------------//

///контайнер - координатный узел 
///значения хранит сжатием ByteAlign
///обладает возможностью чтения из хранилище
template <class TBasisContainer>
class CBaCoordinateNode_4s : 
	public TBasisContainer,
	public CCoordinateNode_4s
{
public:
	CBaCoordinateNode_4s(CContainersFactory* pContainersFactory)
		:CCoordinateNode_4s(pContainersFactory){};

	///читает содержимое контайнера из хранилища
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage)
	{
		if(ISNULL(pNdxStorage)) return;

		START_TICK(L"CBaCoordinateNode_4s::Read");

		//читаем коллекцию координат
		TBasisContainer::Read(pNdxStorage);

		STOP_TICK(L"CBaCoordinateNode_4s::Read");
	};

	///заполняется по буферу
	unsigned int FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize)
	{
		if(ISNULL(pBuffer)) return 0;
		if(ISNULL(uiBufferSize)) return 0;

		//читаем коллекцию координат
		return TBasisContainer::FromBinaryBuffer(pBuffer, uiBufferSize);
	};

	///сбрасывает узел
	void ResetNode(void)
	{
		TBasisContainer::Reset();
		CCoordinateNode_4s::ResetNode();
	};


	///сериализация в строку
	void ToString(wstring& szValue)
	{
		TBasisContainer::ToString(szValue);
	};

	///отображает содержимое объекта класса
	void View(void)
	{
		wstring szValue;
		szValue.append(L"\n//-------CBaCoordinateNode_4s\n");
		ToString(szValue);
		szValue.append(L"\n//-------Eof-CBaCoordinateNode_4s\n");
		ToConsole(szValue.c_str());
	};
};

//--------------------------------------------------------------------//

}
}
}
}
}