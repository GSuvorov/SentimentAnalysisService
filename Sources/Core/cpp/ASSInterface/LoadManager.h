#pragma once

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\ICommonServices.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\IBaseManager.h"
using namespace SS::Interface::Core::ResourceManagers;

#include <tchar.h>


namespace SS
{
namespace Helpers
{

class CLoadManager
{
public:
	CLoadManager() :
			m_pLoadManager(NULL)
	{
	}

	~CLoadManager()
	{
	}

public:
	typedef HRESULT (*TCreateInstance)(const GUID* pGuid, void** pBase);

	bool Create()
	{
		if(m_pLoadManager)
			return true;

		try
		{
			TCHAR szPath[MAX_PATH];
			::GetCurrentDirectory (MAX_PATH, szPath);
			_tcscat(szPath, _T("\\Core\\LoadManager.dll"));

			HMODULE hLoadManager = ::LoadLibrary(szPath);

			TCreateInstance pfnCreateInstance = (TCreateInstance) GetProcAddress(hLoadManager, "CreateInstance");
			if(!pfnCreateInstance)
				return false;

			const GUID Guid = CLSID_LoadManager;
			(*pfnCreateInstance)(&Guid, (void**) &m_pLoadManager);

			return m_pLoadManager != NULL;
		}
		catch(...)
		{
		}

		return false;
	}

	void RunDictionariesMagicCode()
	{
		SS::Interface::Core::CommonServices::IWorkRegister* pWorkRegister =
			((SS::Interface::Core::CommonServices::IWorkRegister*) 
			m_pLoadManager->GetInterface(L"./core/LoadManager.dll",CLSID_WorkRegister,IID_WorkRegister));

		const std::wstring wszDataBaseName = L"NewWordForms1";

		std::wstring m_wsDataBasePath = pWorkRegister->GetParameter(L"PathDataBase");
		std::wstring m_wsDataBasePathRemove = m_wsDataBasePath;
		m_wsDataBasePathRemove += L"IndexedTexts\\ndx\\";
		m_wsDataBasePathRemove += wszDataBaseName;
		m_wsDataBasePathRemove += L"\\";

		SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager = 
			(SS::Interface::Core::ResourceManagers::IBaseManager*)
			m_pLoadManager->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);

		if(!pBaseManager)
			SS_THROW(L"!m_pBaseManager");

		pBaseManager->SetBasePath(m_wsDataBasePath.c_str());

		pBaseManager->OpenBase(wszDataBaseName.c_str());
		pBaseManager->ClearBase();
	}

	ILoadManager* operator*()
	{
		return m_pLoadManager;
	}

	ILoadManager* operator->()
	{
		return m_pLoadManager;
	}

	void Release()
	{
		if(m_pLoadManager)
			m_pLoadManager->Release();
		m_pLoadManager = NULL;
	}

private:
	ILoadManager* m_pLoadManager;
};

}
}
