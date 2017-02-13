//TestRunner.cpp
#include "StdAfx.h"
#include "./testrunner.h"
#include "SyntaxConstructionsTest.h"
#include "./FillSQL.h"

namespace SS
{
	namespace UnitTests
	{
		namespace SyntaxConstructions
		{

			//Запуск тестов
			void CTestRunner::Run()
			{
				CoInitialize(NULL);				
				#ifdef _SS_UNITTESTS
				{
					//Объект с тестами
					//CSyntaxConstructionsTest oSyntaxConstructionsTest(GetLoadManager());
					//oSyntaxConstructionsTest.TestToBinary();
					CFillSQL oFillSQL;
					oFillSQL.SetLoadManager(GetLoadManager());
					oFillSQL.Fill();
					//Задержка
					std::cout << "press key";
					char ch;
					ch = getchar();
				}
				#endif
				CoUninitialize();
			}

			//Освободить память
			ULONG CTestRunner::Release()
			{
				delete this;
				return 0;
			}

			//Получить интерыейс
			HRESULT CTestRunner::QueryInterface(REFIID pIID, void** pBase)
			{
				if(pIID == IID_TestRunner)
				{
					*pBase = (SS::Interface::UnitTests::ITestRunner*)this;
				}
				else if (pIID == IID_Base)
				{
					*pBase = (SS::Interface::IBase*)( this );
				}
				else
				{
					*pBase = NULL;
				}
				return S_OK;
			}

		}
	}
}