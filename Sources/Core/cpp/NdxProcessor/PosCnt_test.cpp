#include "StdAfx.h"
#ifdef _SS_UNITTESTS

#include ".\poscnt_test.h"
#include ".\index_container_nodes_s.h"
#include ".\index_container_nodes.h"
#include ".\test_const.h"
#include ".\console.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Containers::CPosBaCnt_test);

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
using namespace SS::Core::NdxSE::NdxProcessor;

//--------------------------------------------------------------------//

void CPosBaCnt_test::Test(void)
{
	//UT_STOP_LOGING
	UT_START_LOGING
	typedef vector<unsigned char> TVec;
	unsigned int iSize=sizeof(CBaCoordinateNode_4i_Blocked);
	iSize=sizeof(CBaCoordinateNode_4i);
	iSize=sizeof(CBlockBuffer<cucBaCoordinateNode_BlockBufferSize>);
	iSize=sizeof(CBinaryBlock<cucBaCoordinateNode_BlockBufferSize>);
	iSize=sizeof(TVec);
		
	set<unsigned int> Positions;
	CByteAlignCompress ByteAlignCompress;
	//устанавливаем статический упаковщик
	CBinaryContainer::m_pByteAlignCompress=&ByteAlignCompress;
	CBaCoordinateNode_4i_Blocked PosCntIndex;
	
	//заполняем контайнер позициями
	unsigned int uiPosCount=1000;
	unsigned int uiPosition=0;
	for(unsigned int i=0; i<uiPosCount; i++){
		uiPosition+=rand()&0x0f;
		uiPosition++;
		//ToConsole(L"uiPosition", uiPosition);
		PosCntIndex.AddCoordinate(uiPosition);
		Positions.insert(uiPosition);
	}
	
	//PosCntIndex.View();

	//for(unsigned int i=0; i<uiPosCount; i++){
	//	uiPosition+=rand()&0x0f;
	//	uiPosition++;
	//	//ToConsole(L"uiPosition", uiPosition);
	//	PosCntIndex.AddCoordinate(uiPosition);
	//	Positions.insert(uiPosition);
	//}

	PosCntIndex.View();

	CPosBaCntSearch_Blocked PosBaCntSearch;
	CByteAlignCompress::TBytesBuffer Buffer;
	PosCntIndex.ToBinaryBuffer(&Buffer);
	PosBaCntSearch.FromBinaryBuffer(&Buffer.front(), (unsigned int)Buffer.size());
	
	for(unsigned int i=0; i<uiPosCount; i++){
		uiPosition=rand()&0xfff;
		ToConsole(L"LowerBoundPosition", uiPosition);
		wstring szPositions;
		wchar_t buf[50];
		if(PosBaCntSearch.MoveLowerBoundOrt(uiPosition)){
			set<unsigned int>::iterator itPosition=Positions.find(PosBaCntSearch.GetCurrentOrt());

			do{
				uiPosition=PosBaCntSearch.GetCurrentOrt();
				wsprintf(buf, L"%u ", uiPosition);
				//ToConsole(L"uiPosition", uiPosition);
				szPositions.append(buf);
				
				CPPUNIT_ASSERT(uiPosition==(*itPosition));

				itPosition++;
			}while(PosBaCntSearch.MoveNextOrt());
		}

		//ToConsole(szPositions.c_str());
	}

	UT_START_LOGING
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS