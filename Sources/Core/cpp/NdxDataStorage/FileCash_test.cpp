#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\file_cash.h"
#include ".\filecash_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\data_storages_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CFileCash_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxDataStorage
{

using namespace SS::Core::NdxSE::NdxDataStorage::FileCash;
using namespace SS::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

void CFileCash_test::TestCircularBuffer(void)
{
	const unsigned int uiValuesCount=50;
	unsigned int arrValues[uiValuesCount];
	for(unsigned int i=0; i<uiValuesCount; i++) arrValues[i]=i;

	CCircularBuffer<unsigned int> CircularBuffer(30);
	
	ToConsole(L"//----------Bof-PushBack");
	unsigned int* puiPoped=NULL;
	for(unsigned int i=0; i<uiValuesCount; i++){
		puiPoped=CircularBuffer.PushBack(&arrValues[i]);
		if(puiPoped){
			ToConsole(L"puiPoped", *puiPoped);
		}
		ToConsole(L"Size", CircularBuffer.GetSize());
	}
	ToConsole(L"//----------Eof-PushBack");

	ToConsole(L"//----------Bof-PopBack");
	for(unsigned int i=0; i<3; i++){
		puiPoped=CircularBuffer.PopBack();
		if(puiPoped){
			ToConsole(L"puiPoped", *puiPoped);
		}
		ToConsole(L"Size", CircularBuffer.GetSize());
	}
	ToConsole(L"//----------Eof-PopBack");

	ToConsole(L"//----------Bof-PopFront");
	for(unsigned int i=0; i<2; i++){
		puiPoped=CircularBuffer.PopFront();
		if(puiPoped){
			ToConsole(L"puiPoped", *puiPoped);
		}
		ToConsole(L"Size", CircularBuffer.GetSize());
	}
	ToConsole(L"//----------Eof-PopFront");

	ToConsole(L"//----------Bof-PushBack");
	for(unsigned int i=0; i<uiValuesCount; i++){
		puiPoped=CircularBuffer.PushBack(&arrValues[i]);
		if(puiPoped){
			ToConsole(L"puiPoped", *puiPoped);
		}
		ToConsole(L"Size", CircularBuffer.GetSize());
	}
	ToConsole(L"//----------Eof-PushBack");

	ToConsole(L"//----------Bof-Clear");
	while(puiPoped=CircularBuffer.PopFront()){
		ToConsole(L"puiPoped", *puiPoped);
		ToConsole(L"Size", CircularBuffer.GetSize());
	}
	ToConsole(L"//----------Eof-Clear");
}

void CFileCash_test::TestBuffer(void)
{
	unsigned char buf_a[100];
	unsigned char buf_b[100];
	unsigned char buf_0[100];
	unsigned char buf_c[100];
	memset(buf_a, 0xaa, 100);
	memset(buf_b, 0xbb, 100);
	memset(buf_c, 0xcc, 100);
	memset(buf_0, 0, 100);

	CBuffer Buffer(0, 20);
	unsigned int k=Buffer.GetBufferSize();
	k=Buffer.GetFilledSize();
	Buffer.Resize(10);
	k=Buffer.GetFilledSize();
	Buffer.Resize(0);
	
	k=Buffer.Append(buf_a, 30);
	k=Buffer.GetFilledSize();

	k=Buffer.Read(buf_0, 10);
	Buffer.SeekFromFront(10);
	k=Buffer.Read(buf_0, 10);

	Buffer.SeekFromFront(10);
	k=Buffer.Write(buf_b, 5);

	Buffer.SeekFromFront(0);
	k=Buffer.Read(buf_0, 20);
	
	wstring szValue;
	Buffer.ToString(szValue);
	ToConsole(szValue.c_str());
}

void CFileCash_test::TestCash(void)
{
	unsigned char buf_a[100];
	unsigned char buf_b[100];
	unsigned char buf_0[100];
	unsigned char buf_c[100];
	memset(buf_a, 0xaa, 100);
	memset(buf_b, 0xbb, 100);
	memset(buf_c, 0xcc, 100);
	memset(buf_0, 0, 100);
	const unsigned int uiValuesCount=50;
	unsigned char arrValues[uiValuesCount];
	for(unsigned int i=0; i<uiValuesCount; i++) arrValues[i]=i;

	CFileCash Cash(30, 10);
	
	Cash.SetFileOffset(0);
	Cash.AddData(arrValues, uiValuesCount);
	Cash.View();
	
	Cash.SeekPos(25, SEEK_END);
	Cash.Read(buf_0, 10);

	Cash.SeekPos(10, SEEK_SET);
	Cash.Read(buf_0, 10);

	Cash.SeekPos(20, SEEK_CUR);
	Cash.Read(buf_0, 10);

	Cash.SeekPos(-25, SEEK_CUR);
	Cash.Read(buf_0, 10);

	Cash.Write(arrValues, uiValuesCount);
	Cash.View();
	Cash.SeekPos(-10, SEEK_CUR);
	Cash.Read(buf_0, 10);

	Cash.SeekPos(-20, SEEK_CUR);
	Cash.Look(buf_0, 10);
	Cash.Write(arrValues, uiValuesCount);
	Cash.View();

}

void CFileCash_test::TestCashFlush(void)
{
	unsigned char buf_0[100];
	memset(buf_0, 0, 100);
	const unsigned int uiValuesCount=50;
	unsigned char arrValues[uiValuesCount];
	for(unsigned int i=0; i<uiValuesCount; i++) arrValues[i]=i;

	wstring szFile(L"file_cash");
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	DeleteFile(L".//UT\\file_cash.ut"); 
	CDataUnitFactory DataStorageFactory;

	INdxFile* pFile=DataStorageFactory.CreateBasisFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));

	CFileCash Cash(30, 10);
	
	Cash.Init(dynamic_cast<CBasisFile*>(pFile));
	Cash.SetFileOffset(0);
	
	for(unsigned int i=0; i<1000; i++){
		Cash.AddData(arrValues, uiValuesCount);
		//Cash.View();
		
		Cash.SeekPos(-25, SEEK_CUR);
		Cash.Look(buf_0, 10);
		Cash.Write(arrValues, 35);
	}

	//Cash.FlushFilledBuffersToFile();
	Cash.FlushToFile();

	CPPUNIT_ASSERT(pFile->Close());
	pFile->Release();
}

void CFileCash_test::testReadWrite(void)
{
	unsigned char buf_a[100];
	unsigned char buf_b[100];
	unsigned char buf_0[100];
	unsigned char buf_c[100];
	memset(buf_a, 0xaa, 100);
	memset(buf_b, 0xbb, 100);
	memset(buf_c, 0xcc, 100);
	memset(buf_0, 0, 100);

	CFileCash Cash(30, 10);
	Cash.SetFileOffset(0);
	Cash.AddData(buf_a, 25);
	//Cash.AddData(buf_b, 20);
	//Cash.AddData(buf_b, 20);
	//Cash.AddData(buf_b, 20);
	//Cash.View();

	Cash.SeekPos(-5, SEEK_CUR);
	//Cash.Write(buf_c, 1);

	//Cash.SeekPos(0, SEEK_END);
	//Cash.SeekPos(20, SEEK_END);

	Cash.Write(buf_b, 20);
	//Cash.View();

	Cash.SeekPos(0, SEEK_SET);
	Cash.SeekPos(30, SEEK_CUR);
	//Cash.View();
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS

