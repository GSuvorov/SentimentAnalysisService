// NdxQuery.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../ASCInterface/defines.h"
#include "../ASDInterface/INDXQuery.h"
#include "./NDXQuery.h"
#include "./TestRunner.h"
#include "./ParametrizedQuery.h"

SET_LOAD_MANAGER_IMPLEMENTATION;

HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid == CLSID_ParametrizedQuery)
		*pBase = (SS::Interface::IBase*)new SS::Core::NdxSE::NdxQuery::CParametrizedQuery();
	else if(*pGuid == CLSID_TestRunner)
		*pBase=(SS::Interface::IBase*)new SS::UnitTests::NdxSE::NdxQuery::CTestRunner();
	else
		*pBase=NULL;

	return S_OK;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
/*
для отладки
#define DLL_PROCESS_ATTACH   1    
#define DLL_THREAD_ATTACH    2    
#define DLL_THREAD_DETACH    3    
#define DLL_PROCESS_DETACH   0    
#define DLL_PROCESS_VERIFIER 4    
*/

    return TRUE;
}

