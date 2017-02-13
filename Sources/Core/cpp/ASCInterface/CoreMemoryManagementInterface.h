#pragma once

/// Содержит переопределение глобальных операторов и функций работы с динамической памятью.
/// Поддержан режим автоматической загрузки CoreMemoryManager.dll.
/// Если CoreMemoryManager.dll не будет найдена то используется стандартный менеджер памяти.

#include <tchar.h>

//#define FX_STATIC
#define STATIC static
//#define STATIC __declspec(selectany)
//#define STATIC
#ifdef _DEBUG
//#define assert(exp)
#endif

// defines debug messages
//#define DBG_MSGS

// CoreMemoryManager.dll auto load (режим автоматической загрузки)
#define DLL_AUTO_LOAD

// возможность отключения функций 
//#define USE_ENABLE_DISABLE

// CMM_DEBUG
//#define CMM_DEBUG

#include "CoreMemoryManagementBaseInterface.h"

/// Direct function pointers support. Not implemented, reserved
#define DIRECT_MEMORY_MANAGEMENT_FUNCTIONS

#ifdef DIRECT_MEMORY_MANAGEMENT_FUNCTIONS


STATIC void* static_alloc(size_t size);
STATIC void* static_calloc(size_t num,size_t size);
STATIC void* static_realloc(void* p,size_t size);
STATIC void static_dealloc(void* p);
STATIC void static_checkpoint(void* p);

/// 3 global memory management function pointers: alloc,dealloc,realloc

__declspec(selectany) CORE_MEMORY_MANAGER_ALLOC pCoreMemoryManagerAlloc=&static_alloc;
__declspec(selectany) CORE_MEMORY_MANAGER_CALLOC pCoreMemoryManagerCAlloc=&static_calloc;
__declspec(selectany) CORE_MEMORY_MANAGER_DEALLOC pCoreMemoryManagerDeAlloc=&static_dealloc;
__declspec(selectany) CORE_MEMORY_MANAGER_REALLOC pCoreMemoryManagerReAlloc=&static_realloc;
__declspec(selectany) CORE_MEMORY_MANAGER_DEALLOC pCoreMemoryManagerCheckPoint=&static_checkpoint;

__declspec(selectany) CORE_MEMORY_MANAGER_ALLOC pCoreMemoryManagerAllocSave=NULL;
__declspec(selectany) CORE_MEMORY_MANAGER_CALLOC pCoreMemoryManagerCAllocSave=NULL;
__declspec(selectany) CORE_MEMORY_MANAGER_DEALLOC pCoreMemoryManagerDeAllocSave=NULL;
__declspec(selectany) CORE_MEMORY_MANAGER_REALLOC pCoreMemoryManagerReAllocSave=NULL;
/*
STATIC CORE_MEMORY_MANAGER_ALLOC pCoreMemoryManagerAlloc=&static_alloc;
STATIC CORE_MEMORY_MANAGER_CALLOC pCoreMemoryManagerCAlloc=&static_calloc;
STATIC CORE_MEMORY_MANAGER_DEALLOC pCoreMemoryManagerDeAlloc=&static_dealloc;
STATIC CORE_MEMORY_MANAGER_REALLOC pCoreMemoryManagerReAlloc=&static_realloc;

STATIC CORE_MEMORY_MANAGER_ALLOC pCoreMemoryManagerAllocSave=NULL;
STATIC CORE_MEMORY_MANAGER_CALLOC pCoreMemoryManagerCAllocSave=NULL;
STATIC CORE_MEMORY_MANAGER_DEALLOC pCoreMemoryManagerDeAllocSave=NULL;
STATIC CORE_MEMORY_MANAGER_REALLOC pCoreMemoryManagerReAllocSave=NULL;
*/
bool LoadCoreMemoryManager();

STATIC void* fx_alloc_proxy(size_t n)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS fx_alloc_proxy");
	//if(call_once==0) return NULL;
	call_once++;
#endif
return malloc(n);
}

STATIC void* fx_calloc_proxy(size_t num,size_t size)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS fx_calloc_proxy");
	//if(call_once==0) return NULL;
	call_once++;
