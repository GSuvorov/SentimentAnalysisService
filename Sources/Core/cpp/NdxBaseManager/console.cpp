#include "StdAfx.h"
#include ".\console.h"
#include <iostream>
#include <tchar.h>
#include "..\ASCInterface\defines.h"

#ifdef _SS_UNITTESTS
#include <psapi.h>
#endif

USES_LOAD_MANAGER;

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxBaseManager
{

CTickManager TickManager;
bool g_bStopLogging=false;

//--------------------------------------------------------------------//

void ToConsole(const wchar_t* pszString)
{
	#ifdef _SS_UNITTESTS
	if(!g_bStopLogging){
		wprintf(L"\n");
		wprintf(pszString);
		wprintf(L"\n");
		OutputDebugStringW(pszString);
	}
	#else
	//SAVE_LOG(SS_MESSAGE, pszString, NULL);
	OutputDebugStringW(pszString);
	//printf(pszString);
	#endif
};

void ToConsoleEx(const wchar_t* pszString, const wchar_t* pszDest, const wchar_t* pszFile, long Line)
{
	wchar_t* pbuf=new wchar_t[wcslen(pszString)+wcslen(pszDest)+wcslen(pszFile)+50];
	swprintf(pbuf, L"%s: Dest:%s, File: %s, Line:%d", pszString, pszDest, pszFile, Line);
	ToConsole(pbuf);
	delete[] pbuf;
}

void ToConsole(unsigned char* pBuffer, unsigned int uiSize)
{
	unsigned int uiViewSize=30;
	unsigned int uiViewedSize=0;
	unsigned int uiSmallViewedSize=0;

	ToConsole(L"Buffer---------");
	ToConsole(L"Size", uiSize);
	unsigned char* pucByte=pBuffer;
	uiViewedSize=0;
	do{
		wchar_t buf[256]=L"";
		wchar_t buf1[10]=L"";
		uiSmallViewedSize=0;
		while(uiSmallViewedSize<uiViewSize && ((unsigned int)(pucByte-pBuffer))<uiSize){
			swprintf(buf1, L" %x", (unsigned int)*pucByte);
			wcscat(buf, buf1);
			pucByte++;
			uiSmallViewedSize++;
		}
		ToConsole(buf);
		uiViewedSize+=uiViewSize;
	}while(uiViewedSize<uiSize);
	ToConsole(L"End-of-Buffer--");
}

void ToConsole(const wchar_t* pszString, double dbValue)
{
	wchar_t buf[256];
	swprintf(buf, L"%s: %f", pszString, dbValue);
	ToConsole(buf);
}

void ToConsole(const wchar_t* pszString, unsigned int uiValue)
{
	wchar_t buf[256];
	swprintf(buf, L"%s: %u", pszString, uiValue);
	ToConsole(buf);
}

void ViewMemoryStatus(void)
{
#ifdef _SS_UNITTESTS
	PROCESS_MEMORY_COUNTERS pmc={0};

	if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(PROCESS_MEMORY_COUNTERS))){
		wchar_t buf[256];
		swprintf(buf, L"Memory:\n Working: %u, PageFile: %u", (unsigned int)pmc.WorkingSetSize, (unsigned int)pmc.PagefileUsage);
		ToConsole(buf);
	}else
		return;
#endif
}

unsigned int GetMemoryStatus(void)
{
#ifdef _SS_UNITTESTS
	PROCESS_MEMORY_COUNTERS pmc={0};

	if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(PROCESS_MEMORY_COUNTERS))){
		return (unsigned int)pmc.WorkingSetSize;
	}else
		return 0;
#endif

	return 0;
}

