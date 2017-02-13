#include "StdAfx.h"
#include ".\basis_setting.h"

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

CBasisSetting::CBasisSetting(const wchar_t* wszSettingName, SSettingType SettingType)
{
	m_SettingAttributes.m_SettingType=SettingType;
	m_SettingName.GetValue()->assign(wszSettingName);
}

//----------------------------------------------------------------//

}
}