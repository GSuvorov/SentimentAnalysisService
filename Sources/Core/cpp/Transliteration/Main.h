#pragma once

#ifdef __TRANSLITERATION_FRENCH__
#define TRANSLITERATION_FRENCH_FUNC __declspec(dllexport)
#else
#define TRANSLITERATION_FRENCH_FUNC __declspec(dllimport)
#endif

#include "stdafx.h"

extern "C" 
{
	TRANSLITERATION_FRENCH_FUNC HRESULT CreateInstance( const GUID* pGuid, void** pBase );
}

SET_LOAD_MANAGER_DEFINE;