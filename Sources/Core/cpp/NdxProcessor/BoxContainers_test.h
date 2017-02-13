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

class CBoxContainers_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CBoxContainers_test);
	//CPPUNIT_TEST(TestBitMap_4i);
	//CPPUNIT_TEST(TestBitMap_4s);
	//CPPUNIT_TEST(TestBitMap_4s_1);
	CPPUNIT_TEST_SUITE_END();

public:
	void TestBitMap_4i(void);
	void TestBitMap_4s(void);
	void TestBitMap_4s_1(void);

};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
