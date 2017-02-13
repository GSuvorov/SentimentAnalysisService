#include "StdAfx.h"


#include "..\ASSInterface\ICoSeDi.h"
#include "..\ASSInterface\TBaseNames.h"

#include ".\connect.h"

namespace SS
{
namespace CoSeDi
{
namespace SQL
{


CConnect::CConnect(const wchar_t* szBaseName) : m_pLingvoBaseManager(NULL), m_pDataBase(NULL), m_pConnection(NULL)
{
	SS_TRY
	{
		wcscpy(m_szBaseName, szBaseName);
	}
	SS_CATCH(L"");					
}

CConnect::~CConnect(void)
{
	CloseConnection();
}
void CConnect::CloseConnection(void)
{
	SS_TRY
	{
		m_pLingvoBaseManager->UnRegisterDeleteAndClose(m_szBaseName, &m_pDataBase);
		m_pLingvoBaseManager->UnRegisterDeleteAndClose(m_szBaseName, &m_pConnection);

	}				
	SS_CATCH(L"")
}

bool CConnect::InitConnection(void)
{

	SS_TRY
	{

		if( 
			( wcscmp(m_szBaseName, SS::MANAGER::Constants::c_szCoSeDiEng))||
			(wcscmp(m_szBaseName, SS::MANAGER::Constants::c_szCoSeDiRus)))
		{
			
			if(!m_pLingvoBaseManager)
			{
				m_pLingvoBaseManager = 
					(SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
					GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);
			}

			m_pLingvoBaseManager->CreateOpenAndRegister( m_szBaseName, &m_pConnection);

			m_pLingvoBaseManager->CreateOpenAndRegister( m_szBaseName, &m_pDataBase);		
		
			return true;
		}else
			return false;	
	}
	SS_CATCH(L"");

	return false;
	
}

}

namespace DBMS
{

CConnect::CConnect(const wchar_t* szBaseName) : m_pDTable(NULL),  m_pLingvoBaseManager(NULL), m_pDataBase(NULL)
{
	wcscpy(m_szBaseName, szBaseName);
}

CConnect::~CConnect(void)
{
	CloseConnection();
}
void CConnect::CloseConnection(void)
{
	SS_TRY
	{
		if((m_pDTable)&& (m_pDTable->IsOpen()))
		{
			m_pDTable->Close();
			m_pDTable = NULL;
		}

		if(m_pDataBase)   {
			m_pLingvoBaseManager->UnRegisterDeleteAndClose(m_szBaseName, &m_pDataBase);
			m_pDataBase = NULL;
		}
		
	}
	SS_CATCH(L"")
}

bool CConnect::InitConnection(std::wstring& wDataTableName)
{

	SS_TRY
	{
		if(
			(wcscmp(m_szBaseName, SS::MANAGER::Constants::c_szCoSeDiEng) == 0)||
			(wcscmp(m_szBaseName, SS::MANAGER::Constants::c_szCoSeDiRus) == 0))
		{
			if(!m_pLingvoBaseManager)
			{
				m_pLingvoBaseManager = 
					(SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
					GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);
			}

			m_pLingvoBaseManager->CreateOpenAndRegister( m_szBaseName, &m_pDataBase);		

			m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
			m_pDTable = m_pDataBase->OpenDataTable();

			if((m_pDTable == NULL)||(!m_pDTable->IsOpen()))
			{
				SS_THROW(L"таблица не открыта");
			}	
		}								
	}
	SS_CATCH(L"")

	return true;
	
}

}
}
}