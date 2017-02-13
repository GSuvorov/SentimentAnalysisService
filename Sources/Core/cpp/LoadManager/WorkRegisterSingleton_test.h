#pragma once 

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

namespace SS
{
namespace UnitTests
{
namespace Core
{
namespace ResourceManagers
{
namespace LoadManager
{

	class CWorkRegisterSingleton_test : public CPPUNIT_NS::TestFixture
	{
		CPPUNIT_TEST_SUITE(CWorkRegisterSingleton_test);
		CPPUNIT_TEST(testGetParameter);
		CPPUNIT_TEST_SUITE_END();
	public:

		void testGetParameter();
	};
}
}
}
}
}

#endif