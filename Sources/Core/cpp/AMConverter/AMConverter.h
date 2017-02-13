#pragma once

#ifdef AMCONVERTER_EXPORTS
	#define AMCONVERTER_FACTORY_EXPORTS __declspec(dllexport)
#else
	#define AMCONVERTER_FACTORY_EXPORTS __declspec(dllimport)
#endif

extern "C"
HRESULT AMCONVERTER_FACTORY_EXPORTS CreateInstance(const GUID* pGuid, void** pBase);

SET_LOAD_MANAGER_DEFINE;