#endif
return calloc(num,size);
}

STATIC void* fx_realloc_proxy(void* p,size_t size)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS fx_realloc_proxy");
	//if(call_once==0) return NULL;
	call_once++;
#endif
return realloc(p,size);
}

STATIC void fx_dealloc_proxy(void* p)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS fx_dealloc_proxy");
	//if(call_once==0) return;
	call_once++;
#endif
 free(p);
}




STATIC void* static_alloc(size_t size)
{
	OutputDebugStringW(__T("static_alloc")); 
	//return malloc(size);
#ifdef DLL_AUTO_LOAD
	if(LoadCoreMemoryManager())
	{
		OutputDebugStringW(__T("LoadCoreMemoryManager()")); 
		return (*pCoreMemoryManagerAlloc)(size);
	}
#endif
#ifdef FX_STATIC
	pCoreMemoryManagerAlloc=&fx_alloc_proxy;
	return malloc(size);
#endif
	pCoreMemoryManagerAlloc=&malloc;
	OutputDebugStringW(__T("pCoreMemoryManagerAlloc=&malloc;")); 
	return malloc(size);
}

STATIC void* static_calloc(size_t num,size_t size)
{
	OutputDebugStringW(__T("static_calloc")); 
	//return calloc(num,size);
#ifdef DLL_AUTO_LOAD
	if(LoadCoreMemoryManager())
	{
		OutputDebugStringW(__T("LoadCoreMemoryManager()")); 
		return (*pCoreMemoryManagerCAlloc)(num,size);
	}
#endif
#ifdef FX_STATIC
	pCoreMemoryManagerCAlloc=&fx_calloc_proxy;
	return calloc(num,size);
#endif
	pCoreMemoryManagerCAlloc=&calloc;
	OutputDebugStringW(__T("pCoreMemoryManagerCAlloc=&calloc;")); 
	return calloc(num,size);
}
STATIC void* static_realloc(void* p,size_t size)
{
	OutputDebugStringW(__T("static_realloc")); 
	//return realloc(p,size);
#ifdef DLL_AUTO_LOAD
	if(LoadCoreMemoryManager())
	{
		OutputDebugStringW(__T("LoadCoreMemoryManager()")); 
		return (*pCoreMemoryManagerReAlloc)(p,size);
	}
#endif
#ifdef FX_STATIC
	pCoreMemoryManagerReAlloc=&fx_realloc_proxy;
	return realloc(p,size);
#endif
	pCoreMemoryManagerReAlloc=&realloc;
	OutputDebugStringW(__T("pCoreMemoryManagerReAlloc=&realloc;"));
	return realloc(p,size);
}

STATIC void static_dealloc(void* p)
{
	//free(p);
	OutputDebugStringW(__T("static_dealloc")); 
	//return;
#ifdef DLL_AUTO_LOAD
	if(LoadCoreMemoryManager())
	{
		OutputDebugStringW(__T("LoadCoreMemoryManager()")); 
		return (*pCoreMemoryManagerDeAlloc)(p);
	}
#endif
#ifdef FX_STATIC
	pCoreMemoryManagerDeAlloc=&fx_dealloc_proxy;
	return;
#endif

	pCoreMemoryManagerDeAlloc=&free;
	OutputDebugStringW(__T("pCoreMemoryManagerDeAlloc=&free;"));
	free(p);
}

STATIC void static_checkpoint_stub(void* p){}

STATIC void static_checkpoint(void* p)
{
	//free(p);
	OutputDebugStringW(__T("static_checkpoint")); 
	//return;
#ifdef DLL_AUTO_LOAD
	if(LoadCoreMemoryManager())
	{
		OutputDebugStringW(__T("LoadCoreMemoryManager()")); 
		return (*pCoreMemoryManagerCheckPoint)(p);
	}
#endif
	pCoreMemoryManagerCheckPoint=&static_checkpoint_stub;
}

#else
/// Global pointer to memory management instance
__declspec(selectany) BaseCoreMemoryManager* pCoreMemoryManager=NULL;
#endif


