#pragma once

#include "..\ASCInterface\IDBMS.h"
#include "ICommon.h"
#include <atldbcli.h>


///// guid для класса, поддерживающего ILingvoBaseManager (модуль LingvoBaseManager)
//static const GUID CLSID_LinguoBaseManager = 
//{ 0xba24bf91, 0xfe10, 0x4729, { 0x9b, 0xa4, 0x16, 0x59, 0x24, 0x43, 0x21, 0xff } };

// guid для интерфейса ILingvoBaseManager
static const GUID IID_LingvoBaseManager = 
{ 0x54aec5c1, 0x39e9, 0x4bc0, { 0xa3, 0xdb, 0xca, 0xd2, 0xa, 0x77, 0xb, 0x85 } };

// {8C09D01A-9361-4069-BF0F-28BFF80BFE49}
static const GUID CLSID_ConnectionManager = 
{ 0x8c09d01a, 0x9361, 0x4069, { 0xbf, 0xf, 0x28, 0xbf, 0xf8, 0xb, 0xfe, 0x49 } };

// {3D95BDDF-5B78-431b-8B54-603C10186F04}
static const GUID IID_ConnectionManager = 
{ 0x3d95bddf, 0x5b78, 0x431b, { 0x8b, 0x54, 0x60, 0x3c, 0x10, 0x18, 0x6f, 0x4 } };


namespace SS
{
namespace Interface
{
namespace Core
{
namespace ResourceManagers
{

	class ISubConnection;
	class CConnection;

	/// общий интерфейс
	/** обобщает концепцию "закрыть" */
	class ICommon
	{
	public:

		
		/*! \fn Close
		*  \brief  закрываем все
		*/
		virtual void Close()=0;

		virtual ~ICommon(){};
	};

	/// управляет соединением с SQL-базой
	/** обеспечивает хранение и управление соединение с SQL-базой */
	class IConnection
	{
	public:

		/// Открывает соединение с sql-базой 
		/** 
			\param wszName Имя пользователя
			\param wszPassword Пароль пользователя
			\param wszLocation Название базы
			\param wszDataSource Название источника данных (Имя SQL-сервера)
			\return bool true, если соединение прошло успешно
			
		Попытка открыть соединение с SQL - базой
		*/
		virtual bool Open(wchar_t* wszName, wchar_t* wszPassword, wchar_t* wszLocation, wchar_t* wszDataSource)=0;

		/// Закрывает соединение 
		/** 
			
		Обеспечивает закрытие соединения с SQL-базой 
		*/
		virtual void Close()=0;


		/// Проверяет, открыто ли соединение
		/** 
			\return bool если true, то соединение открыто
			
		Проверяет, открыто ли соединение с SQL-базой 
		*/
		virtual bool IsOpen()=0;

		/// Возвращает сессию соединения 
		/** 
			\return ATL::CSession* сессия соединения
		
		*/
		virtual ATL::CSession* GetSession()=0;

		/// Переоткрывает сессию текущего соединения 
		/** 
			\return bool true, если переоткрытие прошло успешно
			
		Для переоткрытия используется текущее соединение
		*/
		virtual bool ReOpenSession()=0;

		/// Возвращает число ссылок на сессию 
		/** 
			\return int число ссылок на сессию
			
		Используется для контроля (пока число ссылок не равно нулю, закрыть сессию не удастся)
		*/
		virtual int CountSessionRef()=0;

		/// Уничтожает соединение 
		/** 
			\return ULONG смысловой нагрузки не несет
			
		просто delete this выполняется 
		*/
		virtual ULONG Release()=0;

		/// деструктор сделан виртуальным на всякий случай, вдруг кому-нибудь захочется сделать delete по указателю на данный интерфейс
		virtual ~IConnection(){};
	};

	/// Определяет подсоединение
	/** Подсоединение относится к какому-нибудь соединению с SQL-базой и необходимо
	для того, чтобы закрыть текущее соединение*/
	class ISubConnection 
	{
	public:

		/// Открывает подсоединение
		/** 
			\param pConnection - соединение, которому принадлежит подсоединение
			\return bool - если true, открытие прошло успешно
		*/
		virtual bool SubOpen(IConnection* pConnection)=0;
		
		/// Закрывает подсоединение 
		virtual void SubClose()=0;

		virtual ~ISubConnection(){};
	};

