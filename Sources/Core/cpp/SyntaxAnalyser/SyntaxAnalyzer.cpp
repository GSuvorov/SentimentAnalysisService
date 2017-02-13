// SyntaxAnalyzer.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SyntaxAnalyzer.h"
#include "ExpertController.h"
#include "BaseSyntaxAnalyzer.h"
#include "../ASCInterface/IInterface.h"

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

HRESULT CreateInstance( const GUID *pGuid, void **ppBase )
{
	if (*pGuid == CLSID_ExpertController)
	{
		*ppBase = (SS::Interface::IBase*) new SS::Syntax::ExpertModel::CExpertController(SS::Syntax::ExpertModel::sbStandart);
	}
	else if (*pGuid == CLSID_ExpertController_RusNQ)
	{
		*ppBase = (SS::Interface::IBase*) new SS::Syntax::ExpertModel::CExpertController(SS::Syntax::ExpertModel::sbRussianNQ);
	}
	else if (*pGuid == CLSID_SyntaxAnalyzer)
	{
		*ppBase = (SS::Interface::IBase*) new SS::Syntax::CBaseSyntaxAnalyzer(SS::Syntax::ExpertModel::sbStandart);
	}
	else if (*pGuid == CLSID_SyntaxAnalyzer_RusNQ)
	{
		*ppBase = (SS::Interface::IBase*) new SS::Syntax::CBaseSyntaxAnalyzer(SS::Syntax::ExpertModel::sbRussianNQ);
	}
#ifdef _SS_UNITTESTS
	else if (*pGuid == CLSID_TestRunner)
	{
		*ppBase = static_cast<SS::Interface::IBase*> (new SS::UnitTests::Syntax::CTestRunner());
	}
#endif	
	else
	{
		*ppBase = NULL;
	}
	return S_OK;
};

