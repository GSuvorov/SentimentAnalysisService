#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

#include ".\cashed_files.h"


namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxDataStorage
{

//--------------------------------------------------------------------//

class CCashedFile_test: public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CCashedFile_test);
	//CPPUNIT_TEST(testOpenClose);
	CPPUNIT_TEST(TestAppend);
	//CPPUNIT_TEST(TestReadAppend);
	CPPUNIT_TEST_SUITE_END();

public:
	struct SBufInfo{
		unsigned int m_uiOffset;
		unsigned char* m_pucBuffer;
		unsigned int m_uiBufferSize;
	};

	void testOpenClose(void);
	void TestAppend(void);
	void TestReadAppend(void);
};

//--------------------------------------------------------------------//

}
}
}
};

#endif //_SS_UNITTESTS
