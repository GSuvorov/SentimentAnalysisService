#pragma once

#include "IInterface.h"
#include "IEnumerator.h"


// guid для интерфейса ISettingsServer
static const GUID IID_SettingsServer = 
{ 0xa9a80d, 0x594c, 0x4e2c, { 0xb9, 0xeb, 0x27, 0x3d, 0x66, 0xa0, 0x2c, 0x6c } };

// guid для интерфейса ISettingsServerEdit
static const GUID IID_SettingsServerEdit = 
{ 0xa9a80d, 0x594c, 0x4e2c, { 0xb9, 0xeb, 0x27, 0x3d, 0x66, 0xa0, 0x2c, 0x68 } };

/// guid для класса CSettingsServer модуля GSServer, поддерживающего ISettingsServer
static const GUID CLSID_SettingsServer = 
{ 0x82acdfda, 0xeb5b, 0x437a, { 0xb3, 0x96, 0x60, 0xa1, 0xf, 0x26, 0x46, 0x6a } };



// guid для класса CPerformanceMeasure, реализующего IPerformanceMeasure в модуле UMCommonServices
static const GUID CLSID_PerformanceMeasure = 
{ 0x8f58c2c5, 0x5a7c, 0x42c6, { 0x8f, 0x20, 0x4a, 0xc5, 0xff, 0x30, 0x36, 0x93 } };

// guid для IPerformanceMeasure
static const GUID IID_PerformanceMeasure = 
{ 0xd4b615f5, 0xa436, 0x4b34, { 0x91, 0x9c, 0xaa, 0x6a, 0x7b, 0x7e, 0x43, 0x52 } };



/// guid для класса CWorkRegister, реализующего IWorkRegister в модуле LoadManager 
static const GUID CLSID_WorkRegister = 
{ 0x2a11d2d5, 0x62e6, 0x4f23, { 0x83, 0x98, 0x7b, 0x6, 0x37, 0xd8, 0x58, 0xb7 } };


/// guid для IWorkRegister
static const GUID IID_WorkRegister = 
{ 0xf73fcf35, 0xbf6b, 0x48fa, { 0xaa, 0x9c, 0x20, 0xd0, 0xe0, 0x8e, 0x45, 0x25 } };



/// guid для класса CException, реализующего IException в модуле UMCommonServices
static const GUID CLSID_Exception = 
{ 0x975b6ba0, 0xa7af, 0x432c, { 0x94, 0x5f, 0x55, 0x5b, 0x46, 0x36, 0xad, 0x89 } };

/// guid для IException
static const GUID IID_Exception = 
{ 0x872e1e0d, 0xb5b5, 0x4cf4, { 0xad, 0xf8, 0x56, 0x53, 0x1f, 0x3c, 0x45, 0x81 } };


/// guid для класса CLogSingleton, реализующего ILog в модуле UMCommonServices
static const GUID CLSID_Log = 
{ 0x81c6a7f8, 0xc597, 0x4f76, { 0x97, 0xc6, 0xf7, 0xfb, 0x7a, 0xe7, 0xbd, 0xf3 } };

/// guid для ILog
static const GUID IID_Log = 
{ 0xb85a5747, 0xfd20, 0x4ff3, { 0x8a, 0x1b, 0x4f, 0x57, 0x90, 0x1d, 0xf3, 0x87 } };

/// guid для класса CServerLog модуля ServerLog, поддерживающего IServerLog
static const GUID CLSID_ServerLog = 
{ 0xc11ed496, 0x642f, 0x4fbf, { 0xa5, 0x28, 0x90, 0x4d, 0x93, 0xb0, 0xab, 0xe1 } };

/// guid для IServerLog
static const GUID IID_ServerLog = 
{ 0xab9ca0ee, 0x30a2, 0x4895, { 0x91, 0x57, 0xff, 0x4b, 0xb, 0x6e, 0xdf, 0x1 } };

/// guid для IFileFinder
static const GUID IID_FileFinder = 
{ 0xab771c1c, 0xf35c, 0x4b30, {0x8d, 0xc4, 0xd8, 0x0e, 0x61, 0xf2, 0xcc, 0xee} };

