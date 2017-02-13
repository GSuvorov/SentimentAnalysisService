#include "StdAfx.h"


#include ".\cosedimanager.h"


SS::CoSeDi::CCoSeDiManager::CCoSeDiManager() : 
		m_pCashCoSeDiDBMS(NULL),               m_pCashCoSeDiSQL(NULL),
		m_pSyntaxAnalysisDBMS(NULL),           m_pSyntaxAnalysisSQL(NULL),            
		m_pCombinatory(NULL),                  m_pCombinatoryExtra(NULL),
		m_pSemanticAnalysisDBMS(NULL),         m_pSemanticAnalysisExtraDBMS(NULL),
		m_pSemanticAnalysisSQL(NULL),          m_pSemanticAnalysisExtraSQL(NULL),
		m_pSemanticClassesTree(NULL),          m_pSemanticClassesWords(NULL),
		m_pSemanticIDSourceTreeDBMS(NULL),     m_pSemanticIDSourceTreeSQL(NULL),
		m_pSemanticCombiSynonimsDBMS(NULL),    m_pSemanticCombiSynonimsSQL(NULL),
		m_pSemanticAnalysisSynonimsDBMS(NULL), m_pSemanticAnalysisSynonimsSQL(NULL), 
		m_eDataBaseType(edbtDBMS)
{
	//strcpy(m_szBaseName, szBaseName);
}

SS::CoSeDi::CCoSeDiManager::~CCoSeDiManager(void)
{
	//SS_TRY
	//{
	if(m_pCashCoSeDiSQL)
	{
		delete m_pCashCoSeDiSQL;
		m_pCashCoSeDiSQL = NULL;
	}

	if(m_pCashCoSeDiDBMS)
	{
		delete m_pCashCoSeDiDBMS;
		m_pCashCoSeDiDBMS = NULL;
	}

	if(m_pCombinatory)
	{
		delete m_pCombinatory;
		m_pCombinatory = NULL;
	}

	if(m_pCombinatoryExtra)
	{
		delete m_pCombinatoryExtra;
		m_pCombinatoryExtra = NULL;
	}

	if(m_pSemanticAnalysisDBMS)
	{
		delete m_pSemanticAnalysisDBMS;
		m_pSemanticAnalysisDBMS = NULL;
	}

	if(m_pSemanticAnalysisExtraDBMS)
	{
		delete m_pSemanticAnalysisExtraDBMS;
		m_pSemanticAnalysisExtraDBMS = NULL;
	}

	if(m_pSemanticAnalysisSQL)
	{
		delete m_pSemanticAnalysisSQL;
		m_pSemanticAnalysisSQL = NULL;
	}

	if(m_pSemanticAnalysisExtraSQL)
	{
		delete m_pSemanticAnalysisExtraSQL;
		m_pSemanticAnalysisExtraSQL = NULL;
	}
		
	if(m_pSemanticAnalysisSynonimsDBMS)
	{
		delete m_pSemanticAnalysisSynonimsDBMS;
		m_pSemanticAnalysisSynonimsDBMS = NULL;
	}

	if(m_pSemanticAnalysisSynonimsSQL)
	{
		delete m_pSemanticAnalysisSynonimsSQL;
		m_pSemanticAnalysisSynonimsSQL = NULL;
	}
		
	if(m_pSemanticClassesTree)
	{
		delete m_pSemanticClassesTree;
		m_pSemanticClassesTree = NULL;
	}

	if(m_pSemanticClassesWords)
	{
		delete m_pSemanticClassesWords;
		m_pSemanticClassesWords = NULL;
	}

	if(m_pSyntaxAnalysisDBMS)
	{
		delete m_pSyntaxAnalysisDBMS;
		m_pSyntaxAnalysisDBMS = NULL;
	}

	if(m_pSyntaxAnalysisSQL)
	{
		delete m_pSyntaxAnalysisSQL;
		m_pSyntaxAnalysisSQL = NULL;
	}

	if(m_pSemanticCombiSynonimsDBMS)
	{
		delete m_pSemanticCombiSynonimsDBMS;
		m_pSemanticCombiSynonimsDBMS = NULL;
	}

	if(m_pSemanticCombiSynonimsSQL)
	{
		delete m_pSemanticCombiSynonimsSQL;
		m_pSemanticCombiSynonimsSQL = NULL;
	}

	if(m_pSemanticIDSourceTreeDBMS)
	{
		delete m_pSemanticIDSourceTreeDBMS;
		m_pSemanticIDSourceTreeDBMS = NULL;
	}

	if(m_pSemanticIDSourceTreeSQL)
	{
		delete m_pSemanticIDSourceTreeSQL;
		m_pSemanticIDSourceTreeSQL = NULL;
	}
	//}
	//SS_CATCH(L"")
}

ULONG SS::CoSeDi::CCoSeDiManager::Release(void)
{
	delete this;
	return 0;
}

HRESULT SS::CoSeDi::CCoSeDiManager::QueryInterface(REFIID pIID, void** pBase)
{
	if (pIID==IID_ICoSeDiManager)
	{
		m_eDataBaseType = edbtDBMS;
		*pBase = (SS::Interface::Core::CoSeDi::ICoSeDiManager*)this;
	}
	else 
		if (pIID==IID_ICoSeDiManagerSQL)
		{
			m_eDataBaseType = edbtSQL;
			*pBase = (SS::Interface::Core::CoSeDi::ICoSeDiManager*)this;
		}
	else 
		if (pIID==IID_Base)
		{
			*pBase = (SS::Interface::IBase*)this;
		}
	else 
		*pBase=NULL;
	return S_OK;
}
								 
SS::Interface::Core::CoSeDi::ICoSeDiLoad * SS::CoSeDi::CCoSeDiManager::GetLoader()
{

	SS_TRY
	{

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		switch(m_eDataBaseType)
		{
		case edbtDBMS:
			if(m_pCashCoSeDiDBMS == NULL)
			{
				m_pCashCoSeDiDBMS = new SS::CoSeDi::DBMS::CCashCoSeDiEx();
				m_pCashCoSeDiDBMS->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ICoSeDiLoad *)m_pCashCoSeDiDBMS;
			break;
		case edbtSQL:
			if(m_pCashCoSeDiSQL == NULL)
			{
				m_pCashCoSeDiSQL = new SS::CoSeDi::SQL::CCashCoSeDiEx();
				m_pCashCoSeDiSQL->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ICoSeDiLoad *)m_pCashCoSeDiSQL;
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}		
	}
	SS_CATCH(L"");

	return NULL;

}

SS::Interface::Core::CoSeDi::ICombinatory * SS::CoSeDi::CCoSeDiManager::GetCombinatory()
{
	SS_TRY
	{
		GetLoader()->Load();

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		if(m_pCombinatory==NULL)   {
			m_pCombinatory = new SS::CoSeDi::DBMS::CCombinatory();
			m_pCombinatory->SetLoadManager(GetLoadManager());
			m_pCombinatory->InitConnection(L"Combinatory", FALSE);
		}

		return(m_pCombinatory);

	}	
	SS_CATCH(L"")


	return NULL;
}

SS::Interface::Core::CoSeDi::ICombinatory * SS::CoSeDi::CCoSeDiManager::GetCombinatoryExtra()
{
	SS_TRY
	{
		GetLoader()->Load();

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		if(m_pCombinatoryExtra==NULL)   {
			m_pCombinatoryExtra = new SS::CoSeDi::DBMS::CCombinatory();
			m_pCombinatoryExtra->SetLoadManager(GetLoadManager());
			m_pCombinatoryExtra->InitConnection(L"CombinatoryExtra", TRUE);
		}

		return(m_pCombinatoryExtra);

	}	
	SS_CATCH(L"")


	return NULL;
}


