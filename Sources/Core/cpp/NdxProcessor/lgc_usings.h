#pragma once

#include ".\logic.h"
#include ".\arg_complex.h"

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

///логика состоящая из нескольких логик, используется формулой для вычислений
class CCompositeLogic : public CLogic
{
public:
	typedef list<CLogic*> TLogicCollection;

	CCompositeLogic(void);
	virtual ~CCompositeLogic(void);

	///добавляет логику
	void AddLogic(CLogic* pLogicUsing);
	///инициализация логики перед вычислениями
	void Init(SS::Core::NdxSE::NdxProcessor::Extracting::CResultArgument* pResultArgument);
	
	///создает сложное значение
	SS::Core::NdxSE::NdxProcessor::Extracting::CComplexValue* CreateComplexValue(void);

	///выставляет флаг холостого прогона
	void SetIdleCalculation(void);
	///сбрасывает флаг холостого прогона
	void UnsetIdleCalculation(void);

protected:
	///коллекция логик, которые может пользовать формула
	TLogicCollection m_LogicCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}