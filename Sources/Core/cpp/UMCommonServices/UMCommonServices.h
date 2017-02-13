#pragma once

#ifdef UMCommonServices_EXPORTS
	#define CommonServices_CreateInstance_Export __declspec(dllexport)
#else
	#define CommonServices_CreateInstance_Export __declspec(dllimport)
#endif

extern "C"
HRESULT CommonServices_CreateInstance_Export CreateInstance(const GUID* clsidEntity, void** ppBase);

SET_LOAD_MANAGER_DEFINE;