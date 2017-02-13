#include "StdAfx.h"
#include ".\storage_manager.h"
#include "..\ASCInterface\defines.h"

USES_LOAD_MANAGER;


namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

using namespace SS::Interface::Core::NdxSE;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CStorageManager::CStorageManager(void)
:m_pNdxBaseManager(NULL),
m_uiCurrentNdxBaseSessionID(0)
{
}

CStorageManager::~CStorageManager(void)
{
	DeleteStorages();
}

bool CStorageManager::Update(void)
{
	if(ISNULL(m_pNdxBaseManager)) SS_THROW(L"m_pNdxBaseManager is NULL");

	SS_TRY

		unsigned int uiCurrentNdxBaseSessionID=m_pNdxBaseManager->GetCurrentSessionID();

		if(m_uiCurrentNdxBaseSessionID!=uiCurrentNdxBaseSessionID){
			//производим обновление, только если изменилась сессия индксной базы
			Init(m_pNdxBaseManager);

			ToConsole(L"NdxSearchEngine was updated");
			return true;
		}
		return false;

	SS_CATCH(L"");
	return false;
}

void CStorageManager::DeleteStorages(void)
{
	for(TStorages::iterator itStorage=m_Storages.begin(); itStorage!=m_Storages.end(); itStorage++){
		//ToConsole(itStorage->first.c_str());
		delete itStorage->second;
	}
	m_Storages.clear();
}

void CStorageManager::AddStorage(const wchar_t* wszStorageName, CBasisStorage* pStorage)
{
	if(ISNULL(pStorage)) return;

	if(m_Storages.find(wszStorageName)==m_Storages.end()){
		m_Storages[wszStorageName]=pStorage;
	}else{
		TO_CONSOLE(L"DataUnit already exist!!!");
		ToConsole(wszStorageName);
	}
}

CBasisStorage* CStorageManager::GetStorage(const wchar_t* wszStorageName)
{
	TStorages::iterator itStorage=m_Storages.find(wszStorageName);
	if(itStorage!=m_Storages.end()){
		return itStorage->second;
	}

	//TO_CONSOLE(L"DataUnit not find!!!");
	//ToConsole(L"DataUnit type", (unsigned int)NdxStorageType);

	return NULL;
}

void CStorageManager::Init(SS::Interface::Core::NdxSE::INdxBaseManager* pNdxBaseManager)
{
	if(ISNULL(pNdxBaseManager)) return;
	
	//очищаем старые хранилища
	DeleteStorages();

	m_pNdxBaseManager=pNdxBaseManager;
	
	INdxDataUnit* pNdxDataUnit=m_pNdxBaseManager->GetFirstNdxBaseUnit();
	if(!pNdxDataUnit){
		ToConsole(L"Warning: Storage list empty, possible base not opened!!!");
	}

	wstring wsStorageName;
	while(pNdxDataUnit){
		
		wsStorageName.assign(pNdxDataUnit->GetDataUnitName());

		if(wsStorageName==ObligatoryStorageNames::TextsInfoStorage){
			//добавляем хранилище информации о проиндексиованных текстах
			AddStorage(wsStorageName.c_str(), 
				new CTextsInfoStorage(dynamic_cast<INdxVectorStorage*>(pNdxDataUnit)));
		}else if(wsStorageName==ObligatoryStorageNames::TextsPathTreeStorage){
			//добавляем хранилище путей к текстам в виде дерева путей
			AddStorage(wsStorageName.c_str(), 
				new CTextsPathsTreeStorage(dynamic_cast<INdxFile*>(pNdxDataUnit)));
		}else if(wsStorageName==ObligatoryStorageNames::TextsFieldsStorage){
			//добавляем хранилище полей текстов в виде таблицы DBMS
			AddStorage(wsStorageName.c_str(), 
				new CTextsFieldsStorage(dynamic_cast<INdxTableStorage*>(pNdxDataUnit)));
		}else{
			if(wsStorageName==L"HEADERS"){
				//добавляем индексное хранилище
				AddStorage(wsStorageName.c_str(), 
					new CIndexStorageHeaders(dynamic_cast<INdxStorageWithStatistic*>(pNdxDataUnit)));
			}else if(wsStorageName==L"TABLES"){
				//добавляем индексное хранилище
				AddStorage(wsStorageName.c_str(), 
					new CIndexStorageTables(dynamic_cast<INdxStorageWithStatistic*>(pNdxDataUnit)));
			}else{
				//добавляем индексное хранилище
				AddStorage(wsStorageName.c_str(), 
					new CIndexStatisticStorage(dynamic_cast<INdxStorageWithStatistic*>(pNdxDataUnit)));
			}
		}

		pNdxDataUnit=m_pNdxBaseManager->GetNextNdxBaseUnit();
	}

	m_uiCurrentNdxBaseSessionID=m_pNdxBaseManager->GetCurrentSessionID();

}

void CStorageManager::SetIndexingMode(void)
{
	for(TStorages::iterator itStorage=m_Storages.begin(); itStorage!=m_Storages.end(); itStorage++){
		if(itStorage->second){
			itStorage->second->SetWorkingMode(CBasisStorage::ewmIndexing);	
		}
	}
}

void CStorageManager::SetExtractingMode(void)
{
	for(TStorages::iterator itStorage=m_Storages.begin(); itStorage!=m_Storages.end(); itStorage++){
		if(itStorage->second){
			itStorage->second->SetWorkingMode(CBasisStorage::ewmExtracting);	
		}
	}
}

bool CStorageManager::IsIndexingMode(void)
{
	for(TStorages::iterator itStorage=m_Storages.begin(); itStorage!=m_Storages.end(); itStorage++){
		if(itStorage->second){
			return itStorage->second->GetWorkingMode()==CBasisStorage::ewmIndexing?true:false;	
		}
	}

	return false;
}

bool CStorageManager::IsExtractingMode(void)
{
	for(TStorages::iterator itStorage=m_Storages.begin(); itStorage!=m_Storages.end(); itStorage++){
		if(itStorage->second){
			return itStorage->second->GetWorkingMode()==CBasisStorage::ewmExtracting?true:false;	
		}
	}
	
	return false;
}

void CStorageManager::PrepareIndexing(void)
{
	for(TStorages::iterator itStorage=m_Storages.begin(); itStorage!=m_Storages.end(); itStorage++){
		if(itStorage->second){
			itStorage->second->PrepareIndexing();	
		}
	}
}

void CStorageManager::CompleteIndexing(void)
{
	for(TStorages::iterator itStorage=m_Storages.begin(); itStorage!=m_Storages.end(); itStorage++){
		if(itStorage->second){
			itStorage->second->CompleteIndexing();	
		}
	}
}

void CStorageManager::CompleteDocumentIndexing(void)
{
	for(TStorages::iterator itStorage=m_Storages.begin(); itStorage!=m_Storages.end(); itStorage++){
		if(itStorage->second){
			itStorage->second->CompleteDocumentIndexing();	
		}
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}