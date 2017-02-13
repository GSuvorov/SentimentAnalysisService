#include "StdAfx.h"
#include "TestRunner.h"
#include <iostream>

namespace SS
{
namespace UnitTests
{
namespace ResourceManagers
{

//ÊÎÍÑÒÐÓÊÒÎÐÛ
CTestRunner::CTestRunner() : m_pBaseManagerTest(NULL)
{
}

CTestRunner::~CTestRunner()
{
	if (m_pBaseManagerTest)
	{
		delete m_pBaseManagerTest;
	}
}

//ÌÅÒÎÄ ITestRunner - à
void CTestRunner::Run()
{
	SS_TRY
	{
		m_pBaseManagerTest = new CBaseManagerTest();
		m_pBaseManagerTest->SetLoadManager( GetLoadManager() );
		m_pBaseManagerTest->Run();	
	}			
	SS_CATCH(L"")
}

ULONG CTestRunner::Release()
{
	delete this;
	return 0;
}

HRESULT CTestRunner::QueryInterface( REFIID pIID, void** pBase )
{

	*pBase=NULL;
	if( pIID == IID_TestRunner )
	{
		*pBase=static_cast< SS::Interface::UnitTests::ITestRunner* >( this );
	}
	else if( pIID == IID_Base )
	{
		*pBase=static_cast< SS::Interface::IBase* >( this );
	}
	
	return S_OK;
}


}
}
}
