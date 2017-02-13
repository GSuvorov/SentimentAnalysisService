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

class CFileCash_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CFileCash_test);
	//CPPUNIT_TEST(testReadWrite);
	//CPPUNIT_TEST(TestCircularBuffer);
	//CPPUNIT_TEST(TestBuffer);
	//CPPUNIT_TEST(TestCash);
	//CPPUNIT_TEST(TestCashFlush);
	CPPUNIT_TEST_SUITE_END();

public:
	void testReadWrite(void);
	void TestCircularBuffer(void);
	void TestBuffer(void);
	void TestCash(void);
	void TestCashFlush(void);
};

//--------------------------------------------------------------------//

}
}
}
};

#endif //_SS_UNITTESTS
