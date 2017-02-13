#pragma once

#ifdef _SS_UNITTESTS

#include "../ASCInterface/IInterface.h"

namespace SS
{
	namespace UnitTests
	{
		namespace Syntax
		{
			class CTestRunner : public SS::Interface::UnitTests::ITestRunner
			{
			public:
				//КОНСТРУКТОРЫ
				CTestRunner();
				~CTestRunner();
			
				//запуск тестов
				virtual void Run();
				//освобождение
				virtual ULONG Release();
				//приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
				virtual HRESULT QueryInterface( REFIID pIID, void** pBase ); 

			};
		}
	}
}

#endif