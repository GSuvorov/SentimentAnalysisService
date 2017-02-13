#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\tablestorage_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\table_storage.h"
#include ".\data_storages_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CTableStorage_test);

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
using namespace SS::Interface::Core::DBMS;

//--------------------------------------------------------------------//

void CTableStorage_test::Test(void)
{
	CPPUNIT_ASSERT(CreateLoadManager());
	CDataStorageFactory* pDataStorageFactory=new CDataStorageFactory();
	pDataStorageFactory->SetLoadManager(m_pLoadManager);

	wchar_t path[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH, (LPWSTR)path);
	wcscat(path, L"\\UT\\");
	
	INdxTableStorage* pTableStorage=pDataStorageFactory->CreateNdxTableStorage();
	
	pTableStorage->Create(L"table_str", L".ndx");
	
	TDataTableFieldDescription DataTableFieldDescription;
	
	DataTableFieldDescription.szFieldName=L"Field_01_INT";
	DataTableFieldDescription.etFieldType=TYPE_INT32;
	DataTableFieldDescription.etFieldModificator=MOD_COMMON;
	pTableStorage->AddField(&DataTableFieldDescription);

	DataTableFieldDescription.szFieldName=L"Field_01_STRING";
	DataTableFieldDescription.etFieldType=TYPE_STRING;
	DataTableFieldDescription.etFieldModificator=MOD_IS_UNIC;
	pTableStorage->AddField(&DataTableFieldDescription);

	pTableStorage->Open(path);
	
	pTableStorage->Clear();

	int Field_01_INT;	
	string Field_02_STRING;
	void* apAddArr[]={&Field_01_INT, &Field_02_STRING};
	void* apFindArr[]={&Field_01_INT, &Field_02_STRING};
	void* apScanArr[]={&Field_01_INT, &Field_02_STRING};
	
	//проверка добавления в таблицу
	if(pTableStorage->InitAdd(apAddArr)!=S_OK){
		TO_CONSOLE(L"AddInit");
		return;
	}

	for(int i=0; i<10000; i++)   {

		char szStr[128]; 
		sprintf(szStr,"Str:%ld",i);

		Field_01_INT=i;  
		Field_02_STRING=szStr;

		if((pTableStorage->Add())!=S_OK)   {
			TO_CONSOLE(L"Add");
			break;
		}
	}

	if((pTableStorage->InitFind(L"Field_01_INT",&Field_01_INT,apFindArr))!=S_OK){
		TO_CONSOLE(L"FindInit");
		return;
	}

	int iCnt=0;
	for(int i=0; i<10000; i++)   {
		Field_01_INT        = i;  
		while(pTableStorage->Find()==S_OK)   {
			if(Field_01_INT==i)   iCnt++;
		}
	}

	if(iCnt==10000)  {
		ToConsole(L"Finded OK", (unsigned int)iCnt);
	} else   {
		TO_CONSOLE(L"Find not all");
	}

	if((pTableStorage->InitScan(0,apScanArr))!=S_OK)   {
		TO_CONSOLE(L"ScanInit");
		return;
	}

	iCnt=0;
	i=0;
	while(pTableStorage->Scan()==S_OK)   {
		if(Field_01_INT==i)   iCnt++;
		i++;
	}

	if(iCnt==10000)  {
		ToConsole(L"Scaned OK", (unsigned int)iCnt);
	} else   {
		TO_CONSOLE(L"Scaned not all");
	}


	pTableStorage->Close();
	
	pTableStorage->Release();
	
	delete pDataStorageFactory;

	DeleteLoadManager();
}

bool CTableStorage_test::CreateLoadManager(void)
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

void CTableStorage_test::DeleteLoadManager(void)
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