/// Get "CoreMemoryManager.dll" full path
__forceinline TCHAR* GetCoreMemoryManagerPath()
{

#define BUF_SIZE 256
   static TCHAR szPath[BUF_SIZE];
   //static TCHAR szNewPath[BUF_SIZE];
	TCHAR tbuf[256];
  
    if( !GetModuleFileName( NULL, szPath, BUF_SIZE ) )
    {
		OutputDebugStringW(__T("GetModuleFileName fail.")); 
        //OutputDebugStringW(szPath); 
    }
	else
	{
		wsprintf(tbuf,L"szPath=%s",szPath);
		OutputDebugStringW(tbuf); 
#ifdef DBG_MSGS
		OutputDebugStringW(__T("GetModuleFileName ok.")); 
		OutputDebugStringW(szPath);	
#endif
		const TCHAR* cmm_dll_name=__T("\\Bin\\Core\\CoreMemoryManager.dll");
		TCHAR* p = wcsstr( szPath, __T("Bin"));
		if(p)
		{
			*p=0;
			wcsncat(szPath,cmm_dll_name,wcslen(cmm_dll_name));
#ifdef DBG_MSGS
			OutputDebugStringW(szPath);	
#endif
			return szPath;
		}
	}
    return NULL;

}

/// Load "CoreMemoryManager.dll" and set global pCoreMemoryManager
__forceinline bool LoadCoreMemoryManager()
{

/*
	static call_once=0;
	if(call_once>0)return false;
	call_once++;
*/

typedef bool (*IS_CORE_MEMORY_MANAGER_EXIST)(void);
typedef HRESULT (*GET_CORE_MEMORY_MANAGER)(void** p);
typedef HRESULT (*GET_CORE_MEMORY_MANAGER_DF)(
	void** p_alloc,
	void** p_calloc,
	void** p_delloc,
	void** p_realloc);



		bool bResult=false;
		/// handle for corememorymanager.dll
		static HMODULE hCoreMMModule=NULL;
		/// loadlibrary
		if(hCoreMMModule==NULL)
		{
			static TCHAR* dll_fname=NULL;
			if(dll_fname==NULL)dll_fname=GetCoreMemoryManagerPath();
			if(dll_fname==NULL)
			{
				wchar_t wbuf[128];
				wsprintf(wbuf,L"Could not find CoreMemoryManager.dll, path=%s",dll_fname);
				OutputDebugStringW(wbuf);
				return false;
			}

			hCoreMMModule=LoadLibrary(dll_fname);
#ifdef DBG_MSGS
			if(hCoreMMModule)OutputDebugStringW(L"hCoreMMModule Loaded!!!");
				else OutputDebugStringW(L"hCoreMMModule could not load!!!");
#endif
		};

		if(hCoreMMModule!=NULL)
		{
#ifdef DIRECT_MEMORY_MANAGEMENT_FUNCTIONS
			// get function pointer
			CORE_MEMORY_MANAGER_ALLOC pCoreMemoryManagerAllocDF=
				(CORE_MEMORY_MANAGER_ALLOC)GetProcAddress(hCoreMMModule,
				"cmm_alloc");

			if(pCoreMemoryManagerAllocDF)
			{
#ifdef DBG_MSGS
				OutputDebugStringW(L"pCoreMemoryManagerAllocDF!=NULL");
#endif
				pCoreMemoryManagerAlloc=pCoreMemoryManagerAllocDF;
			}

			// get function pointer
			CORE_MEMORY_MANAGER_CALLOC pCoreMemoryManagerCAllocDF=
				(CORE_MEMORY_MANAGER_CALLOC)GetProcAddress(hCoreMMModule,
				"cmm_calloc");

			if(pCoreMemoryManagerCAllocDF)
			{
#ifdef DBG_MSGS
				OutputDebugStringW(L"pCoreMemoryManagerCAllocDF!=NULL");
#endif
				pCoreMemoryManagerCAlloc=pCoreMemoryManagerCAllocDF;
			}

			// get function pointer
			CORE_MEMORY_MANAGER_DEALLOC pCoreMemoryManagerDeAllocDF=
				(CORE_MEMORY_MANAGER_DEALLOC)GetProcAddress(hCoreMMModule,
				"cmm_dealloc");

			if(pCoreMemoryManagerDeAllocDF)
			{
#ifdef DBG_MSGS
				OutputDebugStringW(L"pCoreMemoryManagerDeAllocDF!=NULL");
#endif
				pCoreMemoryManagerDeAlloc=pCoreMemoryManagerDeAllocDF;
			}

			// get function pointer
			CORE_MEMORY_MANAGER_REALLOC pCoreMemoryManagerReAllocDF=
				(CORE_MEMORY_MANAGER_REALLOC)GetProcAddress(hCoreMMModule,
				"cmm_realloc");

			if(pCoreMemoryManagerReAllocDF)
			{
#ifdef DBG_MSGS
				OutputDebugStringW(L"pCoreMemoryManagerReAllocDF!=NULL");
#endif
				pCoreMemoryManagerReAlloc=pCoreMemoryManagerReAllocDF;
			}

			// get function pointer
			CORE_MEMORY_MANAGER_DEALLOC pCoreMemoryManagerCheckPointDF=
				(CORE_MEMORY_MANAGER_DEALLOC)GetProcAddress(hCoreMMModule,
				"cmm_checkpoint");

			if(pCoreMemoryManagerCheckPointDF)
			{
#ifdef DBG_MSGS
				OutputDebugStringW(L"pCoreMemoryManagerCheckPointDF!=NULL");
#endif
				pCoreMemoryManagerCheckPoint=pCoreMemoryManagerCheckPointDF;
			}

#ifdef DBG_MSGS
				static wchar_t pbuf[256];
				static counter=0;
				wsprintf(pbuf,L"ret pGetCoreMemoryManagerDF %x,%x,%x,%x %d",pCoreMemoryManagerAlloc,pCoreMemoryManagerCAlloc,pCoreMemoryManagerDeAlloc,pCoreMemoryManagerReAlloc,counter++);
				OutputDebugStringW(pbuf);
#endif

				bResult=pCoreMemoryManagerAllocDF && pCoreMemoryManagerCAllocDF && pCoreMemoryManagerReAllocDF && pCoreMemoryManagerDeAllocDF;
			
#else // DIRECT FUNCTIONs
#ifdef DBG_MSGS
			OutputDebugStringW(L"hCoreMMModule!=NULL");
#endif
			// get function pointer
			GET_CORE_MEMORY_MANAGER pGetCoreMemoryManager=
				(GET_CORE_MEMORY_MANAGER)GetProcAddress(hCoreMMModule,
				"GetCoreMemoryManagerDirectPointer");

			if(pGetCoreMemoryManager)
			{
#ifdef DBG_MSGS
				OutputDebugStringW(L"pGetCoreMemoryManager!=NULL");
#endif
				// get BaseCoreMemoryManager pointer
				BaseCoreMemoryManager* pCMM;
				void* pp[1];*pp=NULL;
				
				(*pGetCoreMemoryManager)(pp);
				pCMM=(BaseCoreMemoryManager*)*pp;
#ifdef DBG_MSGS
				static wchar_t pbuf[256];
				wsprintf(pbuf,L"ret pGetCoreMemoryManager %x",pCMM);
				OutputDebugStringW(pbuf);
#endif
				if(pCMM)
				{
#ifdef DBG_MSGS
					OutputDebugStringW(L"pCoreMemoryManager=pCMM");
#endif
					pCoreMemoryManager=pCMM;
					bResult=true;
				
				}
			}
#endif
		}
	return bResult;
}

