// פאיכ CommonContainers.h

#ifdef COMMONCONTAINERS_EXPORTS
#define COMMONCONTAINERS_API __declspec(dllexport)
#else
#define COMMONCONTAINERS_API __declspec(dllimport)
#endif

extern "C"
HRESULT COMMONCONTAINERS_API CreateInstance(const GUID* pGuid, void** pBase);

SET_LOAD_MANAGER_DEFINE;