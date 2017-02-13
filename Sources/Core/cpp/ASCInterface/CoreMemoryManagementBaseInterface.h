#pragma once

/// Базовый интерфейс менеджера памяти

#ifndef _UNICODE
#error UNICODE ONLY!!! 
#endif

#include <malloc.h>
#include <windows.h>

/// Прототипы указателей функций менеджемента памяти
typedef void* (*CORE_MEMORY_MANAGER_ALLOC)(size_t);
typedef void* (*CORE_MEMORY_MANAGER_CALLOC)(size_t,size_t);
typedef void (*CORE_MEMORY_MANAGER_DEALLOC)(void*);
typedef void* (*CORE_MEMORY_MANAGER_REALLOC)(void*,size_t);

/// Базовый интерфейс менеджера памяти
class BaseCoreMemoryManager
{
	bool m_bEnabled;
public:
	BaseCoreMemoryManager():m_bEnabled(false){};
	virtual __forceinline void* __fastcall alloc(size_t size){return malloc(size);};
	virtual __forceinline void* __fastcall re_alloc(void* p,size_t size){return realloc(p,size);};
	virtual __forceinline void* __fastcall c_alloc(size_t num,size_t size){return calloc(num,size);};
	virtual __forceinline void __fastcall dealloc(void* p){free(p);};
	__forceinline void __fastcall enable(){m_bEnabled=true;};
	__forceinline void __fastcall disable(){m_bEnabled=false;};
	__forceinline bool __fastcall is_enabled(){return m_bEnabled;};
	virtual __forceinline void out_debug_info(void* p){};
	virtual __forceinline void save_status(void* p){};
};

class BaseCoreMemoryManagerDbgException
{
public:
	unsigned int m_status;
	wchar_t* m_msg;
	BaseCoreMemoryManagerDbgException(wchar_t* msg,unsigned int status):m_msg(msg),m_status(status){};
};