#pragma once

#include ".\logic.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASDInterface\INdxQuery.h"
#include ".\cnt_factory.h"
#include ".\index_container_nodes_s.h"
#include ".\arg_res_hdr.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{
namespace Logics
{

//--------------------------------------------------------------------//

///логика заголовков, работает на последнем уровне
///определяет слова и идентификаторы заголовков найденных текстов
class CHeaderLogic : public CLogic 
{
public:
	CHeaderLogic(void);
	virtual ~CHeaderLogic(void);

	///устанавливает запрос для инициализации вычислителей
	void SetQuery(SS::Interface::Core::NdxSE::IParametrizedQuery* pPrmQueryResult);

	///инициализация логики перед вычислениями
	void Init(SS::Core::NdxSE::NdxProcessor::Extracting::CResultArgument* pResultArgument);
	///образует значение логики по значению аргумента
	SS::Core::NdxSE::NdxProcessor::Containers::CValue* CreateLogicValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, 
		SS::Core::NdxSE::NdxProcessor::Extracting::CComplexArgument* pComplexArgument);
	///образует значение аргумента по значению логики
	void ToResultValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, 
		SS::Core::NdxSE::NdxProcessor::Extracting::CResultArgument* pResultArgument);

	///максимальное количество блоков в коллекции
	static const unsigned int MaxBlockCount=10;

protected:
	typedef vector<Containers::CCoordinateNode_4s*> TCoordinateNodeCollection;
	typedef map<unsigned int, TCoordinateNodeCollection*> TIndexCollection;

	///выполняет операцию AND
	void MakeAnd(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, 
		SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue){};
	///выполняет операцию OR
	void MakeOr(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, 
		SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue){};
	
	///наполняет результат значениями
	void FillResult(unsigned int uiEvalID, 
		SS::Core::NdxSE::NdxProcessor::Extracting::CResultArgument_HID* pResultArgument_HID);

	///коллекция индексов с узлами
	TIndexCollection m_IndexCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
