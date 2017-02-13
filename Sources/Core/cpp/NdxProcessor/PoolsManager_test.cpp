#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\poolsmanager_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\pools_manager.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::MemoryManagement::CPoolsManager_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace MemoryManagement
{

using namespace SS::Core::NdxSE::NdxProcessor::MemoryManagement;
using namespace SS::Core::NdxSE::NdxProcessor;

//--------------------------------------------------------------------//

void CPoolsManager_test::Test(void)
{
	UT_STOP_LOGING

	ToConsole(L"CPoolsManager_test::Test");

	CPoolsManager PoolsManager(false);
	PoolsManager.RegisterClass(L"arr", 600);
		
	void* arr[200000];
	for(int i=0; i<200000; i++){
		arr[i]=(CBuffer*)(PoolsManager.AllocObject(600));
		if(ISNULL(arr[i])) continue;
	}

	ToConsole(L"CPoolsManager_test::Alloc");

	PoolsManager.View();

	for(int i=0; i<200000; i++){
		if(ISNULL(arr[i])) continue;
		PoolsManager.FreeObject(arr[i]);
	}

	ToConsole(L"CPoolsManager_test::Free");

	PoolsManager.View();

	UT_START_LOGING
}

void CPoolsManager_test::Test1(void)
{
	UT_STOP_LOGING

	CPoolsManager PoolsManager(false);
	PoolsManager.RegisterClass(L"arr", 10);
		
	void* arr[200000];
	for(int i=0; i<2; i++){
		arr[i]=(CBuffer*)(PoolsManager.AllocObject(10));
		if(ISNULL(arr[i])) continue;
	}
	PoolsManager.View();
	for(int i=0; i<2; i++){
		if(ISNULL(arr[i])) continue;
		PoolsManager.FreeObject(arr[i]);
	}

	ToConsole(L"CPoolsManager_test::Free");
	PoolsManager.ResetPools();
	PoolsManager.View();

	for(int i=0; i<2; i++){
		arr[i]=(CBuffer*)(PoolsManager.AllocObject(10));
		if(ISNULL(arr[i])) continue;
	}
	PoolsManager.View();
	for(int i=0; i<2; i++){
		if(ISNULL(arr[i])) continue;
		PoolsManager.FreeObject(arr[i]);
	}

	UT_START_LOGING
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
