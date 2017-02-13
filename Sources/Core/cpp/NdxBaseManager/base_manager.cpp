#include "StdAfx.h"
#include ".\base_manager.h"
#include ".\const.h"
#include ".\console.h"
#include "..\ASCInterface\defines.h"


USES_LOAD_MANAGER;

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxBaseManager
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

CNdxBaseManager::CNdxBaseManager(void)
:m_bIsBaseOpen(false), 
m_wsCurrentBase(L"empty"), m_wsBasePath(L"empty"), 
m_uiCurrentSessionID(0)
{
	m_itCurrentDataUnit = m_NullCollection.end();
}

CNdxBaseManager::~CNdxBaseManager(void)
{
	//закрываем базу
	CloseBase();
}

HRESULT CNdxBaseManager::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_NdxBaseManager){
		*pBase=(INdxBaseManager*)this;
	}else if(pIID==IID_BaseManager){
		*pBase=(SS::Interface::Core::ResourceManagers::IBaseManager*)this;
	}else if(pIID==IID_Base){
		*pBase=(SS::Interface::IBase*)this;
	}else{
		*pBase=NULL;
	}
	
	return S_OK;
}

ULONG CNdxBaseManager::Release(void)
{
	delete this;
	return 0;
}

void CNdxBaseManager::SetBasePath(const wchar_t* wszPath)
{
	m_wsBasePath.assign(wszPath);
}

HRESULT CNdxBaseManager::OpenBase(const wchar_t* wszBaseName)
{
	ToConsole(L"CNdxBaseManager::OpenBase");
	ToConsole(wszBaseName);
	
	SS_TRY

		if(m_bIsBaseOpen){
			if(!m_wsCurrentBase.compare(wszBaseName)){
				ToConsole(L"Warning: don't open same base many times!!!!");
				ToConsole(m_wsCurrentBase.c_str());
				return S_OK;
			}else{
				TO_CONSOLE(L"Another Database is opened! Close it before!");
				ToConsole(m_wsCurrentBase.c_str());
				SS_THROW(L"Another Database is opened! Close it before!");
			}
		}
		
		//создаем юниты базы фабрикой по структуре индексной базы
		m_NdxBaseFactory.CreateBase(this);

		if(m_wsBasePath.empty()){
			TO_CONSOLE(L"DataBasePath not set!");
			return S_FALSE;
		}

		wstring wsPath(m_wsBasePath);
		wsPath.append(wszBaseName);

		if(wcslen(wszBaseName)){
			CreateDirectory(wsPath.c_str(), NULL);
			wsPath.append(L"\\");
		}
		
		for(TNdxDataUnitCollection::iterator itDataUnit=m_DataUnitCollection.begin(); itDataUnit!=m_DataUnitCollection.end(); itDataUnit++)
		{
			if(!itDataUnit->second->Open(wsPath.c_str())){
				ERR_TO_CONSOLE(erFileIO, itDataUnit->second->GetDataUnitName());
				return S_FALSE;
			}
		}

		//устанавливаем флаг открытия
		m_bIsBaseOpen=true;
		//устанавливаем имя текущей базы
		m_wsCurrentBase.assign(wszBaseName);	
		///меняем идентификатор текущей сессии индексной базы,
		m_uiCurrentSessionID++;
		
		return S_OK;

	SS_CATCH(L"");
	return E_FAIL;
}

void CNdxBaseManager::CloseBase(void)
{
	SS_TRY

		//ToConsole(L"CNdxBaseManager::CloseBase");
		//ToConsole(m_wsCurrentBase.c_str());

		for(TNdxDataUnitCollection::iterator itDataUnit=m_DataUnitCollection.begin(); itDataUnit!=m_DataUnitCollection.end(); itDataUnit++)
		{
			itDataUnit->second->Close();
		}
		
		//удаляем юниты из коллекции, так как при открытии все они создадуться снова
		TotalRemoveAll();

		m_bIsBaseOpen=false;
		m_wsCurrentBase.clear();

	SS_CATCH(L"");
}

