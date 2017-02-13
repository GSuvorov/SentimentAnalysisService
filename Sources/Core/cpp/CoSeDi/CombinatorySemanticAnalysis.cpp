#include "StdAfx.h"


#include ".\combinatorysemanticanalysis.h"


bool	SS::CoSeDi::DBMS::CCombinatorySemanticAnalysis::SetWorkRegion(SS::CoSeDi::DataTypes::EWorkRegion oeWorkRegion)
{
	try
	{
		m_eWorkRegion = oeWorkRegion;

		return true;
	}
	catch(...)
	{
		return false;
	}
}

bool	SS::CoSeDi::DBMS::CCombinatorySemanticAnalysis::SetWords(T_list_IDSource * p_list_IDSource)
{
	try
	{
		m_list_OfIDSource = *p_list_IDSource;

		return true;
	}
	catch(...)
	{
		return false;
	}
}

void	SS::CoSeDi::DBMS::CCombinatorySemanticAnalysis::GetCombinations(list<T_list_IDSource> * p_list_list_IDSource)
{
	try
	{

	}
	catch(...)
	{
	}
}