#include "stdafx.h"
#include "Group.h"

#include "QueryUnit.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CGroup::CGroup() :
		m_pSyntaxFeature(NULL)
{
}

CGroup::~CGroup()
{
	delete m_pSyntaxFeature;
}

/////////////////////////////////////////////////////////////////////////////////////
// IGroup interface

void CGroup::AddQueryUnit(IQueryUnit* pQueryUnit)
{
	AddChild(dynamic_cast<CQueryUnit*>(pQueryUnit));
}

IGroup* CGroup::GetNextGroup()
{
	return dynamic_cast<CGroup*>(GetRight());
}

IGroup* CGroup::GetPrevGroup()
{
	return dynamic_cast<CGroup*>(GetLeft());
}

IQueryUnit* CGroup::GetFirstQueryUnit()
{
	return dynamic_cast<CQueryUnit*>(GetFirstChild());
}

IQueryUnit* CGroup::GetLastQueryUnit()
{
	return dynamic_cast<CQueryUnit*>(GetLastChild());
}

void CGroup::Clear()
{
	ReleaseChildren();

	delete m_pSyntaxFeature;
	m_pSyntaxFeature = NULL;
}

IGroup* CGroup::Release()
{
	return dynamic_cast<CGroup*>(ReleaseItem());
}

ISyntaxFeature* CGroup::GetSyntaxFeature()
{
	if(!m_pSyntaxFeature)
		m_pSyntaxFeature = new CSyntaxFeature();
	return m_pSyntaxFeature;
}

bool CGroup::IsEmpty()
{
	return IsEmpty();
}

void CGroup::SetSequenceID(unsigned int uiSequenceID)
{
	for(IQueryUnit* pUnit=GetFirstQueryUnit(); pUnit; pUnit = pUnit->GetNextQueryUnit())
		pUnit->SetSequenceID(uiSequenceID);	
}

unsigned int CGroup::GetSequenceID()
{
	IQueryUnit* pQueryUnit = GetFirstQueryUnit();
	if(pQueryUnit)
		return pQueryUnit->GetSequenceID();
	else
		return -1;
}

}
}
}
