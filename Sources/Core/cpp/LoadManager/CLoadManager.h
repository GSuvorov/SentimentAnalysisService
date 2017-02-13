#pragma once

#include "ModulesManager.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{
	/// Предоставляет внешним пользователя функционал загрузки требуемых классов
	class CLoadManager : public SS::Interface::Core::ResourceManagers::ILoadManager
	{

		CModulesManager m_oModulesManager;
		const wchar_t* m_wszPath;
	public:

		CLoadManager();
		~CLoadManager();

		///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
		HRESULT QueryInterface(REFIID pIID, void** ppBase); 
		///освобождение
		ULONG Release();

		/// установит рабочую папку для неуправляемых модулей
		void SetWorkPath(const wchar_t* wszPath);
		const wchar_t* GetWorkPath();
		/// Вернет указатель на нужный интерфейс
		/** 
			\param CoreLibrary - идентификатор модуля 
			\param guidEntity - идентификатор сущности(класса)
			\param iidInterface - идентификатор интерфейсв
			\param ptssBase - ссылка на переменную, где будет хранится указатель на переданный интерфейс
			
		Создаем указатель на нужный интерфейс нужного класса нужного модуля,
		(если класс еще не создан, в противном случае используем уже созданный)
		записываем указатель на него в переданный указатель.
		ОСОБЕННОСТЬ: экземпляр класса с guidEntity в кластере ядра всегда один
		*/
		void* GetInterface(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface);

		/// Создание отдельной сущности и возврат нужного интерфейса (данная функция имеет ограничения)
		/** 
			\param CoreLibrary - идентификатор модуля 
			\param guidEntity - идентификатор сущности(класса)
			\param iidInterface - идентификатор интерфейсв
			\param ptssBase - ссылка на переменную, где будет хранится указатель на переданный интерфейс
			
		Создаем указатель на нужный интерфейс нужного класса нужного модуля и
		записываем указатель на него в переданный указатель 
		ВНИМАНИЕ! использовать данную функцию при реализации системы запрещается
		она предназначена только для реализации тестового кода
		*/
		void Create(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase);
		
		void* GetInterface(const wchar_t* wszPath, const GUID& guidEntity, const GUID& iidInterface);
		void Create(const wchar_t* wszPath, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase);
	};
}
}
}