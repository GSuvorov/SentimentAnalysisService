#include "StdAfx.h"
#include ".\level_storage.h"
#include ".\data_storages_factory.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CLevelStorage::CLevelStorage(CDataUnitFactory* pDataStorageFactory)
:CFilesCollectionEx(pDataStorageFactory),
m_cuiMaxFileSize(cuiMaxFileSize), m_cuiMaxFilesNumber(cuiMaxFilesNumber) 
{
}

CLevelStorage::~CLevelStorage()
{
}

HRESULT CLevelStorage::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_NdxDataUnit){
		*pBase=(INdxDataUnit*)this;
	}else if(pIID==IID_NdxLevelStorage){
		*pBase=(INdxLevelStorage*)this;
	}else{
		*pBase=NULL;
	}

	return S_OK;
}

bool CLevelStorage::Open(const wchar_t* pszPath)
{
	m_szCurrentFilePath.assign(pszPath);

	//открываем файл с количеством итемов в коллекции
	unsigned int uiDataUnitNumber=ReadDataUnitNumber();
	if(uiDataUnitNumber==0) uiDataUnitNumber=2;

	//добавляем файл обновляемых данных
	//добавляем файл постоянных данных
	for(unsigned int i=0; i<uiDataUnitNumber; i++){
		AddFile();
	}

	//открываемся
	return CFilesCollection::Open(pszPath);
}

void CLevelStorage::GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName)
{	
	if(uiFileIndex==EMPTY_VALUE){
		ERR_TO_CONSOLE(erFileIO, L"Invalid file index");	
	}

	wchar_t buf[20];
	_itow(uiFileIndex, buf, 10);

	pszFileName->assign(GetDataUnitName());
	pszFileName->append(L"_");
	pszFileName->append(buf);
}

INdxDataUnit* CLevelStorage::CreateFile(unsigned int uiFileIndex)
{
	//генерируем имя файла
	wstring szNewFileName;
	GenerateFileName(GetFilesNumber(), &szNewFileName);

	if(ISNULL(GetDataStorageFactory())) return NULL;

	if(!GetFilesNumber()){
		//создаем в голове коллекции файл обновляемых данных
		CUpdateFile* pUpdateFile=GetDataStorageFactory()->CreateUpdateFile();
		pUpdateFile->Create(szNewFileName.c_str(), GetDataUnitExt());
		return pUpdateFile;
	}else{
		
		//создаем файл с кэшированием
		INdxCashedFile* pNdxCashedFile=GetDataStorageFactory()->CreateCashedFile();
		pNdxCashedFile->Create(szNewFileName.c_str(), GetDataUnitExt());
		//устанавливаем аттрибуты бакапирования
		pNdxCashedFile->SetBackupSettings(SBackupSettings::cmCompressible, SBackupSettings::wmAppend);
		return pNdxCashedFile;
	}
}

CUpdateFile* CLevelStorage::GetUpdateFile(void)
{
	if(!GetFilesNumber()){
		return NULL;
	}else{
		return dynamic_cast<CUpdateFile*>(m_FilesCollection.front());
	}
}

void	CLevelStorage::TellPos(SCompositeOffset* pOffset)
{
	if(ISNULL(pOffset)) return;
	
	pOffset->ucFileIndex=GetCurrentFileIndex();
	pOffset->uiOffset=GetCurrentFile()->TellPos();
}

void	CLevelStorage::SeekPos(SCompositeOffset* pOffset, unsigned int uiOrigin)
{
	if(ISNULL(pOffset)) return;
	
	//при переходе на новый файл смещение всегда должно быть абсолютным от начала файла
	if(pOffset->ucFileIndex!=GetCurrentFileIndex())
		uiOrigin=SEEK_SET;

	//переходим к файлу
	if(!MoveToFile(pOffset->ucFileIndex)){
		ERR_TO_CONSOLE(erFileIO, L"Cann't switch on file...");
		ToConsole(L"FileIndex", (unsigned int)pOffset->ucFileIndex);
		return;
	}

	//смещаемся внутри файла
	GetCurrentFile()->SeekPos(pOffset->uiOffset, uiOrigin);
}

bool	CLevelStorage::StartRead(SCompositeOffset* pOffset, unsigned int* puiDataSizeInFile)
{
	if(ISNULL(pOffset)){
		*puiDataSizeInFile=EMPTY_VALUE;
		return false;
	}

	if(pOffset->ucFileIndex>=m_cuiMaxFilesNumber && pOffset->uiOffset==EMPTY_OFFSET){
		TO_CONSOLE(L"SBlockOffset is empty!!!");
		ToConsole(GetDataUnitName());
		*puiDataSizeInFile=EMPTY_VALUE;
		return false;
	}
	
	if(!pOffset->ucFileIndex){
		if(ISNULL(GetUpdateFile())) return false;
		GetUpdateFile()->StartRead(pOffset->uiOffset, puiDataSizeInFile);
		MoveFirstFile();
		return true;
	}else{
		SeekPos(pOffset, SEEK_SET);	
		return true;
	}

}

void	CLevelStorage::Read(unsigned char* pBuffer, unsigned int uiSize)
{
	if(ISNULL(pBuffer)){ 
		ToConsole(GetDataUnitName());	
		return;
	}

	if(!GetCurrentFileIndex()){
		if(ISNULL(GetUpdateFile())) return;
		GetUpdateFile()->Read(pBuffer, uiSize);
	}else{
		INdxFile* pBasisFile=GetCurrentFile();
		if(ISNULL(pBasisFile)) return;
		
		pBasisFile->Read(pBuffer, uiSize);
	}
	
}

