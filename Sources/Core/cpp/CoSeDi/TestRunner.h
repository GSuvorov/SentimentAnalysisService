#pragma once

#include "..\ASCInterface\IInterface.h"

namespace SS
{
	namespace UnitTests
	{
		namespace CoSeDi
		{
			class CTestRunner : public SS::Interface::UnitTests::ITestRunner
			{
			public:
				CTestRunner(void);
				~CTestRunner(void);

				//запуск тестов
				void Run();
				//освобождение
				ULONG Release();
				//приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
				HRESULT QueryInterface(REFIID pIID, void** pBase); 

			};
		}
	}
}
