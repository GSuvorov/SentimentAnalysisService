//CommonContainers.cpp 
#include "stdafx.h"
#include "CommonContainers.h"
#include "ContainersFactory.h"

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

///Фабрика классов
static SS::Core::CContainersFactory g_oContainersFactory;

HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid == CLSID_IndexationResult)
	{
		*pBase = (SS::Interface::IBase*)g_oContainersFactory.CreateIndexationResult();
	}
	else if(*pGuid == CLSID_SearchResult)
	{
		*pBase = (SS::Interface::IBase*)g_oContainersFactory.CreateSearchResult();
	}
	else if(*pGuid == CLSID_QueryResult)
	{
		*pBase = (SS::Interface::IBase*)g_oContainersFactory.CreateQueryResult();
	}
	else if(*pGuid == CLSID_TextFeature)
	{
		*pBase = (SS::Interface::IBase*)g_oContainersFactory.CreateTextFeature();
	}
#ifdef _SS_UNITTESTS
	else if(*pGuid==CLSID_TestRunner)
	{
		*pBase = (SS::Interface::IBase*)new SS::UnitTests::CommonContainers::CTestRunner();
	}
#endif
	else
	{
		*pBase=NULL;
	}
	return S_OK;
}