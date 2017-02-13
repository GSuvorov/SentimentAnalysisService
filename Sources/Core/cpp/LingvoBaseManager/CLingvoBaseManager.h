#pragma once


#include "..\ASSInterface\ILingvoBaseManager.h"
#include "..\ASCInterface\IBaseManager.h"

#include ".\BaseManager.h"



namespace SS
{
namespace Core
{
namespace ResourceManagers
{

class CLingvoBaseManager 
	: public CBaseManager /*public SS::Interface::Core::ResourceManagers::IBaseManager*/
	, public SS::Interface::Core::ResourceManagers::ILingvoBaseManager
{

public:
	CLingvoBaseManager();
public:
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** ppBase); 

	///освобождение
	ULONG Release();

public:
	/// устанавливает указатель на лоад мэнеджер
	void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

	/// Закрывает базу определенного типа из определенной группы 
	/** 
		\param BaseType - тип базы, (морфологическая,  и т.д.) \see SS::MANAGER::Constants
		\param BaseGroup - группа, к которой относится база (основанная на DBMS и SQL-базы) \see SS::MANAGER::Constants
	*/
	void Close(const wchar_t* BaseType, const wchar_t* BaseGroup);

	/// Открывает базу определенного типа из определенной группы  
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		\param BaseGroup - группа, к которой относится база (основанная на DBMS и SQL-базы) \see SS::MANAGER::Constants
		\return bool - если true, то база открыта
	*/
	bool Open(const wchar_t* BaseType, const wchar_t* BaseGroup);

	/// Закрывает определенный тип баз из всех групп
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
	*/
	void Close(const wchar_t* BaseType);

	/// Открывает определенный тип баз
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		\return bool - если true, то база открыта
	*/
	bool Open(const wchar_t* BaseType);

	/// Закрывает существующие открытые соединения и подсоединения
	void Close();

	/// Открывает существующие, но закрытые соединения и подсоединения
	/** 
		\return bool - если true, то открытие прошло успешно
	*/
	bool Open();

	/// Возвращает имя активного хранилища
	/** 
		\return const wchar_t* ссылка на имя активного хранилища
	*/
	const wchar_t* GetNameActiveStorage();

	/// Задает имя активного хранилища
	/** 
		\param StorageName имя активного хранилища (нечто типа Ind70ZMDF045DF...)
		\return bool - true, если активное хранилище успешно задано
	*/
	bool OpenStorage(const wchar_t* StorageName);

	/// Добавление SubConnection 
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		\param pSubConnection - подсоединение, \see ISubConnection
		
	Добавляет и региструет подсоединение с целью беспрепятственного 
	(при необходимости) закрытия текущей сессии
	*/
	void Register(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection);

	/// Удаление SubConnection
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		\param pSubConnection - подсоединение, \see ISubConnection
		
	Удаляет ранее зарегистрированное подсоединение 
	*/
	void UnRegister(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection);

	/// Дерегистрация ссылки на базу DBMS-группы \see SS::MANAGER::Constants
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		\param ptssConnection - интерфейс по работе с базой DBMS
		
	Если число ссылок равно нулю, база закрывается и уничтожается сущность, работающая с ней 
	*/
	void UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection);

	/// Создание сущности, олицетворяющей собой соединение определенного типа и возврат нужного интерфейса с регистрацией
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		\param ptssConnection - интерфейс для работы с соединением с SQL-базой
		
	Создаем интерфейс класса (если класс еще не создан, в противном случае используем уже созданный),
	олицетворяющего собой соединение определенного типа SQL-группы и записываем указатель на него в переданный указатель 
	*/
	void CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection);

	/// Создание сущности, олицетворяющей собой базу определенного типа и возврат нужного интерфейса с регистрацией
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		\param ptssConnection - интерфейс по работе с базой DBMS
		
	Создаем интерфейс класса (если класс еще не создан, в противном случае используем уже созданный),
	олицетворяющего собой базу определенного типа DBMS-группы и записываем указатель на него в переданный указатель 
	*/
	void CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection);

	/// Дерегистрация ссылки на соединение SQL-группы \see SS::MANAGER::Constants
	/** 
		\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		\param ptssConnection - интерфейс по работе с базой SQL
		
	Если число ссылок равно нулю, соединение закрывается и уничтожается сущность, работающая с ней 
	*/
	void UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection);

};

}
}
}

