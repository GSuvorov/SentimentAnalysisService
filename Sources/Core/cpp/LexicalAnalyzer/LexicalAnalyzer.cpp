// LexicalAnalyzer.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ICommonServices.h"
#include "..\ASCInterface\defines.h"
#include "LexicalAnalyzer.h"

SET_LOAD_MANAGER_IMPLEMENTATION;

#include "Lexica.h"
using namespace SS::LexicalAnalyzer;


HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid == CLSID_LexicaPrimary)
		*pBase = (SS::Interface::IBase*) new CLexicaPrimary();
	else if(*pGuid == CLSID_Lexica)
		*pBase = (SS::Interface::IBase*)(SS::Interface::Core::LexicalAnalyze::ILexica*) new CLexica();
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
