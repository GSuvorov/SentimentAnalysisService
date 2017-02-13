#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxDataStorage
{

//--------------------------------------------------------------------//

class CLevelStorage_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CLevelStorage_test);
	//CPPUNIT_TEST(testOpenAndClose);
	//CPPUNIT_TEST(Test);
	CPPUNIT_TEST_SUITE_END();

public:
	void testOpenAndClose(void);
	void Test(void);
};

//--------------------------------------------------------------------//

}
}
}
};

#endif //_SS_UNITTESTS
