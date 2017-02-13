#pragma once

#ifdef LOADMANAGER_EXPORTS
	#define LoadManager_CreateInstance_Export __declspec(dllexport)
#else
	#define LoadManager_CreateInstance_Export __declspec(dllimport)
#endif



extern "C"
HRESULT LoadManager_CreateInstance_Export CreateInstance(const GUID* clsidEntity, void** ppBase);

SET_LOAD_MANAGER_DEFINE;