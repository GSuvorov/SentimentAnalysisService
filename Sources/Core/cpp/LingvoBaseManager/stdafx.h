// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here
#include <hash_map>
#include <algorithm>
#include <string>


#include "..\ASCInterface\defines.h"


USES_LOAD_MANAGER;


#define BEGIN_QI_MAP() \
   HRESULT QueryInterface(REFIID pIID, void** pBase) \
{

/// Необходимый интерфейс наследован
#define QI_MAP_INHERIT_ENTRY( IID, Type ) \
   if(pIID==(IID)) \
   *pBase = static_cast<Type*>(this); \
   else

/// Необходимый интерфейс можно получить путём приведения переменной класса
#define QI_MAP_MEMBER_ENTRY( IID, Type, Member ) \
   if(pIID==(IID)) \
   *pBase = static_cast<Type*>(&Member); \
   else

#define END_QI_MAP() \
{\
   *pBase = NULL;  \
   return E_NOINTERFACE;  \
}\
   return S_OK;  \
}

#define RELEASE_IMPL() ULONG Release(){ delete this;  return 0; }     

#define APP_ID "SSLBM1"

#include "../[libs]/FirstHeader.h"

using namespace SS::Lib;


