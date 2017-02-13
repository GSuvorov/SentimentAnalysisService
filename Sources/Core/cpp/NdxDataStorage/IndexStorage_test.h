#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>
#include "../../../Sources/Core/ASCInterface/iinterface.h"

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxDataStorage
{

//--------------------------------------------------------------------//

class CIndexStorage_test : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CIndexStorage_test);
	//CPPUNIT_TEST(Test);
	CPPUNIT_TEST_SUITE_END();

public:
	void Test(void);
	bool CreateLoadManager(void);
	void DeleteLoadManager(void);

protected:
	HMODULE m_hLoadManager;
	SS::Interface::Core::ResourceManagers::ILoadManager* m_pLoadManager;
};

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
