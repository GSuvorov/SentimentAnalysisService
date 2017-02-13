// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SEMANTICANALYZER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SEMANTICANALYZER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SEMANTICANALYZER_EXPORTS
#define SEMANTICANALYZER_API __declspec(dllexport)
#else
#define SEMANTICANALYZER_API __declspec(dllimport)
#endif





extern "C" 
{
	SEMANTICANALYZER_API HRESULT CreateInstance(const GUID* pGuid, void** pBase);
}


SET_LOAD_MANAGER_DEFINE;