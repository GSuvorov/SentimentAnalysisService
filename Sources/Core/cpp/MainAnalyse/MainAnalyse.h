#pragma once

#ifdef MAINANALYSE_EXPORTS
	#define MAINANALYSE_FACTORY_EXPORTS __declspec(dllexport)
#else
	#define MAINANALYSE_FACTORY_EXPORTS __declspec(dllimport)
#endif

extern "C"
HRESULT MAINANALYSE_FACTORY_EXPORTS CreateInstance(const GUID* pGuid, void** pBase);

SET_LOAD_MANAGER_DEFINE;