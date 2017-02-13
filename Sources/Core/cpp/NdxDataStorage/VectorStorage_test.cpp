#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\vectorstorage_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\vector_storage.h"
#include ".\data_storages_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CVectorStorage_test);

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

void CVectorStorage_test::TestOpenClose(void)
{
	UT_STOP_LOGING

	wstring szFile(L"vs");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";
	CDataStorageFactory DataStorageFactory;
	
	INdxVectorStorage* pVectorStorage=DataStorageFactory.CreateNdxVectorStorage();
	pVectorStorage->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pVectorStorage->Open(szPath.c_str()));
	CPPUNIT_ASSERT(pVectorStorage->Close());
	pVectorStorage->Release();

	INdxVectorStorage* pVectorStorage1=DataStorageFactory.CreateNdxVectorStorage();
	pVectorStorage1->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pVectorStorage1->Open(szPath.c_str()));
	CPPUNIT_ASSERT(pVectorStorage1->Close());
	pVectorStorage1->Release();

	UT_START_LOGING
}

void CVectorStorage_test::TestReadAppend(void)
{
	UT_STOP_LOGING

	wstring szFile(L"vs");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	unsigned char buf_a[100];
	unsigned char buf_b[100];
	unsigned char buf_0[100];
	memset(buf_a, 0xaa, 100);
	memset(buf_b, 0xbb, 100);
	memset(buf_0, 0, 100);
	CDataStorageFactory DataStorageFactory;
	
	unsigned int uiIndex=0;
	INdxVectorStorage* pVectorStorage=DataStorageFactory.CreateNdxVectorStorage();
	pVectorStorage->Create(szFile.c_str(), szFileExt.c_str());

	CPPUNIT_ASSERT(pVectorStorage->Open(szPath.c_str()));
	pVectorStorage->Clear();
	pVectorStorage->AppendWholeData(buf_a, 30, &uiIndex);	
	pVectorStorage->AppendWholeData(buf_b, 50, &uiIndex);	
	pVectorStorage->AppendWholeData(buf_a, 20, &uiIndex);	
	pVectorStorage->AppendWholeData(buf_0, 40, &uiIndex);	
	
	unsigned int uiDataSize=0;
	pVectorStorage->MoveToAndGetDataSize(2, &uiDataSize);
	pVectorStorage->ReadWholeData(buf_0, uiDataSize);

	pVectorStorage->MoveToAndGetDataSize(1, &uiDataSize);
	pVectorStorage->ReadWholeData(buf_0, uiDataSize);

	pVectorStorage->Delete(1);

	pVectorStorage->MoveToAndGetDataSize(1, &uiDataSize);
	pVectorStorage->ReadWholeData(buf_0, uiDataSize);

	pVectorStorage->AppendWholeData(buf_b, 100, &uiIndex);	
	pVectorStorage->AppendWholeData(buf_a, 40, &uiIndex);	

	memset(buf_0, 0, 100);
	pVectorStorage->AppendDataChunk(buf_a, 10);	
	pVectorStorage->AppendDataChunk(buf_b, 15);	
	pVectorStorage->AppendDataChunk(buf_0, 20);	
	pVectorStorage->AppendDataChunkComplete(&uiIndex);

	pVectorStorage->MoveToAndGetDataSize(uiIndex, &uiDataSize);
	pVectorStorage->ReadWholeData(buf_0, uiDataSize);

	memset(buf_0, 0, 100);
	pVectorStorage->AppendDataChunk(buf_a, 20);	
	pVectorStorage->AppendDataChunk(buf_b, 25);	
	pVectorStorage->AppendDataChunk(buf_0, 30);	
	pVectorStorage->AppendDataChunkComplete(&uiIndex);

	pVectorStorage->MoveToAndGetDataSize(uiIndex, &uiDataSize);
	pVectorStorage->ReadWholeData(buf_0, uiDataSize);

	CPPUNIT_ASSERT(pVectorStorage->Close());

	pVectorStorage->Release();

	UT_START_LOGING
}

void CVectorStorage_test::TestAppend(void)
{
	UT_STOP_LOGING

	wstring szFile(L"vs");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	unsigned char buf_a[100];
	memset(buf_a, 0xaa, 100);
	CDataStorageFactory DataStorageFactory;
	
	unsigned int uiIndex=0;
	INdxVectorStorage* pVectorStorage=DataStorageFactory.CreateNdxVectorStorage();
	pVectorStorage->Create(szFile.c_str(), szFileExt.c_str());

	CPPUNIT_ASSERT(pVectorStorage->Open(szPath.c_str()));
	pVectorStorage->Clear();
	
	for(int i=0; i<10000; i++){
		pVectorStorage->AppendWholeData(buf_a, 40, &uiIndex);	
	}

	CPPUNIT_ASSERT(pVectorStorage->Close());

	pVectorStorage->Release();

	UT_START_LOGING
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
