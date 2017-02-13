#include "StdAfx.h"
#include ".\cnt_factory.h"
#include ".\index_container_nodes.h"
#include ".\index_container_nodes_s.h"
#include ".\index_container_s.h"
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

using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Core::NdxSE::NdxProcessor::MemoryManagement;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
//--------------------------------------------------------------------//

CContainersFactory::CContainersFactory(CIndexStorage* pIndexStorage)
:m_pIndexStorage(pIndexStorage),
m_bCompressWordPositions(true), m_pSearchSettings(NULL)
{
	//инитим менеджер пулов
	InitPoolsManager();
}

CContainersFactory::~CContainersFactory(void)
{
}

void CContainersFactory::InitPoolsManager(void)
{
	if(ISNULL(m_pIndexStorage)) return;
	
	//устанавливаем статический упаковщик
	CBinaryContainer::m_pByteAlignCompress=&m_ByteAlignCompress;

	//регистрируем классы которые будет обслуживать менеджер пулов
	g_PoolsManager.RegisterClass(L"CBitChunk", sizeof(CBitChunk), 1000);
	g_PoolsManager.RegisterClass(L"CBinaryBlock<cucBmCoordinateNode_BlockBufferSize>", sizeof(CBinaryBlock<cucBmCoordinateNode_BlockBufferSize>), 1000);
	g_PoolsManager.RegisterClass(L"CBinaryBlock<cucBaCoordinateNode_BlockBufferSize>", sizeof(CBinaryBlock<cucBaCoordinateNode_BlockBufferSize>), 1000);
	g_PoolsManager.RegisterClass(L"CBmCoordinateNode_4i", sizeof(CBmCoordinateNode_4i), 1000);
	g_PoolsManager.RegisterClass(L"CBmCoordinateNode_4i_Heavy", sizeof(CBmCoordinateNode_4i_Heavy), 1000);
	g_PoolsManager.RegisterClass(L"CBaCoordinateNode_4i", sizeof(CBaCoordinateNode_4i), 1000);
	g_PoolsManager.RegisterClass(L"CBaCoordinateNode_4i_Blocked", sizeof(CBaCoordinateNode_4i_Blocked), 1000);

}

CCompositeValue* CContainersFactory::CreateCompositeValue(unsigned int uiValue, unsigned char ucLevel)
{
	unsigned int uiBmSizeInBits=m_pIndexStorage->GetBitMapSizeByLevel(ucLevel);
	return new CBitMap_4s(uiBmSizeInBits);
}

CValuesContainer* CContainersFactory::CreateValuesContainer(unsigned int uiEntryPoint)
{
	if(ISNULL(m_pIndexStorage)) return NULL;

	if(m_pIndexStorage->GetWorkingMode()==CBasisStorage::ewmIndexing){
		return NULL;
	}else{
		return new CCoordinateContainer_4s(this, uiEntryPoint);
	}
}

CCoordinateNode_4i* CContainersFactory::CreateCoordinateNode_4i(unsigned char ucLevel)
{
	if(ucLevel==m_pIndexStorage->GetLevelStoragesNumber()){
		//return new CBaCoordinateNode_4i();
		return new CBaCoordinateNode_4i_Blocked();
	}else if(ucLevel==m_pIndexStorage->GetLevelStoragesNumber()-1){
		return new CBmCoordinateNode_4i_Heavy();
	}else if(ucLevel<m_pIndexStorage->GetLevelStoragesNumber()-1){
		return new CBmCoordinateNode_4i();
	}else{
		return NULL;
	}
}

CCoordinateNode_4s* CContainersFactory::CreateCoordinateNode_4s(unsigned char ucLevel)
{
	if(ucLevel==m_pIndexStorage->GetLevelStoragesNumber()){
		//return new CBaCoordinateNode_4s<CPosBaCntSearch>(this);
		return new CBaCoordinateNode_4s<CPosBaCntSearch_Blocked>(this);
	}else if(ucLevel==m_pIndexStorage->GetLevelStoragesNumber()-1){
		unsigned int uiBmSizeInBits=m_pIndexStorage->GetBitMapSizeByLevel(ucLevel);
		return new CBmCoordinateNode_4s_Heavy(uiBmSizeInBits, this);
	}else if(ucLevel<m_pIndexStorage->GetLevelStoragesNumber()-1){
		unsigned int uiBmSizeInBits=m_pIndexStorage->GetBitMapSizeByLevel(ucLevel);
		return new CBmCoordinateNode_4s(uiBmSizeInBits, this);
	}else{
		return NULL;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}