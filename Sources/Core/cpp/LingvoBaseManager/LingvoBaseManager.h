#pragma once


extern "C"
HRESULT __declspec(dllexport) CreateInstance(const GUID* clsidEntity, void** ppBase);

SET_LOAD_MANAGER_DEFINE;