#pragma once

#include "baseconnectionsmanager.h"
#include "..\ASSInterface\ILingvoBaseManager.h"
#include "types.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

	/// Реализует функциональность управления, регистрацией соединениями с SQL-базой
	class CSQLManager :
		public CBaseConnectionsManager<SS::Interface::Core::ResourceManagers::IConnection>,
		public SS::Core::CommonServices::CBaseCoreClass
	{

		///Коннекты к SQL-базе (строковое имя + сам коннект)
		SS::Core::ResourceManagers::Types::THMConnections m_hmConnections;

		///объекты, зависящие от подключений
		SS::Core::ResourceManagers::Types::THMSubConnections m_hmSubConnections;

	protected:

		/// Открываем соединение с базой требуемого типа в текущем хранилище 
		/** 
			\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - соединение с SQL - базой
			\return bool - если соединение открылась, true
		*/
		bool Open(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection);

		/// Закрывает соединение (а также все его подсоединения), на которое указывает итератор 
		/** 
			\param it_Connection - итератор, указывающий на соединение
		*/
		void Close(SS::Core::ResourceManagers::Types::THMConnections::iterator it_Connection);

		/// Открывает соединение (а также все его подсоединения), на которое указывает итератор 
		/** 
			\param it_Connection - итератор, указывающий на соединение
		*/
		bool Open(SS::Core::ResourceManagers::Types::THMConnections::iterator it_Connection);

	public:
		CSQLManager();

		/// При удалении закрываются все соединения и их подсоединения 
		~CSQLManager();

		/// Закрываются все соединения и их подсоединения 
		void Close();

		/// Закрывает определенный тип баз из SQL - группы
		/** 
			\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
		*/
		void Close(const wchar_t* BaseType);

		/// Открывает существующие, но закрытые соединения и подсоединения
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

		/// Добавление SubConnection 
		/** 
			\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
			\param pSubConnection - подсоединение, \see ISubConnection
			
		Добавляет и региструет подсоединение с целью беспрепятственного 
		(при необходимости) закрытия текущей сессии
		*/
		void Register(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection);

		/// Удаление SubConnection
		/** 
			\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
			\param pSubConnection - подсоединение, \see ISubConnection
			
		Удаляет ранее зарегистрированное подсоединение 
		*/
		void UnRegister(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection);

		/// Создание сущности, олицетворяющей собой соединение определенного типа и возврат нужного интерфейса с регистрацией
		/** 
			\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - интерфейс для работы с соединением с SQL-базой
			
		Создаем интерфейс класса (если класс еще не создан, в противном случае используем уже созданный),
		олицетворяющего собой соединение определенного типа SQL-группы и записываем указатель на него в переданный указатель 
		*/
		void CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection);

		/// Дерегистрация ссылки на соединение SQL-группы \see SS::MANAGER::Constants
		/** 
			\param BaseType - тип базы, (морфологическая, поисковая, сервисная и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - интерфейс по работе с базой SQL
			
		Если число ссылок равно нулю, соединение закрывается и уничтожается сущность, работающая с ней 
		*/
		void UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection);
	};
}
}
}