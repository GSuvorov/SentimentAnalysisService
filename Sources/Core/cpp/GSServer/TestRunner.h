#pragma once

#include "..\ASCInterface\IInterface.h"

namespace SS
{
namespace UnitTests
{
namespace CommonServices
{

//--------------------------------------------------------------------//

///управляет процессом запуска юнит тестов
class CTestRunner : public SS::Interface::UnitTests::ITestRunner
{
public:
	CTestRunner(void);
	virtual ~CTestRunner(void);

	///запуск тестов
	void Run();
	///приведение к интерфейсу с поданным REFIID. 
	/**pBase будет NULL, если данный интерфейс не поддерживается*/
	HRESULT QueryInterface(REFIID refIID, void** pBase);
	///освобождение
	ULONG Release();
};

//--------------------------------------------------------------------//

}
}
}


