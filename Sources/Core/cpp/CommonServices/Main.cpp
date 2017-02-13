#include "stdafx.h"
#include "Main.h"
#include "TestRunner.h"

SET_LOAD_MANAGER_DEFINE;
SET_LOAD_MANAGER_IMPLEMENTATION;

//HMODULE - этой DLL
HMODULE g_hModuleCurrentDLL;

HRESULT DLLEXPORT CreateInstance(const GUID* pGuid, void** pBase)
{    
   if(*pGuid == CLSID_UsersManager)
   {
      DEBUG_MSG( "CreateInstance GET CLSID_UsersManager" );	
      *pBase = (SS::Interface::IBase*) new SS::CommonServices::UserTextStorage::TUsersManager;
   }
   else if(*pGuid == CLSID_Text)
   {
      DEBUG_MSG( "CreateInstance GET CLSID_Text" );	
      *pBase = (SS::Interface::IBase*) new SS::CommonServices::UserTextStorage::TText;
   }
   else if(*pGuid == CLSID_TextsManager)
   {
      DEBUG_MSG( "CreateInstance GET CLSID_TextsManager" );	
      *pBase = (SS::Interface::IBase*) new SS::CommonServices::UserTextStorage::TTextsManager;
   }
   else if(*pGuid == CLSID_XMLOperation)
   {
      DEBUG_MSG( "CreateInstance GET CLSID_XMLOperation" );	
      *pBase = (SS::Interface::IBase*) new SS::CommonServices::UserTextStorage::TXMLOperation;
   }
#ifdef _SS_UNITTESTS
   else if(*pGuid == CLSID_TestRunner)
   {
      DEBUG_MSG( "CreateInstance GET CLSID_TestRunner" );	
      *pBase = (SS::Interface::IBase*) new SS::UnitTests::CommonServices::UserTextStorage::CTestRunner;
   }
#endif //_SS_UNITTESTS
   else
   {
      DEBUG_MSG( "CreateInstance GET ERROR" );	
      *pBase=NULL;
      return E_NOINTERFACE;
   }
   return S_OK;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
   DEBUG_MSG( "DllMain " << ul_reason_for_call );

   if( ul_reason_for_call == DLL_PROCESS_ATTACH  )
   {
      APL_ASSERT(g_hModuleCurrentDLL == 0);

      g_hModuleCurrentDLL = static_cast<HMODULE>(hModule);
      DisableThreadLibraryCalls( g_hModuleCurrentDLL );
   }

   return TRUE;
}
