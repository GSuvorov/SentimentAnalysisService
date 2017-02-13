#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\unittestsusings.h"
#include ".\testsettings.h"
#include ".\console.h"

typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** ppBase);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{

using SS::Core::NdxSE::NdxProcessor::ToConsole;
//--------------------------------------------------------------------//

bool CUnitTestsUsings::CreateLoadManager(void)
{
	//m_pLoadManager=NULL;
	//wchar_t path[MAX_PATH];
	//::GetCurrentDirectoryW(MAX_PATH, (LPWSTR)path);
	////wcscat(path, L"\\LoadManager.dll");
	//wcscat(path, L"\\core\\loadmanager.dll");
	//m_hLoadManager=::LoadLibraryW(path);
	//
	//CREATE_INSTANCE pCreateInstance=(CREATE_INSTANCE)GetProcAddress(m_hLoadManager, "CreateInstance");


	//if(pCreateInstance==NULL){
	//	wprintf(L"LoadManager entry point not found, error %u\n", GetLastError());
	//	return 0;
	//}
	//
	//const GUID Guid=CLSID_LoadManager;
	//(*pCreateInstance)(&Guid, (void**)&m_pLoadManager);
	//
	//return m_pLoadManager?true:false;


	m_pLoadManager = NULL;
	//	объект для считывания настроек
	tsett::CSettings	settings;
	std::wstring		sWorkingDir;
	if (settings.SettingRead(sWorkingDir, tsett::wcszKey_PathWork))
	{
		ToConsole(L"Working Path = ");
		ToConsole(sWorkingDir.c_str());
	}
	else
	{
		ToConsole(L"reading working path failed!");
		return false;
	}

	//	путь к LoadManager
	std::wstring	sLoadManagerPath = 
		sWorkingDir+std::wstring(L"\\Core\\LoadManager.dll");
	////	загружаем менеджер
	m_hLoadManager = ::LoadLibraryW(sLoadManagerPath.c_str());
	if (m_hLoadManager == NULL) return false;
	CREATE_INSTANCE pCreateInstance=(CREATE_INSTANCE)GetProcAddress(m_hLoadManager, "CreateInstance");
	if(pCreateInstance == NULL) return false;
	const GUID Guid = CLSID_LoadManager;
	(*pCreateInstance)(&Guid, (void**)&m_pLoadManager);
	if (m_pLoadManager == NULL) return false;
	m_pLoadManager->SetWorkPath(sWorkingDir.c_str());
	return true;
}

void CUnitTestsUsings::DeleteLoadManager(void)
{
	if(m_pLoadManager)	m_pLoadManager->Release();
	//if (m_hLoadManager)	::FreeLibrary(m_hLoadManager);
}

const wchar_t* CUnitTestsUsings::GetUTWorkingPath(void)
{
	if(wszUTWorkingPath.empty()){
		wchar_t path[MAX_PATH];
		::GetCurrentDirectoryW(MAX_PATH, (LPWSTR)path);
		wcscat(path, L"\\UT\\");
		wszUTWorkingPath.assign(path);
	}

	return wszUTWorkingPath.c_str();
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS