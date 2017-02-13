#include "StdAfx.h"
#include ".\cashcoseditest.h"
#include ".\CashCoSeDi.h"
#include "../../MiddlewareServer/ManagerConnections/define.h"

#ifdef _SS_UNITTESTS

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::CoSeDi::CCashCoSeDiTest);

void SS::UnitTests::CoSeDi::CCashCoSeDiTest::TestLoad(void)
{
	CoInitialize(NULL);
	
	SS::MANAGER::CManagerConnections ManagerConnection;
	
	SS::CoSeDi::SQL::CCashCoSeDi oCashCoSeDi;
	
	oCashCoSeDi.SetManagerConnections(&ManagerConnection);
	oCashCoSeDi.Load();
	CPPUNIT_ASSERT(oCashCoSeDi.Load());

	CoUninitialize();

}

#endif