HRESULT CNdxBaseManager::ReOpenBase(void)
{
	//запоминаем имя базы
	wstring wszTemp(m_wsCurrentBase);
	//закрываем базу
	CloseBase();
	//заново открываем базу
	return OpenBase(wszTemp.c_str());
}

HRESULT CNdxBaseManager::ClearBase(void)
{
	SS_TRY

		//очищаем все юниты данных
		for(TNdxDataUnitCollection::iterator itDataUnit=m_DataUnitCollection.begin(); itDataUnit!=m_DataUnitCollection.end(); itDataUnit++)
		{
			itDataUnit->second->Clear();
		}

		//заново открываем базу
		return ReOpenBase();

	SS_CATCH(L"");
	return E_FAIL;
}

HRESULT CNdxBaseManager::BackUpBase(const wchar_t* wszPath)
{
	return S_FALSE;
}

void CNdxBaseManager::GetDatabaseFilePathes(std::vector< std::pair<std::wstring, std::wstring> >* vPathes, std::wstring& sBaseName )
{
	std::wstring sFullPath;
	std::vector<std::wstring> vFolders;
	std::vector<std::wstring> vRelPathes;

	SS::Interface::Core::CommonServices::IFileFinder* pFileFinder=
		(SS::Interface::Core::CommonServices::IFileFinder*)this->GetLoadManager()->
		GetInterface(L"./CommonServices/UMCommonServices.dll", CLSID_FileFinder , IID_FileFinder);


	SS::Interface::Core::CommonServices::IWorkRegister * pWorkReg=
		(SS::Interface::Core::CommonServices::IWorkRegister*)this->GetLoadManager()->
		GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister , IID_WorkRegister);

	std::wstring sNDXPath = L"";
	sNDXPath += sBaseName;
	sNDXPath += L"\\";
	vFolders.push_back(sNDXPath);
	vFolders.push_back(L"Ndx_dbms.ini");
	vFolders.push_back(L"SS_DBMS.ini");

	std::wstring sDataBasePath = pWorkReg->GetParameter(L"PathDataBase");

	// заполнение списка путей
	for (size_t i=0; i<vFolders.size(); i++)
	{
		vRelPathes.clear();
		pFileFinder->GetFiles(sDataBasePath, vFolders[i], NULL, &vRelPathes);
		for (size_t j=0; j<vRelPathes.size(); j++)
		{
			sFullPath = sDataBasePath;
			sFullPath += vRelPathes[j];
			vPathes->push_back(std::pair<std::wstring, std::wstring>(sFullPath, vRelPathes[j]));
		}
	}
}

INdxDataUnit* CNdxBaseManager::GetNdxBaseUnit(const wchar_t* wszStorageName)
{
	TNdxDataUnitCollection::iterator itDataUnit=m_DataUnitCollection.find(wszStorageName);
	if(itDataUnit!=m_DataUnitCollection.end()){
		return itDataUnit->second;
	}

	//TO_CONSOLE(L"DataUnit not find!!!");
	//ToConsole(L"DataUnit type", (unsigned int)NdxStorageType);

	return NULL;
}

INdxDataUnit* CNdxBaseManager::GetFirstNdxBaseUnit(void)
{
	if(m_DataUnitCollection.empty()){ 
		m_itCurrentDataUnit=m_NullCollection.end();
		return NULL;
	}else{
		m_itCurrentDataUnit=m_DataUnitCollection.begin();
		return m_itCurrentDataUnit->second;
	}
}

INdxDataUnit* CNdxBaseManager::GetNextNdxBaseUnit(void)
{
	if(m_DataUnitCollection.empty() || m_itCurrentDataUnit==m_NullCollection.end()) return NULL;

	if(m_itCurrentDataUnit==(--m_DataUnitCollection.end())){
		m_itCurrentDataUnit=m_NullCollection.end();
		return NULL;
	}else{
		m_itCurrentDataUnit++;
		return m_itCurrentDataUnit->second;
	}
}

