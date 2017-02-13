#pragma once

#include ".\settings_manager.h"
#include "..\ASCInterface\ICommonServices.h"

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

///оператор настроек, позволяет задавать и получать значения настроек
class  CSettingsOperator :
			public virtual CSettingsManager,
			public SS::Interface::Core::CommonServices::ISettingsServer
{
public:

	//-----------ISettingsServer
	//ISettingsServer implementation
	///установка пути к файлу настроек
	void SetWorkPath (const wchar_t* sPath);
	///запись настроек в файл на диске
	void FlushToDisk ();

	///получить настройку конкретного типа по имени
	unsigned int	GetBoolean	(const wchar_t* wszName){
		return (unsigned int)GetParameter<bool>(wszName, SS::Interface::Core::CommonServices::ptBoolean);
	};
	int				GetInt		(const wchar_t* wszName){return GetParameter<int>(wszName, SS::Interface::Core::CommonServices::ptInt);};
	unsigned int	GetUInt		(const wchar_t* wszName){return GetParameter<unsigned int>(wszName, SS::Interface::Core::CommonServices::ptUInt);};
	__int64			GetInt64	(const wchar_t* wszName){return GetParameter<__int64>(wszName, SS::Interface::Core::CommonServices::ptInt64);};
	double			GetDouble	(const wchar_t* wszName){return GetParameter<double>(wszName, SS::Interface::Core::CommonServices::ptDouble);};

	///установить настройку конкретного типа по имени
	void SetBoolean	(const wchar_t* wszName, bool Value){SetParameter<bool>(Value, wszName, SS::Interface::Core::CommonServices::ptBoolean);};
	void SetInt		(const wchar_t* wszName, int Value){SetParameter<int>(Value, wszName, SS::Interface::Core::CommonServices::ptInt);};
	void SetUInt	(const wchar_t* wszName, unsigned int Value){SetParameter<unsigned int>(Value, wszName, SS::Interface::Core::CommonServices::ptUInt);};
	void SetInt64	(const wchar_t* wszName, __int64 Value){SetParameter<__int64>(Value, wszName, SS::Interface::Core::CommonServices::ptInt64);};
	void SetDouble	(const wchar_t* wszName, double Value){SetParameter<double>(Value, wszName, SS::Interface::Core::CommonServices::ptDouble);};

	///получить строковую настройку
	void GetString	(const wchar_t* wszName, std::wstring* pwsValue){OperateParameter(true, wszName, (void*)pwsValue, SS::Interface::Core::CommonServices::ptString, false);};
	///установить строковую настройку
	void SetString	(const wchar_t* wszName, std::wstring* pwsValue){OperateParameter(false, wszName, (void*)pwsValue, SS::Interface::Core::CommonServices::ptString, false);};

	///получить массив-значение настройки
	void GetArray	(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType peType, void* pArray)
		{OperateParameter(true, wszName, pArray, peType, true);};
	///установить массив-значение настройки
	void SetArray	(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType peType, void* pArray)
		{OperateParameter(false, wszName, pArray, peType, true);};
	//-----------Eof-ISettingsServer

};

//----------------------------------------------------------------//

}
}