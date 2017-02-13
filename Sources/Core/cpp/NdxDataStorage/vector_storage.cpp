#include "StdAfx.h"
#include ".\vector_storage.h"
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

CVectorStorage::CVectorStorage(CDataUnitFactory* pDataStorageFactory) :
	CFilesCollection(pDataStorageFactory),
	m_uiMaxDataFileSize(cuiMaxFileSize), 
	m_uiMaxDataFilesNumber(cuiMaxFilesNumber),
	m_uiCurrOffsetInData(0),
	m_uiCurrDataSize(0)
{
	m_CurrDataInfo.Reset();
	m_CurrAppendedDataInfo.Reset();
}

CVectorStorage::~CVectorStorage()
{
}

HRESULT CVectorStorage::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_NdxDataUnit){
		*pBase=(INdxDataUnit*)this;
	}else if(pIID==IID_Base){
		*pBase=(SS::Interface::IBase*)this;
	}else if(pIID==IID_NdxVectorStorage){
		*pBase=(INdxVectorStorage*)this;
	}else{
		*pBase=NULL;
	}

	return S_OK;
}

bool CVectorStorage::Open(const wchar_t* pszPath)
{
	if(ISNULL(pszPath)) return false;

	//создаем индексный файл
	AddFile();
	//создаем файл данных
	AddFile();
	
	return CFilesCollection::Open(pszPath);
}

void CVectorStorage::GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName)
{
	pszFileName->assign(GetDataUnitName());
	switch((EVectorStorageFiles)uiFileIndex){
	case evsfIndex:
		pszFileName->append(L"_i");
		break;
	case evsfData:
		pszFileName->append(L"_d");
		break;
	default:
		pszFileName->append(L"_udf");
		TO_CONSOLE(L"Unknown index!!!");
		break;
	}
}

INdxDataUnit* CVectorStorage::CreateFile(unsigned int uiFileIndex)
{
	wstring szNewFileName;
	//генерируем имя файла
	GenerateFileName(uiFileIndex, &szNewFileName);
	
	if(ISNULL(GetDataStorageFactory())) return NULL;

	switch((EVectorStorageFiles)uiFileIndex){
	case evsfIndex:
		{
			//создаем файл
			INdxFile* pFile=GetDataStorageFactory()->CreateBasisFile();
			pFile->Create(szNewFileName.c_str(), GetDataUnitExt());
			//устанавливаем аттрибуты бакапирования
			pFile->SetBackupSettings(SBackupSettings::cmCompressible, SBackupSettings::wmAppend);
			return pFile;
		}
		break;
	case evsfData:
		{
			//создаем файл
			CFilesCollectionEx* pFilesCollectionEx=GetDataStorageFactory()->CreateFilesCollectionEx();
			pFilesCollectionEx->Create(szNewFileName.c_str(), GetDataUnitExt());
			//устанавливаем аттрибуты бакапирования
			pFilesCollectionEx->SetBackupSettings(m_BackupSettings.m_CompressMode, SBackupSettings::wmAppend);
			return pFilesCollectionEx;
		}
		break;
	default:
		TO_CONSOLE(L"Unknown index!!!");
		return NULL;
		break;
	}
}

void	CVectorStorage::MoveToAndGetDataSize(unsigned int uiIndex, unsigned int* puiDataSizeInFile)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return;
	}

	if(ISNULL(puiDataSizeInFile)) return;
	
	//вычисляем по индексу смещение 
	unsigned int uiOffset=uiIndex*sizeof(SDataInfo);

	try{
		if(uiOffset>=GetIndexFile()->GetDataUnitSize()){
			(*puiDataSizeInFile)=0;
			return;
		}

		//смещаемся в индексном файле
		GetIndexFile()->SeekPos(uiOffset, SEEK_SET);
		//читаем инфу по данным в индексном файле
		GetIndexFile()->Read((unsigned char*)&m_CurrDataInfo, sizeof(SDataInfo));
		
		//получаем размер данных в файле данных
		if(!m_CurrDataInfo.m_bDeleted && puiDataSizeInFile){
			(*puiDataSizeInFile)=m_CurrDataInfo.m_uiDataSize;
		}else{
			(*puiDataSizeInFile)=0;
		}
		
		//обнуляем смещение в данных
		m_uiCurrOffsetInData=0;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		throw;
	}
}

