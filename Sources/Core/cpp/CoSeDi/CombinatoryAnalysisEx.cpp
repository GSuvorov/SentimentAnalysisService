#include "StdAfx.h"

#include "..\ASSInterface\TDictionaryNames.h"
#include "..\ASSInterface\TBaseNames.h"

#include ".\combinatoryanalysisex.h"


SS::CoSeDi::SQL::CCombinatoryAnalysisEx::CCombinatoryAnalysisEx(SS::CoSeDi::SQL::CCashCoSeDiEx*	pCashCoSeDi) :
		m_oCombinatoryAnalysisRus(&(pCashCoSeDi->m_oCashCoSeDiRus), SS::MANAGER::Constants::c_szCoSeDiRus), 
		m_oCombinatoryAnalysisEng(&(pCashCoSeDi->m_oCashCoSeDiEng), SS::MANAGER::Constants::c_szCoSeDiEng)
{
}

SS::CoSeDi::SQL::CCombinatoryAnalysisEx::~CCombinatoryAnalysisEx(void)
{
}

bool	SS::CoSeDi::SQL::CCombinatoryAnalysisEx::GetCombinations(/*in*/ SS::CoSeDi::DataTypes::TFormInfo & oFormInfo, /*out*/SS::CoSeDi::DataTypes::TlistOfCombination * p_list_CombinationList)
{
		SS_TRY
		
		switch(oFormInfo.m_uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
			if(!m_oCombinatoryAnalysisRus.GetCombinations(/*in*/ oFormInfo, /*out*/p_list_CombinationList))
				return false;			

			for(SS::CoSeDi::DataTypes::TlistOfCombination::iterator iter = p_list_CombinationList->begin();
				iter != p_list_CombinationList->end(); iter++)
			{
				(*iter) += (SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory<<24);
			}
			return true;  
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
			if(!m_oCombinatoryAnalysisEng.GetCombinations(/*in*/ oFormInfo, /*out*/p_list_CombinationList))
				return false;			

			for(SS::CoSeDi::DataTypes::TlistOfCombination::iterator iter = p_list_CombinationList->begin();
				iter != p_list_CombinationList->end(); iter++)
			{
				(*iter) += (SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory<<24);
			}
			return true;  
			break;
		default:
			break;
		}

	SS_CATCH(L"")

		return false;

}

int		SS::CoSeDi::SQL::CCombinatoryAnalysisEx::CreateCombination(/*in*/ SS::CoSeDi::DataTypes::TCombinationInfo * p_CombinationInfo)
{
	unsigned char uchDictionaryNumber;
		SS_TRY
		
		switch(p_CombinationInfo->m_T_list_FormInfo.front().m_uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
			case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
				uchDictionaryNumber = SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory;
			//m_oCombinatoryAnalysisRus.SetCombination(m_iIDCombination);
			return (m_oCombinatoryAnalysisRus.CreateCombination(p_CombinationInfo) + (uchDictionaryNumber<<24));
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
			case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
				uchDictionaryNumber = SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory;
			//m_oCombinatoryAnalysisEng.SetCombination(m_iIDCombination);
			return (m_oCombinatoryAnalysisEng.CreateCombination(p_CombinationInfo) + (uchDictionaryNumber<<24));
			break;
		default:
			break;
		}

	SS_CATCH(L"")

		return -1;


}

void	SS::CoSeDi::SQL::CCombinatoryAnalysisEx::GetCombinationInfo(/*out*/ SS::CoSeDi::DataTypes::TCombinationInfo * p_CombinationInfo)
{
	SS_TRY
		
		switch(m_iIDCombination>>24)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
			m_oCombinatoryAnalysisRus.SetCombination(m_iIDCombination - (SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory<<24));
			m_oCombinatoryAnalysisRus.GetCombinationInfo(p_CombinationInfo);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
			m_oCombinatoryAnalysisEng.SetCombination(m_iIDCombination- (SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory<<24));
			m_oCombinatoryAnalysisEng.GetCombinationInfo(p_CombinationInfo);
			break;
		default:
			break;
		}

	SS_CATCH(L"")

}


