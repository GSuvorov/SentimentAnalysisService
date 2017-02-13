#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>
#include ".\CommonContainersUsing.h"
#include ".\storage_manager.h"


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

class CTextsFieldsStorage_test : public CPPUNIT_NS::TestFixture,
	public SS::UnitTests::NdxSE::NdxProcessor::CCommonContainersUsing
{
	CPPUNIT_TEST_SUITE(CTextsFieldsStorage_test);
	//CPPUNIT_TEST(TestAll);
	CPPUNIT_TEST_SUITE_END();

public:
	void TestAll(void);
protected:
	void FillStorage(SS::Core::NdxSE::NdxProcessor::Storages::CTextsFieldsStorage* pTextsFieldsStorage);
	void TestExtracting(SS::Core::NdxSE::NdxProcessor::Storages::CTextsFieldsStorage* pTextsFieldsStorage);
	void TestFilterExtracting(SS::Core::NdxSE::NdxProcessor::Storages::CTextsFieldsStorage* pTextsFieldsStorage);
};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS