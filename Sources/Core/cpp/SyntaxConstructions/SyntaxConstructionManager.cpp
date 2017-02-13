//SyntaxConstructionManager.cpp

#include "StdAfx.h"
#include ".\syntaxconstructionmanager.h"

//Конструктор
SS::SyntaxConstructions::CSyntaxConstructionManager::CSyntaxConstructionManager(const wchar_t* szBaseName) :
		m_pSyntaxConstuctionsDBMS(NULL),		m_pSyntaxConstuctionsSQL(NULL),
		m_pSyntaxConstructionGroupsDBMS(NULL),	m_pSyntaxConstructionGroupsSQL(NULL),
		m_pDetectWordsGroupsDBMS(NULL),			m_pDetectWordsGroupsSQL(NULL), m_eDataBaseType(edbtUndefined)

{
	wcscpy(m_szBaseName, szBaseName);
}

//Деструктор
SS::SyntaxConstructions::CSyntaxConstructionManager::~CSyntaxConstructionManager()
{
	if (m_pSyntaxConstuctionsDBMS)
	{
		delete m_pSyntaxConstuctionsDBMS;
		m_pSyntaxConstuctionsDBMS = NULL;
	}

	if (m_pSyntaxConstuctionsSQL)
	{
		delete m_pSyntaxConstuctionsSQL;
		m_pSyntaxConstuctionsSQL = NULL;
	}

	if (m_pSyntaxConstructionGroupsDBMS)
	{
		delete m_pSyntaxConstructionGroupsDBMS;
		m_pSyntaxConstructionGroupsDBMS = NULL;
	}

	if (m_pSyntaxConstructionGroupsSQL)
	{
		delete m_pSyntaxConstructionGroupsSQL;
		m_pSyntaxConstructionGroupsSQL = NULL;
	}

	if (m_pDetectWordsGroupsDBMS)
	{
		delete m_pDetectWordsGroupsDBMS;
		m_pDetectWordsGroupsDBMS = NULL;
	}

	if (m_pDetectWordsGroupsSQL)
	{
		delete m_pDetectWordsGroupsSQL;
		m_pDetectWordsGroupsSQL = NULL;
	}
}

ULONG SS::SyntaxConstructions::CSyntaxConstructionManager::Release(void)
{
	delete this;
	return 0;
}

HRESULT SS::SyntaxConstructions::CSyntaxConstructionManager::QueryInterface(REFIID pIID, void** pBase)
{
	m_eDataBaseType = edbtUndefined;

   if (pIID == IID_ISyntaxConstructionManager)
	{
		m_eDataBaseType = edbtDBMS;
		*pBase = (SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionsManager*) this;
	}
	else if (pIID == IID_ISyntaxConstructionManagerSQL)
	{
		m_eDataBaseType = edbtSQL;
		*pBase = (SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionsManager*) this;
	}
	else if (pIID == IID_Base)
	{
		*pBase = (SS::Interface::IBase*) this;
	}
	else 
	{
		*pBase = NULL;
	}
	return S_OK;
}

///Сохраняет в бинарный вид структуры данных 
bool SS::SyntaxConstructions::CSyntaxConstructionManager::ToBinary()
{
	SS_TRY
	{
		CreateObjects();		
		
		m_pSyntaxConstructionGroupsSQL->ToBinary();
		m_pSyntaxConstuctionsSQL->ToBinary();
		m_pDetectWordsGroupsSQL->ToBinary();
		
		if (m_pSyntaxConstructionGroupsDBMS) 
		{ m_pSyntaxConstructionGroupsDBMS->SetRepeatTableInit(); }
		if (m_pSyntaxConstuctionsDBMS) 
		{ m_pSyntaxConstuctionsDBMS->SetRepeatTableInit(); }
		if (m_pDetectWordsGroupsDBMS) 
		{ m_pDetectWordsGroupsDBMS->SetRepeatTableInit(); }
	}
	SS_CATCH(L"")
	return true;
}

///Создание объктов
void SS::SyntaxConstructions::CSyntaxConstructionManager::CreateObjects()
{
	SS_TRY
	{
		if (m_pSyntaxConstructionGroupsSQL == NULL)
		{
			m_pSyntaxConstructionGroupsSQL = new SS::SyntaxConstructions::SQL::CSyntaxConstructionGroups(m_szBaseName);
			m_pSyntaxConstructionGroupsSQL->SetLoadManager(GetLoadManager());
		}
		if (m_pDetectWordsGroupsSQL == NULL)
		{
			m_pDetectWordsGroupsSQL = new SS::SyntaxConstructions::SQL::CDetectWordsGroups(m_szBaseName);
			m_pDetectWordsGroupsSQL->SetLoadManager(GetLoadManager());
		}
		if (m_pSyntaxConstuctionsSQL == NULL)
		{
			m_pSyntaxConstuctionsSQL = new SS::SyntaxConstructions::SQL::CSyntaxConstuctions(m_szBaseName,m_pDetectWordsGroupsSQL);
			m_pSyntaxConstuctionsSQL->SetLoadManager(GetLoadManager());
		}
	}
	SS_CATCH(L"")
}

