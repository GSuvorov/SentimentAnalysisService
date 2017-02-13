#pragma once

#include "..\ASCInterface\ICommonServices.h"
#include ".\setting_value.h"

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

#pragma pack(push,1)  // Fix packing size 
///тип настройки
struct SSettingType
{
	///тип значения настройки
	SS::Interface::Core::CommonServices::EParameterType m_eSettingType;
	///флаг настройки массива
	bool m_bIsArray;
};
#pragma pack(pop)

//----------------------------------------------------------------//

#pragma pack(push,1)  // Fix packing size 
///атрибуты настройки
struct SSettingAttributes
{
	SSettingAttributes(void):m_bPublic(true), m_bReadOnly(false)
	{
		m_SettingType.m_eSettingType = SS::Interface::Core::CommonServices::EParameterType::ptInt;
		m_SettingType.m_bIsArray = false;
	};
	///тип настройки
	SSettingType m_SettingType;
	///флаг общего доступа к настройке
	bool m_bPublic;
	///флаг доступа на чтение настройки
	bool m_bReadOnly;
};
#pragma pack(pop)

//----------------------------------------------------------------//

///базовая настройка
class CBasisSetting
{
public:
	CBasisSetting(const wchar_t* wszSettingName, SSettingType SettingType);
	virtual ~CBasisSetting() {};

	///возвращает название настройки
	const wchar_t* GetSettingName(void){return m_SettingName.GetValue()->c_str();};
	///возвращает тип настройки
	SSettingType GetSettingType(void){return m_SettingAttributes.m_SettingType;};
	///возвращает флаг общего доступа к настройке
	bool IsPublic(void){return m_SettingAttributes.m_bPublic;};
	///устанавливает флаг общего доступа к настройке
	void SetPublic(bool bValue){m_SettingAttributes.m_bPublic=bValue;};
	///возвращает флаг доступа на чтение настройки
	bool IsReadOnly(void){return m_SettingAttributes.m_bReadOnly;};
	///возвращает флаг доступа на чтение настройки
	void SetReadOnly(bool bValue){m_SettingAttributes.m_bReadOnly=bValue;};

	///возвращает значение настройки
	virtual bool GetSettingValue(void* pOutValue, SS::Interface::Core::CommonServices::EParameterType eSettingType, bool bIsArray) = 0;
	///устанавливает значение настройки
	virtual bool SetSettingValue(void* pInValue, SS::Interface::Core::CommonServices::EParameterType eSettingType, bool bIsArray) = 0;

	///переводит настройку в бинарный вид
	virtual void ToBinary(std::vector<unsigned char>* pBuffer) = 0;
	///наполняет настройку по буферу
	virtual void FromBinary(unsigned char* pucBuffer, unsigned int uiBufferSize, unsigned int* puiReadedSize) = 0;
	
	///копирует значение настройки
	virtual void CopyValue(CBasisSetting* pSetting) = 0;

protected:
	///аттрибуты настройки
	SSettingAttributes m_SettingAttributes;
	///название настройки
	CStringValue m_SettingName;
};

//----------------------------------------------------------------//

}
}