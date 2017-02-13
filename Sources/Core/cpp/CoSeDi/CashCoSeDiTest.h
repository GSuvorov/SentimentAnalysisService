#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

#include "./CashCoSeDi.h"


namespace SS
{
	namespace UnitTests
	{
		namespace CoSeDi
		{

			class CCashCoSeDiTest : public CPPUNIT_NS::TestCase
			{
				CPPUNIT_TEST_SUITE(CCashCoSeDiTest);
				CPPUNIT_TEST(TestLoad);
				CPPUNIT_TEST_SUITE_END();
			public:
				void TestLoad(void);
			};
		};
	};
};

#endif