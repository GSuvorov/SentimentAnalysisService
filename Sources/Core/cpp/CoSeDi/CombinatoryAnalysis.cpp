#include "StdAfx.h"



#include ".\combinatoryanalysis.h"



bool	SS::CoSeDi::SQL::CCombinatoryAnalysis::GetCombinations(/*in*/ SS::CoSeDi::DataTypes::TFormInfo & oFormInfo, /*out*/TlistOfCombination * p_list_CombinationList)
{
	SS_TRY;

		if (!m_pCashCoSeDi) 
		{
			SS_THROW(L"Не выполнена загрузка CoSeDi");
		}

		//if(!m_pCashCoSeDi->IsExistIDSource(oFormInfo.m_stIDSource.ushIDRoot, wrCombinatory))
		//	return false;

		
		pSAllCombination = new SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSAllCombination();

		pSAllCombination->Prepare(*m_pConnection->GetSession());

		pSAllCombination->SetIDSource(oFormInfo.m_stIDSource.ushIDRoot);
		pSAllCombination->SetDictionaryNumber(oFormInfo.m_uchDictionaryNumber);
		pSAllCombination->SetIDForm(oFormInfo.m_shIDForm);

		if(pSAllCombination->Open() == S_OK)
		{
			if(pSAllCombination->MoveFirst() != DB_S_ENDOFROWSET)
			{				
				do
				{	
					p_list_CombinationList->push_back(pSAllCombination->GetID());

				}while(pSAllCombination->MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		pSAllCombination->Close();

		if(pSAllCombination != NULL)
		{
			delete pSAllCombination;
			pSAllCombination = NULL;
		}

	SS_CATCH(L"");

	return true;

}

int		SS::CoSeDi::SQL::CCombinatoryAnalysis::CreateCombination(/*out*/ TCombinationInfo * p_CombinationInfo)
{
	int iNewID = -1;

	SS_TRY;

		if (!m_pCashCoSeDi) 
		{
			SS_THROW(L"Не выполнена загрузка CoSeDi");
		}


		pITCombinatory		= new SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedITCombinatory;


		iNewID = GetLastCombinationID() + 1;
		pITCombinatory->Prepare(*m_pConnection->GetSession());

		
		int iLevel = 1;
		T_list_FormInfo::iterator iter;

		for(iter = p_CombinationInfo->m_T_list_FormInfo.begin(); 
			iter != p_CombinationInfo->m_T_list_FormInfo.end();
			iter++)
		{
			pITCombinatory->SetID(iNewID);
			pITCombinatory->SetIDType(p_CombinationInfo->shIDType);
			pITCombinatory->SetIDForm(iter->m_shIDForm);
			pITCombinatory->SetIDSource(*((int*)&iter->m_stIDSource));
			pITCombinatory->SetDictionaryNumber(iter->m_uchDictionaryNumber);
			pITCombinatory->SetLevel(iLevel++);
			pITCombinatory->SetCombiType(p_CombinationInfo->eCombiType);
						
			pITCombinatory->Open();
			pITCombinatory->Close();

			/// добавляем основу в кеш
			m_pCashCoSeDi->AddIDSource(iter->m_stIDSource.ushIDRoot, wrCombinatory);
		}		

		if(pITCombinatory != NULL)
		{
			delete pITCombinatory;
			pITCombinatory = NULL;
		}

	SS_CATCH(L"");

	return iNewID;


}

void	SS::CoSeDi::SQL::CCombinatoryAnalysis::GetCombinationInfo(/*out*/ TCombinationInfo * p_CombinationInfo)
{
	SS_TRY;
	
		pSTCombinatoryInfo		= new SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSTCombinatoryInfo;
		pSTCombinatoryInfo->Prepare(*m_pConnection->GetSession());

		pSTCombinatoryInfo->SetID(m_iIDCombination);
		if(pSTCombinatoryInfo->Open() == S_OK)
		{
			if(pSTCombinatoryInfo->MoveFirst() != DB_S_ENDOFROWSET)
			{				
				p_CombinationInfo->shIDType    = pSTCombinatoryInfo->GetIDType();
				p_CombinationInfo->eCombiType  = pSTCombinatoryInfo->GetCombiType();
				
				SS::CoSeDi::DataTypes::TFormInfo oFormInfo;

				do
				{	
					oFormInfo.m_shIDForm			= pSTCombinatoryInfo->GetIDForm();
					oFormInfo.m_stIDSource			= pSTCombinatoryInfo->GetIDSource(); 
					oFormInfo.m_uchDictionaryNumber	= (unsigned char)pSTCombinatoryInfo->GetDictionaryNumber();
					oFormInfo.m_shIDForm            = pSTCombinatoryInfo->GetIDForm();

					p_CombinationInfo->m_T_list_FormInfo.push_back(oFormInfo);

				}while(pSTCombinatoryInfo->MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		pSTCombinatoryInfo->Close();

		if(pSTCombinatoryInfo != NULL)
		{
			delete pSTCombinatoryInfo;
			pSTCombinatoryInfo = NULL;
		}


	SS_CATCH(L"");

}

bool	SS::CoSeDi::SQL::CCombinatoryAnalysis::DeleteCombination()
{
	SS_TRY;

		if(m_iIDCombination < 0)
			SS_THROW(L"индекс комбинации меньше нуля");

		SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedDTCombinatory oDeleteFromCombinatory;
		oDeleteFromCombinatory.Prepare(*m_pConnection->GetSession());
		
		oDeleteFromCombinatory.SetID(m_iIDCombination);
		oDeleteFromCombinatory.Open();
		oDeleteFromCombinatory.Close();

	SS_CATCH(L"");

	return true;

}

int SS::CoSeDi::SQL::CCombinatoryAnalysis::GetLastCombinationID()
{
	int iLastID = 0;

	SS_TRY;

	
		pSMaxTCombinatory	= new SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSMaxTCombinatory;
		pSMaxTCombinatory->Prepare(*m_pConnection->GetSession());
		if(pSMaxTCombinatory->Open() == S_OK)
		{
			if(pSMaxTCombinatory->MoveFirst() != DB_S_ENDOFROWSET)
			{					
				do
				{
					iLastID = pSMaxTCombinatory->GetID();
				}while(pSMaxTCombinatory->MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		pSMaxTCombinatory->Close();

		if(pSMaxTCombinatory != NULL)
		{
			delete pSMaxTCombinatory;
			pSMaxTCombinatory = NULL;
		}
	
	SS_CATCH(L"");

	return iLastID;

}

/////////////////////////////////////////////////////////////////////////////////


bool	SS::CoSeDi::DBMS::CCombinatoryAnalysis::GetCombinations(/*in*/ SS::CoSeDi::DataTypes::TFormInfo & oFormInfo, /*out*/SS::CoSeDi::DataTypes::TlistOfCombination * p_list_CombinationList)
{
	bool bRetVal = false;
	SS_TRY;	


		if (!m_pCashCoSeDi) 
		{
			SS_THROW(L"Не выполнена загрузка CoSeDi");
		}


//		if(!m_pCashCoSeDi->IsExistIDSource(oFormInfo.m_stIDSource.ushIDRoot, wrCombinatory))
//			return bRetVal;

		m_SCombinatoryAccess.IDSource = *((INT32 *)&oFormInfo.m_stIDSource) | (oFormInfo.m_uchDictionaryNumber<<24);
		
		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
		{
			InitConnection();
		}

		while(m_pDTable->Find(efmByIDSource)==S_OK)   
		{
			if((m_SCombinatoryAccess.DictionaryNumber == oFormInfo.m_uchDictionaryNumber)&&
				((m_SCombinatoryAccess.IDForm == oFormInfo.m_shIDForm) || (m_SCombinatoryAccess.IDForm == -1)))
			{
				bRetVal = true;
				p_list_CombinationList->push_back(m_SCombinatoryAccess.ID);
			}
		}

	SS_CATCH(L"");
    
	return bRetVal;
}

int		SS::CoSeDi::DBMS::CCombinatoryAnalysis::CreateCombination(/*out*/ TCombinationInfo * p_CombinationInfo)
{

	SS_TRY;
	{
		SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	}
	SS_CATCH(L"");

	return -1;
}

void	SS::CoSeDi::DBMS::CCombinatoryAnalysis::GetCombinationInfo(/*out*/ TCombinationInfo * p_CombinationInfo)
{
	SS_TRY;
	
	
	
		if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
		{
			InitConnection();
		}
		
		m_SCombinatoryAccess.ID = m_iIDCombination;

		if(m_pDTable->Find(efmByID) != S_OK)
			return;

		p_CombinationInfo->shIDType    = m_SCombinatoryAccess.IDType;
		p_CombinationInfo->eCombiType  = (ECombiType)m_SCombinatoryAccess.CombiType;			
		   
		std::list<SS::CoSeDi::DataTypes::SCombinatoryAccess> m_listCombinatoryAccess;
		do
		{						
			m_listCombinatoryAccess.push_back(m_SCombinatoryAccess);

		}while( m_pDTable->Find(efmByID) == S_OK );

		m_listCombinatoryAccess.sort();

		SS::CoSeDi::DataTypes::TFormInfo oFormInfo;
		
		for(std::list<SS::CoSeDi::DataTypes::SCombinatoryAccess>::iterator iter = m_listCombinatoryAccess.begin();
			iter != m_listCombinatoryAccess.end(); iter++ )
		{
			oFormInfo.m_shIDForm				= iter->IDForm;
			oFormInfo.m_stIDSource.ushIDRoot	= iter->IDSource;
			oFormInfo.m_uchDictionaryNumber		= iter->DictionaryNumber;
			p_CombinationInfo->m_T_list_FormInfo.push_back(oFormInfo);			
		}
		
		return ;

	SS_CATCH(L"");
}

bool	SS::CoSeDi::DBMS::CCombinatoryAnalysis::DeleteCombination()
{
	SS_TRY;
	{
		SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	}
	SS_CATCH(L"");

	return false;
}

bool SS::CoSeDi::DBMS::CCombinatoryAnalysis::InitConnection(void)
{
	SS_TRY
	{
		std::wstring wDataTableName = L"Combinatory";

		if(!SS::CoSeDi::DBMS::CConnect::InitConnection(wDataTableName))
			return false;

		TableRecord[0] = &m_SCombinatoryAccess.ID; 
		TableRecord[1] = &m_SCombinatoryAccess.IDType; 
		TableRecord[2] = &m_SCombinatoryAccess.IDForm; 
		TableRecord[3] = &m_SCombinatoryAccess.DictionaryNumber;
		TableRecord[4] = &m_SCombinatoryAccess.IDSource; 			 
		TableRecord[5] = &m_SCombinatoryAccess.Level;
		TableRecord[6] = &m_SCombinatoryAccess.CombiType;
		
		if(m_pDTable->FindInit(L"ID", &m_SCombinatoryAccess.ID, TableRecord, efmByID)!=S_OK)   
			SS_THROW(L"Ошибка инициализации поиска в таблице Combinatory по индексу комбинации");;
		
		if(m_pDTable->FindInit(L"IDSource", &m_SCombinatoryAccess.IDSource, TableRecord, efmByIDSource)!=S_OK)   
			SS_THROW(L"Ошибка инициализации поиска в таблице Combinatory по индексу основы");		
	}	
	SS_CATCH(L"")

	return true;
}


