#include "StdAfx.h"

#include "..\ASSInterface\TDictionaryNames.h"
#include "..\ASSInterface\TBaseNames.h"
#include ".\semanticanalysisex.h"


SS::CoSeDi::SQL::CSemanticAnalysisEx::CSemanticAnalysisEx(SS::CoSeDi::SQL::CCashCoSeDiEx*	pCashCoSeDi, std::wstring wTableName) :
	m_pSemanticAnalysisRus(NULL), m_pSemanticAnalysisEng(NULL), m_pSemanticAnalysisSynonimsRus(NULL), m_pSemanticAnalysisSynonimsEng(NULL)
{
	SS_TRY
	{
		if(wTableName == L"Semantics" || wTableName == L"SemanticsExtra")
		{
			if(!m_pSemanticAnalysisRus)
			{
				m_pSemanticAnalysisRus = 
					new SS::CoSeDi::SQL::CSemanticAnalysis(
					&(pCashCoSeDi->m_oCashCoSeDiRus), 
					SS::MANAGER::Constants::c_szCoSeDiRus);
			}
			if(!m_pSemanticAnalysisEng)
			{
				m_pSemanticAnalysisEng = 
					new SS::CoSeDi::SQL::CSemanticAnalysis(
					&(pCashCoSeDi->m_oCashCoSeDiEng), 
					SS::MANAGER::Constants::c_szCoSeDiEng);
			}
		}else
			if(wTableName == L"SemanticsSynonims")
		{
			if(!m_pSemanticAnalysisSynonimsRus)
			{
				m_pSemanticAnalysisSynonimsRus = 
					new SS::CoSeDi::SQL::CSemanticAnalysisSynonims(
					&(pCashCoSeDi->m_oCashCoSeDiRus), 
					SS::MANAGER::Constants::c_szCoSeDiRus);
			}
			if(!m_pSemanticAnalysisSynonimsEng)
			{
				m_pSemanticAnalysisSynonimsEng = 
					new SS::CoSeDi::SQL::CSemanticAnalysisSynonims(
					&(pCashCoSeDi->m_oCashCoSeDiEng), 
					SS::MANAGER::Constants::c_szCoSeDiEng);
			}
		}

	}
	SS_CATCH(L"")
}

SS::CoSeDi::SQL::CSemanticAnalysisEx::~CSemanticAnalysisEx(void)
{
	//SS_TRY
	//{		
	if(m_pSemanticAnalysisRus)
	{
		delete m_pSemanticAnalysisRus;
		m_pSemanticAnalysisRus = NULL;
	}
	if(m_pSemanticAnalysisRus)
	{
		delete m_pSemanticAnalysisRus;
		m_pSemanticAnalysisRus = NULL;
	}

	if(m_pSemanticAnalysisSynonimsRus)
	{
		delete m_pSemanticAnalysisSynonimsRus;
		m_pSemanticAnalysisSynonimsRus = NULL;
	}
	if(m_pSemanticAnalysisSynonimsRus)
	{
		delete m_pSemanticAnalysisSynonimsRus;
		m_pSemanticAnalysisSynonimsRus = NULL;
	}
	//}
	//SS_CATCH(L"")
}


bool	SS::CoSeDi::SQL::CSemanticAnalysisEx::GetClasses(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex, /*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_Class)
{
	SS_TRY
	{
		switch(oDictionaryIndex.GetDictionaryNumber())
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
			{
				if(m_pSemanticAnalysisRus)
					return m_pSemanticAnalysisRus->GetClasses(/*in*/oDictionaryIndex, /*out*/p_l_Class);
				if(m_pSemanticAnalysisSynonimsRus)
					return m_pSemanticAnalysisSynonimsRus->GetClasses(/*in*/oDictionaryIndex, /*out*/p_l_Class);
			}
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			{
				if(m_pSemanticAnalysisEng)
					return m_pSemanticAnalysisEng->GetClasses(/*in*/oDictionaryIndex, /*out*/p_l_Class);
				if(m_pSemanticAnalysisSynonimsEng)
					return m_pSemanticAnalysisSynonimsEng->GetClasses(/*in*/oDictionaryIndex, /*out*/p_l_Class);
			}
			break;
		default:
			break;
		}
	}
	SS_CATCH(L"")
	return false;
}

