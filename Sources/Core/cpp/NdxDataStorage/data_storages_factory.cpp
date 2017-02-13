#include "StdAfx.h"
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
CDataUnitFactory::CDataUnitFactory(void)
:m_pDBMSManager(NULL), m_pDBMSBase(NULL), m_bRegister(false)
{
}

CDataUnitFactory::~CDataUnitFactory(void)
{
	//закрываем базу 
	if(m_pDBMSBase) m_pDBMSBase->Close();
	//убиваем менеджер
	if(m_pDBMSManager) m_pDBMSManager->Release();
}


INdxFile* CDataUnitFactory::CreateBasisFile(void)
{
	m_bRegister=false;
	return new CBasisFile(this);
}

INdxCashedFile* CDataUnitFactory::CreateCashedFile(void) 
{
	m_bRegister=false;
	return new CRACashedFile(this);
}

INdxTableStorage* CDataUnitFactory::CreateTableStorage(void)
{
	m_bRegister=false;
	return new CTableStorage(this, m_pDBMSBase);
}

CFilesCollectionEx* CDataUnitFactory::CreateFilesCollectionEx(void)
{
	m_bRegister=false;
	return new CFilesCollectionEx(this);
}

CUpdateFile* CDataUnitFactory::CreateUpdateFile(void)
{
	m_bRegister=false;
	return new CUpdateFile(this);
}

CACashedFile* CDataUnitFactory::CreateAppendingCashedFile(void)
{
	m_bRegister=false;
	return new CACashedFile(this);
}

INdxLevelStorage* CDataUnitFactory::CreateLevelStorage(void)
{
	m_bRegister=false;
	return new CLevelStorage(this);
}

INdxVectorStorage* CDataUnitFactory::CreateVectorStorage(void)
{
	m_bRegister=false;
	return new CVectorStorage(this);
}

//--------------------------------------------------------------------//

CDataStorageFactory::CDataStorageFactory(void)
{
}

CDataStorageFactory::~CDataStorageFactory(void)
{
	UnRegisterAll();
}

HRESULT CDataStorageFactory::QueryInterface(REFIID pIID, void** pBase) 
{
	if(pIID==IID_Base){
		*pBase=(SS::Interface::IBase*)this;
	}else if(pIID==IID_NdxDataStorageFactory){
		*pBase=(INdxDataStorageFactory*)this;
	}else{
		*pBase=NULL;
	}

	return S_OK;
}

ULONG CDataStorageFactory::Release(void)
{
	delete this;
	return 0;
}

void CDataStorageFactory::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	if(ISNULL(pLoadManager)) return;
	IBase::SetLoadManager(pLoadManager);
	
	GetLoadManager()->Create(L"./core/linguistics/DBMS.dll", CLSID_DBMS, IID_DBManager, (void**)&m_pDBMSManager);
	
	if(ISNULL(m_pDBMSManager)) return;

	HRESULT hResult=m_pDBMSManager->InitDynamic(&m_pDBMSBase);

	if(hResult!=S_OK){
		TO_CONSOLE(L"Init error possible ndx_dbms.ini file not exist");
		return;
	}

	if(ISNULL(m_pDBMSBase)) return;
}

INdxFile* CDataStorageFactory::CreateNdxFile(void)
{
	m_bRegister=true;
	return new CBasisFile(this);
}

INdxCashedFile* CDataStorageFactory::CreateNdxCashedFile(void) 
{
	m_bRegister=true;
	return new CRACashedFile(this);
}

INdxTableStorage* CDataStorageFactory::CreateNdxTableStorage(void)
{
	m_bRegister=true;
	return new CTableStorage(this, m_pDBMSBase);
}

INdxVectorStorage* CDataStorageFactory::CreateNdxVectorStorage(void)
{
	m_bRegister=true;
	return new CVectorStorage(this);
}

INdxStorage* CDataStorageFactory::CreateNdxStorage(void)
{
	m_bRegister=true;
	return new CNdxStorage(this);
}

INdxStorageWithStatistic* CDataStorageFactory::CreateNdxStorageWithStatistic(void)
{
	m_bRegister=true;
	return new CStatisticNdxStorage(this);
}

void CDataStorageFactory::Register(INdxDataUnit* pNdxDataUnit)
{
	if(!m_bRegister) return;

	TDataUnitCollection::iterator itDataUnit=m_DataUnitCollection.find(pNdxDataUnit);
	if(itDataUnit==m_DataUnitCollection.end()){
		m_DataUnitCollection.insert(pNdxDataUnit);
	}else{
		TO_CONSOLE(L"Data unit already exist");
	}

	//сбрасываем флаг регистрации
	m_bRegister=false;
}

void CDataStorageFactory::UnRegister(INdxDataUnit* pNdxDataUnit)
{
	TDataUnitCollection::iterator itDataUnit=m_DataUnitCollection.find(pNdxDataUnit);
	if(itDataUnit!=m_DataUnitCollection.end()){
		m_DataUnitCollection.erase(pNdxDataUnit);
	}
}

void CDataStorageFactory::UnRegisterAll(void)
{
	while(!m_DataUnitCollection.empty()){
		//ToConsole((*m_DataUnitCollection.begin())->GetDataUnitName());
		(*m_DataUnitCollection.begin())->Release();
	}
}

//--------------------------------------------------------------------//

}
}
}
}