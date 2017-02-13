
#pragma once

#ifdef COSEDI_EXPORTS
#define COSEDI_API __declspec(dllexport)
#else
#define COSEDI_API __declspec(dllimport)
#endif




extern "C" 
{
	COSEDI_API HRESULT CreateInstance(const GUID* pGuid, void** pBase);
}

SET_LOAD_MANAGER_DEFINE;