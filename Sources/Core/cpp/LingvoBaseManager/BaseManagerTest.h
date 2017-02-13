#pragma once

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\IBaseManager.h"
#include "..\ASSInterface\ILingvoBaseManager.h"

#include "../ASCInterface/IDBMS.h"

#include ".\consts.h"
#include "..\ASSInterface\TBaseNames.h"

namespace SS
{
namespace UnitTests
{

namespace ResourceManagers
{

struct SStruct
{
	int IDSource;
	int m_IDType;
	std::wstring wsSource;
	std::wstring wsSourceLower;
};


class CBaseManagerTest : public SS::Core::CommonServices::CBaseCoreClass
{
	SS::Interface::Core::ResourceManagers::IBaseManager*			m_pBaseManager;
	SS::Interface::Core::ResourceManagers::ILingvoBaseManager*	m_pLingvoBaseManager;

	// открываем базу
	SS::Interface::Core::DBMS::IDataBase* m_pDataBase;

public:
	CBaseManagerTest(void);
	~CBaseManagerTest(void);
public:
	void Run(void);
private:
	void AddData();
	void Init(void);


};
}
}
}