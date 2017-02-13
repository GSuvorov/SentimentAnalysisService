#pragma once

#ifdef _SS_UNITTESTS

#include "../ASCInterface/iinterface.h"

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{

//--------------------------------------------------------------------//

//вспомогательные функции для тестов
class CUnitTestsUsings
{
public:
	CUnitTestsUsings(void):m_hLoadManager(NULL), m_pLoadManager(NULL){};
	virtual ~CUnitTestsUsings(void){DeleteLoadManager();};

protected:
	///возвращает менеджер загрузки
	virtual SS::Interface::Core::ResourceManagers::ILoadManager* GetLoadManager(void){if(!m_pLoadManager) CreateLoadManager(); return m_pLoadManager;};
	///возвращает путь к рабочей директории юнит тестов
	const wchar_t* GetUTWorkingPath(void);

private:
	///создает менеджер загрузки
	bool CreateLoadManager(void);
	///удаляет менеджер загрузки
	void DeleteLoadManager(void);

	///хэндл модуля загрузки
	HMODULE m_hLoadManager;
	///интерфейс к менеджеру загрузки
	SS::Interface::Core::ResourceManagers::ILoadManager* m_pLoadManager;
	///путь к рабочей директории юнит тестов
	wstring wszUTWorkingPath;
};

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
