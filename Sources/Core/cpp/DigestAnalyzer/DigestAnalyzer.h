#pragma once

#ifdef DIGESTANALYZER_EXPORTS
#define DIGESTANALYZER_API __declspec(dllexport)
#else
#define DIGESTANALYZER_API __declspec(dllimport)
#endif

#include "stdafx.h"

extern "C" 
{
	DIGESTANALYZER_API HRESULT CreateInstance( const GUID* pGuid, void** pBase );
}

SET_LOAD_MANAGER_DEFINE;