#include "StdAfx.h"
#include ".\settings_editor.h"

USES_LOAD_MANAGER;

namespace SS
{
namespace CommonServices
{

using namespace SS::Interface::Core::CommonServices;

//----------------------------------------------------------------//

bool CSettingsEditor::AddParameter	(const wchar_t*		wszName, 
										SS::Interface::Core::CommonServices::EParameterType	eType, 
										bool			bIsArray, 
										bool			bIsPublic,
										bool			bIsReadOnly)
{
	if(GetSetting(wszName))
		SS_THROW(L"Setting by name: " AND wszName AND L" already exist");
	
	//создаем настройку
	SSettingType SettingType;
	SettingType.m_bIsArray=bIsArray;
	SettingType.m_eSettingType=eType;
	CBasisSetting* pBasisSetting=m_SettingFactory.CreateSetting(wszName, SettingType);

	if(!pBasisSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" cann't create");
	
	//устанавливаем атрибуты настройки
	pBasisSetting->SetPublic(bIsPublic);
	pBasisSetting->SetReadOnly(bIsReadOnly);

	//заносим настройку в коллекцию
	if(!AddSetting(pBasisSetting)){
		SS_THROW(L"Cann't add setting: " AND pBasisSetting->GetSettingName());
	}

	return true;
}

EParameterType CSettingsEditor::GetParameterType(const wchar_t* wszName, bool* pbIsArray)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");
	SSettingType SettingType=pSetting->GetSettingType();

	if(pbIsArray)
		(*pbIsArray)=SettingType.m_bIsArray;
	return SettingType.m_eSettingType;
}

unsigned int CSettingsEditor::GetPublicity(const wchar_t* wszName)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");
	return (unsigned int)pSetting->IsPublic();
}

unsigned int CSettingsEditor::GetAccessibility(const wchar_t* wszName)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");
	return (unsigned int)pSetting->IsReadOnly();
}

void CSettingsEditor::DeleteParameter(const wchar_t* wszName)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");
	
	m_SettingsCollection.erase(wszName);
	delete pSetting;
}

void CSettingsEditor::SetParameterName(const wchar_t* wszName, const wchar_t* wszNewName)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");

	//создаем настройку
	SSettingType SettingType;
	SettingType.m_bIsArray=pSetting->GetSettingType().m_bIsArray;
	SettingType.m_eSettingType=pSetting->GetSettingType().m_eSettingType;
	CBasisSetting* pBasisSetting=m_SettingFactory.CreateSetting(wszNewName, SettingType);
	if(!pBasisSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" cann't create");
	
	//копируем члены настройки
	pBasisSetting->SetPublic(pSetting->IsPublic());
	pBasisSetting->SetReadOnly(pSetting->IsReadOnly());
	pBasisSetting->CopyValue(pSetting);

	//удаляем старую настройку
	DeleteParameter(wszName);

	//заносим настройку в коллекцию
	if(!AddSetting(pBasisSetting)){
		SS_THROW(L"Cann't add setting: " AND pBasisSetting->GetSettingName());
	}
	
}

void CSettingsEditor::SetParameterPublicity(const wchar_t* wszName, bool bIsPublic)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");
	pSetting->SetPublic(bIsPublic);
}

void CSettingsEditor::SetParameterAccessibility(const wchar_t* wszName, bool bIsReadOnly)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");
	pSetting->SetReadOnly(bIsReadOnly);
}

void CSettingsEditor::SetParameterType(const wchar_t* wszName, EParameterType eType)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");

	//создаем настройку
	SSettingType SettingType;
	SettingType.m_bIsArray=pSetting->GetSettingType().m_bIsArray;
	SettingType.m_eSettingType=eType;
	CBasisSetting* pBasisSetting=m_SettingFactory.CreateSetting(wszName, SettingType);
	if(!pBasisSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" cann't create");

	pBasisSetting->SetPublic(pSetting->IsPublic());
	pBasisSetting->SetReadOnly(pSetting->IsReadOnly());

	//удаляем старую настройку
	DeleteParameter(wszName);

	//заносим настройку в коллекцию
	if(!AddSetting(pBasisSetting)){
		SS_THROW(L"Cann't add setting: " AND pBasisSetting->GetSettingName());
	}
}

void CSettingsEditor::AddParameterValue(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType eType, void* pValue)
{
	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");

	if(!pSetting->GetSettingType().m_bIsArray)
		SS_THROW(L"Setting by name: " AND wszName AND L" is not array cann't add");
	
	//добавляем значение
	OperateParameter(false, wszName, pValue, pSetting->GetSettingType().m_eSettingType, false);
}

void CSettingsEditor::GetParameterList(std::vector<std::wstring>* pOutSettingsNames, bool bShowPrivate)
{
	if(!pOutSettingsNames)
		SS_THROW(L"pOutSettingsNames is null");

	if(!CheckUpToDate())
		SS_THROW(L"CheckUpToDate not succeeded");
	
	TSettingsCollection::iterator itSetting=m_SettingsCollection.begin();
	while(itSetting!=m_SettingsCollection.end()){
		if(!itSetting->second)
			SS_THROW(L"Setting by name: " AND itSetting->first.c_str() AND L" does not exist");

		if((!itSetting->second->IsPublic() && bShowPrivate) || itSetting->second->IsPublic()){
			pOutSettingsNames->push_back(std::wstring());
			pOutSettingsNames->back().assign(itSetting->second->GetSettingName());
		}
		itSetting++;
	}

}

//----------------------------------------------------------------//

}
}