#ifdef DIRECT_MEMORY_MANAGEMENT_FUNCTIONS
/// experimental
__forceinline void * operator new(size_t n)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS new");
	call_once++;
#endif
	return (*pCoreMemoryManagerAlloc)(n);
}
__forceinline void * operator new[](size_t n)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS new[]");
	call_once++;
#endif
	return (*pCoreMemoryManagerAlloc)(n);
}
__forceinline void operator delete(void* p)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS delete");
	call_once++;
#endif
	(*pCoreMemoryManagerDeAlloc)(p);
}
__forceinline void operator delete[](void* p)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS delete[]");
	call_once++;
#endif
	(*pCoreMemoryManagerDeAlloc)(p);
}

__forceinline void* core_malloc(size_t n)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS core_malloc");
	call_once++;
#endif
	return (*pCoreMemoryManagerAlloc)(n);
}
__forceinline void* core_realloc(void* p,size_t n)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS core_realloc");
	call_once++;
#endif
	return (*pCoreMemoryManagerReAlloc)(p,n);
}
__forceinline void* core_calloc( size_t num, size_t size )
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS core_calloc");
	call_once++;
#endif
	return (*pCoreMemoryManagerCAlloc)(num,size);
}
__forceinline void core_free(void* p)
{
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS core_free");
	call_once++;
#endif
	(*pCoreMemoryManagerDeAlloc)(p);
}

__forceinline void core_disable()
{
#ifndef USE_ENABLE_DISABLE
	return;
#endif

#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS core_disable");
	call_once++;
#endif

	// save
	pCoreMemoryManagerAllocSave=pCoreMemoryManagerAlloc;
	pCoreMemoryManagerCAllocSave=pCoreMemoryManagerCAlloc;
	pCoreMemoryManagerDeAllocSave=pCoreMemoryManagerDeAlloc;
	pCoreMemoryManagerReAllocSave=pCoreMemoryManagerReAlloc;
	// point to default
/*
	pCoreMemoryManagerAlloc=&malloc;
	pCoreMemoryManagerCAlloc=&calloc;
	pCoreMemoryManagerDeAlloc=&free;
	pCoreMemoryManagerReAlloc=&realloc;
*/
	pCoreMemoryManagerAlloc=&fx_alloc_proxy;
	pCoreMemoryManagerCAlloc=&fx_calloc_proxy;
	pCoreMemoryManagerDeAlloc=&fx_dealloc_proxy;
	pCoreMemoryManagerReAlloc=&fx_realloc_proxy;

#ifdef DBG_MSGS
	static wchar_t pbuf[256];
	wsprintf(pbuf,L"disable %x,%x,%x,%x",pCoreMemoryManagerAlloc,pCoreMemoryManagerCAlloc,pCoreMemoryManagerDeAlloc,pCoreMemoryManagerReAlloc);
	OutputDebugStringW(pbuf);
	wsprintf(pbuf,L"disable save %x,%x,%x,%x",pCoreMemoryManagerAllocSave,pCoreMemoryManagerCAllocSave,pCoreMemoryManagerDeAllocSave,pCoreMemoryManagerReAllocSave);
	OutputDebugStringW(pbuf);
#endif

}
__forceinline void core_enable()
{
#ifndef USE_ENABLE_DISABLE
	return;
#endif
#ifdef DBG_MSGS
	static call_once=0;
	if(call_once<10)OutputDebugStringW(L"DIRECT_MEMORY_MANAGEMENT_FUNCTIONS core_enable");
	call_once++;
#endif

	pCoreMemoryManagerAlloc=pCoreMemoryManagerAllocSave;
	pCoreMemoryManagerCAlloc=pCoreMemoryManagerCAllocSave;
	pCoreMemoryManagerDeAlloc=pCoreMemoryManagerDeAllocSave;
	pCoreMemoryManagerReAlloc=pCoreMemoryManagerReAllocSave;

#ifdef DBG_MSGS
	static wchar_t pbuf[256];
	wsprintf(pbuf,L"enable %x,%x,%x,%x",pCoreMemoryManagerAlloc,pCoreMemoryManagerCAlloc,pCoreMemoryManagerDeAlloc,pCoreMemoryManagerReAlloc);
	OutputDebugStringW(pbuf);
	wsprintf(pbuf,L"enable save %x,%x,%x,%x",pCoreMemoryManagerAllocSave,pCoreMemoryManagerCAllocSave,pCoreMemoryManagerDeAllocSave,pCoreMemoryManagerReAllocSave);
	OutputDebugStringW(pbuf);
#endif

}

