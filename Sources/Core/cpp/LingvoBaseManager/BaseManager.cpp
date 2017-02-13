#include "StdAfx.h"
#include ".\basemanager.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{


CBaseManager::CBaseManager(void)
{
}

CBaseManager::~CBaseManager(void)
{
}

void CBaseManager::SetBasePath(const wchar_t* wszPath)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);

		m_wszPath = wszPath;
		m_oDBMSManager.SetITDataBasePath(wszPath);
	}
	SS_CATCH(L"")
}
HRESULT CBaseManager::OpenBase(const wchar_t* wszBaseName)
{
	SS_TRY
	{

		SL_OUT_DEBUG_STR (__WFUNCTION__);

		m_wszBaseName = wszBaseName;


		m_oDBMSManager.OpenStorage(m_wszBaseName.c_str());


		((SS::Interface::Core::ResourceManagers::IConnectionManager*)
			GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_ConnectionManager, IID_ConnectionManager))->Open();


		return S_OK;
	}
	SS_CATCH(L"")
}
HRESULT CBaseManager::ClearBase(void)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);

		CloseBase();

		m_oDBMSManager.Clear(c_szNdxBase);

		OpenBase(m_wszBaseName.c_str());

		return S_OK;
	}
	SS_CATCH(L"")
}
void CBaseManager::CloseBase(void)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);

		((SS::Interface::Core::ResourceManagers::IConnectionManager*)
			GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_ConnectionManager, IID_ConnectionManager))->Close();

		m_oDBMSManager.Close(c_szNdxBase);
	}
	SS_CATCH(L"")
}
HRESULT CBaseManager::BackUpBase(const wchar_t* wszPath)
{
	SS_TRY
	{
		return S_FALSE;
	}
	SS_CATCH(L"")
}

void CBaseManager::GetDatabaseFilePathes(std::vector< std::pair<std::wstring, std::wstring> >* pvPathes, std::wstring& sBaseName)
{
	SS_TRY
	{
		m_oDBMSManager.GetDatabaseFilePathes(pvPathes, sBaseName);
	}
	SS_CATCH(L"")
}



///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
HRESULT CBaseManager::QueryInterface(REFIID pIID, void** ppBase)
{
	SS_TRY
	{
		*ppBase = NULL;

		if(pIID == IID_BaseManager)
			*ppBase = (SS::Interface::Core::ResourceManagers::IBaseManager*) this;
		else if(pIID == IID_Base)
			*ppBase = (SS::Interface::IBase*) this;

		return (!*ppBase) ? S_FALSE : S_OK;
	}
	SS_CATCH(L"")

}
///освобождение
ULONG CBaseManager::Release()
{
	SS_TRY
	{
		delete this;
		return NULL;
	}
	SS_CATCH(L"")
}

}
}
}