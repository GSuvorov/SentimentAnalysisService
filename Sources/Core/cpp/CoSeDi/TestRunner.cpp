#include "StdAfx.h"
#include ".\testrunner.h"

#include ".\CoSeDiManagerTest.h"


#ifdef _SS_UNITTESTS

	#include <cppunit/CompilerOutputter.h>
	#include <cppunit/extensions/TestFactoryRegistry.h>
	#include <cppunit/ui/text/TestRunner.h>

#endif //_SS_UNITTESTS

SS::UnitTests::CoSeDi::CTestRunner::CTestRunner(void)
{
}

SS::UnitTests::CoSeDi::CTestRunner::~CTestRunner(void)
{
}

void SS::UnitTests::CoSeDi::CTestRunner::Run()
{
#ifdef _SS_UNITTESTS

	SS::UnitTests::CoSeDi::CCoSeDiManagerTest oCCoSeDiManagerTest;
	oCCoSeDiManagerTest.SetLoadManager(GetLoadManager());
	oCCoSeDiManagerTest.FullTest();


#endif //_SS_UNITTESTS

}

ULONG SS::UnitTests::CoSeDi::CTestRunner::Release()
{
	delete this;
	return 0;
}

HRESULT SS::UnitTests::CoSeDi::CTestRunner::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_TestRunner)
		*pBase=(SS::Interface::UnitTests::ITestRunner*)this;
	else
		if(pIID==IID_Base)
			*pBase=(SS::Interface::IBase*)this;
		else
		*pBase=NULL;

	return S_OK;
}