__forceinline void core_checkpoint(void* p)
{
	(*pCoreMemoryManagerCheckPoint)(p);
}

#else

/// Global operator new redefinition
__forceinline void * operator new(size_t n)
{
#ifdef DBG_MSGS
	static call_counter=0;
	if(call_counter++==0)OutputDebugStringW(L"global __forceinline void * operator new() FIRST CALL");	
#endif
	register void* p=NULL;
_l1:
	if(pCoreMemoryManager){
#ifdef DBG_MSGS
		static si_i=0;
		if(si_i<10){
			OutputDebugStringW(L"global __forceinline void * operator new()");	
		}
		si_i++;
#endif

		p=pCoreMemoryManager->alloc(n);
	}
	else 
	{
#ifdef DLL_AUTO_LOAD
		static try_load_counter=0;
		if(try_load_counter++==0)
		{
			if(LoadCoreMemoryManager()) goto _l1;
		}
#endif
		p=malloc(n);
#ifdef DBG_MSGS
		static i2=0;
		if(i2++<10){
			OutputDebugStringW(L"p=malloc(n);");
		}
#endif
	}

	if(p==NULL)
	{
		const wchar_t* msg=L"global __forceinline void * operator new() Error p==NULL!!!";
		OutputDebugStringW(msg);	
		throw msg;
	}
	
	return p;
}


/// Global operator delete redefinition
__forceinline void operator delete(void* p)
{
#ifdef DBG_MSGS
	static call_counter=0;
	if(call_counter++==0)OutputDebugStringW(L"global __forceinline void operator delete(void* p) FIRST CALL");	
#endif
	if(pCoreMemoryManager){
#ifdef DBG_MSGS
		static si_i=0;
		if(si_i<10){
			OutputDebugStringW(L"global __forceinline void operator delete(void* p)");	
		}
		si_i++;
#endif		
		pCoreMemoryManager->dealloc(p);
		
	}
	else {
#ifdef DBG_MSGS
		static counter=0;
		if(counter++<10)
		{
		OutputDebugStringW(L"global __forceinline void operator delete(void* p) pCoreMemoryManager==NULL");
		}
#endif
		free(p);
	}
}


/// Global operator new[] redefinition
__forceinline void * operator new[](size_t n)
{
#ifdef DBG_MSGS
	static call_counter=0;
	if(call_counter++==0)OutputDebugStringW(L"global __forceinline void * operator new[]() FIRST CALL");	
#endif
	void* p=NULL;
_l1:
	if(pCoreMemoryManager){
#ifdef DBG_MSGS
		static si_i=0;
		if(si_i<10){
			OutputDebugStringW(L"global __forceinline void * operator new[]()");	
		}
		si_i++;
#endif
		p=pCoreMemoryManager->alloc(n);
		
	}
	else {
#ifdef DLL_AUTO_LOAD
		static try_load_counter=0;
		if(try_load_counter++==0)
		{
			if(LoadCoreMemoryManager()) goto _l1;
		}
#endif
#ifdef DBG_MSGS
		static i2=0;
		if(i2++<10){
			OutputDebugStringW(L"p=malloc[](n);");
		}
#endif
		p=malloc(n);
	}

	if(p==NULL)
	{
		const wchar_t* msg=L"global __forceinline void * operator new[]() Error p==NULL!!!";
		OutputDebugStringW(msg);	
		throw msg;
	}
	
	return p;
}

/// Global operator delete[] redefinition
__forceinline void operator delete[](void* p)
{
#ifdef DBG_MSGS
	static call_counter=0;
	if(call_counter++==0)OutputDebugStringW(L"global __forceinline void operator delete[](void* p) FIRST CALL");	
#endif
	if(pCoreMemoryManager){
#ifdef DBG_MSGS
		static si_i=0;
		if(si_i<10){
			OutputDebugStringW(L"global __forceinline void operator delete[](void* p)");	
		}
		si_i++;
#endif
		pCoreMemoryManager->dealloc(p);

	}
	else {
#ifdef DBG_MSGS		
		static counter=0;
		if(counter++<10)
		{
			OutputDebugStringW(L"global __forceinline void operator delete[](void* p) pCoreMemoryManager==NULL");
		}
#endif
		free(p);
	}
}

