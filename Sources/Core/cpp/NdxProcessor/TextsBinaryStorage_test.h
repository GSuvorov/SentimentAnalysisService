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

class CTextsBinaryStorage_test : public CPPUNIT_NS::TestFixture,
	public SS::UnitTests::NdxSE::NdxProcessor::CCommonContainersUsing
{
	CPPUNIT_TEST_SUITE(CTextsBinaryStorage_test);
	//CPPUNIT_TEST(TestAll);
	//CPPUNIT_TEST(TestExtracting);
	CPPUNIT_TEST_SUITE_END();

public:
	void TestAll(void);
	void TestExtracting(void);

protected:
	//наполняет хранилище данными
	void FillTextsBinaryStorage(SS::Core::NdxSE::NdxProcessor::Storages::CTextsBinaryStorage* pTextsBinaryStorage,
		SS::Core::NdxSE::NdxProcessor::Containers::CInterval* pInterval, unsigned int uiTextNumber, vector<unsigned int>* pCheckVector);
	//тестирует хранилище данными
	void TestTextsBinaryStorage(SS::Core::NdxSE::NdxProcessor::Storages::CTextsBinaryStorage* pTextsBinaryStorage, 
		SS::Core::NdxSE::NdxProcessor::Containers::CInterval* pInterval, unsigned int uiTextNumber, vector<unsigned int>* pCheckVector);
	
};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS