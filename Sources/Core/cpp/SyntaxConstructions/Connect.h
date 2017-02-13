#pragma once
#include <map>

#include "../ASCInterface/IDBMS.h"
#include "../ASSInterface/ILingvoBaseManager.h"
#include "../ASCInterface/IBaseManager.h"

namespace SS
{
	/// класс отвечающий за соединение с SQL базой
	class CConnectSQL : public SS::Core::CommonServices::CBaseCoreClass
	{
	private:
		///Инициализация соединений с базами				
		bool InitConnection();
		///Закрытие соединений с базами
		void CloseConnection();
	
	protected:
		///Cодержит имя базы данных
		wchar_t m_szBaseName[100];
		SS::Interface::Core::ResourceManagers::ILingvoBaseManager*	m_pLingvoBaseManager;
		SS::Interface::Core::ResourceManagers::IConnection*			m_pConnection;
		///Класс обеспечивающий соединение с DBMS базой
		SS::Interface::Core::DBMS::IDataBase*						m_pDataBase;
	
	public:
		///Конструктор
		CConnectSQL(wchar_t* szBaseName);
		///Деструктор
		virtual ~CConnectSQL();
		///Устанавливает указатель на актиный менеджер соединений
		void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	};

	///Класс отвечающий за соединение с DBMS базой
	class CConnectDBMS : public SS::Core::CommonServices::CBaseCoreClass
	{
	private:
		///Хранилище таблиц для базы
		std::map<std::wstring, SS::Interface::Core::DBMS::IDataTable*> m_mapTables;

	private:
		///Установка соединения с базой DBMS			
		void InitConnection();
		///Закрытие соединения с базой DBMS			
		void CloseConnection();
	
	protected:
		///Содержит имя базы данных
		wchar_t m_szBaseName[100];
		///Указатель на текущую DBMS таблицу
		SS::Interface::Core::DBMS::IDataTable*						m_pDTable;
		///Указатель на базу DBMS
		SS::Interface::Core::DBMS::IDataBase*						m_pDataBase;
		///Класс обеспечивающий соединение с DBMS базой
		SS::Interface::Core::ResourceManagers::ILingvoBaseManager*	m_pLingvoBaseManager;

	protected:
		///Конструктор
		CConnectDBMS(wchar_t* szBaseName);
		///Деструктор
		virtual ~CConnectDBMS();
		///Установить соединение с таблицой
		void SetTable(std::wstring & wTableName);
	
	public:	
		///Установить LoadManager (перегруженый метод)
		void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	};

};
