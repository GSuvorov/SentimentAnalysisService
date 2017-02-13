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

class CUpdateFile_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CUpdateFile_test);
	//CPPUNIT_TEST(testOpenClose);
	//CPPUNIT_TEST(Test);
	//CPPUNIT_TEST(TestShrink);
	CPPUNIT_TEST_SUITE_END();

public:
	void testOpenClose(void);
	void Test(void);
	void TestShrink(void);
};

//--------------------------------------------------------------------//

}
}
}
};

#endif //_SS_UNITTESTS