bool	SS::CoSeDi::SQL::CCombinatoryAnalysisEx::DeleteCombination()
{
	SS_TRY
		
		switch(m_iIDCombination>>24)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
			m_oCombinatoryAnalysisRus.SetCombination(m_iIDCombination - (SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory<<24));
			m_oCombinatoryAnalysisRus.DeleteCombination();
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
			m_oCombinatoryAnalysisRus.SetCombination(m_iIDCombination - (SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory<<24));
			m_oCombinatoryAnalysisRus.DeleteCombination();
			break;
		default:
			break;
		}

	SS_CATCH(L"")

		return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


SS::CoSeDi::DBMS::CCombinatoryAnalysisEx::CCombinatoryAnalysisEx(SS::CoSeDi::DBMS::CCashCoSeDiEx*	pCashCoSeDi) :
		m_oCombinatoryAnalysisRus(&(pCashCoSeDi->m_oCashCoSeDiRus), SS::MANAGER::Constants::c_szCoSeDiRus), 
		m_oCombinatoryAnalysisEng(&(pCashCoSeDi->m_oCashCoSeDiEng), SS::MANAGER::Constants::c_szCoSeDiEng)
{
}

SS::CoSeDi::DBMS::CCombinatoryAnalysisEx::~CCombinatoryAnalysisEx(void)
{
}

bool	SS::CoSeDi::DBMS::CCombinatoryAnalysisEx::GetCombinations(/*in*/ SS::CoSeDi::DataTypes::TFormInfo & oFormInfo, /*out*/SS::CoSeDi::DataTypes::TlistOfCombination * p_list_CombinationList)
{
		SS_TRY
		
		switch(oFormInfo.m_uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
			if(!m_oCombinatoryAnalysisRus.GetCombinations(/*in*/ oFormInfo, /*out*/p_list_CombinationList))
				return false;			

			for(SS::CoSeDi::DataTypes::TlistOfCombination::iterator iter = p_list_CombinationList->begin();
				iter != p_list_CombinationList->end(); iter++)
			{
				(*iter) += (SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory<<24);
			}
			return true;  
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
			if(!m_oCombinatoryAnalysisEng.GetCombinations(/*in*/ oFormInfo, /*out*/p_list_CombinationList))
				return false;			

			for(SS::CoSeDi::DataTypes::TlistOfCombination::iterator iter = p_list_CombinationList->begin();
				iter != p_list_CombinationList->end(); iter++)
			{
				(*iter) += (SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory<<24);
			}
			return true;  
			break;
		default:
			break;
		}

	SS_CATCH(L"")

		return false;

}

int		SS::CoSeDi::DBMS::CCombinatoryAnalysisEx::CreateCombination(/*in*/ SS::CoSeDi::DataTypes::TCombinationInfo * p_CombinationInfo)
{
	SS_THROW(L"данная функция для данного режима не предусмотрена")
	return(0);
}

void	SS::CoSeDi::DBMS::CCombinatoryAnalysisEx::GetCombinationInfo(/*out*/ SS::CoSeDi::DataTypes::TCombinationInfo * p_CombinationInfo)
{
	SS_TRY
		
		switch(m_iIDCombination>>24)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
			m_oCombinatoryAnalysisRus.SetCombination(m_iIDCombination - (SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory<<24));
			m_oCombinatoryAnalysisRus.GetCombinationInfo(p_CombinationInfo);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
			m_oCombinatoryAnalysisEng.SetCombination(m_iIDCombination- (SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory<<24));
			m_oCombinatoryAnalysisEng.GetCombinationInfo(p_CombinationInfo);
			break;
		default:
			break;
		}

	SS_CATCH(L"")	
}


bool	SS::CoSeDi::DBMS::CCombinatoryAnalysisEx::DeleteCombination()
{
	SS_THROW(L"данная функция для данного режима не предусмотрена")
	return(FALSE);
}
