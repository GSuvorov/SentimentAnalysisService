#pragma once

#define DLLEXPORT __declspec(dllexport)

extern "C"
{
	HRESULT DLLEXPORT CreateInstance(const GUID* pGuid, void** pBase);
}

SET_LOAD_MANAGER_DEFINE;

