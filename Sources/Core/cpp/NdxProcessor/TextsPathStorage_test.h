#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>
#include ".\UnitTestsUsings.h"
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

class CTextsPathsTree_test : public CPPUNIT_NS::TestFixture, 
	public SS::UnitTests::NdxSE::NdxProcessor::CUnitTestsUsings
{
	CPPUNIT_TEST_SUITE(CTextsPathsTree_test);
	//CPPUNIT_TEST(TestAll);
	CPPUNIT_TEST_SUITE_END();

public:
	void TestAll(void);

protected:
	bool TestFilling(SS::Core::NdxSE::NdxProcessor::Storages::CTextsPathsTreeStorage* pTextsPathsTreeStorage);
	bool TestExtracting(SS::Core::NdxSE::NdxProcessor::Storages::CTextsPathsTreeStorage* pTextsPathsTreeStorage);

	void MakePaths(const wchar_t* szPathFrom, vector<std::wstring>* pPaths);
	bool IsDirectory(WIN32_FIND_DATA* pFD); 
};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
