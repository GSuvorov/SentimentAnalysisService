#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\basisfile_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\data_storages_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CBasisFile_test);

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

void CBasisFile_test::testOpenAndClose(void)
{
	wstring szFile(L"file");
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	DeleteFile(L".//UT\\file.ut"); 
	CDataUnitFactory DataStorageFactory;

	INdxFile* pFile=DataStorageFactory.CreateBasisFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));
	CPPUNIT_ASSERT(pFile->Close());
	pFile->Release();

	INdxFile* pFile1=DataStorageFactory.CreateBasisFile();
	pFile1->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile1->Open(szPath.c_str()));
	CPPUNIT_ASSERT(pFile1->Close());
	pFile1->Release();

	INdxFile* pFile2=DataStorageFactory.CreateBasisFile();
	pFile2->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile2->Open(szPath.c_str()));
	CPPUNIT_ASSERT(pFile2->Close());
	pFile2->Release();
}

void CBasisFile_test::testReadWrite(void)
{
	wstring szFile(L"file");
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	unsigned char buf[10];
	unsigned char buf1[10];
	memset(buf, 0xaa, 10);
	memset(buf1, 0, 10);
	CDataUnitFactory DataStorageFactory;

	INdxFile* pFile=DataStorageFactory.CreateBasisFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));

	pFile->Write(buf, 10);
	pFile->SeekPos(0, ios_base::beg);	
	pFile->Read(buf1, 10);

	for(int i=0; i<10; i++){
		CPPUNIT_ASSERT_EQUAL((unsigned char)0xaa, buf1[i]);
	}

	CPPUNIT_ASSERT(pFile->Close());
	pFile->Release();
}

void CBasisFile_test::testWrite(void)
{
	wstring szFile(L"file");
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	unsigned char buf[10];
	unsigned char buf1[10];
	memset(buf, 0xaa, 10);
	memset(buf1, 0, 10);

	CDataUnitFactory DataStorageFactory;

	INdxFile* pFile=DataStorageFactory.CreateBasisFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));

	pFile->Write(buf, 10);
	CPPUNIT_ASSERT_EQUAL((unsigned int)10, pFile->GetDataUnitSize());

	pFile->Write(buf1, 10);
	CPPUNIT_ASSERT_EQUAL((unsigned int)20, pFile->GetDataUnitSize());

	CPPUNIT_ASSERT(pFile->Close());
	pFile->Release();
}

void CBasisFile_test::testReadWrite1(void)
{
	//пишем 2 раза в начало файла, затем читаем
	//где-то в середине

	wstring szFile(L"file");
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	unsigned char ucTemp=0;
	unsigned char buf[100];
	unsigned char buf1[100];
	unsigned char buf2[100];
	memset(buf, 0xaa, 100);
	memset(buf1, 0, 100);
	memset(buf2, 0xbb, 100);

	CDataUnitFactory DataStorageFactory;

	INdxFile* pFile=DataStorageFactory.CreateBasisFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));

	pFile->Write(buf, 20);
	pFile->SeekPos(0, ios_base::beg);
	pFile->Write(buf1, 10);
	CPPUNIT_ASSERT_EQUAL((unsigned int)20, pFile->GetDataUnitSize());

	pFile->Read(&ucTemp, 1);
	if(ucTemp==0xaa){
		pFile->Write(buf2, 10);
		CPPUNIT_ASSERT_EQUAL((unsigned int)21, pFile->GetDataUnitSize());
	}else{
		CPPUNIT_ASSERT_EQUAL((unsigned char)0xaa, ucTemp);
	}

	CPPUNIT_ASSERT(pFile->Close());
	pFile->Release();
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
