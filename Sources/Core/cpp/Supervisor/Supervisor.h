#pragma once

#ifdef SUPERVISOR_EXPORTS
	#define SUPERVISOR_FACTORY_EXPORTS __declspec(dllexport)
#else
	#define SUPERVISOR_FACTORY_EXPORTS __declspec(dllimport)
#endif

extern "C"
HRESULT SUPERVISOR_FACTORY_EXPORTS CreateInstance(const GUID* pGuid, void** pBase);


SET_LOAD_MANAGER_DEFINE;
