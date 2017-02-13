#include "stdafx.h"
//#include "define.h"
#include "Log.h"
//extern const int c_iLenghtUserName=100;
//extern __declspec(thread) int gt_szNameThread;//[c_iLenghtUserName+1];
#include <crtdbg.h>


namespace SS
{
namespace Core
{
namespace CommonServices
{
	const int c_iSizeOfLogString = 512;

	CLog::CLog(void) : m_uiIndexTLS(-1)
	{
		m_uiIndexTLS=TlsAlloc();
	}

	CLog::~CLog(void)
	{	
		_CrtCheckMemory();
		TlsFree(m_uiIndexTLS);
	}
	void CLog::Save(const wchar_t* wszPath, const wchar_t* wszMessage)
	{
		//запись в файл
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);

		HANDLE	hLog;
		hLog = CreateFileW(wszPath,
						GENERIC_READ | GENERIC_WRITE, 
						FILE_SHARE_READ, 
						NULL,
						OPEN_ALWAYS, 
						FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
						NULL);
		
		if(hLog != INVALID_HANDLE_VALUE)
		{
			try
			{

				LARGE_INTEGER FPos64={0};
				FPos64.LowPart=0;
				SetFilePointer(hLog, FPos64.LowPart, &FPos64.HighPart, FILE_END);

				std::wstring wsBuffer;
				wsBuffer.reserve(c_iSizeOfLogString);
				if(m_uiIndexTLS!=-1)
				{				
					if(TlsGetValue(m_uiIndexTLS))
						wsBuffer += reinterpret_cast<wchar_t*>(TlsGetValue(m_uiIndexTLS));
					else 
						wsBuffer += L"Unknown user  | ";
					
				};
				wchar_t wBuff[40];
				DWORD dwNumberOfBytesWritten;
				wsBuffer += _ltow(SystemTime.wDay,wBuff,10);
				wsBuffer += L":";
				wsBuffer += _ltow(SystemTime.wHour,wBuff,10);
				wsBuffer += L":";
				wsBuffer += _ltow(SystemTime.wMinute,wBuff,10);
				wsBuffer += L":";
				wsBuffer += _ltow(SystemTime.wSecond,wBuff,10);
				wsBuffer += L":";
				wsBuffer += _ltow(SystemTime.wMilliseconds,wBuff,10);
				wsBuffer += wszMessage;
				wsBuffer += L'\n';
				::WriteFile(hLog,wsBuffer.c_str(), wsBuffer.size() * sizeof(wchar_t),&dwNumberOfBytesWritten,NULL);
			}
			catch(...)
			{
				::CloseHandle(hLog);
			}
			::CloseHandle(hLog);
		}
	};


	void CLog::SaveLog(const wchar_t* wszPath, const wchar_t* wszMessage)
	{
		CCriticalSection oCriticalSection(&m_CriticalSection);
		try{

				Save(wszPath, wszMessage);
			}
		catch(...){
			throw;
		}
	}
	void CLog::SetUserName(const wchar_t* wszUserName)
	{
		CCriticalSection oCriticalSection(&m_CriticalSection);
		//if(c_iLenghtUserName<strlen(szUserName))
			TlsSetValue(m_uiIndexTLS, reinterpret_cast<void*>(const_cast<wchar_t*>(wszUserName)));
	};
}
}
}