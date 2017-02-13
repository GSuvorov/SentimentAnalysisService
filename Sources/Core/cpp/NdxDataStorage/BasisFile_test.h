#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

#include ".\basisfile.h"


namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxDataStorage
{


//--------------------------------------------------------------------//

class CBasisFile_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CBasisFile_test);
	//CPPUNIT_TEST(testOpenAndClose);
	//CPPUNIT_TEST(testReadWrite);
	//CPPUNIT_TEST(testWrite);
	//CPPUNIT_TEST(testReadWrite1);
	CPPUNIT_TEST_SUITE_END();

public:
	void testOpenAndClose(void);
	void testReadWrite(void);
	void testReadWrite1(void);
	void testWrite(void);
};

//--------------------------------------------------------------------//

}
}
}
};

#endif //_SS_UNITTESTS

