#include "StdAfx.h"
#include ".\files_collections.h"
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

CFilesCollection::CFilesCollection(CDataUnitFactory* pDataStorageFactory)
:CDataUnit(pDataStorageFactory), 
m_uiMaxFilesNumber(cuiMaxFilesNumber)
{
	m_itCurrentFile = m_NullCollection.end();
}

CFilesCollection::~CFilesCollection(void)
{
	DeleteFiles();
}

bool CFilesCollection::Open(const wchar_t* pszPath)
{
	if(IsOpen()){
		TO_CONSOLE(L"FilesCollection already opened, close before!!!")
		return false;
	}

	m_szCurrentFilePath=pszPath;
	
	for(TFiles::iterator itCurrFile=m_FilesCollection.begin(); itCurrFile!=m_FilesCollection.end(); itCurrFile++){
		if((*itCurrFile)!=NULL){ 
			(*itCurrFile)->Open(m_szCurrentFilePath.c_str());
		}else{
			m_bIsOpen=false;
			return false;			
		}
	}
		
	//выставляем флаг чтоб при добавлении файлы сразу открывались
	m_bIsOpen=true;

	//позиционируемся на первом файле
	return MoveFirstFile();
}

bool CFilesCollection::Close(void)
{
	if(!IsOpen()) return true;

	for(TFiles::iterator itCurrFile=m_FilesCollection.begin(); itCurrFile!=m_FilesCollection.end(); itCurrFile++){
		if((*itCurrFile)!=NULL){ 
			(*itCurrFile)->Close();
		}
	}
	
	m_szCurrentFilePath.clear();
	m_bIsOpen=false;
	
	return true;
}

bool CFilesCollection::Flush(void)
{
	for(TFiles::iterator itCurrFile=m_FilesCollection.begin(); itCurrFile!=m_FilesCollection.end(); itCurrFile++){
		if((*itCurrFile)!=NULL){ 
			(*itCurrFile)->Flush();
		}
	}
	
	return true;
}

void CFilesCollection::Clear(void)
{
	for(TFiles::iterator itCurrFile=m_FilesCollection.begin(); itCurrFile!=m_FilesCollection.end(); itCurrFile++){
		if((*itCurrFile)!=NULL){ 
			(*itCurrFile)->Clear();
		}
	}
}

unsigned int CFilesCollection::GetDataUnitSize(void)
{
	unsigned int uiTotalSize=0;
	for(TFiles::iterator itCurrFile=m_FilesCollection.begin(); itCurrFile!=m_FilesCollection.end(); itCurrFile++){
		if((*itCurrFile)!=NULL){ 
			uiTotalSize+=(*itCurrFile)->GetDataUnitSize();
		}
	}
	
	return uiTotalSize;
}

void CFilesCollection::DeleteFiles(void)
{
	if(IsOpen()) Close();

	for(TFiles::iterator itCurrFile=m_FilesCollection.begin(); itCurrFile!=m_FilesCollection.end(); itCurrFile++){
		if((*itCurrFile)!=NULL){ 
			(*itCurrFile)->Release();
		}
	}

	m_FilesCollection.clear();
	m_itCurrentFile=m_NullCollection.end();
}

void CFilesCollection::GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName)
{	
	wchar_t buf[20];
	_itow(uiFileIndex, buf, 10);

	pszFileName->assign(GetDataUnitName());
	pszFileName->append(L"_");
	pszFileName->append(buf);
}

INdxDataUnit* CFilesCollection::CreateFile(unsigned int uiFileIndex)
{
	//генерируем имя файла
	wstring szNewFileName;
	GenerateFileName((unsigned int)m_FilesCollection.size(), &szNewFileName);
	
	if(ISNULL(GetDataStorageFactory())) return NULL;

	//создаем файл
	INdxFile* pFile=GetDataStorageFactory()->CreateBasisFile();
	pFile->Create(szNewFileName.c_str(), GetDataUnitExt());
	//устанавливаем аттрибуты бакапирования
	pFile->SetBackupSettings(m_BackupSettings.m_CompressMode, m_BackupSettings.m_WriteMode);
	return pFile;
}

bool CFilesCollection::AddFile(void)
{	
	INdxDataUnit* pDataUnit=NULL;
	
	if(m_FilesCollection.size()>=m_uiMaxFilesNumber){
		TO_CONSOLE(L"Cann't add more files to collection size limit exceeded");
		return false;
	}

	//создаем файл
	pDataUnit=CreateFile((unsigned int)m_FilesCollection.size());
	
	if(ISNULL(pDataUnit)) return false;

	//добавляем файл в коллекцию
	m_FilesCollection.push_back(pDataUnit);

	if(IsOpen()){
		//если остальные файлы открыты, открываем и этот		
		if(!pDataUnit->Open(m_szCurrentFilePath.c_str())){
			//не открывается выбрасываем его из коллекции
			pDataUnit->Release();
			m_FilesCollection.pop_back();
			return false;
		}
	}

	return true;
}

