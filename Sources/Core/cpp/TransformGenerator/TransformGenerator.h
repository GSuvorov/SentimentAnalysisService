#pragma once


#ifdef TRANSFORMGENERATOR_EXPORTS
#define FACTORY_EXPORTS __declspec(dllexport)
#else
#define FACTORY_EXPORTS __declspec(dllimport)
#endif

extern "C"
HRESULT FACTORY_EXPORTS CreateInstance(const GUID* pGuid, void** pBase);

SET_LOAD_MANAGER_DEFINE;