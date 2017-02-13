#include "StdAfx.h"

#include "..\ASSInterface\TBaseNames.h"

#include ".\semanticclassestreeex.h"

//SS::CoSeDi::SQL::CSemanticClassesTreeEx::CSemanticClassesTreeEx(void) :
//		m_oSemanticClassesTreeRus(SS::MANAGER::Constants::c_szCoSeDiRus), 
//		m_oSemanticClassesTreeEng(SS::MANAGER::Constants::c_szCoSeDiEng)
//{
//}
//
//SS::CoSeDi::SQL::CSemanticClassesTreeEx::~CSemanticClassesTreeEx(void)
//{
//}
//
//void	SS::CoSeDi::SQL::CSemanticClassesTreeEx::SetClass(/*in*/int iIDClass)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.SetClass(iIDClass);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.SetClass(iIDClass);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//int		SS::CoSeDi::SQL::CSemanticClassesTreeEx::GetClass(/*in*/std::wstring * psClassName)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			return m_oSemanticClassesTreeRus.GetClass(psClassName);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			return m_oSemanticClassesTreeEng.GetClass(psClassName);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//	return -1;
//}
//void	SS::CoSeDi::SQL::CSemanticClassesTreeEx::GetChilds(/*in*/int iLevel, /*out*/std::list<int>* p_list_Childs)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetChilds(iLevel, p_list_Childs);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetChilds(iLevel, p_list_Childs);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//void	SS::CoSeDi::SQL::CSemanticClassesTreeEx::GetSame(/*out*/std::list<int>* p_list_Same)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetSame(p_list_Same);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetSame(p_list_Same);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//int		SS::CoSeDi::SQL::CSemanticClassesTreeEx::GetParent()
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			return m_oSemanticClassesTreeRus.GetParent();
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			return m_oSemanticClassesTreeEng.GetParent();
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//	return -1;
//}
//void	SS::CoSeDi::SQL::CSemanticClassesTreeEx::GetParents(/*in*/int iLevel, /*out*/std::list<int>* p_list_Parents)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetParents(iLevel, p_list_Parents);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetParents(iLevel, p_list_Parents);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//void	SS::CoSeDi::SQL::CSemanticClassesTreeEx::GetRoots(/*out*/std::list<int>* p_list_Root)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetRoots(p_list_Root);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetRoots(p_list_Root);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//void	SS::CoSeDi::SQL::CSemanticClassesTreeEx::GetName(/*out*/ std::wstring * psClassName)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetName(psClassName);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetName(psClassName);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//void	SS::CoSeDi::SQL::CSemanticClassesTreeEx::SetName(/*out*/ std::wstring * psClassName)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.SetName(psClassName);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.SetName(psClassName);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//int		SS::CoSeDi::SQL::CSemanticClassesTreeEx::CreateChild(/*in*/std::wstring * psClassName)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			return m_oSemanticClassesTreeRus.CreateChild(psClassName);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			return m_oSemanticClassesTreeEng.CreateChild(psClassName);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//	return -1;
//}
//void	SS::CoSeDi::SQL::CSemanticClassesTreeEx::DeleteClass()
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.DeleteClass();
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.DeleteClass();
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//
//
//
////////////////////////////////////////////////////////////////////////////////////
//
//SS::CoSeDi::DBMS::CSemanticClassesTreeEx::CSemanticClassesTreeEx(void) :
//		m_oSemanticClassesTreeRus(SS::MANAGER::Constants::c_szCoSeDiRus), 
//		m_oSemanticClassesTreeEng(SS::MANAGER::Constants::c_szCoSeDiEng)
//{
//}
//
//SS::CoSeDi::DBMS::CSemanticClassesTreeEx::~CSemanticClassesTreeEx(void)
//{
//}
//
//void	SS::CoSeDi::DBMS::CSemanticClassesTreeEx::SetClass(/*in*/int iIDClass)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.SetClass(iIDClass);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.SetClass(iIDClass);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//int		SS::CoSeDi::DBMS::CSemanticClassesTreeEx::GetClass(/*in*/std::wstring * psClassName)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			return m_oSemanticClassesTreeRus.GetClass(psClassName);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			return m_oSemanticClassesTreeEng.GetClass(psClassName);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//	return -1;
//}
//void	SS::CoSeDi::DBMS::CSemanticClassesTreeEx::GetChilds(/*in*/int iLevel, /*out*/std::list<int>* p_list_Childs)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetChilds(iLevel, p_list_Childs);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetChilds(iLevel, p_list_Childs);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//void	SS::CoSeDi::DBMS::CSemanticClassesTreeEx::GetSame(/*out*/std::list<int>* p_list_Same)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetSame(p_list_Same);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetSame(p_list_Same);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//int		SS::CoSeDi::DBMS::CSemanticClassesTreeEx::GetParent()
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			return m_oSemanticClassesTreeRus.GetParent();
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			return m_oSemanticClassesTreeEng.GetParent();
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//	return -1;
//}
//void	SS::CoSeDi::DBMS::CSemanticClassesTreeEx::GetParents(/*in*/int iLevel, /*out*/std::list<int>* p_list_Parents)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetParents(iLevel, p_list_Parents);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetParents(iLevel, p_list_Parents);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//void	SS::CoSeDi::DBMS::CSemanticClassesTreeEx::GetRoots(/*out*/std::list<int>* p_list_Root)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetRoots(p_list_Root);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetRoots(p_list_Root);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//void	SS::CoSeDi::DBMS::CSemanticClassesTreeEx::GetName(/*out*/ std::wstring * psClassName)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.GetName(psClassName);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.GetName(psClassName);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//void	SS::CoSeDi::DBMS::CSemanticClassesTreeEx::SetName(/*out*/ std::wstring * psClassName)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.SetName(psClassName);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.SetName(psClassName);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//int		SS::CoSeDi::DBMS::CSemanticClassesTreeEx::CreateChild(/*in*/std::wstring * psClassName)
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.CreateChild(psClassName);
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.CreateChild(psClassName);
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//	return -1;
//}
//void	SS::CoSeDi::DBMS::CSemanticClassesTreeEx::DeleteClass()
//{
//	SS_TRY
//	{
//		switch(m_oDictionariesNumber)
//		{
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
//			m_oSemanticClassesTreeRus.DeleteClass();
//			break;
//		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
//		case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
//			m_oSemanticClassesTreeEng.DeleteClass();
//			break;
//		default:
//			SS_THROW(L"Ќе установлен номер словар€ дл€ работы")
//		}
//	}
//	SS_CATCH(L"")
//
//}
//

