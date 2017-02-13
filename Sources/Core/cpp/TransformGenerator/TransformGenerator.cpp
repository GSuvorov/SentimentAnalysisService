// TransformGenerator.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"

#include ".\TransformGenerator.h"

#include ".\TransformModule.h"

using namespace SS::TransformGenerator;


SET_LOAD_MANAGER_IMPLEMENTATION;


HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid == CLSID_RevModule)
		*pBase = (SS::Interface::IBase*) new CTransformModule();
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
