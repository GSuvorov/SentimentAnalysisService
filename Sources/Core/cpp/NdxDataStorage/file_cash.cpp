#include "StdAfx.h"
#include ".\file_cash.h"

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

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//
CFileCash::CFileCash(unsigned int uiBufferSize, unsigned int uiBuffersMaxNumber) :
	m_uiBufferSize(uiBufferSize), 
	m_FileCashBuffers(uiBuffersMaxNumber), 
	m_pCashedFile(NULL),
	m_uiFileOffset(0)
{
}

void CFileCash::Clear(void)
{
	CBuffer* pPopedBuffer=NULL;
	while(pPopedBuffer=m_FileCashBuffers.PopFront()){
		delete pPopedBuffer;
	}
	m_CurrentCashPosition.Clear();
	m_uiFileOffset=EMPTY_OFFSET;
}

unsigned int CFileCash::GetFilledSize(void)
{
	CBuffer* pBuffer=m_FileCashBuffers.GetTail();
	unsigned int uiFilledSize=0;
	if(pBuffer){
		uiFilledSize+=pBuffer->GetFilledSize();
		uiFilledSize+=(m_FileCashBuffers.GetSize()-1)*m_uiBufferSize;
	}
	return uiFilledSize;
}

CBuffer* CFileCash::AddBuffer(void)
{
	CBuffer* pBuffer=new CBuffer(m_FileCashBuffers.GetSize(), m_uiBufferSize);
	CBuffer* pPopedBuffer=m_FileCashBuffers.PushBack(pBuffer);

	if(pPopedBuffer){
		//пишем буфер в файл
		m_pCashedFile->CBasisFile::SeekPos(m_uiFileOffset, SEEK_SET);
		m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::Write(pPopedBuffer->GetBuffer(), pPopedBuffer->GetFilledSize());
		//сдвигаем смещение
		m_uiFileOffset+=pPopedBuffer->GetFilledSize();
		//удаляем буфер
		delete pPopedBuffer;
	}

	return pBuffer;
}

void CFileCash::SeekPos(unsigned int uiOffset, unsigned int uiOrigin)
{
	//размеры всех буферов кэша постоянны, используем это при позиционировании
	unsigned int uiFilledSize=GetFilledSize();
	if(uiOrigin==SEEK_SET){
	}else if(uiOrigin==SEEK_END){
		//если от смещение от конца, из значения вычитаем размер последнего буфера
		uiOffset=uiFilledSize-uiOffset;
		if(uiOffset>uiFilledSize){
			ERR_TO_CONSOLE(erFileIO, L"Invalid offset!");
			return;
		}
	}else if(uiOrigin==SEEK_CUR){

		unsigned int uiCurrOffset=TellPos();
		int iOffset=(int)uiOffset;
		uiOffset=uiCurrOffset+iOffset;

		if(uiOffset>uiFilledSize){
			ERR_TO_CONSOLE(erFileIO, L"Invalid offset!");
			return;
		}
	}

	CCashPosition CurrentCashPosition;
	ldiv_t dvCashPos;
	//если от смещение начала просто определяем координаты буфера		
	dvCashPos=div((long)uiOffset, (long)m_uiBufferSize);
	CurrentCashPosition.m_uiBufferIndex=dvCashPos.quot;
	CurrentCashPosition.m_uiBufferOffset=dvCashPos.rem;

	if((CurrentCashPosition.m_uiBufferIndex<m_FileCashBuffers.GetSize() && 
		CurrentCashPosition.m_uiBufferOffset<=m_FileCashBuffers.GetByIndex(CurrentCashPosition.m_uiBufferIndex)->GetFilledSize()) 
		||
		(CurrentCashPosition.m_uiBufferIndex==m_FileCashBuffers.GetSize() && 
		CurrentCashPosition.m_uiBufferOffset==0)){
			m_CurrentCashPosition=CurrentCashPosition;
	}else{
		ERR_TO_CONSOLE(erFileIO, L"Invalid offset!");
		CurrentCashPosition.View();
	}
}

unsigned int CFileCash::TellPos(void)
{
	return m_CurrentCashPosition.m_uiBufferIndex*m_uiBufferSize+m_CurrentCashPosition.m_uiBufferOffset;
}

void CFileCash::AddData(unsigned char* pucData, unsigned int uiSize)
{
	if(ISNULL(pucData)) return;
	if(ISNULL(uiSize)) return;

	CBuffer* pBuffer=m_FileCashBuffers.GetTail();
	if(!pBuffer) pBuffer=AddBuffer();
	
	unsigned int uiWorkedDataSize=0;
	while(true){
		uiWorkedDataSize=pBuffer->Append(pucData, uiSize);
		pucData+=uiWorkedDataSize;
		uiSize-=uiWorkedDataSize;

		if(uiSize){
			pBuffer=AddBuffer();
		}else
			break;
	}
	
	//сдвигаем позицию в конец
	SeekPos(0, SEEK_END);
}

void CFileCash::Write(unsigned char* pucData, unsigned int uiSize)
{
	if(ISNULL(pucData)) return;
	if(ISNULL(uiSize)) return;
	if(m_CurrentCashPosition.IsEmpty()){
		ERR_TO_CONSOLE(erFileIO, L"CurrentCashPosition is empty? cann't write");	
		return;
	}
	
	CBuffer* pBuffer=NULL;
	unsigned int uiWorkedDataSize=0;

	while(true){
		//получаем буфер
		pBuffer=m_FileCashBuffers.GetByIndex(m_CurrentCashPosition.m_uiBufferIndex);
		if(!pBuffer){
			//если такого нет добавляем
			pBuffer=AddBuffer();
		}
		//позиционируемся
		pBuffer->SeekFromFront(m_CurrentCashPosition.m_uiBufferOffset);
		//пишем
		uiWorkedDataSize=pBuffer->Write(pucData, uiSize);
		//сдвигаемся
		pucData+=uiWorkedDataSize;
		uiSize-=uiWorkedDataSize;

		if(uiSize){
			//если еще не все переходим к следущему буферу
			m_CurrentCashPosition.m_uiBufferIndex++;
			m_CurrentCashPosition.m_uiBufferOffset=0;
		}else{
			//если все, сдвигаем текущий указатель
			m_CurrentCashPosition.m_uiBufferOffset+=uiWorkedDataSize;
			break;
		}
	}
}

