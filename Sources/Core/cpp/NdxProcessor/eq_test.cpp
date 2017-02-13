#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\eq_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\eq_calc.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Extracting::CEquation_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

using namespace SS::Core::NdxSE::NdxProcessor::Extracting;
using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Core::NdxSE::NdxProcessor::Extracting::Logics;

//--------------------------------------------------------------------//

void CEquation_test::Test(void)
{
	CEvalFactory EvalFactory(NULL);
	CCalcEquation* pEquation=new CCalcEquation(&EvalFactory);
	
	CPPUNIT_ASSERT(pEquation);

	CCalcArgument* pRootArgument=pEquation->GetRootArgument();
	CCalcArgument* pTempArgument=NULL;
	CCalcArgument* pTempArgument1=NULL;

	pTempArgument=new CCalcArgument(pEquation);
	pRootArgument->AddChild(pTempArgument);
	pRootArgument=pTempArgument;
	
	pTempArgument=new CCalcArgument(pEquation);
	pRootArgument->AddChild(pTempArgument);
	pRootArgument=pTempArgument;

	pTempArgument=new CCalcArgument(pEquation);
	pRootArgument->AddChild(pTempArgument);
	pRootArgument=pTempArgument;

	pTempArgument=new CCalcArgument(pEquation, EMPTY_VALUE, CLogic::eoOR);
	pRootArgument->AddChild(pTempArgument);
	pRootArgument=pTempArgument;

	pTempArgument=new CCalcArgument(pEquation, 1);
	pRootArgument->AddChild(pTempArgument);

	pTempArgument=new CCalcArgument(pEquation, 2);
	pRootArgument->AddChild(pTempArgument);

	//pEquation->View();
	
	pEquation->OptimizeCalcEquation();

	//pEquation->View();

	delete pEquation;
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
