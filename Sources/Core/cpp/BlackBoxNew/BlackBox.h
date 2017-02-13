#pragma once

#ifdef BLACKBOX_EXPORTS
	#define BLACKBOX_FACTORY_EXPORTS __declspec(dllexport)
#else
	#define BLACKBOX_FACTORY_EXPORTS __declspec(dllimport)
#endif

extern "C"
HRESULT BLACKBOX_FACTORY_EXPORTS CreateInstance(const GUID* pGuid, void** pBase);


SET_LOAD_MANAGER_DEFINE;
