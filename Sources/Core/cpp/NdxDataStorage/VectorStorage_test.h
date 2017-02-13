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

class CVectorStorage_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CVectorStorage_test);
	//CPPUNIT_TEST(TestOpenClose);
	//CPPUNIT_TEST(TestReadAppend);
	//CPPUNIT_TEST(TestAppend);
	CPPUNIT_TEST_SUITE_END();

public:
	void TestOpenClose(void);
	void TestReadAppend(void);
	void TestAppend(void);
};

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
