#pragma once 

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

#include "..\..\Core\ASCInterface\IInterface.h"

namespace SS
{
namespace UnitTests
{
namespace Core
{
namespace ResourceManagers
{
namespace UMCommonServices
{

	class CException_test : public CPPUNIT_NS::TestFixture
	{
		CPPUNIT_TEST_SUITE(CException_test);
		CPPUNIT_TEST(testThrow);
		CPPUNIT_TEST_SUITE_END();

		SS::Interface::Core::ResourceManagers::ILoadManager* m_pLoadManager;
	public:

		void testThrow();

		/// устанавливает указатель на лоад мэнеджер
		void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

		/// устанавливает указатель на лоад мэнеджер
		SS::Interface::Core::ResourceManagers::ILoadManager* GetLoadManager();
	};
}
}
}
}
}

#endif