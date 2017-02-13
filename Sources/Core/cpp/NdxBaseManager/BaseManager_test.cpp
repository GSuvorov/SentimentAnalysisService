#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\basemanager_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\base_manager.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxBaseManager::CBaseManager_test);

typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** ppBase);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxBaseManager
{

using namespace SS::Core::NdxSE::NdxBaseManager;

//--------------------------------------------------------------------//

void CBaseManager_test::Test(void)
{
	
	CPPUNIT_ASSERT(CreateLoadManager());

	CNdxBaseManager* pBaseManager=new CNdxBaseManager();

	wchar_t path[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH, (LPWSTR)path);
	wcscat(path, L"\\UT\\");
	
	pBaseManager->SetLoadManager(m_pLoadManager);
	pBaseManager->SetBasePath(path);
	pBaseManager->OpenBase(L"test_base");
	pBaseManager->ClearBase();
	pBaseManager->CloseBase();
	
	delete pBaseManager;

	DeleteLoadManager();
}

bool CBaseManager_test::CreateLoadManager(void)
{
	m_pLoadManager=NULL;
	wchar_t path[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH, (LPWSTR)path);
	wcscat(path, L"\\LoadManager.dll");
	m_hLoadManager=::LoadLibraryW(path);
	
	CREATE_INSTANCE pCreateInstance=(CREATE_INSTANCE)GetProcAddress(m_hLoadManager, "CreateInstance");


	if(pCreateInstance==NULL){
		wprintf(L"LoadManager entry point not found, error %u\n", GetLastError());
		return 0;
	}
	
	const GUID Guid=CLSID_LoadManager;
	(*pCreateInstance)(&Guid, (void**)&m_pLoadManager);
	
	return m_pLoadManager?true:false;
}

void CBaseManager_test::DeleteLoadManager(void)
{
	//if(m_pLoadManager) delete m_pLoadManager;
	//::FreeLibrary(m_hLoadManager);
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
