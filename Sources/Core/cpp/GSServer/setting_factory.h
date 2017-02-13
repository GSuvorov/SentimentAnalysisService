#pragma once

#include ".\basis_setting.h"

namespace SS
{
namespace CommonServices
{

///фабрика настроек
class CSettingFactory
{
public:
	///создает настройку заданного типа
	CBasisSetting* CreateSetting(SSettingType SettingType);
	///создает настройку заданного типа
	CBasisSetting* CreateSetting(const wchar_t* wszSettingName, SSettingType SettingType);
};

}
}