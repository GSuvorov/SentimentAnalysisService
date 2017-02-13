#include "StdAfx.h"
#include ".\basis_containers.h"

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

CValue::CValue(void)
:m_fValueWeight(0.0)
{
}

CValue::~CValue(void)
{
}

void CValue::MakeAnd(CValue* pValue)
{
	if(ISNULL(pValue)) return; 
	m_fValueWeight+=pValue->GetValueWeight();
}

void CValue::MakeSmoothAnd(CValue* pValue, unsigned int uiAdjacentSize)
{
	if(ISNULL(pValue)) return; 
	m_fValueWeight+=pValue->GetValueWeight();
}

void CValue::MakeOr(CValue* pValue)
{
	if(ISNULL(pValue)) return; 
	m_fValueWeight+=pValue->GetValueWeight();
}

//--------------------------------------------------------------------//

CValuesContainer::CValuesContainer(unsigned int uiEntryPoint)
:m_uiEntryPoint(uiEntryPoint)
{
}

CValuesContainer::~CValuesContainer(void)
{
}

void CValuesContainer::ToString(wstring& szValue)
{
	wchar_t buf[100];
	wsprintf(buf, L"ValueContainer: entry_point-%u\n", m_uiEntryPoint);
	szValue.append(buf);	
}

//--------------------------------------------------------------------//

CByteAlignCompress* CBinaryContainer::m_pByteAlignCompress=NULL;

unsigned int CBinaryContainer::Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return 0;
	//пишем размер
	unsigned int uiWritedBytes=0;
	unsigned int uiBufferSize=GetBinaryBufferSize();
	uiWritedBytes+=WriteCompressed(pNdxStorage, uiBufferSize);

	//пишем буфер
	pNdxStorage->Append(GetBinaryBuffer(), uiBufferSize);
	return uiWritedBytes;
}

void CBinaryContainer::Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return;
	//резервируем буфер читаем в буфер
	InitBinaryBuffer(ReadCompressed(pNdxStorage));
	pNdxStorage->Read(GetBinaryBuffer(), GetBinaryBufferSize());
}

unsigned int CBinaryContainer::WriteCompressed(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage, unsigned int uiValue)
{
	if(ISNULL(pNdxStorage)) return 0;
	if(ISNULL(m_pByteAlignCompress)) return 0;

	//сживмаем
	unsigned int uiCompressedValue=0;
	unsigned int uiNewSize=m_pByteAlignCompress->Encode(uiValue, &uiCompressedValue);
	//добавляем в хранилище
	pNdxStorage->Append((unsigned char*)&uiCompressedValue, uiNewSize);
	
	return uiNewSize;
}

unsigned int CBinaryContainer::ReadCompressed(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return 0;
	if(ISNULL(m_pByteAlignCompress)) return 0;

	unsigned int uiValue=0;
	unsigned char ucByte=0;
	((CByteAlignCompress*)m_pByteAlignCompress)->StartDecode();
	do{
		//читаем байт
		pNdxStorage->Read(&ucByte, sizeof(unsigned char));
	}while(((CByteAlignCompress*)m_pByteAlignCompress)->Decode(ucByte, &uiValue));
	
	//возвращаем полученный размер
	return uiValue;
}

//--------------------------------------------------------------------//

CBinaryBufferContainer::CBinaryBufferContainer(void)
:m_pucBuffer(NULL), m_uiBufferSize(0)
{
}

CBinaryBufferContainer::~CBinaryBufferContainer(void)
{
	ResetBinaryBuffer();
}

void CBinaryBufferContainer::InitBinaryBuffer(unsigned int uiNewSize)
{
	ResetBinaryBuffer();
	m_pucBuffer=(unsigned char*)malloc(uiNewSize);
	m_uiBufferSize=uiNewSize;
}

void CBinaryBufferContainer::FillBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize)
{
	if(ISNULL(pBuffer)) return;
	if(ISNULL(uiBufferSize)) return;
	
	//инитим буфер и копируем в него
	InitBinaryBuffer(uiBufferSize);
	memcpy(GetBinaryBuffer(), pBuffer, GetBinaryBufferSize());
}

void CBinaryBufferContainer::ResetBinaryBuffer(void)
{
	if(m_pucBuffer) free(m_pucBuffer);
	m_pucBuffer=NULL;
	m_uiBufferSize=0;
}

void CBinaryBufferContainer::ReduceBackBinaryBuffer(unsigned int uiReducedSize)
{
	if(!uiReducedSize) return;
	
	if(uiReducedSize>=m_uiBufferSize){
		//если усекаемый размер больше равен буферу сбрасываем его
		ResetBinaryBuffer();
	}else{
		//уменьшаем текущий размер буфера
		m_uiBufferSize-=uiReducedSize;
		//реалокируем буфер
		unsigned char* tmp = (unsigned char*) realloc(m_pucBuffer, m_uiBufferSize);
		if (tmp) {
			m_pucBuffer = tmp;
		}
	}

}

//--------------------------------------------------------------------//

}
}
}
}
}