SS::Interface::Core::CoSeDi::ISyntaxAnalysis	* SS::CoSeDi::CCoSeDiManager::GetSyntaxAnalysis()
{

	SS_TRY
	{

		GetLoader()->Load();

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		switch(m_eDataBaseType)
		{
		case edbtDBMS:
			if(m_pSyntaxAnalysisDBMS == NULL)
			{
				m_pSyntaxAnalysisDBMS = new SS::CoSeDi::DBMS::CSyntaxAnalysisEx(&(m_pCashCoSeDiDBMS->m_oCashCoSeDiEng));
				m_pSyntaxAnalysisDBMS->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISyntaxAnalysis*)m_pSyntaxAnalysisDBMS;
			break;
		case edbtSQL:
			if(m_pSyntaxAnalysisSQL == NULL)
			{
				m_pSyntaxAnalysisSQL = new SS::CoSeDi::SQL::CSyntaxAnalysisEx(&(m_pCashCoSeDiSQL->m_oCashCoSeDiEng));
				m_pSyntaxAnalysisSQL->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISyntaxAnalysis*)m_pSyntaxAnalysisSQL;
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	
	}
	SS_CATCH(L"")

	return NULL;
}

///	возвращает интерфес для семантического анализа	
SS::Interface::Core::CoSeDi::ISemanticAnalysis          * SS::CoSeDi::CCoSeDiManager::GetRestaurantSemanticAnalysis()
{
	SS_TRY
	{
		GetLoader()->Load();

		std::wstring wTableName = L"SemanticsExtra";

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		switch(m_eDataBaseType)
		{
		case edbtDBMS:
			if(m_pSemanticAnalysisExtraDBMS == NULL)
			{
				m_pSemanticAnalysisExtraDBMS = new SS::CoSeDi::DBMS::CSemanticAnalysisEx(m_pCashCoSeDiDBMS, wTableName);
				m_pSemanticAnalysisExtraDBMS->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticAnalysisExtraDBMS;
			break;
		case edbtSQL:
			if(m_pSemanticAnalysisExtraSQL == NULL)
			{
				m_pSemanticAnalysisExtraSQL = new SS::CoSeDi::SQL::CSemanticAnalysisEx(m_pCashCoSeDiSQL, wTableName);
				m_pSemanticAnalysisExtraSQL->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticAnalysisExtraSQL;
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	
	}
	SS_CATCH(L"")

	return NULL;
}

SS::Interface::Core::CoSeDi::ISemanticAnalysis * SS::CoSeDi::CCoSeDiManager::GetSemanticAnalysis()
{

	SS_TRY
	{
		GetLoader()->Load();

		std::wstring wTableName = L"Semantics";

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		switch(m_eDataBaseType)
		{
		case edbtDBMS:
			if(m_pSemanticAnalysisDBMS == NULL)
			{
				m_pSemanticAnalysisDBMS = new SS::CoSeDi::DBMS::CSemanticAnalysisEx(m_pCashCoSeDiDBMS, wTableName);
				m_pSemanticAnalysisDBMS->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticAnalysisDBMS;
			break;
		case edbtSQL:
			if(m_pSemanticAnalysisSQL == NULL)
			{
				m_pSemanticAnalysisSQL = new SS::CoSeDi::SQL::CSemanticAnalysisEx(m_pCashCoSeDiSQL, wTableName);
				m_pSemanticAnalysisSQL->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticAnalysisSQL;
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	

		//if(m_pSemanticAnalysis == NULL)   {
		//	m_pSemanticAnalysis = new SS::CoSeDi::CSemanticAnalysis(/*m_pCashCoSeDiDBMS, wTableName*/);
		//	m_pSemanticAnalysis->SetLoadManager(GetLoadManager());
		//}
		//return (SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticAnalysis;

	}
	SS_CATCH(L"")

	return NULL;
}

SS::Interface::Core::CoSeDi::ISemanticAnalysis * SS::CoSeDi::CCoSeDiManager::GetCombiSynonimsAnalysis()
{
	SS_TRY
	{

		GetLoader()->Load();

		std::wstring wTableName = L"CombiSynonyms";

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		switch(m_eDataBaseType)
		{
		case edbtDBMS:
			if(m_pSemanticCombiSynonimsDBMS == NULL)
			{
				m_pSemanticCombiSynonimsDBMS = new SS::CoSeDi::DBMS::CSemanticAnalysisEx(m_pCashCoSeDiDBMS, wTableName);
				m_pSemanticCombiSynonimsDBMS->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticCombiSynonimsDBMS;
			break;
		case edbtSQL:
			if(m_pSemanticCombiSynonimsSQL == NULL)
			{
				m_pSemanticCombiSynonimsSQL = new SS::CoSeDi::SQL::CSemanticAnalysisEx(m_pCashCoSeDiSQL, wTableName);
				m_pSemanticCombiSynonimsSQL->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticCombiSynonimsSQL;
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	
	}
	SS_CATCH(L"")

	return NULL;

}


SS::Interface::Core::CoSeDi::ISemanticAnalysis * SS::CoSeDi::CCoSeDiManager::GetSynonimsSemanticAnalysis()
{

	SS_TRY
	{		
		GetLoader()->Load();

		std::wstring wTableName = L"SemanticsSynonims";

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		switch(m_eDataBaseType)
		{
		case edbtDBMS:
			if(m_pSemanticAnalysisSynonimsDBMS == NULL)
			{
				m_pSemanticAnalysisSynonimsDBMS = new SS::CoSeDi::DBMS::CSemanticAnalysisEx(m_pCashCoSeDiDBMS, wTableName);
				m_pSemanticAnalysisSynonimsDBMS->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticAnalysisSynonimsDBMS;
			break;
		case edbtSQL:
			if(m_pSemanticAnalysisSynonimsSQL == NULL)
			{
				m_pSemanticAnalysisSynonimsSQL = new SS::CoSeDi::SQL::CSemanticAnalysisEx(m_pCashCoSeDiSQL, wTableName);
				m_pSemanticAnalysisSynonimsSQL->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticAnalysis*)m_pSemanticAnalysisSynonimsSQL;
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	

	}
	SS_CATCH(L"")

	return NULL;
}

SS::Interface::Core::CoSeDi::ISemanticIDSourceTree		* SS::CoSeDi::CCoSeDiManager::GetSemanticIDSourceTree()
{
	SS_TRY
	{

		GetLoader()->Load();

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		switch(m_eDataBaseType)
		{
		case edbtDBMS:
			if(m_pSemanticIDSourceTreeDBMS == NULL)
			{
				m_pSemanticIDSourceTreeDBMS = new SS::CoSeDi::DBMS::CSemanticIDSourceTreeEx();
				m_pSemanticIDSourceTreeDBMS->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticIDSourceTree *)m_pSemanticIDSourceTreeDBMS;		
			break;
		case edbtSQL:
			if(m_pSemanticIDSourceTreeSQL == NULL)
			{
				m_pSemanticIDSourceTreeSQL = new SS::CoSeDi::SQL::CSemanticIDSourceTreeEx();
				m_pSemanticIDSourceTreeSQL->SetLoadManager(GetLoadManager());
			}
			return 
				(SS::Interface::Core::CoSeDi::ISemanticIDSourceTree *)m_pSemanticIDSourceTreeSQL;
			
			break;
		default :
			SS_THROW(L"не правильно указан тип базы");
			break;
		}	
	}
	SS_CATCH(L"")

	return NULL;
}

SS::Interface::Core::CoSeDi::ISemanticClassesTree * SS::CoSeDi::CCoSeDiManager::GetSemanticClassesTree()
{

	SS_TRY
	{

		GetLoader()->Load();

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		//switch(m_eDataBaseType)
		//{
		//case edbtDBMS:
		//	if(m_pSemanticClassesTreeDBMS == NULL)
		//	{
		//		m_pSemanticClassesTreeDBMS = new SS::CoSeDi::DBMS::CSemanticClassesTreeEx();
		//		m_pSemanticClassesTreeDBMS->SetLoadManager(GetLoadManager());
		//	}
		//	return 
		//		(SS::Interface::Core::CoSeDi::ISemanticClassesTree *)m_pSemanticClassesTreeDBMS;		
		//	break;
		//case edbtSQL:
		//	if(m_pSemanticClassesTreeSQL == NULL)
		//	{
		//		m_pSemanticClassesTreeSQL = new SS::CoSeDi::SQL::CSemanticClassesTreeEx();
		//		m_pSemanticClassesTreeSQL->SetLoadManager(GetLoadManager());
		//	}
		//	return 
		//		(SS::Interface::Core::CoSeDi::ISemanticClassesTree *)m_pSemanticClassesTreeSQL;
		//	
		//	break;
		//default :
		//	SS_THROW(L"не правильно указан тип базы");
		//	break;
		//}

		if(m_pSemanticClassesTree == NULL)   {
			m_pSemanticClassesTree = new SS::CoSeDi::CSemanticClassesTree();
			m_pSemanticClassesTree->SetLoadManager(GetLoadManager());
		}
		return (SS::Interface::Core::CoSeDi::ISemanticClassesTree *)m_pSemanticClassesTree;		
	}
	SS_CATCH(L"")

	return NULL;
}

SS::Interface::Core::CoSeDi::ISemanticClassesWords * SS::CoSeDi::CCoSeDiManager::GetSemanticClassesWords()
{

	SS_TRY
	{

		GetLoader()->Load();

		if(!GetLoadManager())
		{
			SS_THROW(L"GetLoadManager() == NULL, не вызван SetManagerConnection");
		}

		if(m_pSemanticClassesWords == NULL)   {
			m_pSemanticClassesWords = new SS::CoSeDi::CSemanticClassesWords();
			m_pSemanticClassesWords->SetLoadManager(GetLoadManager());
		}
		return (SS::Interface::Core::CoSeDi::ISemanticClassesWords *)m_pSemanticClassesWords;		
	}
	SS_CATCH(L"")

	return NULL;
}


