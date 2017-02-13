#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include "types.h"
#include "services.h"

/// guid для класса, поддерживающего ILoadManager (модуль LoadManager)
static const GUID CLSID_LoadManager = 
{ 0x7e7761a4, 0x90d0, 0x4ba8, { 0x90, 0xc, 0xf, 0x3, 0xb6, 0x20, 0x83, 0xee } };

static const GUID CLSID_LoadManager_IBase = 
{ 0x7e7761a5, 0x90d0, 0x4ba8, { 0x90, 0xc, 0xf, 0x3, 0xb6, 0x20, 0x83, 0xee } };



// guid для интерфейса ILoadManager
static const GUID IID_LoadManager = 
{ 0x6a23ca69, 0x5eec, 0x4a57, { 0x8b, 0xb5, 0x7e, 0x96, 0x6e, 0x57, 0x24, 0xd4 } };

// guid для интерфейса IBase
const GUID IID_Base=
{0xfe034996, 0x510d, 0x4d20, {0x93, 0x89, 0x39, 0x14, 0x29, 0xc8, 0xc0, 0x62}};

/// guid для класса, поддерживающего ITestRunner
static const GUID CLSID_TestRunner = 
{ 0xbdeae75, 0x37a, 0x4d06, { 0xa5, 0xc8, 0x45, 0x81, 0xfc, 0xca, 0x5, 0x0 } };

// guid для интерфейса ITestRunner
static const GUID IID_TestRunner = 
{ 0xa2156f3f, 0xda28, 0x4e06, { 0xbd, 0xcf, 0x49, 0xfe, 0xc0, 0x1d, 0x9d, 0x87 } };


namespace SS
{
	namespace Interface
	{
		class IBaseInterface
		{
		public:
			///освобождение
			virtual ULONG Release() = 0;
			virtual ~IBaseInterface(){};
		};

		///базовый интерфейс для управляемых интерфейсов  SS
		class IBase
			: public IBaseInterface, public SS::Core::CommonServices::CBaseCoreClass
		{
		public:
			///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
			virtual HRESULT QueryInterface(REFIID pIID, void** pBase) = 0; 

			///DECTOR
			virtual ~IBase(){};
		};
		namespace UnitTests
		{
			///стандартный интерфейс для поддержки группового запуска юнит-тестов
			class ITestRunner : public SS::Interface::IBase
			{
			public:
				///запуск тестов
				virtual void Run()=0;
				///DECTOR
				virtual ~ITestRunner(){}
			};
		}
	}
}

namespace SS
{
namespace Interface
{
namespace Core
{
namespace ResourceManagers
{
	class ILoadManager : public virtual SS::Interface::IBase
	{
	public:
		/// установит рабочую папку для неуправляемых модулей
		virtual void SetWorkPath(const wchar_t* wszPath) = 0;
		/// вернет рабочую папку для неуправляемых модулей
		virtual const wchar_t* GetWorkPath() = 0;

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
		//virtual void* GetInterface(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface)=0;

		///// Создание отдельной сущности и возврат нужного интерфейса (данная функция имеет ограничения)
		///** 
		//	\param CoreLibrary - идентификатор модуля 
		//	\param guidEntity - идентификатор сущности(класса)
		//	\param iidInterface - идентификатор интерфейсв
		//	\param ptssBase - ссылка на переменную, где будет хранится указатель на переданный интерфейс
		//	
		//Создаем указатель на нужный интерфейс нужного класса нужного модуля и
		//записываем указатель на него в переданный указатель 
		//ВНИМАНИЕ! использовать данную функцию при реализации системы запрещается
		//она предназначена только для реализации тестового кода
		//*/
		//virtual void Create(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase)=0;

		virtual ~ILoadManager(){};

		virtual void* GetInterface(const wchar_t* wszPath, const GUID& guidEntity, const GUID& iidInterface) = 0;

		virtual void Create(const wchar_t* wszPath, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase) = 0;

	};
}
}
}
}
