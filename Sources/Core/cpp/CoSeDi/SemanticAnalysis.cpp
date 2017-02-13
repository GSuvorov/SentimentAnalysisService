#include "StdAfx.h"

#include ".\semanticanalysis.h"



bool	SS::CoSeDi::SQL::CSemanticAnalysisSynonims::GetClasses(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex, /*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_Class)
{
	SS_TRY
	{
		if (!m_pCashCoSeDi) 
		{
			SS_THROW(L"Не выполнена загрузка CoSeDi");
		}

		CPreparedSTSemanticsSynonimsGetByIDSource oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());

		oPrepared.SetIDSource( oDictionaryIndex.GetId() );
		oPrepared.SetDictionaryNumber(oDictionaryIndex.GetDictionaryNumber());

		if(oPrepared.Open() == S_OK)
		{
			if(oPrepared.MoveFirst() != DB_S_ENDOFROWSET)
			{				
				do
				{	
					p_l_Class->push_back(oPrepared.GetClass());
				
				}while(oPrepared.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepared.Close();
	}
	SS_CATCH(L"")

	return true;

}

void	SS::CoSeDi::SQL::CSemanticAnalysisSynonims::GetWords(/*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_DictionaryIndex)
{
	SS_TRY
	{

		CPreparedSTSemanticsSynonimsGetByClass oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());
		
		oPrepared.SetClass((int)m_oCurrentClass.GetId());

		if(oPrepared.Open() == S_OK)
		{
			SS::CoSeDi::DataTypes::TFormInfo oFormInfo;
			if(oPrepared.MoveFirst() != DB_S_ENDOFROWSET)
			{				
				do
				{
					SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
					oDictionaryIndex.SetId(oPrepared.GetIDSource());
					oDictionaryIndex.SetDictionaryNumber((SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber)oPrepared.GetDictionaryNumber());
					
					p_l_DictionaryIndex->push_back(oDictionaryIndex);

				}while(oPrepared.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepared.Close();		
	}
	SS_CATCH(L"")


}

bool	SS::CoSeDi::SQL::CSemanticAnalysisSynonims::AddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{
		if (!m_pCashCoSeDi) 
		{
			SS_THROW(L"Не выполнена загрузка CoSeDi");
		}

		CPreparedITSemanticsSynonimsAddWord oPrepared;

		oPrepared.Prepare(*m_pConnection->GetSession());
		oPrepared.SetIDSource(oDictionaryIndex.GetId());
		oPrepared.SetDictionaryNumber(oDictionaryIndex.GetDictionaryNumber());
		oPrepared.SetClass(m_oCurrentClass.GetId());
		if(oPrepared.Open() != S_OK)
		{
			oPrepared.Close();
			return false;
		}

		oPrepared.Close();
		
//		m_pCashCoSeDi->AddIDSource(oDictionaryIndex.GetId(), wrSemantic);
	}
	SS_CATCH(L"")

	return true;

}

bool	SS::CoSeDi::SQL::CSemanticAnalysisSynonims::DeleteWord (/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{
		CPreparedDTSemanticsSynonims oPrepared;

		oPrepared.Prepare(*m_pConnection->GetSession());
		oPrepared.SetIDSource(oDictionaryIndex.GetId());
		oPrepared.SetDictionaryNumber(oDictionaryIndex.GetDictionaryNumber());
		oPrepared.SetClass(m_oCurrentClass.GetId());
		oPrepared.Open();
		oPrepared.Close();
	}
	SS_CATCH(L"")

	return true;

}

bool	SS::CoSeDi::SQL::CSemanticAnalysisSynonims::DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber)
{
	SS_TRY
	{
	}
	SS_CATCH(L"")
	return(FALSE);
}

int		SS::CoSeDi::SQL::CSemanticAnalysisSynonims::CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{
	}
	SS_CATCH(L"")
	return(FALSE);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool	SS::CoSeDi::SQL::CSemanticAnalysis::GetClasses(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex, /*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_Class)
{
	SS_TRY;
	{
		if (!m_pCashCoSeDi) 
		{
			SS_THROW(L"Не выполнена загрузка CoSeDi");
		}

		CPreparedSTSemanticsGetByIDSource oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());

		oPrepared.SetIDSource( oDictionaryIndex.GetId());
		oPrepared.SetDictionaryNumber(oDictionaryIndex.GetDictionaryNumber());

		if(oPrepared.Open() == S_OK)
		{
			if(oPrepared.MoveFirst() != DB_S_ENDOFROWSET)
			{				
				do
				{
					SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
					oDictionaryIndex.SetId(oPrepared.GetClass());

					p_l_Class->push_back(oDictionaryIndex);
				
				}while(oPrepared.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepared.Close();
	}
	SS_CATCH(L"");

	return true;

}

void	SS::CoSeDi::SQL::CSemanticAnalysis::GetWords(/*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_DictionaryIndex)
{
	SS_TRY;
	{

		CPreparedSTSemanticsGetByClass oPrepared;
		oPrepared.Prepare(*m_pConnection->GetSession());
		
		oPrepared.SetClass(m_oCurrentClass.GetId());

		if(oPrepared.Open() == S_OK)
		{
			SS::CoSeDi::DataTypes::TFormInfo oFormInfo;
			if(oPrepared.MoveFirst() != DB_S_ENDOFROWSET)
			{				
				do
				{
					SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
					oDictionaryIndex.SetId(oPrepared.GetIDSource());
					oDictionaryIndex.SetDictionaryNumber( (SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber)oPrepared.GetDictionaryNumber());
					
					p_l_DictionaryIndex->push_back(oDictionaryIndex);

				}while(oPrepared.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepared.Close();		
	}
	SS_CATCH(L"");


}

bool	SS::CoSeDi::SQL::CSemanticAnalysis::AddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY;
	{
		if (!m_pCashCoSeDi) 
		{
			SS_THROW(L"Не выполнена загрузка CoSeDi");
		}

		CPreparedITSemanticsAddWord oPrepared;

		oPrepared.Prepare(*m_pConnection->GetSession());
		oPrepared.SetIDSource(oDictionaryIndex.GetId());
		oPrepared.SetDictionaryNumber(oDictionaryIndex.GetDictionaryNumber());
		oPrepared.SetClass(m_oCurrentClass.GetId());
		if(oPrepared.Open() != S_OK)
		{
			oPrepared.Close();
			return false;
		}

		oPrepared.Close();
		
//		m_pCashCoSeDi->AddIDSource(oDictionaryIndex.GetId(), wrSemantic);
	}
	SS_CATCH(L"");

	return true;

}

bool	SS::CoSeDi::SQL::CSemanticAnalysis::DeleteWord (/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY;
	{
		CPreparedDTSemantics oPrepared;

		oPrepared.Prepare(*m_pConnection->GetSession());
		oPrepared.SetIDSource(oDictionaryIndex.GetId());
		oPrepared.SetDictionaryNumber(oDictionaryIndex.GetDictionaryNumber());
		oPrepared.SetClass(m_oCurrentClass.GetId());
		oPrepared.Open();
		oPrepared.Close();
	}
	SS_CATCH(L"");

	return true;

}


bool	SS::CoSeDi::SQL::CSemanticAnalysis::DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber)
{
	SS_TRY
	{
		SS_THROW(L"Метод не используется")
	}
	SS_CATCH(L"")

	return true;
}

int		SS::CoSeDi::SQL::CSemanticAnalysis::CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{
	}
	SS_CATCH(L"")
	return(0);
}


////////////////////////////////////////////////////////////////////////////////////////////

bool	SS::CoSeDi::DBMS::CSemanticAnalysis::GetClasses(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex, /*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_Class)
{
	SS_TRY
	{

		if (!m_pCashCoSeDi) 
		{
			SS_THROW(L"Не выполнена загрузка CoSeDi");
		}

		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
			InitConnection();
		
		m_SSemanticsAccess.IDSource = oDictionaryIndex.GetDictionaryIndexWithoutFlags();		
		
		while(m_pDTable->Find(efmByIDSource)==S_OK)   
		{
			SS::Dictionary::Types::TDictionaryIndex  oSemanticsClass(m_SSemanticsAccess.IDClass);

			//oDictionaryIndex.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
			//oDictionaryIndex.SetId(m_SSemanticsAccess.IDClass);
			//oDictionaryIndex.SetGenericMarker((m_SSemanticsAccess.DictionaryNumber>>7));
			p_l_Class->push_back(oSemanticsClass);			
		}
	}
	SS_CATCH(L"")

	return true;

}

bool SS::CoSeDi::DBMS::CSemanticAnalysis::InitConnection(void)
{
	SS_TRY
	{
		if(!SS::CoSeDi::DBMS::CConnect::InitConnection(m_wDataTableName))
			return false;
			
		TableRecord[0]	= &m_SSemanticsAccess.IDSource;
		TableRecord[1]	= &m_SSemanticsAccess.DictionaryNumber;
		TableRecord[2]	= &m_SSemanticsAccess.IDClass;

		if(m_pDTable->FindInit(L"IDSource", &m_SSemanticsAccess.IDSource, TableRecord, efmByIDSource)!=S_OK)   
		{
			SS_THROW(L"Ошибка инициализации поиска по основам");
		}
		
		if(m_pDTable->FindInit(L"IDClass", &m_SSemanticsAccess.IDClass, TableRecord, efmByIDClass)!=S_OK)   
		{
			SS_THROW(L"Ошибка инициализации поиска по классам");
		}

		if(m_pDTable->AddInit(TableRecord) !=S_OK)   
		{
			SS_THROW(L"Ошибка инициализации процедуры добаления");
		}

		return true;

	}
	SS_CATCH(L"")

	return true;
}

void	SS::CoSeDi::DBMS::CSemanticAnalysis::GetWords(/*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_DictionaryIndex)
{
	SS_TRY;
	{
		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
			InitConnection();	
		
		SS::CoSeDi::DataTypes::TFormInfo oFormInfo;
		
		m_SSemanticsAccess.IDClass	= m_oCurrentClass.GetDictionaryIndex();		
		
		while(m_pDTable->Find(efmByIDClass)==S_OK)   
		{
			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex(m_SSemanticsAccess.IDSource);
			//oDictionaryIndex.SetId(m_SSemanticsAccess.IDSource);
			//oDictionaryIndex.SetDictionaryNumber((SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber)m_SSemanticsAccess.DictionaryNumber);

			p_l_DictionaryIndex->push_back(oDictionaryIndex);			
		}
	}
	SS_CATCH(L"");
}

bool	SS::CoSeDi::DBMS::CSemanticAnalysis::AddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{
		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
			InitConnection();	
		
		m_SSemanticsAccess.IDSource			= oDictionaryIndex.GetDictionaryIndex();
		//oDictionaryIndex.SetDictionaryNumber(oDictionaryIndex.GetDictionaryNumber());
		m_SSemanticsAccess.IDClass			= m_oCurrentClass.GetDictionaryIndex();

		if(m_pDTable->Add() != S_OK)
			return false;

		return true;

	}	
	SS_CATCH(L"")

	return true;
}

bool	SS::CoSeDi::DBMS::CSemanticAnalysis::DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber)
{
	SS_TRY
	{

		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
			InitConnection();
		
		m_SSemanticsAccess.IDClass = m_oCurrentClass.GetDictionaryIndex();		
		
		while(m_pDTable->Find(efmByIDClass)==S_OK)   
		{
			if(m_pDTable->DeleteRecord(m_pDTable->GetFoundIdx(efmByIDClass)) != S_OK)
			{
				SS_THROW(L"Ошибка удаления записи")
			}			
		}
		return true;
	}
	SS_CATCH(L"")
	return(FALSE);
}

bool	SS::CoSeDi::DBMS::CSemanticAnalysis::DeleteWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{

		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
			InitConnection();
		
		m_SSemanticsAccess.IDClass = m_oCurrentClass.GetDictionaryIndex();		
		
		while(m_pDTable->Find(efmByIDClass)==S_OK)   
		{
			if(/*(m_SSemanticsAccess.DictionaryNumber	== oDictionaryIndex.GetDictionaryNumber())&&
				*/(m_SSemanticsAccess.IDSource		== oDictionaryIndex.GetDictionaryIndex()))
			{	
				if(m_pDTable->DeleteRecord(m_pDTable->GetFoundIdx(efmByIDClass)) != S_OK)
				{
					SS_THROW(L"Ошибка удаления записи")
				}
			}			
		}		
	}
	SS_CATCH(L"")

	return true;
}

int		SS::CoSeDi::DBMS::CSemanticAnalysis::CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{

		SS_THROW(L"Данная версия комбинаторно-семантического словаря не поддерживает требуемую функциональность")

	}
	SS_CATCH(L"")
	return(0);
}