void	CVectorStorage::ReadWholeData(unsigned char* pData, unsigned int uiDataSize)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return;
	}

	if(ISNULL(pData)) return;
	if(ISNULL(uiDataSize)) return;
	
	if(uiDataSize!=m_CurrDataInfo.m_uiDataSize){
		TO_CONSOLE(L"GetDataSize was not called or data size not correct");
		return;
	}
	
	//если данные удалены мы их не читаем
	if(m_CurrDataInfo.m_bDeleted) return;

	try{
		//переходим к нужному файлу
		if(GetDataFile()->MoveToFile(m_CurrDataInfo.m_ucFileIndex)){
			//позиционируемся в файле
			GetDataFile()->GetCurrentFile()->
				SeekPos(m_CurrDataInfo.m_uiOffsetInFile, SEEK_SET);
			
			//читаем данные в буфер
			GetDataFile()->GetCurrentFile()->Read(pData, m_CurrDataInfo.m_uiDataSize);	
			
			//все прочитали смещение в данных указвает на конец
			m_uiCurrOffsetInData=m_CurrDataInfo.m_uiDataSize;
		}

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		throw;
	}

}

void	CVectorStorage::ReadDataChunk(unsigned char* pData, unsigned int uiDataSize)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return;
	}

	if(ISNULL(pData)) return;
	if(ISNULL(uiDataSize)) return;
	
	if(uiDataSize>m_CurrDataInfo.m_uiDataSize){
		TO_CONSOLE(L"Too big datasize!!!");
		return;
	}
	
	//если данные удалены мы их не читаем
	if(m_CurrDataInfo.m_bDeleted) return;

	try{
		//переходим к нужному файлу
		if(GetDataFile()->MoveToFile(m_CurrDataInfo.m_ucFileIndex)){
			//позиционируемся в файле
			GetDataFile()->GetCurrentFile()->
				SeekPos(m_CurrDataInfo.m_uiOffsetInFile+m_uiCurrOffsetInData, SEEK_SET);
			
			//читаем данные в буфер
			GetDataFile()->GetCurrentFile()->Read(pData, uiDataSize);	
			
			//увеличиваем смещение на размер считанных данных
			m_uiCurrOffsetInData+=uiDataSize;
		}

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		throw;
	}
}

void	CVectorStorage::SeekInData(unsigned int uiOffset)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return;
	}

	if(uiOffset<m_CurrDataInfo.m_uiDataSize){
		//устанавливаем смещение в данных
		m_uiCurrOffsetInData=uiOffset;
	}else{
		TO_CONSOLE(L"Too big offset!!!");
	}
}

void	CVectorStorage::AppendWholeData(unsigned char* pData, unsigned int uiDataSize, unsigned int* puiIndex)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return;
	}

	//if(ISNULL(pData)) return;
	//if(ISNULL(uiDataSize)) return;
	
	if(!m_CurrAppendedDataInfo.IsEmpty()){
		TO_CONSOLE(L"Previos appending not completed!!!");
		return;
	}

	try{
		if(!GetDataFile()->MoveLastFile()) return;
		if(ISNULL(GetDataFile()->GetCurrentFile())) return;
		
		//проверяем на предельный размер
		if(GetDataFile()->GetCurrentFile()->GetDataUnitSize()+uiDataSize>m_uiMaxDataFileSize){
			//если размер одного файла превышает порог
			//добавляем новый файл
			if(!GetDataFile()->AddFile()) return;
			if(!GetDataFile()->MoveLastFile()) return;
		}
		
		//заполняем информацию по добавляемым данным
		m_CurrAppendedDataInfo.m_uiOffsetInFile=GetDataFile()->GetCurrentFile()->GetDataUnitSize();
		m_CurrAppendedDataInfo.m_ucFileIndex=GetDataFile()->GetCurrentFileIndex();
		m_CurrAppendedDataInfo.m_uiDataSize=uiDataSize;
		m_CurrAppendedDataInfo.m_bDeleted=false;

		//добавляем инфу по данным в индексный файл
		GetIndexFile()->Append((unsigned char*)&m_CurrAppendedDataInfo, sizeof(SDataInfo));
		//добавляем данные в файл данных
		if(pData && m_CurrAppendedDataInfo.m_uiDataSize!=0)
			GetDataFile()->GetCurrentFile()->Append(pData, m_CurrAppendedDataInfo.m_uiDataSize);
		
		if(puiIndex){
			//определяем значение возвращаемого индекса
			(*puiIndex)=(GetIndexFile()->GetDataUnitSize()-sizeof(SDataInfo))/sizeof(SDataInfo);
		}

		m_CurrAppendedDataInfo.Reset();
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		throw;
	}
}

