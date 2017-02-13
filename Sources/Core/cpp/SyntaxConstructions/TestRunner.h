//TestRunner.h
#pragma once
#include "../ASCInterface/IInterface.h"

namespace SS
{
	namespace UnitTests
	{
		namespace SyntaxConstructions
		{
			class CTestRunner : public SS::Interface::UnitTests::ITestRunner
			{
			public:
				///Конструктор
				CTestRunner(){};
				///Деструктор
				~CTestRunner(){};
				///Запуск тестов
				void Run();
				///Освобождение
				ULONG Release();
				///Приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
				HRESULT QueryInterface(REFIID pIID, void** pBase); 
			};
		}
	}
}
