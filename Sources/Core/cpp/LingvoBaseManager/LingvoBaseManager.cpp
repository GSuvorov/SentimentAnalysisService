// LingvoBaseManager.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "LingvoBaseManager.h"

#include ".\TestRunner.h"

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ICommonServices.h"
#include "CLingvoBaseManager.h"
#include "ConnectionManager.h"


SET_LOAD_MANAGER_IMPLEMENTATION;

HRESULT CreateInstance(const GUID* clsidEntity, void** ppBase)
{

	if (*clsidEntity == CLSID_LinguoBaseManager)
		*ppBase = (SS::Interface::IBase*) (new SS::Core::ResourceManagers::CLingvoBaseManager());
	//else
	//	if (*clsidEntity == )
	//		*ppBase = (SS::Interface::IBase*) (new SS::Core::ResourceManagers::CLingvoBaseManager());	
	else if(*clsidEntity==CLSID_TestRunner)
		*ppBase=(SS::Interface::IBase*)new SS::UnitTests::ResourceManagers::CTestRunner();
   else if(*clsidEntity==CLSID_ConnectionManager)
      *ppBase=(SS::Interface::IBase*)new SS::Core::ResourceManagers::CConnectionManager();
	else
		*ppBase=NULL;

	return S_OK;
};
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
 //  DEBUG_MSG( "DllMain " << ul_reason_for_call );

   return TRUE;
}

