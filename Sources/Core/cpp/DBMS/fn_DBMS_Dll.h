#ifdef DBMS_EXPORTS
	#define DBMS_API __declspec(dllexport)
#else
	#define DBMS_API __declspec(dllimport)
#endif

///	Создание объекта DBMS
extern "C" HRESULT DBMS_API CreateInstance(const GUID * pGuid, void ** pBase);

SET_LOAD_MANAGER_DEFINE;
