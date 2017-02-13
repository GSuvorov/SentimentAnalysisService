#pragma once
#include ".\logic.h"


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

///логика в соответствии с принципами нечетких множеств
class CFuzzyLogic : public CLogic
{
public:
	CFuzzyLogic(void);
	CFuzzyLogic(ELogicID LogicID);
	virtual ~CFuzzyLogic(void);

	///образует значение логики по значению аргумента
	SS::Core::NdxSE::NdxProcessor::Containers::CValue* CreateLogicValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, CComplexArgument* pComplexArgument);
	///образует значение аргумента по значению логики
	void ToResultValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, CResultArgument* pResultArgument);

protected:
	///выполняет операцию AND
	void MakeAnd(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue);
	///выполняет операцию OR
	void MakeOr(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue);

	///параметр функции 
	const float m_fBeta;
	///порог выставления бита
	const float m_fThreshold;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}