#include "StdAfx.h"
#include ".\table_storage.h"
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
CTableStorage::CTableStorage(CDataUnitFactory* pDataStorageFactory, IDataBase* pDBMSBase)
:CDataUnit(pDataStorageFactory), 
m_pDBMSBase(pDBMSBase), m_pDBMSTable(NULL), m_LastAccessIndex(0)
{
	ISNULL(m_pDBMSBase);
}

CTableStorage::~CTableStorage(void)
{
	Close();

	m_TTableFieldCollection.clear();
	
	for(unsigned int i=0; i<m_TableFieldNamesCollection.size(); i++){
		if(m_TableFieldNamesCollection[i])
			delete[] m_TableFieldNamesCollection[i];
	}

	m_TableFieldNamesCollection.clear();
}

HRESULT CTableStorage::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_NdxDataUnit){
		*pBase=(INdxDataUnit*)this;
	}else if(pIID==IID_NdxTableStorage){
		*pBase=(INdxTableStorage*)this;
	}else{
		*pBase=NULL;
	}

	return S_OK;
}

bool CTableStorage::Open(const wchar_t* pszPath)
{
	if(ISNULL(m_pDBMSBase)) return false;

	//если таблица открыта
	if(m_pDBMSTable){ 
		TO_CONSOLE(L"Table already opened, close it!!!");
		ToConsole(GetDataUnitName());
		return true;
	}
	
	if(m_TTableFieldCollection.empty()){
		TO_CONSOLE(L"Table fields not exist!!!");
		return S_FALSE;
	}else{
		//добавляем конечную структуру
		TDataTableFieldDescription DataTableFieldDescription;
		DataTableFieldDescription.szFieldName=NULL;
		DataTableFieldDescription.etFieldType=TYPE_FINAL;
		DataTableFieldDescription.etFieldModificator=MOD_FINAL;
		m_TTableFieldCollection.push_back(DataTableFieldDescription);
	}

	try{
		m_pDBMSTable=m_pDBMSBase->DynamicDataTableCreate(pszPath, GetDataUnitName(), 1000, GROW_EVEN, &m_TTableFieldCollection.front(), true);
		
		if(ISNULL(m_pDBMSTable)){
			TO_CONSOLE(L"Cann't open m_pDBMSTable");
			ToConsole(GetDataUnitName());
		}

		m_bIsOpen=true;
		return m_bIsOpen;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		ToConsole(GetDataUnitName());
		return false;
	}

}

bool CTableStorage::Close(void)
{
	if(ISNULL(m_pDBMSBase)) return true;
	
	//если таблица уже закрыта
	if(!m_pDBMSTable) return true;
	
	try{
		//закрывает и освобождает используемую таблицей память
		m_pDBMSBase->DynamicDataTableRefuse(m_pDBMSTable);
		m_pDBMSTable=NULL;
		m_LastAccessIndex=0;
		m_bIsOpen=false;
		return true;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return false;
	}
}

void CTableStorage::Clear(void)
{
	if(ISNULL(m_pDBMSTable)) return ;

	try{
		if(m_pDBMSTable->Clear()!=S_OK){
			ERR_TO_CONSOLE(erDBMS, L"");
		}
		return;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return;
	}
}

bool CTableStorage::Flush(void)
{
	return true;
}

unsigned int CTableStorage::GetDataUnitSize(void)
{
	if(ISNULL(m_pDBMSTable)) return 0;

	return m_pDBMSTable->GetRecordsNumber();
}

HRESULT CTableStorage::AddField(TDataTableFieldDescription* pDataTableFieldDescription)
{
	if(ISNULL(pDataTableFieldDescription)) return E_FAIL;

	//если таблица открыта
	if(m_pDBMSTable){ 
		TO_CONSOLE(L"Table already opened, close before adding fields!!!");
		ToConsole(GetDataUnitName());
		return S_FALSE;
	}
		
	wchar_t* pwszName=new wchar_t[wcslen(pDataTableFieldDescription->szFieldName)+1];
	wcscpy(pwszName, pDataTableFieldDescription->szFieldName);
	*(pwszName+wcslen(pDataTableFieldDescription->szFieldName))=0;

	m_TTableFieldCollection.push_back(*pDataTableFieldDescription);
	m_TTableFieldCollection.back().szFieldName=pwszName;
	m_TableFieldNamesCollection.push_back(pwszName);
	
	return S_OK;
}

