#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\updatefile_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\update_file.h"
#include ".\data_storages_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CUpdateFile_test);

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

void CUpdateFile_test::testOpenClose(void)
{
	wstring szFile(L"file");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";
	CDataUnitFactory DataStorageFactory;

	unsigned char buf_a[100];
	unsigned char buf_b[100];
	unsigned char buf_0[100];
	memset(buf_a, 0xaa, 100);
	memset(buf_b, 0xbb, 100);
	memset(buf_0, 0, 100);

	CUpdateFile* pFile=DataStorageFactory.CreateUpdateFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));
	
	unsigned int uiDataOffset=NULL;
	unsigned int uiDataSize=NULL;
	unsigned int uiDataSize1=NULL;
	unsigned int uiDataOffset1=NULL;
	
	pFile->StartAppend(&uiDataOffset);
	pFile->Append(buf_a, 10);
	pFile->StopAppend(&uiDataSize);

	pFile->StartAppend(&uiDataOffset1);
	pFile->Append(buf_b, 20);
	pFile->StopAppend(&uiDataSize1);

	pFile->StartAppend(&uiDataOffset);
	pFile->Append(buf_0, 40);
	pFile->StopAppend(&uiDataSize);
	
	pFile->StartRead(uiDataOffset1, &uiDataSize1);
	unsigned char* pBuf=new unsigned char[uiDataSize1];
	pFile->Read(pBuf, uiDataSize1-5);
	pFile->StopRead(&uiDataSize);
	delete[] pBuf;
	
	pFile->DeleteData(uiDataOffset);

	pFile->StartAppend(&uiDataOffset);
	pFile->Append(buf_0, 10);
	pFile->StopAppend(&uiDataSize);

	pFile->Flush();
	CPPUNIT_ASSERT(pFile->Close());
}

void CUpdateFile_test::Test(void)
{
	wstring szFile(L"file");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";
	CDataUnitFactory DataStorageFactory;
	
	unsigned char buf_a[100];
	memset(buf_a, 0xaa, 100);

	std::vector<pair<unsigned int, unsigned int> > vDataAttr;

	CUpdateFile* pFile=DataStorageFactory.CreateUpdateFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));
	
	pFile->Clear();
	
	START_TICK(L"Appending");
	unsigned int uiDataOffset=0;
	unsigned int uiDataSize=0;
	for(unsigned int i=0; i<30000; i++){
		pFile->StartAppend(&uiDataOffset);
		pFile->Append(buf_a, 30);
		pFile->StopAppend(&uiDataSize);
		vDataAttr.push_back(pair<unsigned int, unsigned int>(uiDataOffset, uiDataSize));
	}
	STOP_TICK(L"Appending");

	START_TICK(L"Flush");
	pFile->Flush();
	STOP_TICK(L"Flush");

	START_TICK(L"Reading");
	unsigned char Buf[40];
	for(std::vector<pair<unsigned int, unsigned int> >::iterator itDataAttr=vDataAttr.begin(); itDataAttr!=vDataAttr.end(); itDataAttr++)
	{
		uiDataOffset=itDataAttr->first;
		uiDataSize=itDataAttr->second;
		START_TICK(L"StartRead");
		pFile->StartRead(uiDataOffset, &uiDataSize);
		STOP_TICK(L"StartRead");
		//START_TICK(L"Read");
		pFile->Read(Buf, itDataAttr->second);
		//STOP_TICK(L"Read");
		//START_TICK(L"StopRead");
		pFile->StopRead(&itDataAttr->second);
		//STOP_TICK(L"StopRead");
	}
	STOP_TICK(L"Reading");
	
    CPPUNIT_ASSERT(pFile->Close());
	
	VIEW_TICKS();
}

