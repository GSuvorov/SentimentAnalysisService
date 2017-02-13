#pragma once

#ifdef TRANSLIT_EXPORTS
	#define TRANSLIT_FACTORY_EXPORTS __declspec(dllexport)
#else
	#define TRANSLIT_FACTORY_EXPORTS __declspec(dllimport)
#endif

extern "C"
HRESULT TRANSLIT_FACTORY_EXPORTS CreateInstance(const GUID* pGuid, void** pBase);


SET_LOAD_MANAGER_DEFINE;
