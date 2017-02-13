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

class CFilesCollectionEx_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CFilesCollectionEx_test);
	//CPPUNIT_TEST(TestOpenClose);
	CPPUNIT_TEST_SUITE_END();

public:
	void TestOpenClose(void);
};

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
