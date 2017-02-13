// NdxBaseManager.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\ASCInterface\defines.h"
#include "./ndxbasemanager.h"
#include "./base_manager.h"
#include "./TestRunner.h"

SET_LOAD_MANAGER_IMPLEMENTATION;

HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid==CLSID_NdxBaseManager)
		*pBase=(SS::Interface::IBase*)new SS::Core::NdxSE::NdxBaseManager::CNdxBaseManager();
	else if(*pGuid==CLSID_TestRunner)
		*pBase=(SS::Interface::IBase*)new SS::UnitTests::NdxSE::NdxBaseManager::CTestRunner();
	else
		*pBase=NULL;

	return S_OK;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

