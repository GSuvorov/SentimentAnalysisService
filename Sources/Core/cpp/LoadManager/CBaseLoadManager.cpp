#include "stdafx.h"
#include "CBaseLoadManager.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

CBaseLoadManager::CBaseLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
								   : m_pLoadManager(pLoadManager)
{
}

void CBaseLoadManager::Create( TCoreLibrary CoreLibrary, 
							   const GUID& guidEntity, const GUID& iidInterface, void** ptssBase, const wchar_t* wszPath)
{

	//Создаем нужную сущность
	m_oControlDynamicLibrary.CreateInstance(&guidEntity, ptssBase, CoreLibrary, wszPath);

	SS::Interface::IBase* pBase=(SS::Interface::IBase*)*ptssBase;
	pBase->QueryInterface(iidInterface, ptssBase);
	pBase->SetLoadManager(m_pLoadManager);
};

///Создание сущности и возврат нужного интерфейса без регистрации по имени dll
void CBaseLoadManager::Create( const wchar_t* wszDllName, 
							   const GUID& guidEntity, const GUID& iidInterface, void** ptssBase, const wchar_t* wszPath)
{
	//Создаем нужную сущность по имени dll
	m_oControlDynamicLibrary.CreateInstance(&guidEntity, ptssBase, wszDllName, wszPath);
	
	SS::Interface::IBase* pBase=(SS::Interface::IBase*)*ptssBase;
	pBase->QueryInterface(iidInterface, ptssBase);
	pBase->SetLoadManager(m_pLoadManager);
}

}
}
}