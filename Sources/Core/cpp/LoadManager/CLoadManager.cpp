#include "stdafx.h"
#include "CLoadManager.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

	CLoadManager::CLoadManager() : m_oModulesManager (this)
	{
		wchar_t szReturnedString[512];
		szReturnedString[0] = 0;
	
		GetCurrentDirectory(512, szReturnedString);

		wcscat(szReturnedString, L"\\");
		
		m_wszPath = new wchar_t[512];
		wcscpy((wchar_t*)m_wszPath, szReturnedString);
	};

	CLoadManager::~CLoadManager()
	{
		if (m_wszPath)
			delete []m_wszPath;
	};

	void CLoadManager::SetWorkPath(const wchar_t* wszPath)
	{
		wcscpy((wchar_t*)m_wszPath, wszPath);
		wcscat((wchar_t*)m_wszPath, L"\\");
	}

	const wchar_t* CLoadManager::GetWorkPath()
	{
		return m_wszPath;
	}
	void* CLoadManager::GetInterface(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface)
	{
		void* pBase = NULL;
		m_oModulesManager.CreateAndRegister(CoreLibrary, guidEntity, iidInterface, &pBase, m_wszPath);
		return pBase;
	};

	void* CLoadManager::GetInterface(const wchar_t* wszPath, const GUID& guidEntity, const GUID& iidInterface)
	{
		void* pBase = NULL;
		m_oModulesManager.CreateAndRegister(wszPath, guidEntity, iidInterface, &pBase, m_wszPath);
		return pBase;
	}

	void CLoadManager::Create(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase)
	{
		m_oModulesManager.Create(CoreLibrary, guidEntity, iidInterface, ptssBase, m_wszPath);
	};

	void CLoadManager::Create(const wchar_t* wszPath, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase)
	{
		m_oModulesManager.Create(wszPath, guidEntity, iidInterface, ptssBase, m_wszPath);
	};



	HRESULT CLoadManager::QueryInterface(REFIID pIID, void** ppBase)
	{
		*ppBase = NULL;

		if(pIID == IID_LoadManager)
			*ppBase = (SS::Interface::Core::ResourceManagers::ILoadManager*) this;
		else if(pIID == IID_Base)
			*ppBase = (SS::Interface::IBase*) this;
		else
			*ppBase = NULL;
		
		return (*ppBase == NULL) ? S_FALSE : S_OK;
	}; 

	ULONG CLoadManager::Release()
	{
	
		delete this;
		return 0;
	};
}
}
}