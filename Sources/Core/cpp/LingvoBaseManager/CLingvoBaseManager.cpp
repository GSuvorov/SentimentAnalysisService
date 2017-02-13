#include "stdafx.h"
#include "CLingvoBaseManager.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

CLingvoBaseManager::CLingvoBaseManager()
{
}

void CLingvoBaseManager::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY
	{
		SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
		m_oDBMSManager.SetLoadManager(pLoadManager);
		m_oSQLManager.SetLoadManager(pLoadManager);
	}
	SS_CATCH(L"")

}

void CLingvoBaseManager::Close(const wchar_t* BaseType, const wchar_t* BaseGroup)
{
	SS_TRY
	{
		if(!wcscmp(c_szDBMS,BaseGroup))
		{
			m_oDBMSManager.Close(BaseType);		
		}else if(!wcscmp(c_szSql,BaseGroup))
		{
			m_oSQLManager.Close(BaseType);
		};
	}
	SS_CATCH(L"")
}

bool CLingvoBaseManager::Open(const wchar_t* BaseType, const wchar_t* BaseGroup)
{
	SS_TRY
	{
		if(!wcscmp(c_szDBMS,BaseGroup))
		{
			return m_oDBMSManager.Open(BaseType);		
		}else if(!wcscmp(c_szSql,BaseGroup))
		{
			return m_oSQLManager.Open(BaseType);
		};
	}
	SS_CATCH(L"")
	return false;		
}

void CLingvoBaseManager::Close(const wchar_t* BaseType)
{
	SS_TRY
	{
		m_oSQLManager.Close(BaseType);
		m_oDBMSManager.Close(BaseType);
	}
	SS_CATCH(L"")
}

bool CLingvoBaseManager::Open(const wchar_t* BaseType)
{
	SS_TRY
	{
		return (m_oSQLManager.Open(BaseType) && m_oDBMSManager.Open(BaseType));		
	}
	SS_CATCH(L"")
	return false;
}

void CLingvoBaseManager::Close()
{
	SS_TRY
	{
		m_oSQLManager.Close();
		m_oDBMSManager.Close();		
	}
	SS_CATCH(L"")
}

bool CLingvoBaseManager::Open()
{
	SS_TRY
	{
		return (m_oSQLManager.Open() && m_oDBMSManager.Open());		
	}
	SS_CATCH(L"")
	return false;
}

const wchar_t* CLingvoBaseManager::GetNameActiveStorage()
{
	SS_TRY
	{
		return m_oDBMSManager.GetActiveStorage().c_str();		
	}
	SS_CATCH(L"")
	return (NULL);
}

bool CLingvoBaseManager::OpenStorage(const wchar_t* StorageName)
{	
	SS_TRY
	{
		m_oSQLManager.OpenStorage(StorageName);
		m_oDBMSManager.OpenStorage(StorageName);
	}
	SS_CATCH(L"")
	return true;
}

void CLingvoBaseManager::Register(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection)
{
	SS_TRY
	{
		m_oSQLManager.Register(BaseType, pSubConnection);
	}
	SS_CATCH(L"")
}

void CLingvoBaseManager::UnRegister(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection)
{
	SS_TRY
	{
		m_oSQLManager.UnRegister(BaseType, pSubConnection);		
	}
	SS_CATCH(L"")
}

void CLingvoBaseManager::UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection)
{
	SS_TRY
	{
		m_oDBMSManager.UnRegisterDeleteAndClose(BaseType, ptssConnection);
	}
	SS_CATCH(L"")
}

void CLingvoBaseManager::CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection)
{
	SS_TRY
	{
		m_oSQLManager.CreateOpenAndRegister(BaseType, ptssConnection);
	}
	SS_CATCH(L"")
}

void CLingvoBaseManager::CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection)
{
	SS_TRY
	{
		m_oDBMSManager.CreateOpenAndRegister(BaseType, ptssConnection);
	}
	SS_CATCH(L"")
}

void CLingvoBaseManager::UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection)
{
	SS_TRY
	{
		m_oSQLManager.UnRegisterDeleteAndClose(BaseType, ptssConnection);		
	}
	SS_CATCH(L"")
}

HRESULT CLingvoBaseManager::QueryInterface(REFIID pIID, void** ppBase)
{
	*ppBase = NULL;

	if(pIID == IID_LingvoBaseManager)
		*ppBase = (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*) this;
	else if(pIID == IID_BaseManager)
		*ppBase = (SS::Interface::Core::ResourceManagers::IBaseManager*) this;
	else if(pIID == IID_Base)
		*ppBase = (SS::Interface::IBase*) this;

	return (!*ppBase) ? S_FALSE : S_OK;
} 

ULONG CLingvoBaseManager::Release()
{
	delete this;
	return NULL;
}

}
}
}

