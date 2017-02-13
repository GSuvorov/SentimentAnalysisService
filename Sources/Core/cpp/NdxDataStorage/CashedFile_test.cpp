#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\cashedfile_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\data_storages_factory.h"


CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CCashedFile_test);

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

void CCashedFile_test::TestAppend(void)
{
	wstring szFile(L"cashed_file");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	CDataUnitFactory DataStorageFactory;

	INdxCashedFile* pFile=DataStorageFactory.CreateCashedFile();
	//INdxFile* pFile=DataStorageFactory.CreateBasisFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());

	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));
	pFile->Clear();
	
	START_TICK(L"Append");
	unsigned char* pusBuffer=NULL;
	unsigned int uiBufferSize;
	unsigned int uiTotalBufferSize=0;
	for(unsigned int i=0; i<1500000; i++){

		uiBufferSize=rand()&0xff;
		uiBufferSize=uiBufferSize==0?100:uiBufferSize;
	
		pusBuffer=(unsigned char*)malloc(uiBufferSize);
		memset(pusBuffer, 0, uiBufferSize);
		uiTotalBufferSize+=uiBufferSize;
		pFile->Append(pusBuffer, uiBufferSize);
		if(pusBuffer) free(pusBuffer);
	}
	STOP_TICK(L"Append");
	VIEW_TICKS();

	CPPUNIT_ASSERT(pFile->GetDataUnitSize()==uiTotalBufferSize);

	CPPUNIT_ASSERT(pFile->Close());
	pFile->Release();
	
}

void CCashedFile_test::TestReadAppend(void)
{
	wstring szFile(L"cashed_file");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	CDataUnitFactory DataStorageFactory;

	INdxCashedFile* pFile=DataStorageFactory.CreateCashedFile();
	//INdxFile* pFile=DataStorageFactory.CreateBasisFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());

	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));
	pFile->Clear();
	
	vector<SBufInfo> vecBuffers;
	
	unsigned int uiStep=0;
	unsigned int uiCheckedBufferForStep=10000;
	unsigned int uiAppenedBufferForStep=30000;
	unsigned int uiTotalBufferSize=0;
	while(uiStep++<500){
		ToConsole(L"Current Step", uiStep);

		ToConsole(L"//---------------Bof-Filling");

		//добавляем буфера в файл
		START_TICK(L"Append");
		unsigned char* pucBuffer=NULL;
		unsigned int uiBufferSize;
		for(unsigned int i=0; i<uiAppenedBufferForStep; i++){

			uiBufferSize=rand()&0x0000ff;
			uiBufferSize=uiBufferSize==0?15:uiBufferSize;
		
			pucBuffer=(unsigned char*)malloc(uiBufferSize);
			memset(pucBuffer, uiBufferSize, uiBufferSize);
			uiTotalBufferSize+=uiBufferSize;
			pFile->Append(pucBuffer, uiBufferSize);

			SBufInfo BufInfo;
			BufInfo.m_uiOffset=pFile->TellPos()-uiBufferSize;
			BufInfo.m_pucBuffer=pucBuffer;
			BufInfo.m_uiBufferSize=uiBufferSize;
			vecBuffers.push_back(BufInfo);

		}
		STOP_TICK(L"Append");

		ToConsole(L"//---------------Eof-Filling");
		ToConsole(L"File Size", pFile->GetDataUnitSize());

		CPPUNIT_ASSERT(pFile->GetDataUnitSize()==uiTotalBufferSize);
		
		ToConsole(L"//---------------Bof-Checking");
		//теперь проходим по массиву буферов, читаем из файла и сравниваем с оригиналом
		unsigned int k=0;
		for(unsigned int i=0; i<uiCheckedBufferForStep; i++){
			//случайно выбираем буфер, и проверяем его
			k=rand()&((unsigned int)vecBuffers.size()-1);
			SBufInfo BufInfo=vecBuffers[k];

			pucBuffer=(unsigned char*)malloc(BufInfo.m_uiBufferSize);

			//читаем буфер 
			pFile->SeekPos(BufInfo.m_uiOffset, SEEK_SET);
			pFile->Read(pucBuffer, BufInfo.m_uiBufferSize);
			
			//сравниваем его содержимое с эталонным
			int iRes=memcmp(pucBuffer, BufInfo.m_pucBuffer, BufInfo.m_uiBufferSize);
			if(iRes!=0){
				int a=0;
			}
			CPPUNIT_ASSERT(iRes==0);

			free(pucBuffer);
		}
		ToConsole(L"//---------------Eof-Checking");

		for(unsigned int i=0; i<(unsigned int)vecBuffers.size(); i++){
			if(vecBuffers[i].m_pucBuffer) free(vecBuffers[i].m_pucBuffer);
		}
		vecBuffers.clear();

		//Sleep(1000);
	}


	CPPUNIT_ASSERT(pFile->Close());
	pFile->Release();
	
	VIEW_TICKS();

}

void CCashedFile_test::testOpenClose(void)
{
	wstring szFile(L"cashed_file");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";

	unsigned char buf_a[100];
	unsigned char buf_b[100];
	unsigned char buf_0[100];
	memset(buf_a, 0xaa, 100);
	memset(buf_b, 0xbb, 100);
	memset(buf_0, 0, 100);
	CDataUnitFactory DataStorageFactory;

	CACashedFile* pFile=DataStorageFactory.CreateAppendingCashedFile();
	pFile->Create(szFile.c_str(), szFileExt.c_str());

	CPPUNIT_ASSERT(pFile->Open(szPath.c_str()));
	pFile->SeekPos(0, SEEK_SET);
	pFile->Append(buf_a, 10);
	pFile->Append(buf_b, 10);
	pFile->SeekPos(-10, SEEK_CUR);
	pFile->Read(buf_0, 10);
	pFile->Flush();
	pFile->SeekPos(0, SEEK_SET);
	pFile->Write(buf_a, 10);
	CPPUNIT_ASSERT(pFile->Close());
	pFile->Release();

	CACashedFile* pFile1=DataStorageFactory.CreateAppendingCashedFile();
	pFile1->Create(szFile.c_str(), szFileExt.c_str());

	CPPUNIT_ASSERT(pFile1->Open(szPath.c_str()));
	pFile1->SeekPos(0, SEEK_SET);
	pFile1->Read(buf_0, 10);
	unsigned int i=pFile1->TellPos();
	pFile1->Read(buf_0, 10);
	i=pFile1->TellPos();
	CPPUNIT_ASSERT(pFile1->Close());
	pFile1->Release();
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
