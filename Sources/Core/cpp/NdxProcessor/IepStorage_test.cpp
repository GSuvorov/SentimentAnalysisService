#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\iepstorage_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\iep_storage.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Storages::CIndexEntryPointTable_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

void CIndexEntryPointTable_test::TestAll(void)
{
	UT_STOP_LOGING
	
	//получаем фабрику хранилищ
	INdxDataStorageFactory* pNdxDataStorageFactory=(INdxDataStorageFactory*)GetLoadManager()->
	GetInterface(L"./core/NdxDataStorage.dll", CLSID_NdxDataStorageFactory, IID_NdxDataStorageFactory);
	
	//проверка добавления данных
	TestAddData(pNdxDataStorageFactory);
	//проверка получения данных
	TestGetData(pNdxDataStorageFactory);
	//проверка изменения данных
	TestModifyData(pNdxDataStorageFactory);

	UT_START_LOGING

}

void CIndexEntryPointTable_test::TestAddData(INdxDataStorageFactory* pNdxDataStorageFactory)
{
	if(ISNULL(pNdxDataStorageFactory)) return;

	INdxStorage* pNdxStorage=pNdxDataStorageFactory->CreateNdxStorage();
	if(ISNULL(pNdxStorage)) return;
	pNdxStorage->Create(L"iep_table_test", L".ut");

	CPPUNIT_ASSERT(pNdxStorage->Open(GetUTWorkingPath()));
	
	CIndexEntryPointTable* pIndexEntryPointTable=new CIndexEntryPointTable(pNdxStorage->GetEntryPointsStorage());

	for(unsigned int i=0; i<1000; i++) 
		CPPUNIT_ASSERT(pIndexEntryPointTable->SetIndexEntryPoint(i, i));


	delete pIndexEntryPointTable;
	CPPUNIT_ASSERT(pNdxStorage->Close());
	pNdxStorage->Release();
}

void CIndexEntryPointTable_test::TestGetData(INdxDataStorageFactory* pNdxDataStorageFactory)
{
	if(ISNULL(pNdxDataStorageFactory)) return;

	INdxStorage* pNdxStorage=pNdxDataStorageFactory->CreateNdxStorage();
	if(ISNULL(pNdxStorage)) return;
	pNdxStorage->Create(L"iep_table_test", L".ut");

	CPPUNIT_ASSERT(pNdxStorage->Open(GetUTWorkingPath()));
	
	CIndexEntryPointTable* pIndexEntryPointTable=new CIndexEntryPointTable(pNdxStorage->GetEntryPointsStorage());

	for(unsigned int i=0; i<1000; i++)
		CPPUNIT_ASSERT_EQUAL(pIndexEntryPointTable->GetIndexEntryPoint(i), i);

	delete pIndexEntryPointTable;
	CPPUNIT_ASSERT(pNdxStorage->Close());
	pNdxStorage->Release();
}

void CIndexEntryPointTable_test::TestModifyData(INdxDataStorageFactory* pNdxDataStorageFactory)
{
	if(ISNULL(pNdxDataStorageFactory)) return;

	INdxStorage* pNdxStorage=pNdxDataStorageFactory->CreateNdxStorage();
	if(ISNULL(pNdxStorage)) return;
	pNdxStorage->Create(L"iep_table_test", L".ut");

	CPPUNIT_ASSERT(pNdxStorage->Open(GetUTWorkingPath()));
	
	CIndexEntryPointTable* pIndexEntryPointTable=new CIndexEntryPointTable(pNdxStorage->GetEntryPointsStorage());

	for(unsigned int i=0; i<1000; i++) 
		CPPUNIT_ASSERT(pIndexEntryPointTable->ModifyIndexRecordDF(i, i));

	delete pIndexEntryPointTable;
	CPPUNIT_ASSERT(pNdxStorage->Close());
	pNdxStorage->Release();


	pNdxStorage=pNdxDataStorageFactory->CreateNdxStorage();
	if(ISNULL(pNdxStorage)) return;
	pNdxStorage->Create(L"iep_table_test", L".ut");

	CPPUNIT_ASSERT(pNdxStorage->Open(GetUTWorkingPath()));
	
	pIndexEntryPointTable=new CIndexEntryPointTable(pNdxStorage->GetEntryPointsStorage());

	for(unsigned int i=0; i<1000; i++){ 
		CPPUNIT_ASSERT_EQUAL(pIndexEntryPointTable->GetIndexDF(i), i);
	}

	delete pIndexEntryPointTable;
	CPPUNIT_ASSERT(pNdxStorage->Close());
	pNdxStorage->Release();

}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS