#include "stdafx.h"
#include "ExceptionParams.h"


namespace SS
{
namespace Core
{
namespace CommonServices
{

	CExceptionParams::CExceptionParams() : 
			m_eteTypeError(eteCritical),
			m_wsFunction(L""),
			m_wsFile(L""),
			m_lLine(NULL)
	{

	};

	CExceptionParams::CExceptionParams(
			///тип ошибки
			SS::Core::CommonServices::ETypeError eteTypeError,
			///имя функции
			std::wstring wsFunction,
			///имя файла
			std::wstring wsFile,
			///номер строки в файле
			long lLine) : 
			m_eteTypeError(eteTypeError),
			m_wsFunction(wsFunction),
			m_wsFile(wsFile),
			m_lLine(lLine)
	{

	};

	void CExceptionParams::SetMessage(wchar_t* wszMessage...)
	{
		m_wsMessage = L"";

		//извлечение аргументов
		va_list args;
		va_start(args,wszMessage);

		m_wsMessage.clear();
		m_wsMessage+=wszMessage;
		for(;;)
		{
			wchar_t* wszArg=va_arg(args, wchar_t*);
			if(!wszArg) break;
			m_wsMessage+=wszArg;
		};

		va_end(args);		
	};

	const std::wstring& CExceptionParams::GetMessage()
	{
		return m_wsMessage;
	};		

	void CExceptionParams::SetFunction(const std::wstring wsFunction)
	{
		m_wsFunction = wsFunction;
	};

	const std::wstring& CExceptionParams::GetFunction()
	{
		return m_wsFunction;		
	};

	void CExceptionParams::SetFile(const std::wstring wsFile)
	{
		m_wsFile = wsFile;
	};

	const std::wstring& CExceptionParams::GetFile()
	{
		return m_wsFile;		
	};

	void CExceptionParams::SetLine(const long lLine)
	{
		m_lLine = lLine;
	};

	const long CExceptionParams::GetLine()
	{
		return m_lLine;		
	};

	void CExceptionParams::SetTypeError(const SS::Core::CommonServices::ETypeError eteTypeError)
	{
		m_eteTypeError = eteTypeError;
	};

	const SS::Core::CommonServices::ETypeError CExceptionParams::GetTypeError()
	{
		return m_eteTypeError;		
	};

}
}
}