//Возвращает интерфес для работы с группами синтаксических конструкций	
SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroups* 
SS::SyntaxConstructions::CSyntaxConstructionManager::GetSyntaxConstructionGroups()
{
	SS_TRY
	{
		switch (m_eDataBaseType)
		{
		case edbtDBMS:
			if (m_pSyntaxConstructionGroupsDBMS == NULL)
			{
				m_pSyntaxConstructionGroupsDBMS = new SS::SyntaxConstructions::DBMS::CSyntaxConstructionGroups(m_szBaseName);
				m_pSyntaxConstructionGroupsDBMS->SetLoadManager(GetLoadManager());
			}
			return (SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroups*) m_pSyntaxConstructionGroupsDBMS;
			break;
		
		case edbtSQL:
			if (m_pSyntaxConstructionGroupsSQL == NULL)
			{
				m_pSyntaxConstructionGroupsSQL = new SS::SyntaxConstructions::SQL::CSyntaxConstructionGroups( m_szBaseName);
				m_pSyntaxConstructionGroupsSQL->SetLoadManager(GetLoadManager());
			}
			return (SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroups*) m_pSyntaxConstructionGroupsSQL;
			break;
		
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	
	}
	SS_CATCH(L"");
	return NULL;
}

//Возвращает интерфес для работы с синтаксическими конструкциями
SS::Interface::Core::SyntaxConstructions::ISyntaxConstructions*
SS::SyntaxConstructions::CSyntaxConstructionManager::GetSyntaxConstructions()
{
	SS_TRY
	{
		switch (m_eDataBaseType)
		{
		case edbtDBMS:
			if (m_pSyntaxConstuctionsDBMS == NULL)
			{
				m_pSyntaxConstuctionsDBMS = new SS::SyntaxConstructions::DBMS::CSyntaxConstuctions(m_szBaseName,	GetDetectWordsGroups());
				m_pSyntaxConstuctionsDBMS->SetLoadManager(GetLoadManager());
			}
			return (SS::Interface::Core::SyntaxConstructions::ISyntaxConstructions*) m_pSyntaxConstuctionsDBMS;
			break;
		
		case edbtSQL:
			if (m_pSyntaxConstuctionsSQL == NULL)
			{
				m_pSyntaxConstuctionsSQL = new SS::SyntaxConstructions::SQL::CSyntaxConstuctions(m_szBaseName, GetDetectWordsGroups());
				m_pSyntaxConstuctionsSQL->SetLoadManager(GetLoadManager());
			}
			return (SS::Interface::Core::SyntaxConstructions::ISyntaxConstructions*) m_pSyntaxConstuctionsSQL;
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	
	}
	SS_CATCH(L"");
	return NULL;
}

//Возвращает интерфес для работы с группами синтаксических конструкций и их названиями
SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroupsNames*
SS::SyntaxConstructions::CSyntaxConstructionManager::GetSyntaxConstructionGroupsNames()
{
	SS_TRY
	{
		switch (m_eDataBaseType)
		{
		case edbtDBMS:
			break;
		case edbtSQL:
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	
	}
	SS_CATCH(L"");
	return NULL;
}

//Возвращает интерфес для работы с группами слов для детектирования
SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups*
SS::SyntaxConstructions::CSyntaxConstructionManager::GetDetectWordsGroups()
{
	SS_TRY
	{
		switch (m_eDataBaseType)
		{
		case edbtDBMS:
			if (m_pDetectWordsGroupsDBMS == NULL)
			{
				m_pDetectWordsGroupsDBMS = new SS::SyntaxConstructions::DBMS::CDetectWordsGroups(m_szBaseName);
				m_pDetectWordsGroupsDBMS->SetLoadManager(GetLoadManager());
			}
			return (SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups*) m_pDetectWordsGroupsDBMS;
			break;
		
		case edbtSQL:
			if (m_pDetectWordsGroupsSQL == NULL)
			{
				m_pDetectWordsGroupsSQL = new SS::SyntaxConstructions::SQL::CDetectWordsGroups(m_szBaseName);
				m_pDetectWordsGroupsSQL->SetLoadManager(GetLoadManager());
			}
			return (SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups*) m_pDetectWordsGroupsSQL;
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}
	}
	SS_CATCH(L"");
	return NULL;
}
