#pragma once

#ifdef LEXICALANALYZER_EXPORTS
	#define LEXICANALYZE_FACTORY_EXPORTS __declspec(dllexport)
#else
	#define LEXICANALYZE_FACTORY_EXPORTS __declspec(dllimport)
#endif

extern "C"
HRESULT LEXICANALYZE_FACTORY_EXPORTS CreateInstance(const GUID* pGuid, void** pBase);

SET_LOAD_MANAGER_DEFINE;
