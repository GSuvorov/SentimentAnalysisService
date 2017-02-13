// Dictionaries.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "Main.h"
#include "TestRunner.h"
#include "Dictionary.h"
#include "MinimalHypothesisTree.h"
//#include "..\ASSInterface\IDictionary.h"
#include "NumberDictionary\NumberCombinatory.h"

SET_LOAD_MANAGER_DEFINE;
SET_LOAD_MANAGER_IMPLEMENTATION;

HRESULT DLLEXPORT CreateInstance(const GUID* pGuid, void** pBase)
{
	if(*pGuid == CLSID_IDictionary)
	{
		DEBUG_MSG_LOG1( "CreateInstance GET CLSID_IDictionary" );	
		*pBase = (SS::Interface::IBase*)new SS::Dictionary::CDictionary();
	}
   else if(*pGuid == CLSID_INumberCombinatory)
   {
		DEBUG_MSG_LOG1( "CreateInstance GET CLSID_INumberCombinatory" );	
		*pBase = (SS::Interface::IBase*)
			new SS::Dictionary::NumberCombinatory::CNumberCombinatory;

   }
   else if(*pGuid == CLSID_IMinimalHypothesisTreeBuilder)
   {
      DEBUG_MSG_LOG1( "CreateInstance GET CLSID_IMinimalHypothesisTreeBuilder" );	
      *pBase = (SS::Interface::IBase*)
         new SS::Dictionary::MinimalHypothesisTree::CMinimalHypothesisTreeBuilder;

   }
#ifdef _SS_UNITTESTS
	else if(*pGuid == CLSID_TestRunner)
	{
		/*-------------------
		DEBUG_MSG_LOG1( "CreateInstance GET CLSID_TestRunner" );	
		*pBase = (SS::Interface::IBase*)new SS::UnitTests::Dictionary::CTestRunner();
		*/
	}
#endif //_SS_UNITTESTS
	else
	{
		DEBUG_MSG_LOG1( "CreateInstance GET ERROR" );	
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
   using namespace SS::Dictionary::InitData;

   DEBUG_MSG_LOG1( "DllMain " << ul_reason_for_call );

   if( ul_reason_for_call == DLL_PROCESS_ATTACH  )
   {
      APL_ASSERT(g_hModuleCurrentDLL == 0);

      g_hModuleCurrentDLL = static_cast<HMODULE>(hModule);
      DisableThreadLibraryCalls( g_hModuleCurrentDLL );
   }
    
   return TRUE;
}

