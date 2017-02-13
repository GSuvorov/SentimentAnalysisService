#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

namespace SS
{
namespace UnitTests
{
namespace CommonServices
{

//--------------------------------------------------------------------//

class CSettingsManager_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CSettingsManager_test);
	CPPUNIT_TEST(Test);
	//CPPUNIT_TEST(TestAdding);
	CPPUNIT_TEST_SUITE_END();

public:
	void Test(void);
	void TestAdding(void);
	void TestGetting(void);
	void TestGetList(void);
};

//--------------------------------------------------------------------//

}
}
}

#endif //_SS_UNITTESTS
