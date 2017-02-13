// DigestAnalyzer.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "DigestAnalyzer.h"
#include "BaseDigestAnalyzer.h"

SET_LOAD_MANAGER_IMPLEMENTATION;

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	return TRUE;
}

HRESULT CreateInstance( const GUID *pGuid, void **ppBase )
{	
	if (*pGuid == CLSID_DigestAnalyzer)
	{
		*ppBase = (SS::Interface::IBase*) new SS::Digest::CBaseDigestAnalyzer();
	}
	else if (*pGuid == CLSID_DigestAnalyzer_RusNQ)
	{
		*ppBase = (SS::Interface::IBase*) new SS::Digest::CBaseDigestAnalyzer();
	}
	else
	{
		*ppBase = NULL;
	}
	return S_OK;
};