void	CLevelStorage::StopRead(unsigned int* puiReadedDataSize)
{
	if(!GetCurrentFileIndex()){
		if(ISNULL(GetUpdateFile())) return;
		if(ISNULL(puiReadedDataSize)) return;
		GetUpdateFile()->StopRead(puiReadedDataSize);
	}
}

void	CLevelStorage::StartCompleteDataAppend(SCompositeOffset* pOffset)
{
	if(ISNULL(pOffset)) return;

	if(!(pOffset->ucFileIndex>=m_cuiMaxFilesNumber && pOffset->uiOffset==EMPTY_OFFSET)){
		TO_CONSOLE(L"SBlockOffset already filled cann't append!!!");
		ToConsole(L"File Index", (unsigned int)pOffset->ucFileIndex);	
		ToConsole(L"Offset", pOffset->uiOffset);	
		ToConsole(GetDataUnitName());
	}else{
		//позиционируемся на файле, отдаем смещение на данные наверх
		MoveLastFile();
		
		if(GetCurrentFile()->GetDataUnitSize()>=m_cuiMaxFileSize){
			//если превышен лимит по размеру файла 
			//буфер старого сбрасываем на диск он больше не нужен
			GetCurrentFile()->Flush();
			//добавляем новый файл
			if(!AddFile()) return;
			if(!MoveLastFile()) return;
		}

		pOffset->ucFileIndex=GetCurrentFileIndex();
		pOffset->uiOffset=GetCurrentFile()->GetDataUnitSize();
		SeekPos(pOffset, SEEK_SET);	
	}
}

void	CLevelStorage::StartUnCompleteDataAppend(SCompositeOffset* pOffset)
{
	if(ISNULL(pOffset)) return;
	if(ISNULL(GetUpdateFile())) return;

	if(!(pOffset->ucFileIndex>=m_cuiMaxFilesNumber && pOffset->uiOffset==EMPTY_OFFSET)){
		TO_CONSOLE(L"SBlockOffset already filled cann't append!!!");
		ToConsole(L"File Index", (unsigned int)pOffset->ucFileIndex);	
		ToConsole(L"Offset", pOffset->uiOffset);	
		ToConsole(GetDataUnitName());
	}else{
		//позиционируемся на граничном файле, отдаем смещение на данные наверх
		pOffset->ucFileIndex=0;
		GetUpdateFile()->StartAppend(&(pOffset->uiOffset));
		MoveFirstFile();
	}
}

void	CLevelStorage::Append(unsigned char* pBuffer, unsigned int uiSize)
{
	if(ISNULL(pBuffer)) return;

	if(!GetCurrentFileIndex()){
		if(ISNULL(GetUpdateFile())) return;
		GetUpdateFile()->Append(pBuffer, uiSize);
	}else{
		INdxFile* pBasisFile=GetCurrentFile();
		if(ISNULL(pBasisFile)) return;
		
		pBasisFile->Append(pBuffer, uiSize);
	}
}

void	CLevelStorage::StopAppend(unsigned int* puiAppendedDataSize)
{
	if(!GetCurrentFileIndex()){
		if(ISNULL(GetUpdateFile())) return;
		if(ISNULL(puiAppendedDataSize)) return;
		GetUpdateFile()->StopAppend(puiAppendedDataSize);
	}
}

unsigned int CLevelStorage::GetAppendedUnCompleteDataSize(void)
{
	if(ISNULL(GetUpdateFile())) return 0;
	return GetUpdateFile()->GetWorkedDataSize();
}

void	CLevelStorage::SeekPosInAppendedUnCompleteData(unsigned int uiOffset, unsigned int uiOrigin)
{
	if(ISNULL(GetUpdateFile())) return;

	GetUpdateFile()->SeekPosInData(uiOffset, uiOrigin);
}

void	CLevelStorage::WriteInAppendedUnCompleteData(unsigned char* pBuffer, unsigned int uiSize)
{
	if(ISNULL(pBuffer)) return;
	if(ISNULL(GetUpdateFile())) return;

	GetUpdateFile()->WriteData(pBuffer, uiSize);
}

void	CLevelStorage::Delete(SCompositeOffset* pOffset)
{
	if(ISNULL(pOffset)) return;
	
	//ToConsole(L"CLevelStorage::Delete");
	//ToConsole(L"Delete File index", (unsigned int)pOffset->ucFileIndex);
	//ToConsole(L"Delete File offset", pOffset->uiOffset);

	if(pOffset->ucFileIndex>=m_cuiMaxFilesNumber && pOffset->uiOffset==EMPTY_OFFSET){
		TO_CONSOLE(L"SBlockOffset is empty!!!");
		return;
	}

	if(pOffset->ucFileIndex==0){
		if(ISNULL(GetUpdateFile())) return;
		GetUpdateFile()->DeleteData(pOffset->uiOffset);
	}else{
		TO_CONSOLE(L"Cann't delete from Complete files!!!");
	}
}

void	CLevelStorage::UpdateData(void)
{
	TO_CONSOLE(L"Not supported");
	//if(GetFilesNumber()>1){
	//	TFiles::iterator itCurrFile=++m_FilesCollection.begin();
	//	do{
	//		if(ISNULL(*itCurrFile)) return;
	//		((CRACashedFile*)(*itCurrFile))->FlushFilledBuffers();
	//	}while(++itCurrFile!=m_FilesCollection.end());
	//}
}

//--------------------------------------------------------------------//

}
}
}
}