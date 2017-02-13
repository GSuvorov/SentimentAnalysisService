#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\filescollectionex_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\files_collections.h"
#include ".\data_storages_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CFilesCollectionEx_test);

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

void CFilesCollectionEx_test::TestOpenClose(void)
{
	wstring szFile(L"mfile");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";
	CDataUnitFactory DataStorageFactory;

	UT_STOP_LOGING

	CFilesCollectionEx* pFilesCollectionEx=DataStorageFactory.CreateFilesCollectionEx();
	pFilesCollectionEx->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFilesCollectionEx->Open(szPath.c_str()));
	pFilesCollectionEx->DeleteFiles();
	pFilesCollectionEx->AddFile();	
	pFilesCollectionEx->AddFile();	
	CPPUNIT_ASSERT(pFilesCollectionEx->Close());
	pFilesCollectionEx->Release();

	UT_START_LOGING
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
