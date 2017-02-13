#include "StdAfx.h"

#include "..\ASSInterface\TDictionaryNames.h"

#include "..\ASSInterface\TBaseNames.h"

#include ".\semanticidsourcetreeex.h"


SS::CoSeDi::SQL::CSemanticIDSourceTreeEx::CSemanticIDSourceTreeEx(void) :
		m_oSemanticIDSourceTreeRus(SS::MANAGER::Constants::c_szCoSeDiRus), 
		m_oSemanticIDSourceTreeEng(SS::MANAGER::Constants::c_szCoSeDiEng)
{
}

SS::CoSeDi::SQL::CSemanticIDSourceTreeEx::~CSemanticIDSourceTreeEx(void)
{
}

void SS::CoSeDi::SQL::CSemanticIDSourceTreeEx::GetParents(std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource)
{
	SS_TRY

		switch(m_oFullIDSourceChild.m_uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:

			m_oSemanticIDSourceTreeRus.SetIDChild(m_oFullIDSourceChild);
			m_oSemanticIDSourceTreeRus.GetParents(plistOfFullIDSource);
			
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:

			m_oSemanticIDSourceTreeEng.SetIDChild(m_oFullIDSourceChild);
			m_oSemanticIDSourceTreeEng.GetParents(plistOfFullIDSource);

			return ;
			break;
		default:
			break;
		}
	SS_CATCH(L"")
}

void SS::CoSeDi::SQL::CSemanticIDSourceTreeEx::Add()
{
	SS_TRY
		
		switch(m_oFullIDSourceChild.m_uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:

			if(
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus) &&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName) &&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRProperName) &&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory)
				)
			{
				SS_THROW(L"Индексы родителя и ребенка из русского и английского словарей одновременно");
			}

			m_oSemanticIDSourceTreeRus.SetIDChild(m_oFullIDSourceChild);
			m_oSemanticIDSourceTreeRus.SetIDParent(m_oFullIDSourceParent);
		
			m_oSemanticIDSourceTreeRus.Add();
			
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
			if(
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus)&&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName)&&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRProperName)&&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory)
				)
			{
				SS_THROW(L"Индексы родителя и ребенка из русского и английского словарей одновременно");
			}

			m_oSemanticIDSourceTreeRus.SetIDChild(m_oFullIDSourceChild);
			m_oSemanticIDSourceTreeRus.SetIDParent(m_oFullIDSourceParent);

			m_oSemanticIDSourceTreeRus.Add();

			return ;
			break;
		default:
			break;
		}
		
	SS_CATCH(L"")
}

void SS::CoSeDi::SQL::CSemanticIDSourceTreeEx::Delete()
{
	SS_TRY

		switch(m_oFullIDSourceChild.m_uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:

			if(
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus)&&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName)&&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRProperName)&&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory)
				)
			{
				SS_THROW(L"Индексы родителя и ребенка из русского и английского словарей одновременно");
			}

			m_oSemanticIDSourceTreeRus.SetIDChild(m_oFullIDSourceChild);
			m_oSemanticIDSourceTreeRus.SetIDParent(m_oFullIDSourceParent);
		
			m_oSemanticIDSourceTreeRus.Delete();
			
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
			if(
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus) &&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName) &&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRProperName) &&
				(m_oFullIDSourceParent.m_uchDictionaryNumber != SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory)
				)
			{
				SS_THROW(L"Индексы родителя и ребенка из русского и английского словарей одновременно");
			}

			m_oSemanticIDSourceTreeRus.SetIDChild(m_oFullIDSourceChild);
			m_oSemanticIDSourceTreeRus.SetIDParent(m_oFullIDSourceParent);

			m_oSemanticIDSourceTreeRus.Delete();

			return ;
			break;
		default:
			break;
		}

	SS_CATCH(L"")
}

/////////////////////////////////////////////////////////////////////////////////////////

SS::CoSeDi::DBMS::CSemanticIDSourceTreeEx::CSemanticIDSourceTreeEx(void) :
		m_oSemanticIDSourceTreeRus(SS::MANAGER::Constants::c_szCoSeDiRus), 
		m_oSemanticIDSourceTreeEng(SS::MANAGER::Constants::c_szCoSeDiEng)
{
}

SS::CoSeDi::DBMS::CSemanticIDSourceTreeEx::~CSemanticIDSourceTreeEx(void)
{
}

void SS::CoSeDi::DBMS::CSemanticIDSourceTreeEx::GetParents(std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource)
{

	SS_TRY

		switch(m_oFullIDSourceChild.m_uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:

			m_oSemanticIDSourceTreeRus.SetIDChild(m_oFullIDSourceChild);
			m_oSemanticIDSourceTreeRus.GetParents(plistOfFullIDSource);
			
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:

			m_oSemanticIDSourceTreeEng.SetIDChild(m_oFullIDSourceChild);
			m_oSemanticIDSourceTreeEng.GetParents(plistOfFullIDSource);

			return ;
			break;
		default:
			break;
		}

	SS_CATCH(L"")
}

void SS::CoSeDi::DBMS::CSemanticIDSourceTreeEx::Add()
{

	SS_TRY

		SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	
	SS_CATCH(L"");

}

void SS::CoSeDi::DBMS::CSemanticIDSourceTreeEx::Delete()
{

	SS_TRY

		SS_THROW(L"Функция для данного режима работы не предусмотрена обратитесь в службу технической поддержки");
	
	SS_CATCH(L"");

}