void CNdxBaseManager::AddNdxBaseUnit(const wchar_t* wszStorageName, ENdxStorageConfig StorageConfig)
{
	m_BaseStructCollection.push_back(TNdxBaseStructCollection::value_type(wszStorageName, StorageConfig));
}

void CNdxBaseManager::AddNdxBaseUnit(const wchar_t* wszStorageName, INdxDataUnit* pNdxDataUnit)
{
	if(m_DataUnitCollection.find(wszStorageName)==m_DataUnitCollection.end()){
		m_DataUnitCollection[wszStorageName]=pNdxDataUnit;
	}else{
		TO_CONSOLE(L"DataUnit already exist!!!");
		ToConsole(wszStorageName);
	}
}

void CNdxBaseManager::DeleteNdxBaseUnit(const wchar_t* wszStorageName)
{
	if(m_wsCurrentBase.empty()){
		TO_CONSOLE(L"DataBase not set!");
		return;
	}

	TNdxDataUnitCollection::iterator itDataUnit=m_DataUnitCollection.find(wszStorageName);
	if(itDataUnit!=m_DataUnitCollection.end()){
		itDataUnit->second->Close();

		wstring wszDataUnitPath=m_wsBasePath;
		wszDataUnitPath+=m_wsCurrentBase;
		wszDataUnitPath+=L"\\";
		wszDataUnitPath+=itDataUnit->second->GetDataUnitName();
		wszDataUnitPath+=itDataUnit->second->GetDataUnitExt();
		DeleteFile(wszDataUnitPath.c_str());
		itDataUnit->second->Release();
		m_DataUnitCollection.erase(itDataUnit);
		return;
	}else{
		TO_CONSOLE(L"DataUnit not find!!!");
		ToConsole(wszStorageName);
		return;
	}
}

void CNdxBaseManager::TotalRemoveAll(void)
{
	for(TNdxDataUnitCollection::iterator itDataUnit=m_DataUnitCollection.begin(); itDataUnit!=m_DataUnitCollection.end(); itDataUnit++)
	{
		itDataUnit->second->Release();
	}
	m_DataUnitCollection.clear();
}

bool CNdxBaseManager::ClearFolder(const wchar_t* wszPathFrom) 
{

	WIN32_FIND_DATA fd; 
	wchar_t wszPath[MAX_PATH]; 
	wchar_t wszFilename[MAX_PATH]; 

	SS_TRY
		wsprintf(wszPath, L"%s\\*.*", wszPathFrom); 

		HANDLE hFile = FindFirstFile(wszPath, &fd); 
		if (hFile != INVALID_HANDLE_VALUE) 
		{ 
		     
			wsprintf(wszFilename, L"%s\\%s", wszPathFrom, fd.cFileName); 
			if (IsFolder(&fd)) 
			{ 
				ClearFolder(wszFilename); 
				RemoveDirectory(wszFilename); 
			} 
			else 
			DeleteFile(fd.cFileName); 
		 
			while (FindNextFile(hFile, &fd)!=0) 
			{ 
			wsprintf(wszFilename, L"%s\\%s", wszPathFrom, fd.cFileName); 
			if(IsFolder(&fd)) 
			{ 
				ClearFolder(wszFilename); 
				RemoveDirectory(wszFilename); 
			} 
			else 
				DeleteFile(wszFilename); 
			} 
			FindClose(hFile); 
			RemoveDirectory(wszPathFrom);
		} 

	SS_CATCH(L"");
	return true;
}

bool CNdxBaseManager::IsFolder(WIN32_FIND_DATA* pFD) 
{
  if (wcscmp(pFD->cFileName,  L".")==0) return false;
  if (wcscmp(pFD->cFileName, L"..")==0) return false;
  return ( pFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )!=0?true:false;
}

//--------------------------------------------------------------------//

}
}
}
}