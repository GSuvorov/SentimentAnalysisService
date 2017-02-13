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

	class CLogSingleton_test : public CPPUNIT_NS::TestFixture
	{
		CPPUNIT_TEST_SUITE(CLogSingleton_test);
		CPPUNIT_TEST(testSAVE_LOG);
		CPPUNIT_TEST_SUITE_END();

		SS::Interface::Core::ResourceManagers::ILoadManager* m_pLoadManager;
	public:

		void testSAVE_LOG();

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