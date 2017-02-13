#include "StdAfx.h"
#include ".\testrunner.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

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


//--------------------------------------------------------------------//

CTestRunner::CTestRunner(void)
{
}

CTestRunner::~CTestRunner(void)
{
}

void CTestRunner::Run()
{

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
}

