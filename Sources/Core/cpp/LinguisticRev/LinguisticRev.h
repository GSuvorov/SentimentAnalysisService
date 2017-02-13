#pragma once

#ifdef LINGUISTICREV_EXPORTS
	#define LINGUISTICREV_API __declspec(dllexport)
#else
	#define LINGUISTICREV_API __declspec(dllimport)
#endif

extern "C"
HRESULT LINGUISTICREV_API CreateInstance(const GUID* pGuid, void** pBase);

SET_LOAD_MANAGER_DEFINE;




