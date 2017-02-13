// SemanticAnalyzer.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SemanticAnalyzer.h"

#include "..\ASSInterface\ISemanticAnalyze.h"

#include ".\semanticanalyze.h"

SET_LOAD_MANAGER_IMPLEMENTATION;


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

HRESULT CreateInstance(const GUID *pGuid, void **ppBase)
{
	if(*pGuid==CLSID_SemanticAnalyzer)
	{
		*ppBase=(SS::Interface::IBase*)new SS::Core::CSemanticAnalyze();
	}else
	{
		*ppBase = NULL;
	}

	return S_OK;
}
