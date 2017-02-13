#include "StdAfx.h"


#include ".\semanticidsourcetree.h"


/*	SQL	*/
SS::CoSeDi::SQL::CSemanticIDSourceTree::CSemanticIDSourceTree(const wchar_t* szBaseName)  
				: SS::CoSeDi::SQL::CConnect(szBaseName)
{
	SS_TRY;
	SS_CATCH(L"");
}

SS::CoSeDi::SQL::CSemanticIDSourceTree::~CSemanticIDSourceTree(void)
{
}
/*	возвращает всех родителей текущего ребенка	*/
void SS::CoSeDi::SQL::CSemanticIDSourceTree::GetParents(std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource)
{
	SS_TRY;
		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		
		CPreparedCSSemanticIDSourceTree oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());

		GetParent( m_oFullIDSourceChild, oPrepared, plistOfFullIDSource);
	SS_CATCH(L"");
}
/*	возвращает родителя поданного на вход ребенка	*/
void SS::CoSeDi::SQL::CSemanticIDSourceTree::GetParent(
	SS::CoSeDi::DataTypes::SFullIDSource& oFullIDSource,
	SS::CoSeDi::SQL::SQL_ACCESS::Semantic::CPreparedCSSemanticIDSourceTree& oPrepared,
	std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource)
{
	SS_TRY;
			
		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		

		oPrepared.SetDictionaryNumberChild(oFullIDSource.m_uchDictionaryNumber);
		oPrepared.SetIDSourceChild(*((long*)&oFullIDSource.m_ouiIDSource));

		if(oPrepared.Open() == S_OK)
		{
			if(oPrepared.MoveFirst() != DB_S_ENDOFROWSET)
			{	
				oFullIDSource.m_ouiIDSource			= oPrepared.GetIDSourceParent();
				oFullIDSource.m_uchDictionaryNumber = (unsigned char)oPrepared.GetDictionaryNumberParent();
				
				plistOfFullIDSource->push_back(oFullIDSource);
				
				GetParent(oFullIDSource, oPrepared, plistOfFullIDSource);
				oPrepared.Close();
			}
		}
		oPrepared.Close();

	SS_CATCH(L"");
}


/*	добавляет текущую пару (родитель, ребенок)	*/				
void SS::CoSeDi::SQL::CSemanticIDSourceTree::Add()
{
	SS_TRY;
		
		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		CPreparedCISemanticIDSourceTree oPrepared;
		
		oPrepared.Prepare(*m_pConnection->GetSession());

		oPrepared.SetDictionaryNumberParent( (short)m_oFullIDSourceParent.m_uchDictionaryNumber );
		oPrepared.SetIDSourceParent(  *((long*)&m_oFullIDSourceParent.m_ouiIDSource) );
		
		oPrepared.SetDictionaryNumberChild( (short)m_oFullIDSourceChild.m_uchDictionaryNumber );
		oPrepared.SetIDSourceChild( *((long*)&m_oFullIDSourceChild.m_ouiIDSource) );
		
		oPrepared.Open();
		oPrepared.Close();

	SS_CATCH(L"");
}
/*	удаляет пару текущую (родитель, ребенок)	*/				
void SS::CoSeDi::SQL::CSemanticIDSourceTree::Delete()
{
	SS_TRY;
		
		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		CPreparedCDSemanticIDSourceTree oPrepared;
		
		oPrepared.Prepare(*m_pConnection->GetSession());

		oPrepared.SetDictionaryNumberParent( (short)m_oFullIDSourceParent.m_uchDictionaryNumber );
		oPrepared.SetIDSourceParent(  *((long*)&m_oFullIDSourceParent.m_ouiIDSource) );
		
		oPrepared.SetDictionaryNumberChild( (short)m_oFullIDSourceChild.m_uchDictionaryNumber );
		oPrepared.SetIDSourceChild( *((long*)&m_oFullIDSourceChild.m_ouiIDSource) );
		
		oPrepared.Open();
		oPrepared.Close();


	SS_CATCH(L"");
}