	class ILingvoBaseManager : public virtual IBase
	{
	public:
		///// Закрывает базу определенного типа из определенной группы 
		///** 
		//	\param BaseType - тип базы, (морфологическая,  и т.д.) \see SS::MANAGER::Constants
		//	\param BaseGroup - группа, к которой относится база (основанная на DBMS и SQL-базы) \see SS::MANAGER::Constants
		//*/
		//virtual void Close(const wchar_t* BaseType, const wchar_t* BaseGroup)=0;

		///// Открывает базу определенного типа из определенной группы  
		///** 
		//	\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		//	\param BaseGroup - группа, к которой относится база (основанная на DBMS и SQL-базы) \see SS::MANAGER::Constants
		//	\return bool - если true, то база открыта
		//*/
		//virtual bool Open(const wchar_t* BaseType, const wchar_t* BaseGroup)=0;

		///// Закрывает определенный тип баз из всех групп
		///** 
		//	\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		//*/
		//virtual void Close(const wchar_t* BaseType)=0;

		///// Открывает определенный тип баз
		///** 
		//	\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		//	\return bool - если true, то база открыта
		//*/
		//virtual bool Open(const wchar_t* BaseType)=0;

		/// Закрывает существующие открытые соединения и подсоединения
		virtual void Close()=0;

		/// Открывает существующие, но закрытые соединения и подсоединения
		/** 
			\return bool - если true, то открытие прошло успешно
		*/
		virtual bool Open()=0;

		/// Возвращает имя активного хранилища
		/** 
			\return const wchar_t* ссылка на имя активного хранилища
		*/
		virtual const wchar_t* GetNameActiveStorage()=0;

		///// Задает имя активного хранилища
		///** 
		//	\param StorageName имя активного хранилища (нечто типа Ind70ZMDF045DF...)
		//	\return bool - true, если активное хранилище успешно задано
		//*/
		//virtual bool OpenStorage(const wchar_t* StorageName)=0;

		///// Добавление SubConnection 
		///** 
		//	\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		//	\param pSubConnection - подсоединение, \see ISubConnection
		//	
		//Добавляет и региструет подсоединение с целью беспрепятственного 
		//(при необходимости) закрытия текущей сессии
		//*/
		//virtual void Register(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection)=0;

		///// Удаление SubConnection
		///** 
		//	\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
		//	\param pSubConnection - подсоединение, \see ISubConnection
		//	
		//Удаляет ранее зарегистрированное подсоединение 
		//*/
		//virtual void UnRegister(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection)=0;

		/// Дерегистрация ссылки на базу DBMS-группы \see SS::MANAGER::Constants
		/** 
			\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - интерфейс по работе с базой DBMS
			
		Если число ссылок равно нулю, база закрывается и уничтожается сущность, работающая с ней 
		*/
		virtual void UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection) = 0;

		/// Создание сущности, олицетворяющей собой базу определенного типа и возврат нужного интерфейса с регистрацией
		/** 
			\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - интерфейс по работе с базой DBMS
			
		Создаем интерфейс класса (если класс еще не создан, в противном случае используем уже созданный),
		олицетворяющего собой базу определенного типа DBMS-группы и записываем указатель на него в переданный указатель 
		*/
		virtual void CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection)=0;

		/// Создание сущности, олицетворяющей собой соединение определенного типа и возврат нужного интерфейса с регистрацией
		/** 
			\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - интерфейс для работы с соединением с SQL-базой
			
		Создаем интерфейс класса (если класс еще не создан, в противном случае используем уже созданный),
		олицетворяющего собой соединение определенного типа SQL-группы и записываем указатель на него в переданный указатель 
		*/
		virtual void CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection)=0;


		/// Дерегистрация ссылки на соединение SQL-группы \see SS::MANAGER::Constants
		/** 
			\param BaseType - тип базы, (морфологическая и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - интерфейс по работе с базой SQL
			
		Если число ссылок равно нулю, соединение закрывается и уничтожается сущность, работающая с ней 
		*/
		virtual void UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection)=0;


		virtual ~ILingvoBaseManager(){};
	};



	/// менеджер соединений
	class IConnectionManager 
		: public virtual IBase
		, public SS::Interface::Core::Common::IConnection
	{
	public:
		virtual void Register(SS::Interface::Core::Common::IConnection * pConnection) = 0;
	};



}
}
}
}