/// Global core_malloc function
__forceinline void* core_malloc(size_t n)
{
#ifdef DBG_MSGS
	static call_counter=0;
	if(call_counter++==0)OutputDebugStringW(L"global __forceinline void* core_malloc(size_t n) FIRST CALL");	
#endif
_l1:
	if(pCoreMemoryManager){
#ifdef DBG_MSGS
		static si_i=0;
		if(si_i<10){
			OutputDebugStringW(L"global __forceinline void* core_malloc(size_t n)");	
		}
		si_i++;
#endif
		return pCoreMemoryManager->alloc(n);
	}
	else 
	{
#ifdef DLL_AUTO_LOAD
		static try_load_counter=0;
		if(try_load_counter++==0)
		{
			if(LoadCoreMemoryManager()) goto _l1;
		}
#endif
		return malloc(n);
	}
}

/// Global core_realloc function
__forceinline void* core_realloc(void* p,size_t n)
{
#ifdef DBG_MSGS
	static call_counter=0;
	if(call_counter++==0)OutputDebugStringW(L"global __forceinline void* core_realloc(void* p,size_t n) FIRST CALL");	
#endif

	if(pCoreMemoryManager)
		return 
			pCoreMemoryManager->re_alloc(p,n);
	else 
		return 
			realloc(p,n);
}

__forceinline void* core_calloc( size_t num, size_t size )
{
#ifdef DBG_MSGS
	static call_counter=0;
	if(call_counter++==0)OutputDebugStringW(L"__forceinline void* core_calloc( size_t num, size_t size ) FIRST CALL");	
#endif
_l1:

	if(pCoreMemoryManager)
	{
#ifdef DBG_MSGS
		static si_i=0;
		if(si_i<10){
			OutputDebugStringW(L"__forceinline void* core_calloc( size_t num, size_t size)");	
		}
		si_i++;
#endif
		return 
			pCoreMemoryManager->c_alloc(num,size);
	}
	else 
	{
#ifdef DLL_AUTO_LOAD
		static try_load_counter=0;
		if(try_load_counter++==0)
		{
			if(LoadCoreMemoryManager()) goto _l1;
		}
#endif
		return 
			calloc(num,size);
	}
}

/// Global core_free function
__forceinline void core_free(void* p)
{
	if(pCoreMemoryManager) 
		pCoreMemoryManager->dealloc(p);
	else free(p);
}

#endif

#ifdef CMM_DEBUG

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define __WFILE__ WIDEN(__FILE__)
#define __WFUNCTION__ WIDEN(__FUNCTION__)

#define CMM_TRY try
#define CMM_CATCH(x) \
catch(BaseCoreMemoryManagerDbgException& dbg_e) \
{ \
		wchar_t wbuf[256]; \
		wsprintf(wbuf,L"%s catch %s,status:%d,func:%s,file:%s,line%d", \
			x,dbg_e.m_msg,dbg_e.m_status,func,file,line); \
		OutputDebugStringW(wbuf); \
} \
catch(...) \
{ \
		wchar_t wbuf[256]; \
		wsprintf(wbuf,L"%s catch(...),func:%s,file:%s,line%d", \
			x,func,file,line); \
		OutputDebugStringW(wbuf); \
}
#define CMM_CATCH_RET(x) \
catch(BaseCoreMemoryManagerDbgException& dbg_e) \
{ \
		wchar_t wbuf[256]; \
		wsprintf(wbuf,L"%s catch %s,status:%d,func:%s,file:%s,line%d", \
			x,dbg_e.m_msg,dbg_e.m_status,func,file,line); \
		OutputDebugStringW(wbuf); \
		return NULL; \
} \
catch(...) \
{ \
		wchar_t wbuf[256]; \
		wsprintf(wbuf,L"%s catch(...),func:%s,file:%s,line%d", \
			x,func,file,line); \
		OutputDebugStringW(wbuf); \
		return NULL; \
}



