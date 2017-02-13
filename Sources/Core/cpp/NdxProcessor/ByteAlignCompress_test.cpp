#include "StdAfx.h"
#ifdef _SS_UNITTESTS

#include ".\bytealigncompress_test.h"
#include ".\byte_align.h"
#include ".\test_const.h"
#include ".\console.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Containers::CByteAlignCompress_test);

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

void CByteAlignCompress_test::Test(void)
{
	UT_STOP_LOGING

	CByteAlignCompress ByteAlignCompress;
	unsigned int arrTemp[]={1267, 8278, 238, 923, 293, 829, 12, 2837, 22, 78};
	CByteAlignCompress::TBytesBuffer BytesBuffer;
	
	for(unsigned int i=0; i<10; i++){
		ByteAlignCompress.EncodeToBuffer(arrTemp[i], &BytesBuffer);
	}
	
	ToConsole(L"Uncompressed: 40 bytes, Compressed", (unsigned int)BytesBuffer.size());

	unsigned int uiStartByte=0;
	unsigned int uiWorkedBytesNumber=0;
	unsigned int uiValue=0;
	for(unsigned int i=0; i<10; i++){
		uiWorkedBytesNumber=0;
		uiValue=ByteAlignCompress.Decode(&BytesBuffer.front(), (unsigned int)BytesBuffer.size(), uiStartByte, &uiWorkedBytesNumber);
		CPPUNIT_ASSERT_EQUAL(uiValue, arrTemp[i]);
		uiStartByte+=uiWorkedBytesNumber;
		//ToConsole(L"", uiValue);
	}
	
	uiValue=0;
	unsigned int uiCompressedValue=0;
	unsigned int uiCompressedSize=ByteAlignCompress.Encode(uiValue, &uiCompressedValue);
	
	unsigned int uiDecodedValue=0;
	ByteAlignCompress.StartDecode();
	int k=0;
	while(ByteAlignCompress.Decode(*(((unsigned char*)&uiCompressedValue)+k), &uiDecodedValue)){
		k++;
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
