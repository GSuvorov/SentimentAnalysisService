#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\boxcontainers_test.h"
#include ".\box_containers.h"
#include ".\box_containers_s.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Containers::CBoxContainers_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Core::NdxSE::NdxProcessor::MemoryManagement;
using namespace SS::Core::NdxSE::NdxProcessor;

//--------------------------------------------------------------------//

void  CBoxContainers_test::TestBitMap_4i(void)
{
	g_PoolsManager.RegisterClass(L"CBitChunk", sizeof(CBitChunk), 1000);
	int a=sizeof(CBitMap);
	CBitMap BitMap;
	//BitMap.View();

	//BitMap.AddBit(0);
	//BitMap.View();
	
	BitMap.AddBit(2);
	BitMap.AddBit(5);
	BitMap.AddBit(7);
	BitMap.AddBit(34);
	BitMap.AddBit(41);
	BitMap.AddBit(67);
	BitMap.AddBit(189);
	BitMap.AddBit(274);
	BitMap.AddBit(345);
	BitMap.AddBit(346);
	BitMap.AddBit(347);
	BitMap.AddBit(1015);
	BitMap.AddBit(2516);
	BitMap.AddBit(2836);
	BitMap.AddBit(11020);
	BitMap.AddBit(10000);
	BitMap.AddBit(11021);
//	BitMap.View();

	unsigned short usBitChunkNumber=BitMap.GetBitChunkNumber();
	CBitChunk* pTempBitChunk=BitMap.GetTailBitChunk();
	unsigned char ucChunkSize=pTempBitChunk->GetBitBoxesNumber(); 
	while(pTempBitChunk){
		unsigned char* pBuffer=pTempBitChunk->GetBitBoxBuffer();
		ucChunkSize=cucBoxInBitChunk;
		pTempBitChunk=pTempBitChunk->GetNextItem();
	}
}

void CBoxContainers_test::TestBitMap_4s(void)
{
	CBitMap_4s BitMap(4096);
	BitMap.SetOrt(0);
	BitMap.SetOrt(1);
	BitMap.SetOrt(4);
	BitMap.SetOrt(6);
	BitMap.SetOrt(7);
	BitMap.SetOrt(2068);
	BitMap.SetOrt(8);
	BitMap.SetOrt(28);
	BitMap.SetOrt(135);
	BitMap.SetOrt(41);
	BitMap.SetOrt(1025);
	BitMap.SetOrt(23);
	BitMap.View();

	bool bRes;
	bRes=BitMap.HasOrt(41);
	bRes=BitMap.HasOrt(15);
	bRes=BitMap.HasOrt(76);
	bRes=BitMap.HasOrt(28);
	bRes=BitMap.HasOrt(31);
	bRes=BitMap.HasOrt(1025);
	
	unsigned int i=BitMap.GetOrtsMaxNumber();

	BitMap.MoveLastOrt();
	i=BitMap.GetCurrentOrt();

	CBitMap_4s BitMap1(4096);
	BitMap1.SetOrt(23);
	BitMap1.SetOrt(43);
	BitMap1.SetOrt(56);
	BitMap1.SetOrt(28);
	BitMap1.SetOrt(1098);
	BitMap1.SetOrt(518);
	BitMap1.SetOrt(1025);
	BitMap1.SetOrt(2068);
	BitMap1.SetOrt(41);
	BitMap1.View();
	BitMap.MakeAnd(&BitMap1);
	BitMap.View();

	BitMap.MoveLastOrt();
	i=BitMap.GetCurrentOrt();

	CBitMap_4s BitMap2(4096);
	BitMap2.SetOrt(43);
	BitMap2.SetOrt(56);
	BitMap2.SetOrt(1098);
	BitMap2.SetOrt(518);
	BitMap2.SetOrt(4032);
	BitMap2.SetOrt(30292);
	BitMap2.View();
	BitMap.MakeOr(&BitMap2);
	BitMap.View();

	CBitMap_4s BitMap3(4096);
	BitMap3.SetOrt(13);
	CBitMap_4s BitMap4(4096);
	BitMap4.SetOrt(2);
	BitMap3.MakeOr(&BitMap4);
	BitMap3.View();
}

void CBoxContainers_test::TestBitMap_4s_1(void)
{
	CBitMap_4s BitMap(4096);
	for(unsigned int i=0; i<62; i++){
		BitMap.SetOrt(i);
	}

	CBitMap_4s BitMap1(4096);
	for(unsigned int i=0; i<63; i++){
		BitMap1.SetOrt(i);
	}
	
	BitMap.View();
	BitMap1.View();
	BitMap.MakeOr(&BitMap1);
	//BitMap.MakeAnd(&BitMap1);
	
	BitMap.View();
	ToConsole(L"LastBitValue", BitMap.GetLastBitValue());
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS