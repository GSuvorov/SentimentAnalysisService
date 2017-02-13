//TestRunner.cpp

#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include "TestRunner.h"
#include "./UnitTests/SyntaxTest.h"


namespace SS
{
	namespace UnitTests
	{
		namespace Syntax
		{

			//Конструктор
			CTestRunner::CTestRunner()
			{
			}
			//Деструктор
			CTestRunner::~CTestRunner()
			{
			}

			//Другие методы
			void CTestRunner::Run()
			{
				SS::UnitTests::Syntax::CSyntaxTest oSyntaxTest(GetLoadManager());
				oSyntaxTest.TestCommon();
				//oSyntaxTest.IndexationSpeedTest();
				//oSyntaxTest.TestContainer();
			}

			ULONG CTestRunner::Release()
			{
				delete this;
				return 0;
			}

			HRESULT CTestRunner::QueryInterface( REFIID pIID, void** pBase )
			{
				if (pIID == IID_TestRunner)
				{
					*pBase = static_cast<SS::Interface::UnitTests::ITestRunner*> (this);
				}
				else if (pIID == IID_Base)
				{
					*pBase = static_cast<SS::Interface::IBase*> (this);
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

#endif
