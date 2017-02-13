#pragma once

#include "types.h"
#include "baseconnectionsmanager.h"
#include "..\ASCInterface\idbms.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

/// Реализует функциональность работы с DBMS - базами
class CDBMSManager 
	: public CBaseConnectionsManager<SS::Interface::Core::DBMS::IDataBase>
	, public SS::Core::CommonServices::CBaseCoreClass
{
protected:

	///Коннекты к DBMS-базе (строковое имя + сам коннект)
	SS::Core::ResourceManagers::Types::THMDBMSConnections m_hmdbmsConnections;

	///менеджер DBMS-баз (постоянных)
	SS::Interface::Core::DBMS::IDBManager * m_pConstDBManager;

	///менеджер DBMS-баз (NDX)
	SS::Interface::Core::DBMS::IDBManager * m_pNDXDBManager;

	/// Путь к базам
	std::wstring m_wsDataBasePath;
	/// Путь к индексным базам
	std::wstring m_wsITDataBasePath;

protected:
	/// Инициализируем переменные класса
	void Init(void);

	/// Открываем базу требуемого типа в текущем хранилище 
	/** 
		\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
		\param ptssConnection - соединение с DBMS - базой
		\return bool - если соединение открылась, true
	*/
	bool Open(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection);

	/// Закрывает соединение, на которое указывает итератор 
	/** 
		\param it_Connection - итератор, указывающий на соединение
	*/
	void Close(SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_Connection);

	/// Очищает базу, на которую указывает итератор 
	/** 
		\param it_Connection - итератор, указывающий на соединение
	*/
	void Clear(SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_Connection);

	/// Открывает соединение, на которое указывает итератор 
	/** 
		\param it_Connection - итератор, указывающий на соединение
	*/
	bool Open(SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_Connection);

	/// Инициализируется DBMS - база 
	/** 
		\param sDataBaseInit - путь к ини-файлу, в котором описана структура базы
		\param sDataBaseRoot - путь к папке, в которой находится база
		\param ppDBManager - менеджер баз, с помощью которого происходит инициализация
	*/
	void CreateAndInit(std::wstring sDataBaseInit, std::wstring sDataBaseRoot, SS::Interface::Core::DBMS::IDBManager** ppDBManager);

	void GetFileNamesFromDirectory(/*in*/std::pair<std::wstring, std::wstring>& DirectoryPaths, /*out*/std::vector< std::pair<std::wstring, std::wstring> >* vPathes);

public:
	/// конструктор
	CDBMSManager();
	/// Закрытие всех соединений, уничтожение менеджеров баз
	~CDBMSManager();

public:
	/// устанавливает указатель на лоад мэнеджер
	void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

public:
	/// Очищает определенный тип баз из DBMS - группы
	/** 
		\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
	*/
	bool Clear(const wchar_t* BaseType);
	/// устанавливает путь к индексным базам данных
	void SetITDataBasePath(const wchar_t * wszITDataBasePath);

	/// Закрываются все соединения 
	void Close();

	/// Закрывает определенный тип баз из DBMS - группы
	/** 
		\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
	*/
	void Close(const wchar_t* BaseType);

	/// Открывает существующие, но закрытые соединения
	/** 
		\return bool - если true, то открытие прошло успешно
	*/
	bool Open();

	/// Открывает определенный тип баз
	/** 
		\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
		\return bool - если true, то база открыта
	*/
	bool Open(const wchar_t* BaseType);

	/// Задает имя активного хранилища
	/** 
		\param StorageName имя активного хранилища (нечто типа Ind70ZMDF045DF...)
		\return bool - true, если активное хранилище успешно задано
	*/
	bool OpenStorage(const wchar_t* StorageName);

	/// Создание сущности, олицетворяющей собой базу определенного типа и возврат нужного интерфейса с регистрацией
	/** 
		\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
		\param ptssConnection - интерфейс по работе с базой DBMS
		
	Создаем интерфейс класса (если класс еще не создан, в противном случае используем уже созданный),
	олицетворяющего собой базу определенного типа DBMS-группы и записываем указатель на него в переданный указатель 
	*/
	void CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection);

	/// Дерегистрация ссылки на базу DBMS-группы \see SS::MANAGER::Constants
	/** 
		\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
		\param ptssConnection - интерфейс по работе с базой DBMS
		
	Если число ссылок равно нулю, база закрывается и уничтожается сущность, работающая с ней 
	*/
	void UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection);

	/// заполнит вектор путями к файлам базы (необходимо для бакапа)
	void GetDatabaseFilePathes(std::vector< std::pair<std::wstring, std::wstring> >* vPathes, std::wstring& sBaseName);

};

}
}
}
