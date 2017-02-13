#pragma once
#include "..\ASCInterface\iinterface.h"
#include "./tst_ParametrizedQuery.h"

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxQuery
{

//--------------------------------------------------------------------//

///	Управляет процессом запуска юнит тестов
class CTestRunner : public SS::Interface::UnitTests::ITestRunner
{
public:
	///	Конструктор
	CTestRunner(void);

	///	Деструктор
	virtual ~CTestRunner(void);

	///	Запуск тестов
	virtual void Run();

	///	Приведение к интерфейсу с поданным REFIID. 
	/**	pBase будет NULL, если данный интерфейс не поддерживается*/
	virtual HRESULT QueryInterface(REFIID refIID, void** pBase);

	///	Освобождение
	virtual ULONG Release();
};

//--------------------------------------------------------------------//

}
}
}
}