void ErrorToConsole(ErrorCode EC, const wchar_t* pszString, const wchar_t* pszDest, const wchar_t* pszFile, long Line)
{
wchar_t buf[500];

	switch(EC){
	case erIsNull:
		swprintf(buf, L"%s is NULL", pszString);
		ToConsoleEx(buf, pszDest, pszFile, Line);
		return;
		break;
	case erNotFind:
		swprintf(buf, L"%s not find", pszString);
		break;
	case erNotEqual:
		swprintf(buf, L"%s not equal", pszString);
		break;
	case erDBMS:
		swprintf(buf, L"DBMS error: %s!", pszString);
		break;
	case erTryCatch:
		swprintf(buf, L"Module: NDX has catch errorcomments: %s", pszString);
		break;
	case erFileIO:
		swprintf(buf, L"IO error in %s", pszString);
		break;
	case erBadHandle:
		swprintf(buf, L"%s is Bad Handle", pszString);
		break;
	default:
		wcscpy(buf, L"Unrecognized error!!!");
		break;
	}
	
	ToConsoleEx(buf, pszDest, pszFile, Line);


	//גûגמה ג כמד פאיכ
	wchar_t buf1[500]=L"";
	wsprintf(buf1, L"\n\t%s\n\t%s %s %u", buf, pszDest, pszFile, Line);
	SAVE_LOG(SS_ERROR AND buf1);
}

void* st_malloc		 (unsigned long n_bytes)
{
  if (n_bytes)
  {
    void *mem;
    mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, n_bytes);
    if (mem)
      return mem;
  }
  return NULL;
}

int st_free (void *mem)
{
  if (mem) {
    return HeapFree(GetProcessHeap(), 0, mem);
  }else return 0;
}

//--------------------------------------------------------------------//

#pragma warning( disable : 4996 )

CTickManager::CTickManager()
{

}

CTickManager::~CTickManager()
{

}

void	CTickManager::StartTick(const wchar_t* szID)
{
	TTickMeasures::iterator itMeasure=m_TickMeasures.find(szID);
	
	if(itMeasure!=m_TickMeasures.end()){
	}else{
		itMeasure=m_TickMeasures.insert(TTickMeasures::value_type(szID, CTickMeasure())).first;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&itMeasure->second.m_iStartTick);
	itMeasure->second.m_iStopTick=0;
	itMeasure->second.m_uiStartsCount++;
}

void	CTickManager::StopTick(const wchar_t* szID)
{
	TTickMeasures::iterator itMeasure=m_TickMeasures.find(szID);
	
	if(itMeasure!=m_TickMeasures.end()){
		QueryPerformanceCounter((LARGE_INTEGER*)&itMeasure->second.m_iStopTick);
		itMeasure->second.m_iSumTick+=itMeasure->second.m_iStopTick-itMeasure->second.m_iStartTick;
		itMeasure->second.m_iStartTick=0;
		itMeasure->second.m_iStopTick=0;
	}
}

double	CTickManager::GetTicks(const wchar_t* szID)
{
	TTickMeasures::iterator itMeasure=m_TickMeasures.find(szID);
	double	dbTime=0;

	if(itMeasure!=m_TickMeasures.end()){
		__int64 iFrequency;
		QueryPerformanceFrequency((LARGE_INTEGER*)&iFrequency);
		dbTime=(double)itMeasure->second.m_iSumTick/(double)iFrequency;
	}

	return dbTime;
}

void	CTickManager::ViewTicks(const wchar_t* szID)
{
	ToConsole(szID, GetTicks(szID));
}

void	CTickManager::ViewTicks(void)
{
	wchar_t buf[500];
	ToConsole(L"//-------Ticks-Measures");
	for(TTickMeasures::iterator itMeasure=m_TickMeasures.begin(); itMeasure!=m_TickMeasures.end(); itMeasure++){
		swprintf(buf, L"%s: %f - %u", itMeasure->first.c_str(), GetTicks(itMeasure->first.c_str()), itMeasure->second.m_uiStartsCount);
		ToConsole(buf);
		//סבנאסûגאול
		itMeasure->second.m_uiStartsCount=0;
		itMeasure->second.m_iSumTick=0;
		itMeasure->second.m_iStartTick=0;
		itMeasure->second.m_iStopTick=0;
	}
	ToConsole(L"//----Eof-Ticks-Measures");
}

#pragma warning( default : 4996 )

//--------------------------------------------------------------------//

}
}
}
}
