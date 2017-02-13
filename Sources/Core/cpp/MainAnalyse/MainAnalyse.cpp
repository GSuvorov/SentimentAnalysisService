// MainAnalyse.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MainAnalyse.h"
#include "AnalyseParams.h"
#include "TestRunner.h"

#include ".\linguisticprocessor.h"

SET_LOAD_MANAGER_IMPLEMENTATION;



HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid==CLSID_LinguisticProcessor)
		*pBase = (SS::Interface::IBase*)new SS::LinguisticProcessor::CLinguisticProcessor();
	else if (*pGuid==CLSID_AnalyseParams)
		*pBase = (SS::Interface::IBase*)new SS::MainAnalyse::CAnalyseParams();
#ifdef _SS_UNITTESTS
	else if(*pGuid==CLSID_TestRunner)
		*pBase = (SS::Interface::IBase*)new SS::UnitTests::MainAnalyse::CTestRunner();
#endif //_SS_UNITTESTS
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

