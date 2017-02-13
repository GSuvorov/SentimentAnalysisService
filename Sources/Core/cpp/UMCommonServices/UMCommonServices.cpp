// CommonServices.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\ASCInterface\defines.h"
#include "UMCommonServices.h"
#include "PerformanceMeasure.h"
#include "exception.h"
#include "CLogSingleton.h"
#include "TestRunner.h"
#include "FileFinder.h"


SET_LOAD_MANAGER_IMPLEMENTATION;


HRESULT CreateInstance(const GUID* clsidEntity, void** ppBase)
{

	if(*clsidEntity == CLSID_Exception)
		*ppBase = (SS::Interface::IBase*) new SS::Core::CommonServices::CException();
	if(*clsidEntity == CLSID_Log)
		*ppBase = (SS::Interface::IBase*) new SS::Core::CommonServices::CLogSingleton();
	if(*clsidEntity == CLSID_PerformanceMeasure)
		*ppBase = (SS::Interface::IBase*) new SS::Core::CommonServices::CPerformanceMeasure();
	else if(*clsidEntity == CLSID_TestRunner)
		*ppBase=(SS::Interface::IBase*)new SS::UnitTests::Core::ResourceManagers::LoadManager::CTestRunner();
	else if(*clsidEntity == CLSID_FileFinder)
		*ppBase=(SS::Interface::IBase*)new SS::Core::CommonServices::CFileFinder();


	return S_OK;
};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

