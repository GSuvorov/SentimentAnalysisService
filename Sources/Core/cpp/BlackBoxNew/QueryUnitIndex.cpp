#include "stdafx.h"
#include "QueryUnitIndex.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CQueryUnitIndex::CQueryUnitIndex()
{
}

CQueryUnitIndex::~CQueryUnitIndex()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IQueryUnitIndex interface

IQueryUnitIndex* CQueryUnitIndex::GetNextQueryUnitIndex()
{
	return dynamic_cast<CQueryUnitIndex*>(GetRight());
}

IQueryUnitIndex* CQueryUnitIndex::GetPrevQueryUnitIndex()
{
	return dynamic_cast<CQueryUnitIndex*>(GetLeft());
}

void CQueryUnitIndex::SetIndex(TDictionaryIndex oIndex)
{
	m_oIndex = oIndex;
}

TDictionaryIndex& CQueryUnitIndex::GetIndex()
{
	return m_oIndex;
}

IQueryUnitIndex* CQueryUnitIndex::Release()
{
	return dynamic_cast<CQueryUnitIndex*>(ReleaseItem());
}

void CQueryUnitIndex::AddIDForm(short shForm)
{
	m_stIDForms.insert(shForm);
}

bool CQueryUnitIndex::GetFirstIDForm(short* pshForm)
{
	m_itIDForm = m_stIDForms.begin();
	if(m_itIDForm == m_stIDForms.end())
		return false;

	*pshForm = *m_itIDForm;
	return true;
}

bool CQueryUnitIndex::GetNextIDForm(short* pshForm)
{
	if(++m_itIDForm == m_stIDForms.end())
		return false;

	*pshForm = *m_itIDForm;
	return true;
}

bool CQueryUnitIndex::ContainsIDForm(short idForm)
{
	TFormSet::iterator it = m_stIDForms.find(idForm);
	if(it == m_stIDForms.end())
		return false;
	return true;
}

}
}
}
