#include "StdAfx.h"
#include ".\index_stat_storage.h"
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

CStatisticNdxStorage::CStatisticNdxStorage(CDataUnitFactory* pDataStorageFactory)
:CNdxStorage(pDataStorageFactory)
{
}

CStatisticNdxStorage::~CStatisticNdxStorage(void)
{
}

HRESULT CStatisticNdxStorage::QueryInterface(REFIID pIID, void** pBase)
{
	//if(pIID==IID_NdxDataUnit){
	//	*pBase=(INdxDataUnit*)this;
	//}else if(pIID==IID_NdxStorage){
	//	*pBase=(INdxStorage*)this;
	//}else{
	//	*pBase=NULL;
	//}

	return S_OK;
}

void CStatisticNdxStorage::Create(const wchar_t* pszDataUnitName, const wchar_t* pszDataUnitExt)
{
	//создаем базовое хранилище
	CNdxStorage::Create(pszDataUnitName, pszDataUnitExt);

	//добавляем таблицу норм
	AddFile();
	//добавляем хранилище частот
	AddFile();
}

void CStatisticNdxStorage::GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName)
{
	if(uiFileIndex==EMPTY_VALUE){
		ERR_TO_CONSOLE(erFileIO, L"Invalid file index");	
	}

	if(uiFileIndex==eisuNormTable){
		pszFileName->assign(L"_norm");
	}else if(uiFileIndex==eisuFreqUnit){
		pszFileName->assign(L"_freq");
	}else{
		CNdxStorage::GenerateFileName(uiFileIndex, pszFileName);
	}
}

INdxDataUnit* CStatisticNdxStorage::CreateFile(unsigned int uiFileIndex)
{
	//генерируем имя файла
	wstring szNewFileName;
	GenerateFileName(GetFilesNumber(), &szNewFileName);

	if(ISNULL(GetDataStorageFactory())) return NULL;
	
	if(uiFileIndex==eisuNormTable){
		//создаем хранилище норм документов
		INdxFile* pStorage=GetDataStorageFactory()->CreateBasisFile();
		pStorage->Create(szNewFileName.c_str(), GetDataUnitExt());
		//устанавливаем аттрибуты бакапирования
		pStorage->SetBackupSettings(SBackupSettings::cmCompressible, SBackupSettings::wmAppend);
		return pStorage;
	}else if(uiFileIndex==eisuFreqUnit){
		//создаем хранилище часот индексов
		INdxVectorStorage* pVectorStorage=GetDataStorageFactory()->CreateVectorStorage();
		pVectorStorage->Create(szNewFileName.c_str(), GetDataUnitExt());
		return pVectorStorage;
	}else{
		//создаем хранилища базового класса
		return 	CNdxStorage::CreateFile(uiFileIndex);
	}
}

INdxVectorStorage* CStatisticNdxStorage::GetIndexFrequencyStorage(void)
{
	if(m_FilesCollection.empty()) return NULL;

	return dynamic_cast<INdxVectorStorage*>(m_FilesCollection[eisuFreqUnit]);
}

INdxFile* CStatisticNdxStorage::GetDocumentNormStorage(void)
{
	if(m_FilesCollection.empty()) return NULL;

	return dynamic_cast<INdxFile*>(m_FilesCollection[eisuNormTable]);
}

//--------------------------------------------------------------------//

}
}
}
}