////////////////////////////////////////////////////////////////////////////////////////////////
/*	DBMS	*/
SS::CoSeDi::DBMS::CSemanticIDSourceTree::CSemanticIDSourceTree(const wchar_t* szBaseName)  
				: SS::CoSeDi::DBMS::CConnect(szBaseName)
{
	SS_TRY;
	SS_CATCH(L"");
}

SS::CoSeDi::DBMS::CSemanticIDSourceTree::~CSemanticIDSourceTree(void)
{
}

bool SS::CoSeDi::DBMS::CSemanticIDSourceTree::InitConnection(void)
{
	SS_TRY;
	{
		std::wstring wDataTableName = L"SemanticIDSourceTree";

		if(!SS::CoSeDi::DBMS::CConnect::InitConnection(wDataTableName))
			return false;
		
		TableRecord[0] = &m_SemanticIDSourceTree.DictionaryNumberParent; 
		TableRecord[1] = &m_SemanticIDSourceTree.IDSourceParent; 
		TableRecord[2] = &m_SemanticIDSourceTree.DictionaryNumberChild; 
		TableRecord[3] = &m_SemanticIDSourceTree.IDSourceChild;
		
		if(m_pDTable->FindInit(L"IDSourceParent", &m_SemanticIDSourceTree.IDSourceParent, TableRecord, efmByIDParent)!=S_OK)   
		{
			SS_THROW(L"Ошибка инициализации поиска по основе родителя таблицы SemanticIDSourceTree");
		}
		
		if(m_pDTable->FindInit(L"IDSourceChild", &m_SemanticIDSourceTree.IDSourceChild, TableRecord, efmByIDChild)!=S_OK)   
		{
			SS_THROW(L"Ошибка инициализации поиска по основе ребенка таблицы SemanticIDSourceTree");
		}
	}		
	SS_CATCH(L"");

	return true;
}


/*	возвращает всех родителей текущего ребенка	*/
void SS::CoSeDi::DBMS::CSemanticIDSourceTree::GetParents(std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource)
{
	SS_TRY;
	{
		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
		{
			InitConnection();
		}
		
		SS::CoSeDi::DataTypes::SFullIDSource oCurChild;
		
		oCurChild.m_ouiIDSource			= m_oFullIDSourceChild.m_ouiIDSource;
		oCurChild.m_uchDictionaryNumber	= m_oFullIDSourceChild.m_uchDictionaryNumber;

		GetParents(plistOfFullIDSource, oCurChild);

	
	}
	SS_CATCH(L"");
}

void SS::CoSeDi::DBMS::CSemanticIDSourceTree::GetParents(std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource, SS::CoSeDi::DataTypes::SFullIDSource& oCurChild)
{
	SS_TRY;
	{
		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
		{
			InitConnection();
		}	
		
		m_SemanticIDSourceTree.IDSourceChild		= *(int*)&oCurChild.m_ouiIDSource;		
        
		if(m_pDTable->Find(efmByIDChild) != S_OK)
			return;

		m_pDTable->FindStop();

		if(m_SemanticIDSourceTree.DictionaryNumberChild == oCurChild.m_uchDictionaryNumber)
		{
			oCurChild.m_ouiIDSource			= m_SemanticIDSourceTree.IDSourceParent;
			oCurChild.m_uchDictionaryNumber = m_SemanticIDSourceTree.DictionaryNumberParent;
			plistOfFullIDSource->push_back(oCurChild);
			GetParents(plistOfFullIDSource, oCurChild);	
		}
		
	}
	SS_CATCH(L"");
}

/*	добавляет текущую пару (родитель, ребенок)	*/				
void SS::CoSeDi::DBMS::CSemanticIDSourceTree::Add()
{
	SS_TRY

		SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	
	SS_CATCH(L"");
}
/*	удаляет пару текущую (родитель, ребенок)	*/				
void SS::CoSeDi::DBMS::CSemanticIDSourceTree::Delete()
{
	SS_TRY

		SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	
	SS_CATCH(L"");
}
