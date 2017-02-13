#include "StdAfx.h"
#include ".\file_cash_members.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{
namespace FileCash
{

//--------------------------------------------------------------------//

unsigned int CBuffer::Append(unsigned char* pucData, unsigned int uiSize)
{
	SeekFromBack(0);
	return Write(pucData, uiSize);
}

unsigned int CBuffer::Write(unsigned char* pucData, unsigned int uiSize)
{
	if(!m_pCurrent) return 0;
	unsigned int uiAvailableSize=m_pEnd-m_pCurrent;
	if(uiSize<uiAvailableSize)	uiAvailableSize=uiSize;
	
	unsigned int uiFutureSize=(m_pCurrent-m_pBegin)+uiAvailableSize;
	if(uiFutureSize>GetFilledSize())
		m_pFree+=uiFutureSize-GetFilledSize();

	memcpy(m_pCurrent, pucData, uiAvailableSize);
	
	m_pCurrent+=uiAvailableSize;
	return uiAvailableSize;
}

unsigned int CBuffer::Read(unsigned char* pucData, unsigned int uiSize)
{
	if(!m_pCurrent) return 0;
	unsigned int uiAvailableSize=m_pFree-m_pCurrent;
	if(uiSize<uiAvailableSize)	uiAvailableSize=uiSize;

	memcpy(pucData, m_pCurrent, uiAvailableSize);
	m_pCurrent+=uiAvailableSize;

	return uiAvailableSize;
}

void CBuffer::SeekFromFront(unsigned int uiOffset)
{
	if(uiOffset>GetFilledSize()) return;
	
	m_pCurrent=m_pBegin+uiOffset;
}

void CBuffer::SeekFromBack(unsigned int uiOffset)
{
	if(uiOffset>GetFilledSize()) return;
	
	m_pCurrent=m_pFree-uiOffset;
}

void CBuffer::Resize(unsigned int uiNewSize)
{
	if(uiNewSize>GetBufferSize()) return;
	m_pFree=m_pBegin+uiNewSize;
}

void CBuffer::ToString(wstring& wsValue)
{

	wchar_t buf[300];
	wsValue.append(L"\n//---------Bof-CBuffer\n");
	wsprintf(buf, L"Id: %u, Fill: %u , Size: %u\n", GetBufferId(), GetFilledSize(), GetBufferSize());
	wsValue.append(buf);
	unsigned char* pucByte=m_pBegin;
	unsigned int uiViewSize=30;
	unsigned int uiViewedSize=0;
	unsigned int uiSmallViewedSize=0;
	uiViewedSize=0;
	buf[0]=0;
	do{
		wchar_t buf1[10]=L"";
		unsigned int uiSmallViewedSize=0;
		while(uiSmallViewedSize<uiViewSize && ((unsigned int)(pucByte-m_pBegin))<GetFilledSize()){
			swprintf(buf1, L" %u", (unsigned int)*pucByte);
			wcscat(buf, buf1);
			pucByte++;
			uiSmallViewedSize++;
		}
		wsValue.append(buf);
		uiViewedSize+=uiViewSize;
	}while(uiViewedSize<GetFilledSize());
	wsValue.append(L"\n//---------Eof-CBuffer\n");
}

//--------------------------------------------------------------------//

}
}
}
}
}