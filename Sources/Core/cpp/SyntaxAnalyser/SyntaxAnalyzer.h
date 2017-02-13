
#ifdef SYNTAXANALYZER_EXPORTS
#define SYNTAXANALYZER_API __declspec(dllexport)
#else
#define SYNTAXANALYZER_API __declspec(dllimport)
#endif

#include "TestRunner.h"

extern "C" 
{
	SYNTAXANALYZER_API HRESULT CreateInstance( const GUID* pGuid, void** pBase );
}

SET_LOAD_MANAGER_DEFINE;