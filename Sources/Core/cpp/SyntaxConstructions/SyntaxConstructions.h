#pragma once

#ifdef SYNTAXCONSTRUCTIONS_EXPORTS
#define SYNTAXCONSTRUCTIONS_API __declspec(dllexport)
#else
#define SYNTAXCONSTRUCTIONS_API __declspec(dllimport)
#endif




extern "C" 
{
	SYNTAXCONSTRUCTIONS_API HRESULT CreateInstance(const GUID* pGuid, void** pBase);
}

SET_LOAD_MANAGER_DEFINE;