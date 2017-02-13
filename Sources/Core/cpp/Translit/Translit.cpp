#include "stdafx.h"
#include "Translit.h"

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ICommonServices.h"
#include "..\ASCInterface\defines.h"

#include "Convertor.h"

SET_LOAD_MANAGER_IMPLEMENTATION;


HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid == CLSID_Translit)
		*pBase = (SS::Interface::IBase*) new CConvertor;
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
