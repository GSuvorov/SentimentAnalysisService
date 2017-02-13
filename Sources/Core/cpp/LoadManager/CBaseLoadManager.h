#pragma once

#include "ControlDynamicLibrary.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

		/// Базовый класс для работы с модулями
		/** Хранит информацию об активном хранилище, реализует базовую функциональность создания
		нужного класса нужного модуля и возвращения заправшиваемого интерфейса*/
		class CBaseLoadManager
		{
		protected:
			///класс для загрузки модулей (dll)
			CControlDynamicLibrary m_oControlDynamicLibrary;

			SS::Interface::Core::ResourceManagers::ILoadManager* m_pLoadManager;
		public:
			CBaseLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

			/// Создание сущности и возврат нужного интерфейса без регистрации
			/** 
				\param CoreLibrary - идентификатор модуля 
				\param guidEntity - идентификатор сущности(класса)
				\param iidInterface - идентификатор интерфейсв
				\param ptssBase - ссылка на переменную, где будет хранится указатель на переданный интерфейс
				
			Создаем указатель на нужный интерфейс нужного класса нужного модуля и
			записываем указатель на него в переданный указатель 
			*/
			void Create(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase, const wchar_t* wszPath);
			///Создание сущности и возврат нужного интерфейса без регистрации по имени dll
			void Create(const wchar_t* wszDllName, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase, const wchar_t* wszPath);
		};

}
}
}