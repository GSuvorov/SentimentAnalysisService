#pragma once

#include "..\ASCInterface\ICommonServices.h"
#include "Log.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{

	class CLogSingleton : public SS::Interface::Core::CommonServices::ILog,
		SS::Core::CommonServices::CSingle<CLog>
	{
	protected:

		std::wstring m_wsPathToLog;
		void Init();
	public:

		CLogSingleton();

		~CLogSingleton();

		///запись набора лог-сообщений szMessage на диск
		void SaveLog(wchar_t* wszFirst...);		

		///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
		HRESULT QueryInterface(REFIID pIID, void** pBase); 
		///освобождение
		ULONG Release();
	};
}
}
}