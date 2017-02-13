#include "StdAfx.h"
#include ".\byte_align.h"
#include ".\console.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

void CByteAlignCompress::EncodeToBuffer(unsigned int uiValue, TBytesBuffer* pBytesBuffer) const
{
	if(ISNULL(pBytesBuffer)) return;

	//кладем 7-ми битки в вектор, выставля всем кроме последней значащий бит
	unsigned char ucBytesNumber=GetNeededBytesNumber(uiValue);
	unsigned char i=0;
	unsigned char ucTemp;
	while(i<ucBytesNumber){
		ucTemp=(uiValue&arrMasks[i])>>arrOffsets[i];
		if(i==ucBytesNumber-1){
			//последняя
			pBytesBuffer->push_back(ucTemp);
		}else{
			//очередная
			pBytesBuffer->push_back(m_cucSignBit|ucTemp);
		}
		i++;
	}
}

void CByteAlignCompress::ReverseEncodeToBuffer(unsigned int uiValue, TBytesBuffer* pBytesBuffer) const
{
	if(ISNULL(pBytesBuffer)) return;

	//кладем 7-ми битки в вектор, выставля всем кроме последней значащий бит
	unsigned char ucBytesNumber=GetNeededBytesNumber(uiValue);
	unsigned char i=ucBytesNumber;
	unsigned char ucTemp;
	while(i!=0){
		--i;
		ucTemp=(uiValue&arrMasks[i])>>arrOffsets[i];
		if(i==ucBytesNumber-1){
			//последняя
			pBytesBuffer->push_back(ucTemp);
		}else{
			//очередная
			pBytesBuffer->push_back(m_cucSignBit|ucTemp);
		}
	}
}

unsigned int CByteAlignCompress::ReverseDecode(unsigned char* pBytesBuffer, unsigned int uiBytesBufferSize, unsigned int uiStartByte, unsigned int* puiWorkedBytesNumber) const
{
	if(ISNULL(pBytesBuffer)) return 0;
	if(ISNULL(puiWorkedBytesNumber)) return 0;

	unsigned int uiBufferSize=uiBytesBufferSize;
	
	if(uiStartByte>=uiBufferSize){
		TO_CONSOLE(L"Too big StartByte!!!");
		ToConsole(L"uiStartByte", uiStartByte);
		ToConsole(L"uiBufferSize", uiBufferSize);
		return 0;
	}

	//в цикле берем из буфера байты, вытаскиваем из них 7-ми битки и пихаем их в uiValue
	unsigned int uiValue=0;
	unsigned int uiTemp=0;
	unsigned int uiWorkedBytes=0;
	unsigned char* pStartByte=pBytesBuffer+uiStartByte;
	unsigned char i=0;
	while(true){
		uiTemp=(*(pStartByte-i))&m_cucUnsignBit;
		uiValue|=(uiTemp<<arrOffsets[i]);
		uiWorkedBytes++;
		if(((*(pStartByte-i))&m_cucSignBit)==0){
			//если значащий бит в нуле это был последний байт
			break;
		}

		i++;
		
		//проверка на выход за границу буфера
		if(uiStartByte<i){
			TO_CONSOLE(L"Buffer overhead!!!");
			ToConsole(L"uiStartByte", uiStartByte);
			ToConsole(L"i", (unsigned int)i);
			return 0;
		}
	}
	
	(*puiWorkedBytesNumber)=uiWorkedBytes;
	return uiValue;
}

unsigned int CByteAlignCompress::Decode(unsigned char* pBytesBuffer, unsigned int uiBytesBufferSize, unsigned int uiStartByte, unsigned int* puiWorkedBytesNumber) const
{
	if(ISNULL(pBytesBuffer)) return 0;
	if(ISNULL(puiWorkedBytesNumber)) return 0;

	unsigned int uiBufferSize=uiBytesBufferSize;
	
	if(uiStartByte>=uiBufferSize){
		TO_CONSOLE(L"Too big StartByte!!!");
		ToConsole(L"uiStartByte", uiStartByte);
		ToConsole(L"uiBufferSize", uiBufferSize);
		return 0;
	}

	//в цикле берем из буфера байты, вытаскиваем из них 7-ми битки и пихаем их в uiValue
	unsigned int uiValue=0;
	unsigned int uiTemp=0;
	unsigned int uiWorkedBytes=0;
	unsigned char* pStartByte=pBytesBuffer+uiStartByte;
	unsigned char i=0;
	while(true){
		uiTemp=(*(pStartByte+i))&m_cucUnsignBit;
		uiValue|=(uiTemp<<arrOffsets[i]);
		uiWorkedBytes++;
		if(((*(pStartByte+i))&m_cucSignBit)==0){
			//если значащий бит в нуле это был последний байт
			break;
		}

		i++;
		
		//проверка на выход за границу буфера
		if((uiStartByte+i)==uiBufferSize){
			TO_CONSOLE(L"Buffer overhead!!!");
			ToConsole(L"uiStartByte", uiStartByte);
			ToConsole(L"uiBufferSize", uiBufferSize);
			return 0;
		}
	}
	
	(*puiWorkedBytesNumber)=uiWorkedBytes;
	return uiValue;
}

unsigned int CByteAlignCompress::Encode(unsigned int uiValue, unsigned int* puiEncodedValue) const
{
	unsigned char ucBytesNumber=GetNeededBytesNumber(uiValue);
	
	//если значение не нужно просто возвращаем размер
	if(!puiEncodedValue) return ucBytesNumber;

	//кладем 7-ми битки в вектор, выставля всем кроме последней значащий бит
	unsigned char i=0;
	unsigned char ucTemp;
	while(i<ucBytesNumber){
		ucTemp=(uiValue&arrMasks[i])>>arrOffsets[i];
		if(i==ucBytesNumber-1){
			//последняя
			*(((unsigned char*)puiEncodedValue)+i)=ucTemp;
		}else{
			//очередная
			*(((unsigned char*)puiEncodedValue)+i)=m_cucSignBit|ucTemp;
		}
		i++;
	}

	return ucBytesNumber;
}

bool CByteAlignCompress::Decode(unsigned char ucValue, unsigned int* puiDecodedValue)
{
	unsigned int uiValue=0;
	unsigned int uiTemp=0;
	uiTemp=ucValue&m_cucUnsignBit;
	(*puiDecodedValue)|=(uiTemp<<arrOffsets[m_ucCurrentDecodedByte]);
	m_ucCurrentDecodedByte++;
	return (ucValue&m_cucSignBit)==0?false:true;
}

//--------------------------------------------------------------------//

}
}
}
}
}