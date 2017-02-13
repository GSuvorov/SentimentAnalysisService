#include "StdAfx.h"
#include ".\index_storage.h"
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
using namespace SS::Interface::Core::DBMS;

//--------------------------------------------------------------------//

CNdxStorage::CNdxStorage(CDataUnitFactory* pDataStorageFactory)
:CFilesCollection(pDataStorageFactory)
{
}

CNdxStorage::~CNdxStorage(void)
{
}

HRESULT CNdxStorage::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_NdxDataUnit){
		*pBase=(INdxDataUnit*)this;
	}else if(pIID==IID_NdxStorage){
		*pBase=(INdxStorage*)this;
	}else{
		*pBase=NULL;
	}

	return S_OK;
}

void CNdxStorage::Create(const wchar_t* pszDataUnitName, const wchar_t* pszDataUnitExt)
{
	CDataUnit::Create(pszDataUnitName, pszDataUnitExt);

	//добавляем таблицу точек входа
	AddFile();
	//добавляем хранилище образов документов
	AddFile();

	//добавляем новое хранилище образов документов
	AddFile();
	AddFile();
	AddFile();

	//добавляем хранилище информации о структуре уровней
	AddFile();

	//добавляем хранилище координат индексов
	AddFile();
}

bool CNdxStorage::Open(const wchar_t* pszPath)
{
	m_szCurrentFilePath.assign(pszPath);
	m_szCurrentFilePath.append(GetDataUnitName());

	//создаем директорию с именем индексного хранилища
	CreateDirectory(m_szCurrentFilePath.c_str(), NULL);
	m_szCurrentFilePath.append(L"\\");

	//открываемся
	bool bResult=CFilesCollection::Open(m_szCurrentFilePath.c_str());
	
	//обновляем информацию о структуре уровней
	UpdateLevelsInfo();

	return bResult; 
}

void CNdxStorage::GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName)
{
	if(uiFileIndex==EMPTY_VALUE){
		ERR_TO_CONSOLE(erFileIO, L"Invalid file index");	
	}

	switch((EIndexStorageUnits)uiFileIndex){
	case eisuEPTable:
		pszFileName->assign(L"_ept_");
		pszFileName->append(GetDataUnitName());
		break;
	case eisuTextImages:
		pszFileName->assign(L"_timg");
		break;
	case eisuTextImages_TextsOffsets:
		pszFileName->assign(L"_timg_tof");
		break;
	case eisuTextImages_TextBlocksOffsets:
		pszFileName->assign(L"_timg_tbof");
		break;
	case eisuTextImages_TextBlocksContent:
		pszFileName->assign(L"_timg_tbc");
		break;
	case eisuLevelsInfo:
		pszFileName->assign(L"_lvl_inf");
		break;
	case eisuCoordinates:
		pszFileName->assign(L"_crd");
		break;
	default:
		pszFileName->clear();
		break;
	}
}

INdxDataUnit* CNdxStorage::CreateFile(unsigned int uiFileIndex)
{
	//генерируем имя файла
	wstring szNewFileName;
	GenerateFileName(GetFilesNumber(), &szNewFileName);

	if(ISNULL(GetDataStorageFactory())) return NULL;

	switch((EIndexStorageUnits)uiFileIndex){
	case eisuEPTable:
		{
		//создаем таблицу точек вход
		INdxTableStorage* pTableStorage=GetDataStorageFactory()->CreateTableStorage();
		pTableStorage->Create(szNewFileName.c_str(), GetDataUnitExt());
		CreateEntryPointsStorage(pTableStorage);
		return pTableStorage;
		}
		break;
	case eisuTextImages:
		{
		//создаем хранилище образов документов
		INdxVectorStorage* pVectorStorage=GetDataStorageFactory()->CreateVectorStorage();
		pVectorStorage->Create(szNewFileName.c_str(), GetDataUnitExt());
		return pVectorStorage;
		}
		break;
	case eisuTextImages_TextBlocksContent:
		{
		//создаем хранилище образов документов
		INdxVectorStorage* pVectorStorage=GetDataStorageFactory()->CreateVectorStorage();
		pVectorStorage->Create(szNewFileName.c_str(), GetDataUnitExt());
		//устанавливаем аттрибуты бакапирования
		pVectorStorage->SetBackupSettings(SBackupSettings::cmUncompressible, SBackupSettings::wmAppend);
		return pVectorStorage;
		}
		break;
	case eisuLevelsInfo: case eisuTextImages_TextsOffsets: case eisuTextImages_TextBlocksOffsets:
		{
		INdxFile* pStorage=GetDataStorageFactory()->CreateBasisFile();
		pStorage->Create(szNewFileName.c_str(), GetDataUnitExt());
		//устанавливаем аттрибуты бакапирования
		pStorage->SetBackupSettings(SBackupSettings::cmCompressible, SBackupSettings::wmAppend);
		return pStorage;
		}
		break;
	case eisuCoordinates:
		{
		//создаем координатый уровень
		INdxLevelStorage* pLevelStorage=GetDataStorageFactory()->CreateLevelStorage();
		pLevelStorage->Create(szNewFileName.c_str(), GetDataUnitExt());
		return pLevelStorage;
		}
		break;
	default:
		return NULL;
		break;
	}
}

void CNdxStorage::AddLevelInfo(SNdxLevelInfo* pNdxLevelInfo)
{
	if(ISNULL(pNdxLevelInfo)) return;
	
	if(pNdxLevelInfo->m_ucLevelNumber!=m_NdxLevelInfoCollection.size()){
		TO_CONSOLE(L"Levels added not consequently");
		ToConsole(L"LevelNumber", (unsigned int)pNdxLevelInfo->m_ucLevelNumber);
		return;
	}

	//добавляем себе
	SNdxLevelInfo NdxLevelInfo;
	memcpy((void*)&NdxLevelInfo, (void*)pNdxLevelInfo, sizeof(SNdxLevelInfo));
	m_NdxLevelInfoCollection.push_back(NdxLevelInfo);
}