unsigned char CFilesCollection::GetCurrentFileIndex(void)
{
	if(m_FilesCollection.empty() || m_itCurrentFile==m_NullCollection.end()){
		return UC_EMPTY;
	}else{
		return (unsigned char)(m_itCurrentFile-m_FilesCollection.begin());
	}
}

bool CFilesCollection::MoveFirstFile(void)
{
	if(!m_FilesCollection.empty()){
		m_itCurrentFile=m_FilesCollection.begin();
		return true;
	}else{
		TO_CONSOLE(L"m_FilesCollection is empty");
		return false;
	}
}

bool CFilesCollection::MoveLastFile(void)
{
	if(!m_FilesCollection.empty()){
		m_itCurrentFile=--m_FilesCollection.end();
		return true;
	}else{
		TO_CONSOLE(L"m_FilesCollection is empty");
		return false;
	}
}

INdxDataUnit* CFilesCollection::GetCurrentFile(void)
{
	if(m_itCurrentFile!=m_NullCollection.end()){
		return (*m_itCurrentFile);
	}else{
		return NULL;
	}
}

bool CFilesCollection::MoveToFile(unsigned int uiFileIndex)
{
	if(uiFileIndex<m_FilesCollection.size()){
		m_itCurrentFile=(m_FilesCollection.begin()+uiFileIndex);
		return true;
	}else{
		TO_CONSOLE(L"Too big uiFileIndex");
		return false;
	}
}

//--------------------------------------------------------------------//

CFilesCollectionEx::CFilesCollectionEx(CDataUnitFactory* pDataStorageFactory)
:CFilesCollection(pDataStorageFactory)
{
}

CFilesCollectionEx::~CFilesCollectionEx(void)
{
}

bool CFilesCollectionEx::Open(const wchar_t* pszPath)
{
	m_szCurrentFilePath.assign(pszPath);

	INdxDataUnit* pDataUnit=NULL;

	//открываем файл с количеством итемов в коллекции
	unsigned int uiDataUnitNumber=ReadDataUnitNumber();
	//если файлов с юнитам нет создаем хотя бы один начальный
	if(!uiDataUnitNumber) uiDataUnitNumber++;

	//открываем файлы коллекции
	unsigned int i=0;
	while(i<uiDataUnitNumber){
		//создаем файл
		pDataUnit=CreateFile((unsigned int)m_FilesCollection.size());

		if(ISNULL(pDataUnit)) return false;

		//открываем файл в данном режиме
		pDataUnit->Open(m_szCurrentFilePath.c_str());
		//если все открылось добавляем файл в коллекцию
		m_FilesCollection.push_back(pDataUnit);
		i++;
	}
	
	//выставляем флаг открытия
	m_bIsOpen=true;

	//позиционируемся на первом файле
	return MoveFirstFile();
}

bool CFilesCollectionEx::Close(void)
{
	//Если уже закрыты
	if(!IsOpen()) return true;
	//Запоминаем количество юнитов, если количество
	//В базе не совпадает с текущим количеством
	if (ReadDataUnitNumber() != GetFilesNumber())
	{ WriteDataUnitNumber(); }
	//Закрываемся
	return CFilesCollection::Close();
}

INdxFile* CFilesCollectionEx::GetCurrentFile(void)
{
	return dynamic_cast<INdxFile*>(CFilesCollection::GetCurrentFile());
}

unsigned int CFilesCollectionEx::ReadDataUnitNumber(void)
{
	//открываем хранилище с количеством итемов в коллекции
	unsigned int uiDataUnitNumber=0;
	INdxFile* pDataUnitNumber=GetDataStorageFactory()->CreateBasisFile();
	if(ISNULL(pDataUnitNumber)) return 0;
	pDataUnitNumber->Create(GetDataUnitName(), GetDataUnitExt());
	pDataUnitNumber->Open(m_szCurrentFilePath.c_str());
	pDataUnitNumber->SeekPos(0, SEEK_SET);
	pDataUnitNumber->Read((unsigned char*)&uiDataUnitNumber, sizeof(unsigned int));
	pDataUnitNumber->Close();
	pDataUnitNumber->Release();
	return uiDataUnitNumber;
}

void CFilesCollectionEx::WriteDataUnitNumber(void)
{
	//открываем хранилище с количеством итемов в коллекции
	unsigned int uiDataUnitNumber=GetFilesNumber();
	INdxFile* pDataUnitNumber=GetDataStorageFactory()->CreateBasisFile();
	if(ISNULL(pDataUnitNumber)) return;
	pDataUnitNumber->Create(GetDataUnitName(), GetDataUnitExt());
	pDataUnitNumber->Open(m_szCurrentFilePath.c_str());
	pDataUnitNumber->SeekPos(0, SEEK_SET);
	pDataUnitNumber->Write((unsigned char*)&uiDataUnitNumber, sizeof(unsigned int));
	pDataUnitNumber->Close();
	pDataUnitNumber->Release();
}

//--------------------------------------------------------------------//

}
}
}
}