void CFileCash::Read(unsigned char* pucData, unsigned int uiSize)
{
	if(ISNULL(pucData)) return;
	if(ISNULL(uiSize)) return;
	if(m_CurrentCashPosition.IsEmpty()){
		ERR_TO_CONSOLE(erFileIO, L"CurrentCashPosition is empty? cann't read");	
		return;
	}
	
	CBuffer* pBuffer=NULL;
	unsigned int uiWorkedDataSize=0;

	while(true){
		//получаем буфер
		pBuffer=m_FileCashBuffers.GetByIndex(m_CurrentCashPosition.m_uiBufferIndex);
		if(ISNULL(pBuffer)){
			ERR_TO_CONSOLE(erFileIO, L"No more buffers");
			return;
		}
		//позиционируемся
		pBuffer->SeekFromFront(m_CurrentCashPosition.m_uiBufferOffset);
		//читаем
		uiWorkedDataSize=pBuffer->Read(pucData, uiSize);
		//сдвигаемся
		pucData+=uiWorkedDataSize;
		uiSize-=uiWorkedDataSize;

		if(uiSize){
			//если еще не все переходим к следущему буферу
			m_CurrentCashPosition.m_uiBufferIndex++;
			m_CurrentCashPosition.m_uiBufferOffset=0;
		}else{
			//если все, сдвигаем текущий указатель
			m_CurrentCashPosition.m_uiBufferOffset+=uiWorkedDataSize;
			break;
		}
	}
}

void CFileCash::Look(unsigned char* pucData, unsigned int uiSize)
{
	CCashPosition CurrentCashPosition=m_CurrentCashPosition;
	Read(pucData, uiSize);
	m_CurrentCashPosition=CurrentCashPosition;
}

void CFileCash::FlushToFile(void)
{
	if(ISNULL(m_pCashedFile)) return;

	m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::SeekPos(m_uiFileOffset, SEEK_SET);
	CBuffer* pPopedBuffer=NULL;
	while(pPopedBuffer=m_FileCashBuffers.PopFront()){
		m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::Write(pPopedBuffer->GetBuffer(), pPopedBuffer->GetFilledSize());
		delete pPopedBuffer;
	}
	
	m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::Flush();
	m_CurrentCashPosition.Clear();
	m_uiFileOffset=m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::GetDataUnitSize();
}

void CFileCash::FlushFilledBuffersToFile(void)
{
	if(ISNULL(m_pCashedFile)) return;

	m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::SeekPos(m_uiFileOffset, SEEK_SET);
	CBuffer* pPopedBuffer=NULL;
	while((pPopedBuffer=m_FileCashBuffers.PopFront())!=NULL){
		m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::Write(pPopedBuffer->GetBuffer(), pPopedBuffer->GetFilledSize());
		delete pPopedBuffer;

		if(m_FileCashBuffers.GetSize()==1)
			break;
	}
	
	m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::Flush();
	m_CurrentCashPosition.Clear();
	m_uiFileOffset=m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::GetDataUnitSize();
}

void CFileCash::View(void)
{
	wstring wsValue;
	ToConsole(L"//---------------Bof-CFileCash");
	
	ToConsole(L"FilledSize", GetFilledSize());

	for(unsigned int i=0; i<m_FileCashBuffers.GetSize(); i++){
		wsValue.clear();
		m_FileCashBuffers.GetByIndex(i)->ToString(wsValue);
		ToConsole(wsValue.c_str());
	}

	m_CurrentCashPosition.View();

	ToConsole(L"//---------------Eof-CFileCash");
}

//--------------------------------------------------------------------//

CReadFileCash::CReadFileCash(unsigned int uiBufferSize)
:CFileCash(uiBufferSize, 1)
{
}

CReadFileCash::~CReadFileCash(void)
{
}

void CReadFileCash::FillFromFile(void)
{
	if(ISNULL(m_pCashedFile)) return;

	if(m_uiFileOffset==EMPTY_OFFSET){
		TO_CONSOLE(L"Invalid FileOffset!!!");
		return;
	}
	
	//определяем размер файла
	unsigned int uiFileSize=m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::GetDataUnitSize();
	
	if(m_uiFileOffset>=uiFileSize){
		ToConsole(L"To big file offset", m_uiFileOffset);
		m_pCashedFile->CBasisFile::View();
		return;
	}

	//позиционируемся в файле
	m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::SeekPos(m_uiFileOffset, SEEK_SET);

	//корректируем размер читаемых данных из файла
	unsigned int uiWorkedSize=uiFileSize-m_uiFileOffset;
	CBuffer* pBuffer=m_FileCashBuffers.GetByIndex(0);
	if(!pBuffer){
		pBuffer=AddBuffer();
	}
	pBuffer->Resize(min(uiWorkedSize, m_uiBufferSize));
	//читаем в буфер из файла
	m_pCashedFile->SS::Core::NdxSE::NdxDataStorage::CBasisFile::Read(pBuffer->GetBuffer(), pBuffer->GetFilledSize());
	
	//позиционируемся в начале кэша
	SeekPos(0, SEEK_SET);
}

//--------------------------------------------------------------------//

}
}
}
}
}