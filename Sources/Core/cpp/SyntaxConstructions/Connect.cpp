
#include "StdAfx.h"
#include "..\ASSInterface\ICoSeDi.h"
#include "..\ASSInterface\TBaseNames.h"
#include ".\connect.h"

namespace SS
{
	//Конструктор
	CConnectSQL::CConnectSQL(wchar_t* szBaseName) : m_pLingvoBaseManager(NULL), m_pConnection(NULL), m_pDataBase(NULL)
	{
		wcscpy(m_szBaseName, szBaseName);
	}

	//Деструктор
	CConnectSQL::~CConnectSQL()
	{
		CloseConnection();
	}
	
	///Устанавливает указатель на актиный менеджер соединений
	void CConnectSQL::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
		InitConnection();
	}
	
	//Закрытие соединений с базами
	void CConnectSQL::CloseConnection()
	{
		SS_TRY
		{
			if (m_pLingvoBaseManager)
			{
				m_pLingvoBaseManager->UnRegisterDeleteAndClose(m_szBaseName, &m_pConnection);
				m_pLingvoBaseManager->UnRegisterDeleteAndClose(m_szBaseName, &m_pDataBase);
			}
		}
		SS_CATCH(L"")
	}

	//Инициализация соединений с базами
	bool CConnectSQL::InitConnection()
	{
		SS_TRY
		{
			if (!m_pLingvoBaseManager)
			{
				m_pLingvoBaseManager = (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
				GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);
			}
			m_pLingvoBaseManager->CreateOpenAndRegister(m_szBaseName, &m_pConnection);
			m_pLingvoBaseManager->CreateOpenAndRegister(m_szBaseName, &m_pDataBase);
		}
		SS_CATCH(L"")
		return (m_pConnection && m_pDataBase);
	}

	//********************************************* DBMS ********************************************

	//Конструктор
	CConnectDBMS::CConnectDBMS(wchar_t* szBaseName) : m_pLingvoBaseManager(NULL), m_pDTable(NULL), m_pDataBase(NULL)
	{
		wcscpy(m_szBaseName, szBaseName);
	}

	//Деструктор
	CConnectDBMS::~CConnectDBMS()
	{
		CloseConnection();
	}
	
	//Прервать соединение с DBMS базой (собственный метод)
	void CConnectDBMS::CloseConnection()
	{
		SS_TRY;
		{
			std::map<std::wstring, SS::Interface::Core::DBMS::IDataTable*>::iterator itTable;
			for (itTable = m_mapTables.begin(); itTable != m_mapTables.end(); itTable++)
			{ itTable->second->Close();	}
			
			if (m_pLingvoBaseManager)
			{ m_pLingvoBaseManager->UnRegisterDeleteAndClose(m_szBaseName, &m_pDataBase); }
		}
		SS_CATCH(L"");	
	}

	//Установка соединения с базой DBMS	(собственный метод)
	void CConnectDBMS::InitConnection()
	{
		SS_TRY
		{
			m_pLingvoBaseManager = (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
			GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);
			
			if ( !m_pLingvoBaseManager ) 
			{ SS_THROW(L"Не удалось получить LingvoBaseManager"); }
			
			m_pLingvoBaseManager->CreateOpenAndRegister(m_szBaseName, &m_pDataBase);

			if ( !m_pDataBase )
			{ SS_THROW(L"Не удалось получить базу"); }
		}
		SS_CATCH(L"");
	}

	//Установить соединение с таблицой
	void CConnectDBMS::SetTable(std::wstring & wTableName)
	{
		SS_TRY
		{
			std::map<std::wstring, SS::Interface::Core::DBMS::IDataTable*>::iterator itTable;
			
			itTable = m_mapTables.find(wTableName);
			
			if ( itTable == m_mapTables.end() ) 
			{
				m_pDataBase->MoveToDataTable(wTableName.c_str());
				m_pDTable = m_pDataBase->OpenDataTable();
				if ( (m_pDTable == NULL) || (!m_pDTable->IsOpen()) )
				{ SS_THROW(L"Не удалось получить таблицу"); }
				m_mapTables.insert( std::map<std::wstring, SS::Interface::Core::DBMS::IDataTable*>::value_type(wTableName, m_pDTable) );
			}
			else
			{
				m_pDTable = itTable->second;	
			}
		}
		SS_CATCH(L"")
	}

	///Установить LoadManager (перегруженый метод)
	void CConnectDBMS::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		SS_TRY
		{
			//Установка LoadManager
			SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
			//Получение LingvoBaseManager и установка соед с базой
			InitConnection();
		}
		SS_CATCH(L"")
	};

}