__forceinline void* core_malloc_dbg(size_t n, wchar_t* func,wchar_t* file, int line)
CMM_TRY
{
	return core_malloc(n);
}
CMM_CATCH_RET(L"core_malloc_dbg");

__forceinline void core_free_dbg(void* p, wchar_t* func,wchar_t* file, int line)
CMM_TRY
{
	core_free(p);
}
CMM_CATCH(L"core_free_dbg");

__forceinline void* core_realloc_dbg(void* p,size_t n, wchar_t* func,wchar_t* file, int line)
CMM_TRY
{
	return core_realloc(p,n);
}
CMM_CATCH_RET(L"core_realloc_dbg");

__forceinline void* core_calloc_dbg( size_t num, size_t size, wchar_t* func,wchar_t* file, int line)
CMM_TRY
{
	return core_calloc(num,size);
}
CMM_CATCH_RET(L"core_calloc_dbg");

/*
__forceinline void * operator new(size_t n, wchar_t* func,wchar_t* file, int line)
CMM_TRY
{
	return ::operator new(n);
}
CMM_CATCH_RET(L"operator new");

__forceinline void operator delete(void* p, wchar_t* func,wchar_t* file, int line)
CMM_TRY
{
	::operator delete(p);
}
CMM_CATCH(L"operator delete");
*/

/// **************DEBUG************** ///
/// malloc replace macros 
#define malloc(n) core_malloc_dbg(n,__WFUNCTION__, __WFILE__, __LINE__)
/// free replace macros
#define free(p) core_free_dbg(p,__WFUNCTION__, __WFILE__, __LINE__)
/// realloc replace macros
#define realloc(p,n) core_realloc_dbg(p,n,__WFUNCTION__, __WFILE__, __LINE__)
/// calloc
#define calloc(num,size) core_calloc_dbg(num,size,__WFUNCTION__, __WFILE__, __LINE__)

/*
#define DEBUG_NEW new(__WFUNCTION__, __WFILE__, __LINE__)
#define new DEBUG_NEW

#define DEBUG_DELETE delete(__WFUNCTION__, __WFILE__, __LINE__)
#define delete DEBUG_DELETE
*/
#else
/// **************NODEBUG************** ///
/// malloc replace macros 
#define malloc(n) core_malloc(n)
/// free replace macros
#define free(p) core_free(p)
/// realloc replace macros
#define realloc(p,n) core_realloc(p,n)
/// calloc
#define calloc(num,size) core_calloc(num,size)

#endif


/// LOCAL_ALLOC_OPTIMIZATION provides stack allocations of small objects instead of heap
#define LOCAL_ALLOC_OPTIMIZATION

#ifdef LOCAL_ALLOC_OPTIMIZATION

/// size of stack memory
#define LOCAL_POOL_SIZE 4

/// defines memory allocation less LOCAL_POOL_SIZE in local_pool array or malloc
#define LOCAL_ALLOC_POOL(n,p) \
	unsigned char local_pool[LOCAL_POOL_SIZE]; \
		if(n<=LOCAL_POOL_SIZE)p=&(local_pool[0]); \
	else \
		p=(unsigned char*)malloc(n); 

/// defines memory free
#define LOCAL_FREE_POOL(n,p) \
		if(n>LOCAL_POOL_SIZE)free(p);

#endif

__forceinline void out_core_memory_manager_debug_info(void* p=NULL)
{
#ifdef DIRECT_MEMORY_MANAGEMENT_FUNCTIONS
#else
	if(pCoreMemoryManager)
		pCoreMemoryManager->out_debug_info(p);
#endif

}
__forceinline void save_core_memory_manager_status(void* p)
{
#ifdef DIRECT_MEMORY_MANAGEMENT_FUNCTIONS
#else
	if(pCoreMemoryManager)
		pCoreMemoryManager->save_status(p);
#endif
}
