#pragma once

#include "CBaseLoadManager.h"
#include "types.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

		/// Реализует продвинутую функциональность по созданию классов
		/** Регистрирует и дерегистрирует все созданные классы 
		(а также выданные интерфейсы данных классов), загруженные модули*/
		class CModulesManager : public CBaseLoadManager
		{

		protected:
			///отображение всех библиотек, классов, интерфейсов, необходимых для работы
			SS::Core::ResourceManagers::Types::TMWorkLibraries m_mWorkLibraries;

			///создаем либо находим нужный модуль
			/** 
				\param CoreLibrary - идентификатор нужного модуля
				\return SS::Core::ResourceManagers::Types::TMWorkLibraries::iterator - итератор на нужный модуль
			*/
			SS::Core::ResourceManagers::Types::TMWorkLibraries::iterator FindOrCreateLibrary(TCoreLibrary CoreLibrary);

			///создаем либо находим нужный класс в нужном модуле
			/** 
				\param CoreLibrary - идентификатор модуля 
				\param guidEntity - идентификатор сущности(класса)
				\param iidInterface - идентификатор интерфейса
				\param ptssBase - ссылка на переменную, где хранится указатель на интерфейс
				\return SS::Core::ResourceManagers::Types::TMWorkEntities::iterator - итератор на нужную сущность		 
			*/
			SS::Core::ResourceManagers::Types::TMWorkEntities::iterator FindOrCreateEntity(TCoreLibrary CoreLibrary, SS::Core::ResourceManagers::Types::TMWorkLibraries::iterator& it_Library, const GUID& guidEntity, void** ptssBase, const wchar_t* wszPath);

			///создаем либо находим нужный интерфейс в нужном классе
			/** 
				\param guidEntity - идентификатор сущности(класса)
				\param iidInterface - идентификатор интерфейса
				\param ptssBase - ссылка на переменную, где хранится указатель на интерфейс
				\return SS::Core::ResourceManagers::Types::TMWorkInterfaces::iterator - итератор на нужный интерфейс			 
			*/
			SS::Core::ResourceManagers::Types::TMWorkInterfaces::iterator FindOrCreateInterface(SS::Core::ResourceManagers::Types::TMWorkEntities::iterator it_Entity, const GUID& iidInterface, void** ptssBase);

			///удаляем все сущности
			/** 
				\param CoreLibrary - идентификатор модуля 
				\param iidInterface - идентификатор интерфейса
			*/
			void DeleteAll();

		public:
			CModulesManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

			~CModulesManager();			

			/// Создание сущности и возврат нужного интерфейса с регистрацией
			/** 
				\param CoreLibrary - идентификатор модуля 
				\param guidEntity - идентификатор сущности(класса)
				\param iidInterface - идентификатор интерфейсв
				\param ptssBase - ссылка на переменную, где будет хранится указатель на переданный интерфейс
				
			Создаем указатель на нужный интерфейс нужного класса нужного модуля,
			(если класс еще не создан, в противном случае используем уже созданный)
			записываем указатель на него в переданный указатель и
			запоминаем указатель на переданный указатель
			*/
			void CreateAndRegister(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase, const wchar_t* wszPath);
			void CreateAndRegister(const wchar_t* wszLibraryPath, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase, const wchar_t* wszPath);


		};
}
}
}