#pragma once
#include ".\eq.h"
#include ".\arg_calc.h"

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

//--------------------------------------------------------------------//

///вычислительная формула - по ней происходит вычисление промежуточных результатов
class CCalcEquation : public CEquation<CCalcArgument, CResultArgument>
{
public:
	CCalcEquation(CEvalFactory* pEvalFactory);
	virtual ~CCalcEquation(void);
	
	///оптимизирует формулу
	void OptimizeCalcEquation(void);
};

//--------------------------------------------------------------------//

}
}
}
}
}