#pragma once

//#include <VARARGS.H>
#include <STDARG.H>
#include <string>

#include "ExceptionParams.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{

	///базовый класс для всех ошибок системы СС
	class CException : public SS::Interface::Core::CommonServices::IException,
		public SS::Core::Types::Enumerators::TListEnumerator<SS::Core::CommonServices::CExceptionParams>
	{
		
		std::wstring m_wsExceptions;
	public:

		///добавление очередной ошибки в список
		SS::Interface::Core::CommonServices::IExceptionParams* Add();
		
		///возвращается текущая ошибка
		SS::Interface::Core::CommonServices::IExceptionParams* GetCurrent();

		///возвращает в виде строки накопленные ошибки
		std::wstring& ToString();

		///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
		HRESULT QueryInterface(REFIID pIID, void** pBase); 
		///освобождение
		ULONG Release();
	};
}
}
}

