#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>
#include ".\CommonContainersUsing.h"
#include ".\r_logic.h"

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

//--------------------------------------------------------------------//

class CProximityEvaluator_test : public CPPUNIT_NS::TestFixture, 
	public SS::UnitTests::NdxSE::NdxProcessor::CCommonContainersUsing
{
	CPPUNIT_TEST_SUITE(CProximityEvaluator_test);
	//CPPUNIT_TEST(Test);
	CPPUNIT_TEST_SUITE_END();

public:
	void Test(void);

protected:
	///наполняет вычислитель значениями аргументов
	void FillProximityEvaluator(SS::Core::NdxSE::NdxProcessor::Extracting::Logics::CRelevanceLogic* pRelevanceLogic,
		SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory);
};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
