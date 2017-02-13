#include "stdafx.h"

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ICommonServices.h"
#include "..\ASCInterface\defines.h"

#include "BlackBox.h"
#include "TextStorage.h"
#include "QueryStorage.h"

SET_LOAD_MANAGER_IMPLEMENTATION;


HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid == CLSID_BlackBoxTextStorage)
		*pBase = (SS::Interface::IBase*) new SS::Core::BlackBox::CTextStorage();
	else if(*pGuid == CLSID_BlackBoxQueryStorage)
		*pBase = (SS::Interface::IBase*) new SS::Core::BlackBox::CQueryStorage();
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
