#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

class CPosBaCnt_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CPosBaCnt_test);
	//CPPUNIT_TEST(Test);
	CPPUNIT_TEST_SUITE_END();
public:
	void Test(void);
};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