HRESULT CTableStorage::InitFind(wchar_t* pKeyName, void* pKeyValue, void* ArrayOfPointersToData[], unsigned short* pusFindId)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;
	if(ISNULL(pKeyName)) return E_FAIL;
	if(ISNULL(pKeyValue)) return E_FAIL;
	if(ISNULL(ArrayOfPointersToData)) return E_FAIL;

	try{
		if(pusFindId){
			if(m_pDBMSTable->FindInitEx(pKeyName, pKeyValue, ArrayOfPointersToData, pusFindId)!=S_OK){
				ERR_TO_CONSOLE(erDBMS, L"");
				return E_FAIL;
			}
		}else{
			if(m_pDBMSTable->FindInit(pKeyName, pKeyValue, ArrayOfPointersToData)!=S_OK){
				ERR_TO_CONSOLE(erDBMS, L"");
				return E_FAIL;
			}
		}
		return S_OK;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::Find(unsigned int uiFindId)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;

	try{
		return m_pDBMSTable->Find(uiFindId);
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::FindOne(unsigned int uiFindId)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;

	try{
		return m_pDBMSTable->FindOne(uiFindId);
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::SpanFindInit(wchar_t* pKeyName, void* pLowerKeyValue, void* pUpperKeyValue, 
	std::set<unsigned int>* pSetOfRecordsIndexes, unsigned short* pusFindId)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;
	if(ISNULL(pKeyName)) return E_FAIL;
	if(ISNULL(pLowerKeyValue)) return E_FAIL;
	if(ISNULL(pUpperKeyValue)) return E_FAIL;
	if(ISNULL(pSetOfRecordsIndexes)) return E_FAIL;

	try{
		if(pusFindId){
			if(m_pDBMSTable->SpanFindInitEx(pKeyName, pLowerKeyValue, pUpperKeyValue, pSetOfRecordsIndexes, pusFindId)!=S_OK){
				ERR_TO_CONSOLE(erDBMS, L"");
				return E_FAIL;
			}
		}else{
			if(m_pDBMSTable->SpanFindInit(pKeyName, pLowerKeyValue, pUpperKeyValue, pSetOfRecordsIndexes)!=S_OK){
				ERR_TO_CONSOLE(erDBMS, L"");
				return E_FAIL;
			}
		}
		return S_OK;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::SpanFind(unsigned int uiFindId)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;

	try{
		return m_pDBMSTable->SpanFind(uiFindId);
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

unsigned int CTableStorage::GetFoundRecordIndex(unsigned int uiFindId)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;

	try{
		return m_pDBMSTable->GetFoundIdx(uiFindId);
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

void CTableStorage::DeleteRecord(unsigned int uiRecordIndex)
{
	if(ISNULL(m_pDBMSTable)) return;

	try{
		m_pDBMSTable->DeleteRecord(uiRecordIndex);
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return;
	}
}

HRESULT CTableStorage::InitScan(long nStartRecordIdx, void* ArrayOfPointersToData[])
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;
	if(ISNULL(ArrayOfPointersToData)) return E_FAIL;

	try{
		if(m_pDBMSTable->ScanInit(nStartRecordIdx, ArrayOfPointersToData)!=S_OK){
			ERR_TO_CONSOLE(erDBMS, L"");
			return E_FAIL;
		}
		return S_OK;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::Scan(void)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;

	try{
		return m_pDBMSTable->Scan();
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::ScanOne(unsigned int uiRecordIndex)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;

	try{
		return m_pDBMSTable->ScanOne(uiRecordIndex);
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::InitAdd(void* ArrayOfPointersToData[])
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;
	if(ISNULL(ArrayOfPointersToData)) return E_FAIL;

	try{
		if(m_pDBMSTable->AddInit(ArrayOfPointersToData)!=S_OK){
			ERR_TO_CONSOLE(erDBMS, L"");
			return E_FAIL;
		}
		return S_OK;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::Add(void)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;

	try{
		if(m_pDBMSTable->Add()!=S_OK){
			ERR_TO_CONSOLE(erDBMS, L"");
			return E_FAIL;
		}
		return S_OK;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::InitModify(void* ArrayOfPointersToData[])
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;
	if(ISNULL(ArrayOfPointersToData)) return E_FAIL;

	try{
		if(m_pDBMSTable->ChangeInit(ArrayOfPointersToData)!=S_OK){
			ERR_TO_CONSOLE(erDBMS, L"");
			return E_FAIL;
		}
		return S_OK;
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

HRESULT CTableStorage::FindOneAndChange(void)
{
	if(ISNULL(m_pDBMSTable)) return E_FAIL;

	try{
		return m_pDBMSTable->FindOneAndChange();
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return E_FAIL;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
