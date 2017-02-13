#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\levelstorage_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\level_storage.h"
#include ".\data_storages_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxDataStorage::CLevelStorage_test);

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

void CLevelStorage_test::testOpenAndClose(void)
{
	wstring szFile(L"level_str");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";
	
	SCompositeOffset CompositeOffset;
	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	unsigned char buf_a[100];
	unsigned char buf_b[100];
	unsigned char buf_0[100];
	memset(buf_a, 0xaa, 100);
	memset(buf_b, 0xbb, 100);
	memset(buf_0, 0, 100);
	CDataUnitFactory DataStorageFactory;

	INdxLevelStorage* pLevelStorage=DataStorageFactory.CreateLevelStorage();
	pLevelStorage->Create(szFile.c_str(), szFileExt.c_str());
	if(szFile!=pLevelStorage->GetDataUnitName()){
		TO_CONSOLE(L"szFile");
	}
	CPPUNIT_ASSERT(pLevelStorage->Open(szPath.c_str()));

	pLevelStorage->Clear();
	
	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;
	
	pLevelStorage->StartCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_a, 20);
	pLevelStorage->StopAppend(NULL);

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;
	unsigned int uiSize=0;

	pLevelStorage->StartUnCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_a, 20);
	pLevelStorage->StopAppend(&uiSize);

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	pLevelStorage->StartUnCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_b, 20);
	pLevelStorage->StopAppend(&uiSize);

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	pLevelStorage->StartCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_b, 20);
	pLevelStorage->StopAppend(NULL);

	
	pLevelStorage->Flush();

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	pLevelStorage->StartCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_0, 10);
	pLevelStorage->StopAppend(NULL);

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	pLevelStorage->StartCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_0, 30);
	pLevelStorage->StopAppend(NULL);

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	pLevelStorage->StartCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_a, 30);
	pLevelStorage->StopAppend(NULL);

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	pLevelStorage->StartUnCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_0, 10);
	pLevelStorage->StopAppend(&uiSize);

	pLevelStorage->Flush();

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	pLevelStorage->StartUnCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_b, 10);
	pLevelStorage->StopAppend(&uiSize);

	CompositeOffset.ucFileIndex=0;
	CompositeOffset.uiOffset=0;
	pLevelStorage->Delete(&CompositeOffset);

	CompositeOffset.ucFileIndex=0xff;
	CompositeOffset.uiOffset=EMPTY_OFFSET;

	pLevelStorage->StartUnCompleteDataAppend(&CompositeOffset);
	pLevelStorage->Append(buf_a, 10);
	pLevelStorage->StopAppend(&uiSize);

	CPPUNIT_ASSERT(pLevelStorage->Close());
	pLevelStorage->Release();
}

void CLevelStorage_test::Test(void)
{
	wstring szFile(L"level_str");	
	wstring szFileExt(L".ut");
	wstring szPath=L".//UT\\";
	
	unsigned char buf_a[100000];
	memset(buf_a, 0xaa, 100000);
	std::vector<pair<unsigned int, unsigned int> > vDataAttr;

	CDataUnitFactory DataStorageFactory;

	INdxLevelStorage* pLevelStorage=DataStorageFactory.CreateLevelStorage();
	pLevelStorage->Create(szFile.c_str(), szFileExt.c_str());
	if(szFile!=pLevelStorage->GetDataUnitName()){
		TO_CONSOLE(L"szFile");
	}
	CPPUNIT_ASSERT(pLevelStorage->Open(szPath.c_str()));
	
	SCompositeOffset CompositeOffset;

	pLevelStorage->Clear();
	
	START_TICK(L"Appending");
	unsigned int uiDataOffset=0;
	unsigned int uiDataSize=0;
	for(unsigned int i=0; i<30000; i++){
		CompositeOffset.uiOffset=EMPTY_VALUE;
		CompositeOffset.ucFileIndex=UC_EMPTY;
		pLevelStorage->StartUnCompleteDataAppend(&CompositeOffset);
		pLevelStorage->Append(buf_a, 30);
		pLevelStorage->StopAppend(&uiDataSize);
		vDataAttr.push_back(pair<unsigned int, unsigned int>(CompositeOffset.uiOffset, uiDataSize));
	}
	STOP_TICK(L"Appending");

	START_TICK(L"Flush");
	pLevelStorage->Flush();
	STOP_TICK(L"Flush");

	START_TICK(L"Reading");
	unsigned char Buf[40];
	for(std::vector<pair<unsigned int, unsigned int> >::iterator itDataAttr=vDataAttr.begin(); itDataAttr!=vDataAttr.end(); itDataAttr++)
	{
		CompositeOffset.uiOffset=itDataAttr->first;
		CompositeOffset.ucFileIndex=0;
		uiDataSize=itDataAttr->second;
		START_TICK(L"StartRead");
		pLevelStorage->StartRead(&CompositeOffset, &uiDataSize);
		STOP_TICK(L"StartRead");
		//START_TICK(L"Read");
		pLevelStorage->Read(Buf, itDataAttr->second);
		//STOP_TICK(L"Read");
		//START_TICK(L"StopRead");
		pLevelStorage->StopRead(&itDataAttr->second);
		//STOP_TICK(L"StopRead");
	}
	STOP_TICK(L"Reading");
	
	VIEW_TICKS();

	CPPUNIT_ASSERT(pLevelStorage->Close());
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
