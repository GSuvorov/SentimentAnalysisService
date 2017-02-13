#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\indexstorage_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\index_storage.h"
#include ".\data_storages_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CIndexStorage_test);

typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** ppBase);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxDataStorage
{

using namespace SS::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

void CIndexStorage_test::Test(void)
{
	CPPUNIT_ASSERT(CreateLoadManager());
	CDataStorageFactory* pDataStorageFactory=new CDataStorageFactory();
	pDataStorageFactory->SetLoadManager(m_pLoadManager);

	wchar_t path[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH, (LPWSTR)path);
	wcscat(path, L"\\UT\\");
	
	INdxStorage* pNdxStorage=pDataStorageFactory->CreateNdxStorage();
	
	pNdxStorage->Create(L"index_str", L".ndx");
	
	SNdxLevelInfo NdxLevelInfo;
	
	NdxLevelInfo.m_ucLevelNumber=0;
	NdxLevelInfo.m_IndexCoordinateType=SNdxLevelInfo::ictSentenceAbsNumber;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctUndefined;
	NdxLevelInfo.m_eControlByType=SNdxLevelInfo::lctUndefined;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);

	NdxLevelInfo.m_ucLevelNumber++;
	pNdxStorage->AddLevelInfo(&NdxLevelInfo);

	NdxLevelInfo.m_ucLevelNumber++;
	pNdxStorage->AddLevelInfo(&NdxLevelInfo);

	NdxLevelInfo.m_ucLevelNumber++;
	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	
	pNdxStorage->Open(path);
	
	pNdxStorage->Close();

	pNdxStorage->Release();

	delete pDataStorageFactory;
	DeleteLoadManager();
}

bool CIndexStorage_test::CreateLoadManager(void)
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

void CIndexStorage_test::DeleteLoadManager(void)
{
	if(m_pLoadManager) delete m_pLoadManager;
	::FreeLibrary(m_hLoadManager);
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
