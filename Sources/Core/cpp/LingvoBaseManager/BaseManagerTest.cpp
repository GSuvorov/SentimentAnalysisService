#include "StdAfx.h"
#include <iostream>
#include ".\basemanagertest.h"

namespace SS
{
namespace UnitTests
{

namespace ResourceManagers
{


CBaseManagerTest::CBaseManagerTest(void) : m_pBaseManager(NULL), m_pLingvoBaseManager(NULL), m_pDataBase(NULL)
{
}

CBaseManagerTest::~CBaseManagerTest(void)
{
}

void CBaseManagerTest::Init(void)
{
	SS_TRY
	{	

		m_pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
			GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);

		m_pLingvoBaseManager = (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
			GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

	}
	SS_CATCH(L"")
}

void CBaseManagerTest::AddData()
{
	SS_TRY
	{
		if(!m_pDataBase)
		{
			std::wcout << __WFUNCTION__ << L"error !m_pDataBase" << "\n";
			return;
		}
		m_pDataBase->MoveToDataTable(L"NewFormRussian");
		SS::Interface::Core::DBMS::IDataTable* pDataTable = m_pDataBase->OpenDataTable();

		void       * m_pTableRecord[4];

		SStruct ost;
		std::list<SStruct> listSStruct;

		m_pTableRecord[0] = &ost.IDSource;
		m_pTableRecord[1] = &ost.wsSource;
		m_pTableRecord[2] = &ost.wsSourceLower;
		m_pTableRecord[3] = &ost.m_IDType;

		pDataTable->AddInit(m_pTableRecord);
		pDataTable->ScanInit(0, m_pTableRecord);

		ost.IDSource		= 0;
		ost.wsSource		= L"0";
		ost.wsSourceLower	= L"0";
		ost.m_IDType		= 0;
		pDataTable->Add();
		listSStruct.push_back(ost);

		ost.IDSource		= 1;
		ost.wsSource		= L"1";
		ost.wsSourceLower	= L"1";
		ost.m_IDType		= 1;
		pDataTable->Add();
		listSStruct.push_back(ost);

		ost.IDSource		= 2;
		ost.wsSource		= L"2";
		ost.wsSourceLower	= L"2";
		ost.m_IDType		= 2;
		pDataTable->Add();
		listSStruct.push_back(ost);

		ost.IDSource		= 3;
		ost.wsSource		= L"3";
		ost.wsSourceLower	= L"3";
		ost.m_IDType		= 3;
		pDataTable->Add();
		listSStruct.push_back(ost);

		std::list<SStruct>::iterator iter = listSStruct.begin();
		
		pDataTable->ScanStart(0);
		
		while(pDataTable->Scan() == S_OK)
		{
			if(
				(ost.IDSource		== iter->IDSource)&&
				(ost.m_IDType		== iter->m_IDType)&&
				(ost.wsSource		== iter->wsSource)&&
				(ost.wsSourceLower	== iter->wsSourceLower)
				)
			{
				iter++;
			}
			else
			{
				std::wcout << __WFUNCTION__ << L"error: data in table are not equal to test data" << "\n";

			}			
		}
		pDataTable->Close();
	}
	SS_CATCH(L"")

}

void CBaseManagerTest::Run(void)
{
	SS_TRY
	{
		Init();

		///*	получаем директорий базы данных	*/
		std::wstring m_wsDataBasePath = 
			((SS::Interface::Core::CommonServices::IWorkRegister*) 
			GetLoadManager()->GetInterface(L"./core/LoadManager.dll",CLSID_WorkRegister,IID_WorkRegister))->GetParameter(L"PathDataBase");
		m_pBaseManager->SetBasePath(m_wsDataBasePath.c_str());

		m_pLingvoBaseManager->CreateOpenAndRegister(SS::MANAGER::Constants::c_szNdxBase, &m_pDataBase);


		m_pBaseManager->OpenBase(L"NewWordForms1");

		AddData();

		m_pBaseManager->CloseBase();

		m_pBaseManager->OpenBase(L"NewWordForms1");
		m_pBaseManager->ClearBase();
		m_pBaseManager->CloseBase();

		m_pBaseManager->OpenBase(L"NewWordForms2");
		m_pBaseManager->ClearBase();
		m_pBaseManager->CloseBase();

		m_pBaseManager->OpenBase(L"NewWordForms3");
		m_pBaseManager->ClearBase();
		m_pBaseManager->CloseBase();

		m_pLingvoBaseManager->UnRegisterDeleteAndClose(SS::MANAGER::Constants::c_szNdxBase, &m_pDataBase);

		int i = 0;
	}
	SS_CATCH(L"")
}



}
}
}