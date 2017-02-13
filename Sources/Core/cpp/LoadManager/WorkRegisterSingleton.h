#pragma	once

#include "..\ASCInterface\ICommonServices.h"
#include "..\ASCInterface\services.h"
#include "WorkRegister.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{


	/// Предоставляет внешним пользователя функционал работы с настройками
	class CWorkRegisterSingleton : public SS::Interface::Core::CommonServices::IWorkRegister,
		SS::Core::CommonServices::CSingle<CWorkRegister>
	{

	public:

		CWorkRegisterSingleton();
		~CWorkRegisterSingleton();

		///возвращает значение параметра по его имени (юникод версия)
		std::wstring GetParameter(std::wstring wParametrName);

		///установка рабочей директории
		void SetWorkDir(std::wstring wPathToDir);

		///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
		HRESULT QueryInterface(REFIID pIID, void** ppBase); 

		///освобождение
		ULONG Release();

	private:

		void CreateSSDirectories();
		
		bool CreateSSDirectory(LPCWSTR szPath, LPCWSTR szDir);

	};

}
}
}



