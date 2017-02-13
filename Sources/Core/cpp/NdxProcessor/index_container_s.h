#pragma once

#include ".\index_container_nodes_s.h"
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

///контайнер координат, содержит координаты индекса, используется для поиска
///может считывать координаты согласно точке входа или поданному текущему узлу, 
///создавать дополнительные контайнер для хранения координат
///координаты храняттся в виде дерева, в зависимости от заданной структуры,
///части координат хранятся в узлах, сам контайнер содержит дерево таких узлов
class CCoordinateContainer_4s : public CValuesContainer, public CFactoryClient<CContainersFactory>
{
public:
	CCoordinateContainer_4s(CContainersFactory* pContainersFactory, unsigned int uiEntryPoint = EMPTY_VALUE);
	~CCoordinateContainer_4s(void);

	///выставляет значения коллекции результата по последнему использованому значению
	void SetFreshValues(CValue* pPrevValue, TValuePtrsBuffer* pValuePtrsBuffer);
	///освобождает значения по последнему породившему значению
	void FreeValues(CValue* pGenerativeValue);

protected:
	///корневой узел координат
	CCoordinateNode_4s* m_pRootNode;
};

//--------------------------------------------------------------------//

}
}
}
}
}