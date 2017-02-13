#include "StdAfx.h"
#include ".\testrunner.h"

#ifdef _SS_UNITTESTS

namespace SS
{
namespace UnitTests
{
namespace MainAnalyse
{
	CTestRunner::CTestRunner(void)
	{
	}

	CTestRunner::~CTestRunner(void)
	{
	}

	void CTestRunner::Run()
	{
	}

	ULONG CTestRunner::Release()
	{
		delete this;
		return 0;
	}

	HRESULT CTestRunner::QueryInterface(REFIID pIID, void** pBase)
	{
		if(pIID==IID_TestRunner)
			*pBase=(SS::Interface::UnitTests::ITestRunner*)this;
		else
			*pBase=NULL;

		return S_OK;
	}

}
}
}

#endif //_SS_UNITTESTS
