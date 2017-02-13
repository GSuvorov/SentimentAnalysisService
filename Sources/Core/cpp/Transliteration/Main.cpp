#define __TRANSLITERATION_FRENCH__
#include "Main.h"
#include "TransliterationFrench.h"

SET_LOAD_MANAGER_IMPLEMENTATION;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

HRESULT CreateInstance(const GUID *pGuid, void **ppBase )
{	
    if (*pGuid == CLSID_TransliterationFrench)
	{
        *ppBase = (SS::Interface::IBase*) new SS::Transliteration::CTransliterationFrench();
	}
	else
	{
		*ppBase = NULL;
	}
	return S_OK;
}