void CVectorStorage::AppendDataChunk(unsigned char* pData, unsigned int uiDataSize)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return;
	}

	if(ISNULL(pData)) return;
	//if(ISNULL(uiDataSize)) return;
	
	try{
		if(!GetDataFile()->MoveLastFile()) return;
		if(ISNULL(GetDataFile()->GetCurrentFile())) return;
		
		if(m_CurrAppendedDataInfo.IsEmpty()){
			//проверям на предельный размер
			if(GetDataFile()->GetCurrentFile()->GetDataUnitSize()+uiDataSize>m_uiMaxDataFileSize){
				//если размер одного файла превышает порог
				//добавляем новый файл
				if(!GetDataFile()->AddFile()) return;
				if(!GetDataFile()->MoveLastFile()) return;
			}
			
			//заполняем информацию по добавляемым данным
			m_CurrAppendedDataInfo.m_uiOffsetInFile=GetDataFile()->GetCurrentFile()->GetDataUnitSize();
			m_CurrAppendedDataInfo.m_ucFileIndex=GetDataFile()->GetCurrentFileIndex();
			m_CurrAppendedDataInfo.m_bDeleted=false;
		}
		
		//обновляем размер будущих данных
		m_CurrAppendedDataInfo.m_uiDataSize+=uiDataSize;
	
		//добавляем пакет данных в файл данных
		if(pData && uiDataSize!=0)
			GetDataFile()->GetCurrentFile()->Append(pData, uiDataSize);
		
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		throw;
	}
}

void CVectorStorage::AppendDataChunkComplete(unsigned int* puiIndex)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return;
	}

	try{
		if(m_CurrAppendedDataInfo.IsEmpty()){
			//viellsky TO_CONSOLE(L"m_CurrDataInfo is empty!!! No data was appended...");
			//viellsky 
			TO_CONSOLE(L"m_CurrDataInfo is empty!!! Empty block was appended...");

			//viellsky
			//unsigned char uc;
			//AppendDataChunk(&uc, 0);

			//viellsky return;
		}	
		
		//добавляем инфу по данным в индексный файл
		GetIndexFile()->Append((unsigned char*)&m_CurrAppendedDataInfo, sizeof(SDataInfo));

		if(puiIndex){
			//определяем значение возвращаемого индекса
			(*puiIndex)=(GetIndexFile()->GetDataUnitSize()-sizeof(SDataInfo))/sizeof(SDataInfo);
		}

		m_CurrAppendedDataInfo.Reset();

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		throw;
	}
}

void CVectorStorage::Delete(unsigned int uiIndex)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return;
	}

	//вычисляем по индексу смещение 
	unsigned int uiOffset=uiIndex*sizeof(SDataInfo);

	SDataInfo DataInfo;

	try{
		//смещаемся в индексном файле
		GetIndexFile()->SeekPos(uiOffset, SEEK_SET);
		//читаем инфу по данным в индексном файле
		GetIndexFile()->Read((unsigned char*)&DataInfo, sizeof(SDataInfo));
		
		//выставляем флаг удаления
		DataInfo.m_bDeleted=true;

		//смещаемся в индексном файле
		GetIndexFile()->SeekPos(uiOffset, SEEK_SET);
		//пишем инфу по данным в индексном файле
		GetIndexFile()->Write((unsigned char*)&DataInfo, sizeof(SDataInfo));
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		throw;
	}
}

bool CVectorStorage::IsDeleted(unsigned int uiIndex)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return false;
	}

	//вычисляем по индексу смещение 
	unsigned int uiOffset=uiIndex*sizeof(SDataInfo);

	SDataInfo DataInfo;

	try{
		//смещаемся в индексном файле
		GetIndexFile()->SeekPos(uiOffset, SEEK_SET);
		//читаем инфу по данным в индексном файле
		GetIndexFile()->Read((unsigned char*)&DataInfo, sizeof(SDataInfo));
		
		//выставляем флаг удаления
		return DataInfo.m_bDeleted;

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		throw;
		return false;
	}

	return false;
}

unsigned int CVectorStorage::GetIndexesNumber(void)
{
	if(!IsOpen()){
		TO_CONSOLE(L"VectorStorage not opened!!!");
		return 0;
	}

	return (GetIndexFile()->GetDataUnitSize()/sizeof(SDataInfo));
}

//--------------------------------------------------------------------//

}
}
}
}