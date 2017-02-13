#include "StdAfx.h"
#include ".\ev_factory.h"
#include ".\subst.h"
#include ".\res.h"
#include ".\eq_res.h"
#include ".\arg_res.h"
#include ".\eq_calc.h"
#include ".\b_logic.h"
#include ".\eval.h"
#include ".\res_check.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Core::NdxSE::NdxProcessor::Extracting::Logics;

//--------------------------------------------------------------------//

CEvalFactory::CEvalFactory(CContainersFactory* pContainersFactory)
:CFactoryClient<CContainersFactory>(pContainersFactory)
{
	ISNULL(GetFactory());
}

CEvalFactory::~CEvalFactory(void)
{
}

CBlockEquation* CEvalFactory::CreateCalcEquation(void)
{
	return new CBlockEquation(this);
}

CResultEquation* CEvalFactory::CreateResultEquation(void)
{
	return new CResultEquation(this);
}

CResultsChecker* CEvalFactory::CreateResultsChecker(unsigned int uiValidResultsDepth)
{
	return new CResultsChecker(uiValidResultsDepth);
}

CSubster* CEvalFactory::CreateSubster(void)
{
	return new CSubster();
}

CResultsContainer* CEvalFactory::CreateResultsContainer(void)
{
	if(ISNULL(GetFactory())) return NULL;

	return new CResultsContainer(this, GetFactory()->GetIndexStorage()->GetLevelStoragesNumber()-1);
}

CCompositeLogic* CEvalFactory::CreateEquationLogic(void)
{
	CCompositeLogic* pCompositeLogic=new CCompositeLogic();
	pCompositeLogic->AddLogic(new CBooleanLogic());
	return pCompositeLogic;
}

CEvaluator* CEvalFactory::CreateEvaluator(void)
{
	return new CEvaluator(this);
}

CResultArgument* CEvalFactory::CreateResultArgument(unsigned int uiArgumentID, float fArgumentWeight, unsigned int uiArgumentDepth)
{
	return new CResultArgument(uiArgumentID, fArgumentWeight);
}

//--------------------------------------------------------------------//

}
}
}
}
}