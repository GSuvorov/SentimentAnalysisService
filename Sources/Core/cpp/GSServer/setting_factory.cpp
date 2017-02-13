#include "StdAfx.h"

#include ".\setting_factory.h"
#include ".\single_setting.h"
#include ".\setting_value.h"

namespace SS
{
namespace CommonServices
{

using namespace SS::Interface::Core::CommonServices;

CBasisSetting* CSettingFactory::CreateSetting(SSettingType SettingType)
{
	return CreateSetting(L"", SettingType);
}

CBasisSetting* CSettingFactory::CreateSetting(const wchar_t* wszSettingName, SSettingType SettingType)
{
	if(!SettingType.m_bIsArray){
		switch(SettingType.m_eSettingType){
		case ptBoolean:
			return new CSetting< CSingleValue<bool> >(wszSettingName, SettingType);
			break;
		case ptDouble:
			return new CSetting< CSingleValue<double> >(wszSettingName, SettingType);
			break;
		case ptInt:
			return new CSetting< CSingleValue<int> >(wszSettingName, SettingType);
			break;
		case ptInt64:
			return new CSetting< CSingleValue<__int64> >(wszSettingName, SettingType);
			break;
		case ptUInt:
			return new CSetting< CSingleValue<unsigned int> >(wszSettingName, SettingType);
			break;
		case ptString:
			return new CSetting< CStringValue >(wszSettingName, SettingType);
			break;
		default:
			return NULL;
			break;
		}
	}else{
		switch(SettingType.m_eSettingType){
		case ptBoolean:
			return new CSetting< CArrayValue<bool> >(wszSettingName, SettingType);
			break;
		case ptDouble:
			return new CSetting< CArrayValue<double> >(wszSettingName, SettingType);
			break;
		case ptInt:
			return new CSetting< CArrayValue<int> >(wszSettingName, SettingType);
			break;
		case ptInt64:
			return new CSetting< CArrayValue<__int64> >(wszSettingName, SettingType);
			break;
		case ptUInt:
			return new CSetting< CArrayValue<unsigned int> >(wszSettingName, SettingType);
			break;
		case ptString:
			return new CSetting< CStringArrayValue >(wszSettingName, SettingType);
			break;
		default:
			return NULL;
			break;
		}
	}
}

}
}