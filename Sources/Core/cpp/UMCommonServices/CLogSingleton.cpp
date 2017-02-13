#include "stdafx.h"
#include "CLogSingleton.h"

#include "shlobj.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{
	
	CLogSingleton::CLogSingleton()
	{	

	};

	CLogSingleton::~CLogSingleton()
	{
	};

	void CLogSingleton::Init()
	{
		if(m_wsPathToLog.empty())
		{
			SS::Interface::Core::CommonServices::IWorkRegister* pWorkRegister = 
				(SS::Interface::Core::CommonServices::IWorkRegister*)
				GetLoadManager()->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);

			m_wsPathToLog = pWorkRegister->GetParameter(L"PathLogs");

			//TCHAR szAppData[MAX_PATH];
			//HRESULT hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);

			//m_wsPathToLog = szAppData;
			//m_wsPathToLog += L"\\SS\\SS";

			m_wsPathToLog += L"\\log.txt";
			//OutputDebugString(m_wsPathToLog.c_str());
		};
	};

	void CLogSingleton::SaveLog(wchar_t* wszFirst...)
	{
		Init();
		try{
			//извлечение аргументов
			va_list args;
			va_start(args,wszFirst);

			std::wstring wsArgs=SEPARATOR;
			wsArgs.reserve(c_iSizeOfLogString);
			wsArgs+=wszFirst;
			for(;;)
			{


				wchar_t* wszArg=va_arg(args, wchar_t*);
				if(wszArg==NULL) break;

				wsArgs+=SEPARATOR;

				wsArgs+=wszArg;
			};

			if(!wsArgs.empty())
				wsArgs+=TERMINAL_SEPARATOR;

			va_end(args);
	
			Instance().SaveLog(m_wsPathToLog.c_str(),wsArgs.c_str());
		}
		catch(...){
			throw;
		}
	};

	HRESULT CLogSingleton::QueryInterface(REFIID pIID, void** pBase)
	{
		*pBase = NULL;

		if (pIID == IID_Log)
			*pBase = (SS::Interface::Core::CommonServices::ILog*) this;
		else if (pIID == IID_Base)
			*pBase = (SS::Interface::IBase*) this;

		return (*pBase) ? S_OK : S_FALSE;
	}; 

	ULONG CLogSingleton::Release()
	{
		delete this;
		return NULL;
	};
}
}
}