/// guid для класса, поддерживающего IFileFinder
static const GUID CLSID_FileFinder = 
{ 0x7ba0ace0, 0x7ee0, 0x484f, {0x82, 0x62, 0x10, 0xff, 0xfd, 0x8d, 0xe3, 0x9b} };


namespace SS
{
namespace Core
{
namespace CommonServices
{

	///тип ошибки
	enum ETypeError
	{
		///критическая
		eteCritical,
		///обычная
		eteNormal
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
namespace CommonServices
{
	enum EParameterType
		{
			ptInt,		//int
			ptUInt,		//unsigned int
			ptInt64,	//__int64
			ptString,	//wchar_t*
			ptDouble,	//double
			ptBoolean	//bool
		};
	/// глобальный сервер настроек системы
	class ISettingsServer : public SS::Interface::IBase
	{
	public:
		//устанавливает текущую директорию
		virtual void SetWorkPath (const wchar_t* sPath) = 0;
		///Сбрасывает текущие настройки на диск
		virtual void FlushToDisk () = 0;
		///// вернет значение параметра, определяемого именем sName. 
		///** Тип параметра возвращается в EParameterType
		//	если это список параметров, то в puiArrayCount будет выставлено значение, 
		//	неравное 0xffffffff (количество элементов в массиве)
		//*/
		//virtual void* GetParameter(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType* peType, unsigned int* puiArrayCount) = 0;

		/// вернет значение параметра, определяемого именем sName. 
		virtual unsigned int GetBoolean(const wchar_t* wszName) = 0;
		/// вернет значение параметра, определяемого именем sName. 
		virtual int GetInt(const wchar_t* wszName) = 0;
		/// вернет значение параметра, определяемого именем sName. 
		virtual unsigned int GetUInt(const wchar_t* wszName) = 0;
		/// вернет значение параметра, определяемого именем sName. 
		virtual void GetString(const wchar_t* wszName, std::wstring* pwsValue) = 0;
		/// вернет значение параметра, определяемого именем sName. 
		virtual __int64 GetInt64(const wchar_t* wszName) = 0;
		/// вернет значение параметра, определяемого именем sName. 
		virtual double GetDouble(const wchar_t* wszName) = 0;

		/// установит значение параметра, определяемого именем sName. 
		virtual void SetBoolean(const wchar_t* wszName, bool Value) = 0;
		/// установит значение параметра, определяемого именем sName. 
		virtual void SetInt(const wchar_t* wszName, int Value) = 0;
		/// установит значение параметра, определяемого именем sName. 
		virtual void SetUInt(const wchar_t* wszName, unsigned int Value) = 0;
		/// установит значение параметра, определяемого именем sName. 
		virtual void SetString(const wchar_t* wszName, std::wstring* pwsValue) = 0;
		/// установит значение параметра, определяемого именем sName. 
		virtual void SetInt64(const wchar_t* wszName, __int64 Value) = 0;
		/// установит значение параметра, определяемого именем sName. 
		virtual void SetDouble(const wchar_t* wszName, double Value) = 0;

		/// вернет значение параметра, определяемого именем sName (вернется спсисок, типы элементов которого определяются выставляемым sType)
		virtual void GetArray(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType eType, void* pArray) = 0;
		/// установит значение параметра, определяемого именем sName 
		virtual void SetArray(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType eType, void* pArray) = 0;
	};