void	SS::CoSeDi::SQL::CSemanticAnalysisEx::GetWords(/*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_DictionaryIndex)
{
	SS_TRY

		if(m_pSemanticAnalysisRus&&m_pSemanticAnalysisEng)
		{
			m_pSemanticAnalysisRus->SetClass(/*in*/m_oCurrentClass);
			m_pSemanticAnalysisEng->SetClass(/*in*/m_oCurrentClass);

			m_pSemanticAnalysisRus->GetWords(p_l_DictionaryIndex);
			m_pSemanticAnalysisEng->GetWords(p_l_DictionaryIndex);
		}

		if(m_pSemanticAnalysisSynonimsRus&&m_pSemanticAnalysisSynonimsEng)
		{
			m_pSemanticAnalysisSynonimsRus->SetClass(/*in*/m_oCurrentClass);
			m_pSemanticAnalysisSynonimsEng->SetClass(/*in*/m_oCurrentClass);

			m_pSemanticAnalysisSynonimsRus->GetWords(p_l_DictionaryIndex);
			m_pSemanticAnalysisSynonimsEng->GetWords(p_l_DictionaryIndex);
		}

	SS_CATCH(L"")
}

bool	SS::CoSeDi::SQL::CSemanticAnalysisEx::AddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY

		switch(oDictionaryIndex.GetDictionaryNumber())
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
			{
				if(m_pSemanticAnalysisRus)
				{
					m_pSemanticAnalysisRus->SetClass(/*in*/m_oCurrentClass);
					return m_pSemanticAnalysisRus->AddWord(/*in*/oDictionaryIndex);
				}
				if(m_pSemanticAnalysisSynonimsRus)
				{
					m_pSemanticAnalysisSynonimsRus->SetClass(/*in*/m_oCurrentClass);
					return m_pSemanticAnalysisSynonimsRus->AddWord(/*in*/oDictionaryIndex);
				}
			}
			
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			{
				if(m_pSemanticAnalysisEng)
				{
					m_pSemanticAnalysisEng->SetClass(/*in*/m_oCurrentClass);
					return m_pSemanticAnalysisEng->AddWord(/*in*/oDictionaryIndex);
				}
				if(m_pSemanticAnalysisSynonimsEng)
				{
					m_pSemanticAnalysisSynonimsEng->SetClass(/*in*/m_oCurrentClass);
					return m_pSemanticAnalysisSynonimsEng->AddWord(/*in*/oDictionaryIndex);
				}
			}

			break;
		default:
			break;
		}

	SS_CATCH(L"")
	return false;
}

bool	SS::CoSeDi::SQL::CSemanticAnalysisEx::DeleteWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY

		switch(oDictionaryIndex.GetDictionaryNumber())
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
			{
				if(m_pSemanticAnalysisRus)
				{
					m_pSemanticAnalysisRus->SetClass(/*in*/m_oCurrentClass);
					return m_pSemanticAnalysisRus->DeleteWord(/*in*/oDictionaryIndex);
				}
				if(m_pSemanticAnalysisSynonimsRus)
				{
					m_pSemanticAnalysisSynonimsRus->SetClass(/*in*/m_oCurrentClass);
					return m_pSemanticAnalysisSynonimsRus->DeleteWord(/*in*/oDictionaryIndex);
				}

			}
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			{
				if(m_pSemanticAnalysisEng)
				{
					m_pSemanticAnalysisEng->SetClass(/*in*/m_oCurrentClass);
					return m_pSemanticAnalysisEng->DeleteWord(/*in*/oDictionaryIndex);
				}
				if(m_pSemanticAnalysisSynonimsEng)
				{
					m_pSemanticAnalysisSynonimsEng->SetClass(/*in*/m_oCurrentClass);
					return m_pSemanticAnalysisSynonimsEng->DeleteWord(/*in*/oDictionaryIndex);
				}

			}
			break;
		default:
			break;
		}

	SS_CATCH(L"")
	return false;
}



bool	SS::CoSeDi::SQL::CSemanticAnalysisEx::DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber)
{
	SS_TRY
	{
	}		
	SS_CATCH(L"")

	return true;
}

