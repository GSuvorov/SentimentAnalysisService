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
namespace MemoryManagement
{

//--------------------------------------------------------------------//

class CPoolsManager_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CPoolsManager_test);
	//CPPUNIT_TEST(Test);
	//CPPUNIT_TEST(Test1);
	CPPUNIT_TEST_SUITE_END();

public:
	void Test(void);
	void Test1(void);
};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
