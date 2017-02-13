// LoadManager.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ICommonServices.h"
#include "..\ASCInterface\defines.h"
#include "WorkRegisterSingleton.h"
#include "LoadManager.h"
#include "CLoadManager.h"
#include "TestRunner.h"


SET_LOAD_MANAGER_IMPLEMENTATION;

HRESULT CreateInstance(const GUID* clsidEntity, void** ppBase)
{
	if (*clsidEntity == CLSID_LoadManager)
		*ppBase = (SS::Interface::Core::ResourceManagers::ILoadManager*) (new SS::Core::ResourceManagers::CLoadManager());
	else if (*clsidEntity == CLSID_LoadManager_IBase)
		*ppBase = (SS::Interface::IBase*) (new SS::Core::ResourceManagers::CLoadManager());
	else if(*clsidEntity == CLSID_WorkRegister)
		*ppBase = (SS::Interface::IBase*) new SS::Core::CommonServices::CWorkRegisterSingleton();
	else if(*clsidEntity==CLSID_TestRunner)
		*ppBase=(SS::Interface::IBase*)new SS::UnitTests::Core::ResourceManagers::LoadManager::CTestRunner();

	return S_OK;
};



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