	///oпределяет редактирование настроек
	class ISettingsServerEdit// : public SS::Interface::IBase
	{
	public:
		///возвращает список параметров. последний - null
		virtual void GetParameterList (std::vector<std::wstring>* pOutSettingsNames, bool bShowPrivate) = 0;
		///возвращает тип параметра по его имени. может установить дополнительную перемнную, является ли параметер массивом значений
		virtual SS::Interface::Core::CommonServices::EParameterType GetParameterType (const wchar_t* wszName, bool* bIsArray = NULL) = 0;
		///добавляет параметер
		virtual bool AddParameter (const wchar_t* wszName, EParameterType eType, bool bIsArray = false, 
					bool bIsPublic = true, bool bIsReadOnly = false) = 0;
		///удаляет параметер
		virtual void DeleteParameter (const wchar_t* wszName) = 0;
		///возвращает публичность
		virtual unsigned int GetPublicity				(const wchar_t* wszName) = 0;
		///возвращает тип доступа
		virtual unsigned int GetAccessibility			(const wchar_t* wszName) = 0;
		///устанавливает новое имя параметра	
		virtual void SetParameterName			(const wchar_t* wszName, const wchar_t* wszNewName) = 0;
		///устанавливает публичность
		virtual void SetParameterPublicity (const wchar_t* wszName, bool bIsPublic) = 0;
		///устанавливает тип доступа
		virtual void SetParameterAccessibility (const wchar_t* wszName, bool bIsReadOnly) = 0;
		///устанавливает тип
		virtual void SetParameterType (const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType eType) = 0;
		///добавляет значение параметра в список
		virtual void AddParameterValue (const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType eType, void* pValue) = 0;
	};

	enum ELogEntryType
	{
		etException,
		etInfo,
		etError,
		etAnyType // for categorized query
	};

	enum ELogEntrySeverity
	{
		etCritical,
		etNormal,
		etLow,
		etAnySeverity // for categorized query
	};

	class IServerLog : public SS::Interface::IBase
	{
	public:
		virtual void Save(
			SS::Interface::Core::CommonServices::ELogEntryType LogEntryType,
			SS::Interface::Core::CommonServices::ELogEntrySeverity LogEntrySeverity,
			wchar_t* wszMessage,wchar_t* wszComputerHost,wchar_t* wszServiceHost,
			wchar_t* wszModule,wchar_t* wszClass,wchar_t* wszMethod ) = 0;

		virtual wchar_t* GetMessages() = 0;
		virtual wchar_t* GetMessages(unsigned long lFirstMessage,unsigned long lLastMessage) = 0;
		virtual wchar_t* GetFirstMessages(unsigned long iNumberOfMessages) = 0;
		virtual wchar_t* GetLastMessages(unsigned long iNumberOfMessages) = 0;

		virtual wchar_t* GetCategorizedMessages(
			SS::Interface::Core::CommonServices::ELogEntryType LogEntryType,
			SS::Interface::Core::CommonServices::ELogEntrySeverity LogEntrySeverity,
			wchar_t* wszMessage,wchar_t* wszComputerHost,wchar_t* wszServiceHost,
			wchar_t* wszModule,wchar_t* wszClass,wchar_t* wszMethod ) = 0;

		virtual wchar_t* GetCategorizedMessages(
			unsigned long lFirstMessage,unsigned long lLastMessage,
			SS::Interface::Core::CommonServices::ELogEntryType LogEntryType,
			SS::Interface::Core::CommonServices::ELogEntrySeverity LogEntrySeverity,
			wchar_t* wszMessage,wchar_t* wszComputerHost,wchar_t* wszServiceHost,
			wchar_t* wszModule,wchar_t* wszClass,wchar_t* wszMethod ) = 0;

		virtual wchar_t* GetCategorizedFirstMessages(
			unsigned long iNumberOfMessages,
			SS::Interface::Core::CommonServices::ELogEntryType LogEntryType,
			SS::Interface::Core::CommonServices::ELogEntrySeverity LogEntrySeverity,
			wchar_t* wszMessage,wchar_t* wszComputerHost,wchar_t* wszServiceHost,
			wchar_t* wszModule,wchar_t* wszClass,wchar_t* wszMethod ) = 0;

		virtual wchar_t* GetCategorizedLastMessages(
			unsigned long iNumberOfMessages,
			SS::Interface::Core::CommonServices::ELogEntryType LogEntryType,
			SS::Interface::Core::CommonServices::ELogEntrySeverity LogEntrySeverity,
			wchar_t* wszMessage,wchar_t* wszComputerHost,wchar_t* wszServiceHost,
			wchar_t* wszModule,wchar_t* wszClass,wchar_t* wszMethod ) = 0;

