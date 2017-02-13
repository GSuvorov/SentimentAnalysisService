#include "StdAfx.h"
#include ".\eq_calc.h"

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

CCalcEquation::CCalcEquation(CEvalFactory* pEvalFactory)
:CEquation<CCalcArgument, CResultArgument>(pEvalFactory, NULL)
{
	if(ISNULL(GetFactory())) return;
	m_pEquationLogic=GetFactory()->CreateEquationLogic();

	m_pRootArgument=new CCalcArgument(this);
}

CCalcEquation::~CCalcEquation(void)
{
}

void CCalcEquation::OptimizeCalcEquation(void)
{
	if(ISNULL(m_pRootArgument)) return;

	m_pRootArgument->OptimizeCalcEquation();
}

//--------------------------------------------------------------------//

}
}
}
}
}