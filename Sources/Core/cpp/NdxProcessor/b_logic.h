#pragma once
#include ".\logic.h"
#include <math.h>


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

///логика в соответствии с принципами булевой логики
class CBooleanLogic : public CLogic
{
public:
	CBooleanLogic(void);
	CBooleanLogic(ELogicID LogicID);
	virtual ~CBooleanLogic(void);

	///образует значение логики по значению аргумента
	SS::Core::NdxSE::NdxProcessor::Containers::CValue* CreateLogicValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, CComplexArgument* pComplexArgument);
	///образует значение аргумента по значению логики
	void ToResultValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, CResultArgument* pResultArgument);

protected:
	///выполняет операцию AND
	void MakeAnd(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue);
	///выполняет операцию OR
	void MakeOr(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue);
	///вычисляет результирующий процентый вес
	float EvalResultWeight(float fInputWeight){return floor((fInputWeight/(m_fResultMaxWeight/100)));};

	///максимальный вес результата вычисленного по булевой логике
	float m_fResultMaxWeight;
};

//--------------------------------------------------------------------//

///булевая логика с учетом координат в смежных предложениях
class CBooleanLogicWithSentencesAdjacent : public CBooleanLogic
{
public:
	CBooleanLogicWithSentencesAdjacent(int iSizeAdjacent/*, 
		SS::Core::Ndx::SSearchParameter::EAdjacentSearchIntervalType IntervalType*/);
	virtual ~CBooleanLogicWithSentencesAdjacent(void);

protected:
	///выполняет операцию AND
	void MakeAnd(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue);
	
	///длина интервала 
	int m_iSizeAdjacent;
	/////тип интервала
	//SS::Core::Ndx::SSearchParameter::EAdjacentSearchIntervalType m_IntervalType;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}