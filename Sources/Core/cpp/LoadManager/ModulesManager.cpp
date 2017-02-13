#include "stdafx.h"
#include "modulesmanager.h"

#include "..\ASCInterface\defines.h"

USES_LOAD_MANAGER;

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

CModulesManager::CModulesManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager) : CBaseLoadManager(pLoadManager)
{

}

CModulesManager::~CModulesManager()
{
	DeleteAll();
}			

SS::Core::ResourceManagers::Types::TMWorkLibraries::iterator CModulesManager::FindOrCreateLibrary
	(TCoreLibrary CoreLibrary)
{

	SS::Core::ResourceManagers::Types::TMWorkLibraries::iterator it_Library=m_mWorkLibraries.find(CoreLibrary);

	//если такой библиотеки нет, создаем вход
	if(it_Library==m_mWorkLibraries.end())
	{
		
		//вставляем библиотеку
		m_mWorkLibraries.insert(SS::Core::ResourceManagers::Types::TMWorkLibraries::value_type(CoreLibrary,
			SS::Core::ResourceManagers::Types::TMWorkEntities()));

		it_Library=m_mWorkLibraries.find(CoreLibrary);
	};

	return it_Library;
};

SS::Core::ResourceManagers::Types::TMWorkEntities::iterator CModulesManager::FindOrCreateEntity
	(TCoreLibrary CoreLibrary, SS::Core::ResourceManagers::Types::TMWorkLibraries::iterator& it_Library, const GUID& guidEntity, void** ptssBase, const wchar_t* wszPath)
{

	SS::Core::ResourceManagers::Types::TMWorkEntities::iterator it_Entity=it_Library->second.find(SS::Core::ResourceManagers::Types::CGuidShell(guidEntity));

	if(!ptssBase)
	{
		return it_Library->second.end();
	};

	if(it_Entity==it_Library->second.end())
	{

		//создаем нужную сущность
		Create(CoreLibrary, guidEntity, IID_Base, ptssBase, wszPath);

		//вставляем сущность
		it_Library->second.insert(SS::Core::ResourceManagers::Types::TMWorkEntities::value_type(SS::Core::ResourceManagers::Types::CGuidShell(guidEntity),
			SS::Core::ResourceManagers::Types::TPWorkInterfaces((SS::Interface::IBase*)*ptssBase, SS::Core::ResourceManagers::Types::TMWorkInterfaces())));

		it_Entity=it_Library->second.find(SS::Core::ResourceManagers::Types::CGuidShell(guidEntity));
	};
		

	return it_Entity;
};

SS::Core::ResourceManagers::Types::TMWorkInterfaces::iterator CModulesManager::FindOrCreateInterface
	(SS::Core::ResourceManagers::Types::TMWorkEntities::iterator it_Entity, const GUID& iidInterface, void** ptssBase)
{

	SS::Core::ResourceManagers::Types::TMWorkInterfaces::iterator it_WorkInterfaces=it_Entity->second.second.find(SS::Core::ResourceManagers::Types::CGuidShell(iidInterface));

	if(!ptssBase)
	{
		return it_Entity->second.second.end();
	};

	if(it_WorkInterfaces==it_Entity->second.second.end())
	{

		//Получаем базовый интерфейс
		SS::Interface::IBase* pBase=NULL;

		if(it_Entity->second.first)
		{
			pBase=it_Entity->second.first;
		}
		else
		{
			return it_Entity->second.second.end();
		};

		pBase->QueryInterface(iidInterface, ptssBase);

		if(!*ptssBase)
		{
			return it_Entity->second.second.end();
		};

		//вставляем интерфейс
		it_Entity->second.second.insert
			(SS::Core::ResourceManagers::Types::TMWorkInterfaces::value_type(SS::Core::ResourceManagers::Types::CGuidShell(iidInterface),SS::Core::ResourceManagers::Types::TInterface(reinterpret_cast<int>(*ptssBase))));		
	}
	else
	{
		//записываем в выходной параметр указатель на рабочий интерфейс
		*ptssBase=reinterpret_cast<void*>(it_WorkInterfaces->second);

		if(!*ptssBase)
		{
//			SS_THROW("Указатель на рабочий интерфейс (" AND m_DumpConnections.FindDescription(it_Entity->first) AND ") нулевой (возможно, отсутствует UnRegister)");
		};
	}

	return it_WorkInterfaces;
};

void CModulesManager::DeleteAll()
{
	SS_TRY;

	std::wstring wszCurrentClassGUID = L"";

	for(SS::Core::ResourceManagers::Types::TMWorkLibraries::iterator it_Lib=m_mWorkLibraries.begin();
		it_Lib!=m_mWorkLibraries.end(); it_Lib++)
	{
		TCoreLibrary o = it_Lib->first;
		for(SS::Core::ResourceManagers::Types::TMWorkEntities::iterator it_Entity=it_Lib->second.begin();
			it_Entity!=it_Lib->second.end(); it_Entity++)
		{
			if(it_Entity->second.first)
			{
				wszCurrentClassGUID = it_Entity->first.ToString().c_str();
				try
				{
					it_Entity->second.first->Release();
				}
				catch(...)
				{
SAVE_LOG(SS_MESSAGE AND L"При удалении класса с GUID " AND wszCurrentClassGUID.c_str() AND L" возникло исключение" AND __WFUNCTION__);
				}
				it_Entity->second.first=NULL;
				//обнуляем все интерфейсы				
				for(SS::Core::ResourceManagers::Types::TMWorkInterfaces::iterator it_WorkInterfaces=it_Entity->second.second.begin();
					it_WorkInterfaces!=it_Entity->second.second.end(); it_WorkInterfaces++)
				{
					it_WorkInterfaces->second = NULL;
					
				}
			}			
		}		
	}
	SS_CATCH(L"");
};

void CModulesManager::CreateAndRegister(TCoreLibrary CoreLibrary, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase, const wchar_t* wszPath)
{
	SS::Core::ResourceManagers::Types::TMWorkLibraries::iterator it_Library=FindOrCreateLibrary(CoreLibrary);

	SS::Core::ResourceManagers::Types::TMWorkEntities::iterator it_Entity=FindOrCreateEntity(CoreLibrary, it_Library, guidEntity, ptssBase, wszPath);

	if(it_Entity==it_Library->second.end())
		return;

	SS::Core::ResourceManagers::Types::TMWorkInterfaces::iterator it_WorkInterfaces=FindOrCreateInterface(it_Entity, iidInterface, ptssBase);
}

void CModulesManager::CreateAndRegister(const wchar_t* wszLibraryPath, const GUID& guidEntity, const GUID& iidInterface, void** ptssBase, const wchar_t* wszPath)
{
	TCoreLibrary Lib = m_oControlDynamicLibrary.AddLibraryByPath(wszLibraryPath);
	CreateAndRegister(Lib, guidEntity, iidInterface, ptssBase, wszPath);
}


}
}
}