SNdxLevelInfo const* CNdxStorage::GetLevelInfo(unsigned int uiLevelNumber)
{
	if(uiLevelNumber>=m_NdxLevelInfoCollection.size()){
		TO_CONSOLE(L"Too big level number!!!");
		ToConsole(L"uiLevelNumber", uiLevelNumber);
		return NULL;
	}
	
	return &m_NdxLevelInfoCollection[uiLevelNumber];
}

void CNdxStorage::CreateEntryPointsStorage(INdxTableStorage* pTableStorage)
{
	if(ISNULL(pTableStorage)) return;

	TDataTableFieldDescription DataTableFieldDescription;
	
	DataTableFieldDescription.szFieldName=L"Index";
	DataTableFieldDescription.etFieldType=TYPE_UINT32;
	DataTableFieldDescription.etFieldModificator=MOD_IS_UNIC;
	pTableStorage->AddField(&DataTableFieldDescription);

	DataTableFieldDescription.szFieldName=L"LevelEP";
	DataTableFieldDescription.etFieldType=TYPE_UINT32;
	DataTableFieldDescription.etFieldModificator=MOD_IS_DATA;
	pTableStorage->AddField(&DataTableFieldDescription);

	DataTableFieldDescription.szFieldName=L"IndexDocFreq";
	DataTableFieldDescription.etFieldType=TYPE_UINT32;
	DataTableFieldDescription.etFieldModificator=MOD_IS_DATA;
	pTableStorage->AddField(&DataTableFieldDescription);
}

INdxTableStorage* CNdxStorage::GetEntryPointsStorage(void)
{
	if(m_FilesCollection.empty()) return NULL;

	return dynamic_cast<INdxTableStorage*>(m_FilesCollection[eisuEPTable]);
}

INdxVectorStorage* CNdxStorage::GetTextImagesStorage(void)
{
	if(m_FilesCollection.empty()) return NULL;

	return dynamic_cast<INdxVectorStorage*>(m_FilesCollection[eisuTextImages]);
}

void CNdxStorage::GetTextImagesStorages(INdxFile** ppNdxTextsOffsets, 
	INdxFile** ppNdxTextBlocksOffsets, INdxVectorStorage** ppNdxTextBlocksContent)
{
	if(ISNULL(ppNdxTextsOffsets)) return;
	if(ISNULL(ppNdxTextBlocksOffsets)) return;
	if(ISNULL(ppNdxTextBlocksContent)) return;
	if(m_FilesCollection.empty()) return;
	
	*ppNdxTextsOffsets=dynamic_cast<INdxFile*>(m_FilesCollection[eisuTextImages_TextsOffsets]);
	*ppNdxTextBlocksOffsets=dynamic_cast<INdxFile*>(m_FilesCollection[eisuTextImages_TextBlocksOffsets]);
	*ppNdxTextBlocksContent=dynamic_cast<INdxVectorStorage*>(m_FilesCollection[eisuTextImages_TextBlocksContent]);
}

INdxLevelStorage* CNdxStorage::GetCoordinateStorage(void)
{
	return dynamic_cast<INdxLevelStorage*>(m_FilesCollection[eisuCoordinates]);
}

void CNdxStorage::UpdateLevelsInfo(void)
{
	INdxFile* pLevelsInfoFile=dynamic_cast<INdxFile*>(m_FilesCollection[eisuLevelsInfo]);

	if(ISNULL(pLevelsInfoFile)) return;
	if(!pLevelsInfoFile->IsOpen()){
		ERR_TO_CONSOLE(erFileIO, L"LevelsInfoFile not opened");	
	}

	unsigned int uiLevelsInfoFileSize=pLevelsInfoFile->GetDataUnitSize();
	if(uiLevelsInfoFileSize!=0){
		//если файл заполнен, то правильно то что в файле, обновляем коллекцию
		m_NdxLevelInfoCollection.clear();
		m_NdxLevelInfoCollection.resize(uiLevelsInfoFileSize/sizeof(SNdxLevelInfo));
		pLevelsInfoFile->Read((unsigned char*)&m_NdxLevelInfoCollection[0], uiLevelsInfoFileSize);
	}else{
		//если файл пустой верна текущая структура уровней, ее мы и запишем
		pLevelsInfoFile->Write((unsigned char*)&m_NdxLevelInfoCollection[0], (unsigned int)m_NdxLevelInfoCollection.size()*sizeof(SNdxLevelInfo));
	}
}

void CNdxStorage::SaveLevelsInfo(void)
{
	INdxFile* pLevelsInfoFile=dynamic_cast<INdxFile*>(m_FilesCollection[eisuLevelsInfo]);

	if(ISNULL(pLevelsInfoFile)) return;
	if(!pLevelsInfoFile->IsOpen()){
		ERR_TO_CONSOLE(erFileIO, L"LevelsInfoFile not opened");	
	}

	unsigned int uiLevelsInfoFileSize=pLevelsInfoFile->GetDataUnitSize();
	if(uiLevelsInfoFileSize==0){
		//если файл пустой верна текущая структура уровней, ее мы и запишем при закрытии
		pLevelsInfoFile->Write((unsigned char*)&m_NdxLevelInfoCollection[0], (unsigned int)m_NdxLevelInfoCollection.size()*sizeof(SNdxLevelInfo));
	}
}

//--------------------------------------------------------------------//

}
}
}
}
