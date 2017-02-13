#include "StdAfx.h"
#include ".\testrunner.h"
#include ".\console.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxDataStorage
{

using namespace SS::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CTestRunner::CTestRunner(void)
{
}

CTestRunner::~CTestRunner(void)
{
}

void CTestRunner::Run()
{
	ToConsole(L"//-----------------------NDX-I-level-UnitTests------------------------//");
	

#ifdef _SS_UNITTESTS

	// Get the top level suite from the registry
	CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

	// Adds the test to the list of test to run
	CPPUNIT_NS::TextUi::TestRunner runner;
	runner.addTest( suite );

	// Change the default outputter to a compiler error format outputter
	runner.setOutputter( new CPPUNIT_NS::CompilerOutputter( &runner.result(),
															std::cerr ) );
	// Run the test.
	bool wasSucessful = runner.run();

#endif //_SS_UNITTESTS

	ToConsole(L"//--------------------End-of-NDX-I-level-UnitTests--------------------//\n");
}

ULONG CTestRunner::Release()
{
	delete this;
	return 0;
}

HRESULT CTestRunner::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_Base)
		*pBase=(SS::Interface::IBase*)this;
	else if(pIID==IID_TestRunner)
		*pBase=(SS::Interface::UnitTests::ITestRunner*)this;
	else
		*pBase=NULL;

	return S_OK;
}

//--------------------------------------------------------------------//

}
}
}
}

