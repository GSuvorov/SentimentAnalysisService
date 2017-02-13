#pragma once

#include "../ASCInterface/IInterface.h"
#include "./BaseManagerTest.h"

namespace SS
{
namespace UnitTests
{
namespace ResourceManagers
{
class CTestRunner : public SS::Interface::UnitTests::ITestRunner
{
private:
	CBaseManagerTest* m_pBaseManagerTest;
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