#pragma once

#include ".\factory_client.h"
#include ".\index_storage.h"
#include ".\byte_align.h"
#include "..\ASDInterface\TNdxSearchEngineTypes.h"

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

class CBinaryContainer;
class CPosCnt;
class CValuesContainer;
class CCompositeValue;
class CIndexAttributes;
class CCoordinateNode_4i;
class CCoordinateNode_4s;

//--------------------------------------------------------------------//

///фабрика создающая контайнеры для хранения координат индексов
///привязана к характеристикам индексного хранилища, для которого создаются контайнеры
class CContainersFactory
{
public:
	CContainersFactory(SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* pIndexStorage);
	virtual ~CContainersFactory(void);
	
	///возвращает индексное хранилище
	SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* GetIndexStorage(void){return m_pIndexStorage;};

	///создает контейнер значений
	CValuesContainer* CreateValuesContainer(unsigned int uiEntryPoint);
	///создает составное значение
	CCompositeValue* CreateCompositeValue(unsigned int uiValue, unsigned char ucLevel);
	///создает координатный узел
	CCoordinateNode_4i* CreateCoordinateNode_4i(unsigned char ucLevel);
	///создает координатный узел
	CCoordinateNode_4s* CreateCoordinateNode_4s(unsigned char ucLevel);

	///возвращает текущие параметры поиска
	SS::Interface::Core::NdxSE::CNdxSearchSettings* GetSearchSettings(void){return m_pSearchSettings;};
	///устанавливает текущие параметры поиска
	void SetSearchSettings(SS::Interface::Core::NdxSE::CNdxSearchSettings* pValue){m_pSearchSettings=pValue;};

protected:
	///инитит менджер пулов
	void InitPoolsManager(void);

	///индексное хранилище
	SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* m_pIndexStorage;
	///флаг сжатия позиций слов
	bool m_bCompressWordPositions;
	///упаковщик позиций в буфер
	CByteAlignCompress m_ByteAlignCompress;
	///текущие параметры поиска
	SS::Interface::Core::NdxSE::CNdxSearchSettings* m_pSearchSettings;
};

//--------------------------------------------------------------------//

}
}
}
}
}