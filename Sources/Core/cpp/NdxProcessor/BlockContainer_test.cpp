#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\blockcontainer_test.h"
#include ".\block_container.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Containers::CBlockContainer_test);

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

void CBlockContainer_test::Test(void)
{
	unsigned char buf_a[100];
	unsigned char buf_b[100];
	memset(buf_a, 0xaa, 100);
	memset(buf_b, 0xbb, 100);
	unsigned int uiSize=0;

	CBlockBuffer<10> BlockBuffer;
	BlockBuffer.PushBack(buf_a, 15);
	uiSize=BlockBuffer.GetBinaryBufferSize();
	BlockBuffer.PushBack(buf_b, 25);
	uiSize=BlockBuffer.GetBinaryBufferSize();
	
	BlockBuffer.PopBack(buf_a, 37);
	uiSize=BlockBuffer.GetBinaryBufferSize();
	
	BlockBuffer.Reset();
	uiSize=BlockBuffer.GetBinaryBufferSize();
	BlockBuffer.PushBack(buf_b, 25);
	uiSize=BlockBuffer.GetBinaryBufferSize();
	uiSize=sizeof(CBlockBuffer<10>);
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS