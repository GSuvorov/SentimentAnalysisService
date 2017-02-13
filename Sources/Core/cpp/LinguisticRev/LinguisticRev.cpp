// LinguisticRev.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "LinguisticRev.h"

#include "CLinguisticRev.h"


SET_LOAD_MANAGER_IMPLEMENTATION;


HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(CLSID_LinguisticRelevance == *pGuid)
		*pBase = (SS::Interface::IBase*)new SS::LinguisticRev::CLinguisticRev();
	else
		*pBase=NULL;

	return S_OK;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