		virtual unsigned long GetNumberOfMessages() = 0;

		virtual void SetWorkpath(wchar_t* wszWorkpath) = 0;

		virtual void CallManagedSaveHandler(
			long LogEntryType,
			long LogEntrySeverity,
			wchar_t* wszMessage,wchar_t* wszComputerHost,
			wchar_t* wszHost,wchar_t* wszModule,
			wchar_t* wszClass,wchar_t* wszMethod) = 0;
	};

	/// определяет фукциональность 
	class IPerformanceMeasure : public SS::Interface::IBase
	{
	public:
		///запуск подсчета времени выполнения
		virtual void	StartCalculation(const wchar_t* szID) = 0;
		///очистка тикера по идентификатору
		virtual void	ClearCalculation(const wchar_t* szID) = 0;
		///очистка тикеров
		virtual void	ClearCalculations() = 0;
		///останов подсчета времени выполнения
		virtual void	StopCalculation(const wchar_t* szID) = 0;
		///возврат времени выполнения
		virtual double	GetCalculation(const wchar_t* szID) = 0;
		///отображение времени выполнения
		virtual void	ViewCalculation(const wchar_t* szID) = 0;
		///отображение всех замеров времени выполнения
		virtual void	ViewCalculations(void) = 0;
	};

	class IWorkRegister : public SS::Interface::IBase
	{
	public:

		///возвращает значение параметра по его имени (юникод версия)
		virtual std::wstring GetParameter(std::wstring wParametrName) = 0;

		///установка рабочей директории
		virtual void SetWorkDir(std::wstring wPathToDir) = 0;
	};

	class ILog : public SS::Interface::IBase
	{
	public:

		///запись набора лог-сообщений szMessage на диск
		virtual void SaveLog(wchar_t* szFirst...) = 0;		
	};

	class IExceptionParams 
	{
	public:

		///установит сообщение об ошибке
		virtual void SetMessage(wchar_t* wszMessage...) = 0;
		///вернет сообщение об ошибке
		virtual const std::wstring& GetMessage() = 0;		

		///установит имя функции, вызвавшей ошибку
		virtual void SetFunction(const std::wstring wsFunction) = 0;
		///вернет имя функции, вызвавшей ошибку
		virtual const std::wstring& GetFunction() = 0;

		///установит имя файла с кодом
		virtual void SetFile(const std::wstring wsFile) = 0;
		///вернет имя файла с кодом
		virtual const std::wstring& GetFile() = 0;

		///установит номер строки в файле с кодом 
		virtual void SetLine(const long lLine) = 0;
		///вернет номер строки в файле с кодом 
		virtual const long GetLine() = 0;

		///установит тип ошибки
		virtual void SetTypeError(const SS::Core::CommonServices::ETypeError eteTypeError) = 0;
		///вернет тип ошибки
		virtual const SS::Core::CommonServices::ETypeError GetTypeError() = 0;
	};

	class IException : public SS::Interface::IBase,
		virtual public SS::Interface::Enumerators::IEnumerator
	{
	public:

		///добавление очередной ошибки в список
		virtual SS::Interface::Core::CommonServices::IExceptionParams* Add() = 0;
		
		///возвращается текущая ошибка
		virtual SS::Interface::Core::CommonServices::IExceptionParams* GetCurrent() = 0;

		///возвращает в виде строки накопленные ошибки
		virtual std::wstring& ToString() = 0;
	};

	/// поиск файлов (для бэкапа)
	class IFileFinder : public SS::Interface::IBase
	{
	public:
		/// ищет файлы в папке (vIgnoreExt игнорировать расширения (формат - ".ext"))
		virtual bool GetFiles(std::wstring& sRootPath, std::wstring& sSubPath, std::vector<std::wstring>* vIgnoreExt, std::vector<std::wstring>* vRelPaths)=0;
	};


}
}
}
}