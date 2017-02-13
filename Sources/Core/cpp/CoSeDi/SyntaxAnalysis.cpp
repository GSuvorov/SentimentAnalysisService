#include "StdAfx.h"

#include ".\syntaxanalysis.h"


void SS::CoSeDi::SQL::CSyntaxAnalysis::Get(/*out*/std::list<TSyntaxInfo>* p_list_OfSyntaxInfo)
{

	SS_TRY;
		
		SS::CoSeDi::SQL::SQL_ACCESS::Syntax::CPreparedSyntaxSbyIDSource oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());
		
		oPrepared.SetIDSource(*((int*)&m_oIDSource));
		oPrepared.SetDictionaryNumber(m_oDictionaryNumber);

		if(oPrepared.Open() == S_OK)
		{
			if(oPrepared.MoveFirst() != DB_S_ENDOFROWSET)
			{					
				do
				{
					p_list_OfSyntaxInfo->push_back(oPrepared.GetSyntaxInfo());
				}while(oPrepared.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepared.Close();

		

	SS_CATCH(L"");
}

void SS::CoSeDi::SQL::CSyntaxAnalysis::Add(/*in*/std::list<TSyntaxInfo>* p_list_OfSyntaxInfo)
{

	SS_TRY;
		
		SS::CoSeDi::SQL::SQL_ACCESS::Syntax::CPreparedSyntaxI oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());

		oPrepared.SetIDSource(*((int*)&GetIDSource()));
		oPrepared.SetDictionaryNumber(m_oDictionaryNumber);

		std::list<TSyntaxInfo>::iterator iter = p_list_OfSyntaxInfo->begin();
		while( iter != p_list_OfSyntaxInfo->end())
		{
			oPrepared.SetSyntaxInfo(*iter);
			oPrepared.Open();
			oPrepared.Close();
			iter++;
		}
	
	SS_CATCH(L"");
}
void SS::CoSeDi::SQL::CSyntaxAnalysis::Delete()
{

	SS_TRY;
		
		SS::CoSeDi::SQL::SQL_ACCESS::Syntax::CPreparedSyntaxD oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());

		oPrepared.SetIDSource(*((int*)&GetIDSource()));
		oPrepared.SetDictionaryNumber(m_oDictionaryNumber);

		oPrepared.Open();
		oPrepared.Close();
	
	SS_CATCH(L"");
}

void SS::CoSeDi::SQL::CSyntaxAnalysis::Delete(/*in*/std::list<TSyntaxInfo>* p_list_OfSyntaxInfo)
{

	SS_TRY;
		
		SS::CoSeDi::SQL::SQL_ACCESS::Syntax::CPreparedSyntaxDSelected oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());

		std::list<TSyntaxInfo>::iterator iter = p_list_OfSyntaxInfo->begin();
		
		oPrepared.SetIDSource(*((int*)&GetIDSource()));
		oPrepared.SetDictionaryNumber(m_oDictionaryNumber);

		
		while(iter != p_list_OfSyntaxInfo->end())
		{
			oPrepared.SetSyntaxInfo(*iter);

			oPrepared.Open();
			oPrepared.Close();

			iter++;
		}

		
	SS_CATCH(L"");
}


///////////////////////////////////////////////////////////////////////


void SS::CoSeDi::DBMS::CSyntaxAnalysis::Get(/*out*/std::list<TSyntaxInfo>* p_list_OfSyntaxInfo)
{

	SS_TRY
	{
		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
		{
			SS_THROW(L"таблица не проинициализированна"); 
		}
		
		m_SSyntaxAccess.IDSource	= *((unsigned int*)&GetIDSource());

		while(m_pDTable->Find(efmByIDSource) == S_OK)   
		{
			if(m_oDictionaryNumber == m_SSyntaxAccess.DictionaryNumber)
			{
				p_list_OfSyntaxInfo->push_back(m_SSyntaxAccess.SyntaxInfo);
			}
		}
	}
	SS_CATCH(L"")
}

void SS::CoSeDi::DBMS::CSyntaxAnalysis::Add(/*in*/std::list<TSyntaxInfo>* p_list_OfSyntaxInfo)
{

	SS_TRY
	{
		for(std::list<TSyntaxInfo>::iterator iter = p_list_OfSyntaxInfo->begin();
			iter != p_list_OfSyntaxInfo->end();
			iter++)
		{
			
			m_SSyntaxAccess.DictionaryNumber = m_oDictionaryNumber;
			m_SSyntaxAccess.IDSource = m_oIDSource.ushIDRoot;
			m_SSyntaxAccess.SyntaxInfo = (*iter);

			if( S_OK != m_pDTable->Add() )
			{
				SS_THROW(L"Ошибка добавления записи");
			}
		}
		
		//SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	}
	SS_CATCH(L"")
}
void SS::CoSeDi::DBMS::CSyntaxAnalysis::Delete()
{
	SS_TRY;
	{
		SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	}
	SS_CATCH(L"");

}

bool SS::CoSeDi::DBMS::CSyntaxAnalysis::InitConnection(void)
{
	SS_TRY;

		std::wstring wDataTableName = L"Syntax";
		
		if(!SS::CoSeDi::DBMS::CConnect::InitConnection(wDataTableName))
			return false;		

		TableRecord[0]	= &m_SSyntaxAccess.DictionaryNumber;
		TableRecord[1]	= &m_SSyntaxAccess.IDSource;
		TableRecord[2]	= &m_SSyntaxAccess.SyntaxInfo;
		
		if(m_pDTable->FindInit(L"IDSource", &m_SSyntaxAccess.IDSource, TableRecord, efmByIDSource) != S_OK)  
		{
			SS_THROW(L"Ошибка инициализации поиска по таблице Syntax");
		}			

		if(m_pDTable->AddInit( TableRecord) != S_OK)  
		{
			SS_THROW(L"Ошибка инициализации добаления слов в таблицу Syntax");
		}			
		

	SS_CATCH(L"");

	return true;
	
}

void SS::CoSeDi::DBMS::CSyntaxAnalysis::Delete(/*in*/std::list<TSyntaxInfo>* p_list_OfSyntaxInfo)
{
	SS_TRY;
	{
		SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	}
	SS_CATCH(L"");
}