int	SS::CoSeDi::SQL::CSemanticAnalysisEx::CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{
	}		
	SS_CATCH(L"")

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SS::CoSeDi::DBMS::CSemanticAnalysisEx::CSemanticAnalysisEx(SS::CoSeDi::DBMS::CCashCoSeDiEx*	pCashCoSeDi, std::wstring wTableName) :
	m_oSemanticAnalysisRus(&(pCashCoSeDi->m_oCashCoSeDiRus), SS::MANAGER::Constants::c_szCoSeDiRus, wTableName), 
	m_oSemanticAnalysisEng(&(pCashCoSeDi->m_oCashCoSeDiEng), SS::MANAGER::Constants::c_szCoSeDiEng, wTableName)
{
}

SS::CoSeDi::DBMS::CSemanticAnalysisEx::~CSemanticAnalysisEx(void)
{
}


bool	SS::CoSeDi::DBMS::CSemanticAnalysisEx::GetClasses(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex, /*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_Class)
{
	SS_TRY
	{

		switch(oDictionaryIndex.GetDictionaryNumber())
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
			return m_oSemanticAnalysisRus.GetClasses(/*in*/oDictionaryIndex, /*out*/p_l_Class);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			return m_oSemanticAnalysisEng.GetClasses(/*in*/oDictionaryIndex, /*out*/p_l_Class);
			break;
		default:
			break;
		}
	}
	SS_CATCH(L"")
	return false;
}

void	SS::CoSeDi::DBMS::CSemanticAnalysisEx::GetWords(/*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_DictionaryIndex)
{
	SS_TRY
	{
		m_oSemanticAnalysisRus.SetClass(/*in*/m_oCurrentClass);
		m_oSemanticAnalysisEng.SetClass(/*in*/m_oCurrentClass);

		m_oSemanticAnalysisRus.GetWords(p_l_DictionaryIndex);
		m_oSemanticAnalysisEng.GetWords(p_l_DictionaryIndex);
	}
	SS_CATCH(L"")
}

bool	SS::CoSeDi::DBMS::CSemanticAnalysisEx::AddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	bool bRetVal = false;

	SS_TRY
	{
		switch(oDictionaryIndex.GetDictionaryNumber())
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
			m_oSemanticAnalysisRus.SetClass(/*in*/m_oCurrentClass);		
			bRetVal = m_oSemanticAnalysisRus.AddWord(/*in*/ oDictionaryIndex);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			m_oSemanticAnalysisEng.SetClass(/*in*/m_oCurrentClass);
			bRetVal = m_oSemanticAnalysisEng.AddWord(/*in*/ oDictionaryIndex);
			break;
		default:
			break;
		}

	}
	SS_CATCH(L"")

	return bRetVal;
}

bool	SS::CoSeDi::DBMS::CSemanticAnalysisEx::DeleteWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{
		switch(oDictionaryIndex.GetDictionaryNumber())
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
			m_oSemanticAnalysisEng.SetClass(/*in*/m_oCurrentClass);
			return m_oSemanticAnalysisRus.DeleteWord(/*in*/ oDictionaryIndex);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			m_oSemanticAnalysisEng.SetClass(/*in*/m_oCurrentClass);
			return m_oSemanticAnalysisEng.DeleteWord(/*in*/ oDictionaryIndex);
			break;
		default:
			break;
		}
	}		
	SS_CATCH(L"")

	return true;
}


bool	SS::CoSeDi::DBMS::CSemanticAnalysisEx::DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber)
{
	SS_TRY
	{
		switch(oDictionariesNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
			m_oSemanticAnalysisEng.SetClass(/*in*/m_oCurrentClass);
			return m_oSemanticAnalysisRus.DeleteClass(/*in*/ oDictionariesNumber);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			m_oSemanticAnalysisEng.SetClass(/*in*/m_oCurrentClass);
			return m_oSemanticAnalysisEng.DeleteClass(/*in*/ oDictionariesNumber);
			break;
		default:
			break;
		}
	}		
	SS_CATCH(L"")

	return true;
}

int	SS::CoSeDi::DBMS::CSemanticAnalysisEx::CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex)
{
	SS_TRY
	{

		switch(oDictionaryIndex.GetDictionaryNumber())
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
			return m_oSemanticAnalysisRus.CreateClassAndAddWord(/*in*/ oDictionaryIndex);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			return m_oSemanticAnalysisEng.CreateClassAndAddWord(/*in*/ oDictionaryIndex);
			break;
		default:
			break;
		}
	}		
	SS_CATCH(L"")

	return true;
}

