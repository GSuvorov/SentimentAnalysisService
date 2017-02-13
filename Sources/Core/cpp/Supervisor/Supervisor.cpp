#include "stdafx.h"
#include "Supervisor.h"

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ICommonServices.h"
#include "..\ASCInterface\defines.h"

#include "SupervisorModule.h"

SET_LOAD_MANAGER_IMPLEMENTATION;


HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid == CLSID_Supervisor)
		*pBase = (SS::Interface::IBase*) new CSupervisorModule;
	else
		*pBase = NULL;

	return S_OK;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
