#include "StdAfx.h"
#include ".\eq_res.h"

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

CResultEquation::CResultEquation(CEvalFactory* pEvalFactory)
:CEquation<CResultArgument, CArgument>(pEvalFactory, NULL)
{
	if(ISNULL(GetFactory())) return;

	m_pRootArgument=GetFactory()->CreateResultArgument(EMPTY_VALUE, 0.0, 0);
}

CResultEquation::~CResultEquation(void)
{
}

//--------------------------------------------------------------------//

}
}
}
}
}