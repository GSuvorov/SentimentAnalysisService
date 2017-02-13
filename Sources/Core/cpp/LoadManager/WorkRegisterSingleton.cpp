#include "stdafx.h"
#include "WorkRegisterSingleton.h"

#include <shlobj.h>
#include <shlwapi.h >
#pragma comment(lib, "shlwapi.lib")


namespace SS
{
namespace Core
{
namespace CommonServices
{

CWorkRegisterSingleton::CWorkRegisterSingleton()
{
	CreateSSDirectories();
}

CWorkRegisterSingleton::~CWorkRegisterSingleton()
{
}

std::wstring CWorkRegisterSingleton::GetParameter(std::wstring wParametrName)
{
	return Instance().GetParameter(wParametrName, GetLoadManager()->GetWorkPath());
}

void CWorkRegisterSingleton::SetWorkDir(std::wstring wPathToDir)
{
	Instance().SetWorkDir(wPathToDir);
}

HRESULT CWorkRegisterSingleton::QueryInterface(REFIID pIID, void** ppBase)
{
	*ppBase = NULL;

	if(pIID == IID_WorkRegister)
		*ppBase = (SS::Interface::Core::CommonServices::IWorkRegister*) this;
	else if(pIID == IID_Base)
		*ppBase = (SS::Interface::IBase*) this;

	return (!*ppBase) ? S_FALSE : S_OK;
} 

ULONG CWorkRegisterSingleton::Release()
{
	delete this;
	return NULL;
}

void CWorkRegisterSingleton::CreateSSDirectories()
{
	//try
	//{
	//	WCHAR szAppData[MAX_PATH];
	//	if(SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, szAppData)))
	//	{
	//		PathAppend(szAppData, L"SS");
	//		CreateDirectoryW(szAppData, NULL);

	//		PathAppend(szAppData, L"SS");
	//		CreateDirectoryW(szAppData, NULL);

	//		CreateSSDirectory(szAppData, L"ErrorsLogs");
	//		CreateSSDirectory(szAppData, L"ss_temp");
	//		CreateSSDirectory(szAppData, L"TemporaryFiles");
	//	}
	//}
	//catch(...)
	//{
	//}
}

bool CWorkRegisterSingleton::CreateSSDirectory(LPCWSTR szPath, LPCWSTR szDir)
{
	WCHAR szDirPath[MAX_PATH];
	wcscpy(szDirPath, szPath);
	if(!PathAppend(szDirPath, szDir))
		return false;
	if(!CreateDirectoryW(szDirPath, NULL))
		return false;
	return true;
}

}
}
}
