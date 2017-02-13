#pragma once

#ifdef _SS_UNITTESTS

#include "../ASCInterface/IInterface.h"
#include "./UnitTests/ContainersTest.h"

namespace SS
{
	namespace UnitTests
	{
		namespace CommonContainers
		{
			class CTestRunner : public SS::Interface::UnitTests::ITestRunner
			{
			private:
				CContainersTest* m_pContainersTest;
			public:
				//КОНСТРУКТОРЫ
				CTestRunner();
				~CTestRunner();
				//МЕТОДЫ БАЗОВЫХ КЛАССОВ
				virtual void Run();					//запуск тестов
				virtual ULONG Release();			//освобождение
				virtual HRESULT QueryInterface( REFIID pIID, void** pBase );	//приведение к интерфейсу с поданным REFIID. 
																				//pBase будет NULL, если данный интерфейс не поддерживается
			};
		}
	}
}

#endif //_SS_UNITTESTS