#include "StdAfx.h"

#include "..\ASSInterface\TBaseNames.h"
#include ".\syntaxanalysisex.h"


SS::CoSeDi::SQL::CSyntaxAnalysisEx::CSyntaxAnalysisEx(SS::CoSeDi::SQL::CCashCoSeDi*	pCashCoSeDi) :
		m_oSyntaxAnalysisRus(pCashCoSeDi, SS::MANAGER::Constants::c_szCoSeDiRus), 
		m_oSyntaxAnalysisEng(pCashCoSeDi, SS::MANAGER::Constants::c_szCoSeDiEng)
{
}

SS::CoSeDi::SQL::CSyntaxAnalysisEx::~CSyntaxAnalysisEx(void)
{
}

void SS::CoSeDi::SQL::CSyntaxAnalysisEx::Get(/*out*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo)
{
	SS_TRY
	{
		switch(m_oDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
			m_oSyntaxAnalysisRus.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisRus.Get(p_list_OfSyntaxInfo);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
			m_oSyntaxAnalysisEng.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisEng.Get(p_list_OfSyntaxInfo);
			break;
		default:
			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
		}

	}
	SS_CATCH(L"")
}
void SS::CoSeDi::SQL::CSyntaxAnalysisEx::Add(/*in*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo)
{
	SS_TRY
	{
		switch(m_oDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
			m_oSyntaxAnalysisRus.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisRus.Add(p_list_OfSyntaxInfo);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
			m_oSyntaxAnalysisEng.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisEng.Add(p_list_OfSyntaxInfo);
			break;
		default:
			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
		}

	}
	SS_CATCH(L"")
}
void SS::CoSeDi::SQL::CSyntaxAnalysisEx::Delete()
{
	SS_TRY
	{
		switch(m_oDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
			m_oSyntaxAnalysisRus.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisRus.Delete();
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
			m_oSyntaxAnalysisEng.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisEng.Delete();
			break;
		default:
			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
		}
	}
	SS_CATCH(L"")
}
void SS::CoSeDi::SQL::CSyntaxAnalysisEx::Delete(/*in*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo)
{
	SS_TRY
	{
		switch(m_oDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
			m_oSyntaxAnalysisRus.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisRus.Delete(p_list_OfSyntaxInfo);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
			m_oSyntaxAnalysisEng.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisEng.Delete(p_list_OfSyntaxInfo);
			break;
		default:
			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
		}
	}
	SS_CATCH(L"")
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SS::CoSeDi::DBMS::CSyntaxAnalysisEx::CSyntaxAnalysisEx(SS::CoSeDi::DBMS::CCashCoSeDi*	pCashCoSeDi) :
		m_oSyntaxAnalysisRus(pCashCoSeDi, SS::MANAGER::Constants::c_szCoSeDiRus), 
		m_oSyntaxAnalysisEng(pCashCoSeDi, SS::MANAGER::Constants::c_szCoSeDiEng)
{
}

SS::CoSeDi::DBMS::CSyntaxAnalysisEx::~CSyntaxAnalysisEx(void)
{
}
void SS::CoSeDi::DBMS::CSyntaxAnalysisEx::Get(/*out*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo)
{
	SS_TRY
	{
		switch(m_oDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
			m_oSyntaxAnalysisRus.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisRus.Get(p_list_OfSyntaxInfo);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
			m_oSyntaxAnalysisEng.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisEng.Get(p_list_OfSyntaxInfo);
			break;
		default:
			break;
		}
	}
	SS_CATCH(L"")
}

void SS::CoSeDi::DBMS::CSyntaxAnalysisEx::Add(/*in*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo)
{
	SS_TRY
	{
		switch(m_oDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
			m_oSyntaxAnalysisRus.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisRus.Add(p_list_OfSyntaxInfo);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
			m_oSyntaxAnalysisEng.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisEng.Add(p_list_OfSyntaxInfo);
			break;
		default:
			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
		}
	}
	SS_CATCH(L"")
}

void SS::CoSeDi::DBMS::CSyntaxAnalysisEx::Delete()
{
	SS_TRY
	{
		switch(m_oDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
			m_oSyntaxAnalysisRus.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisRus.Delete();			
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
			m_oSyntaxAnalysisEng.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisEng.Delete();
			break;
		default:
			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
		}
	}
	SS_CATCH(L"")
}

void SS::CoSeDi::DBMS::CSyntaxAnalysisEx::Delete(/*in*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo)
{
	SS_TRY
	{
		switch(m_oDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
			m_oSyntaxAnalysisRus.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisRus.Delete(p_list_OfSyntaxInfo);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
			m_oSyntaxAnalysisEng.SetDictionaryNumber(m_oDictionaryNumber);
			m_oSyntaxAnalysisEng.Delete(p_list_OfSyntaxInfo);
			break;
		default:
			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
		}
	}
	SS_CATCH(L"")
}
