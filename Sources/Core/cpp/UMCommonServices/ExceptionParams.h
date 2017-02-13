#pragma once

#include "..\ASCInterface\ICommonServices.h"
#include "..\ASCInterface\TListEnumerator.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{

	class CExceptionParams : public SS::Interface::Core::CommonServices::IExceptionParams
	{
		///сообщение об ошибке
		std::wstring m_wsMessage;
		///тип ошибки
		SS::Core::CommonServices::ETypeError m_eteTypeError;
		///имя функции
		std::wstring m_wsFunction;
		///имя файла
		std::wstring m_wsFile;
		///номер строки в файле
		long m_lLine;

	public:

		CExceptionParams();
		CExceptionParams(
			///тип ошибки
			SS::Core::CommonServices::ETypeError eteTypeError,
			///имя функции
			std::wstring wsFunction,
			///имя файла
			std::wstring wsFile,
			///номер строки в файле
			long lLine);

		///установит сообщение об ошибке
		void SetMessage(wchar_t* wszMessage...);
		///вернет сообщение об ошибке
		const std::wstring& GetMessage();		

		///установит имя функции, вызвавшей ошибку
		void SetFunction(const std::wstring wsFunction);
		///вернет имя функции, вызвавшей ошибку
		const std::wstring& GetFunction();

		///установит имя файла с кодом
		void SetFile(const std::wstring wsFile);
		///вернет имя файла с кодом
		const std::wstring& GetFile();

		///установит номер строки в файле с кодом 
		void SetLine(const long lLine);
		///вернет номер строки в файле с кодом 
		const long GetLine();

		///установит тип ошибки
		void SetTypeError(const SS::Core::CommonServices::ETypeError eteTypeError);
		///вернет тип ошибки
		const SS::Core::CommonServices::ETypeError GetTypeError();
	};
}
}
}
