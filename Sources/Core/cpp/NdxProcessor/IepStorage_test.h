#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>
#include ".\UnitTestsUsings.h"
#include "../ASDInterface/INdxDataStorage.h"

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

//--------------------------------------------------------------------//

class CIndexEntryPointTable_test : public CPPUNIT_NS::TestFixture, 
	public SS::UnitTests::NdxSE::NdxProcessor::CUnitTestsUsings
{
	CPPUNIT_TEST_SUITE(CIndexEntryPointTable_test);
	//CPPUNIT_TEST(TestAll);
	CPPUNIT_TEST_SUITE_END();

public:
	void TestAll(void);

protected:
	void TestAddData(SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataStorageFactory* pNdxDataStorageFactory);
	void TestGetData(SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataStorageFactory* pNdxDataStorageFactory);
	void TestModifyData(SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataStorageFactory* pNdxDataStorageFactory);
};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