void CUpdateFile_test::TestShrink(void)
{
	wstring szFile(L"file");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";
	CDataUnitFactory DataStorageFactory;

	CUpdateFile* pFile=DataStorageFactory.CreateUpdateFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());
	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));
	
	pFile->Clear();

	const unsigned int uiValuesNumber=255;
	const unsigned int uiStepsNumber=100;
	unsigned char arrValues[uiValuesNumber];
	unsigned int arrSizes[uiValuesNumber];
	unsigned int arrOffsets[uiValuesNumber];
	unsigned int arrTotalSize[uiValuesNumber];
	unsigned char ucMarker=0xff;

	for(unsigned int i=0; i<uiValuesNumber; i++){
		arrValues[i]=i;

		arrSizes[i]=rand()&0x00001fff;
		arrSizes[i]=arrSizes[i]<500?arrSizes[i]+500:arrSizes[i];

		arrOffsets[i]=EMPTY_VALUE;
		arrTotalSize[i]=0;
	}


	unsigned char* pusBuffer=NULL;
	unsigned int uiBufferSize;
	unsigned int uiTemp;

	START_TICK(L"Filling");

	ToConsole(L"//---------------Bof-Filling");

	for(unsigned int uiStep=0; uiStep<uiStepsNumber; uiStep++){
		ToConsole(L"Step", uiStep);

		START_TICK(L"Other");
		for(unsigned int i=0; i<uiValuesNumber; i++){
			
			pusBuffer=NULL;
			uiBufferSize=0;

			if(arrOffsets[i]!=EMPTY_VALUE){
				pFile->StartRead(arrOffsets[i], &uiBufferSize);

				pFile->Read((unsigned char*)&uiTemp, sizeof(unsigned int));
				
				CPPUNIT_ASSERT(uiTemp==arrTotalSize[i]);
				arrTotalSize[i]=uiTemp;
				
				uiBufferSize-=sizeof(unsigned int);
				pusBuffer=(unsigned char*)malloc(uiBufferSize);
				pFile->Read(pusBuffer, uiBufferSize);
				pFile->StopRead(&uiTemp);
				pFile->DeleteData(arrOffsets[i]);
			}
			
			pFile->StartAppend(&arrOffsets[i]);
			pFile->Append((unsigned char*)&arrTotalSize[i], sizeof(unsigned int));
			if(pusBuffer)
				pFile->Append(pusBuffer, uiBufferSize);

			uiBufferSize=arrSizes[i];
			//arrTotalSize[i]+=uiBufferSize;

			pusBuffer=(unsigned char*)realloc(pusBuffer, uiBufferSize);
			memset(pusBuffer, arrValues[i], uiBufferSize);
			pFile->Append(pusBuffer, uiBufferSize);
			pFile->Append((unsigned char*)&ucMarker, sizeof(unsigned char));

			pFile->StopAppend(&uiBufferSize);
			arrTotalSize[i]=uiBufferSize;
			
			int a=-(int)(arrTotalSize[i]);
			pFile->SeekPosInData(a, SEEK_CUR);
			arrTotalSize[i]-=sizeof(unsigned int);
			pFile->WriteData((unsigned char*)&arrTotalSize[i], sizeof(unsigned int));

			if(pusBuffer) free(pusBuffer);
		}
		STOP_TICK(L"Other");

		//ViewMemoryStatus();
		//if(uiStep%100==0)
		START_TICK(L"Shrink");
		pFile->Flush();
		STOP_TICK(L"Shrink");
		//ViewMemoryStatus();
	}

	ViewMemoryStatus();
	START_TICK(L"Shrink");
	pFile->Shrink();
	STOP_TICK(L"Shrink");
	ViewMemoryStatus();
	
	ToConsole(L"//---------------Eof-Filling");
	STOP_TICK(L"Filling");

	ToConsole(L"//---------------Bof-Checking");
	for(unsigned int i=0; i<uiValuesNumber; i++){

		pFile->StartRead(arrOffsets[i], &uiBufferSize);

		pFile->Read((unsigned char*)&uiTemp, sizeof(unsigned int));
		CPPUNIT_ASSERT(uiTemp==arrTotalSize[i]);
		arrTotalSize[i]=uiTemp;
		
		uiBufferSize-=sizeof(unsigned int);
		pusBuffer=(unsigned char*)malloc(uiBufferSize);
		pFile->Read(pusBuffer, uiBufferSize);
		pFile->StopRead(&uiTemp);
		
		unsigned char* pCurrPtr=pusBuffer;
		while(pCurrPtr<pusBuffer+uiBufferSize){
			CPPUNIT_ASSERT(*(pCurrPtr+arrSizes[i])==ucMarker);
			pCurrPtr+=arrSizes[i]+1;
		}
	}	
	ToConsole(L"//---------------Eof-Checking");
	
    CPPUNIT_ASSERT(pFile->Close());
	
	VIEW_TICKS();
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
