#include "stdafx.h"
#include "QueryUnit.h"

#include "QueryUnitIndexCollection.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CQueryUnit::CQueryUnit() :
		m_uiSequenceID(-1)
{
}

CQueryUnit::~CQueryUnit()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IQueryUnit interface

IQueryUnit* CQueryUnit::GetNextQueryUnit()
{
	return dynamic_cast<CQueryUnit*>(GetRight());
}

IQueryUnit* CQueryUnit::GetPrevQueryUnit()
{
	return dynamic_cast<CQueryUnit*>(GetLeft());
}

void CQueryUnit::Clear()
{
	m_uiSequenceID = -1;
	m_strWord.clear();
	m_sNumbers.clear();
	ReleaseChildren();
}

IQueryUnit* CQueryUnit::ReleaseQueryUnit()
{
	return dynamic_cast<CQueryUnit*>(ReleaseItem());
}

void CQueryUnit::AddQueryUnitIndexCollection(IQueryUnitIndexCollection* pColl)
{
	AddChild(dynamic_cast<CQueryUnitIndexCollection*>(pColl));
}

IQueryUnitIndexCollection* CQueryUnit::GetFirstQueryUnitIndexCollection()
{
	return dynamic_cast<CQueryUnitIndexCollection*>(GetFirstChild());
}

IQueryUnitIndexCollection* CQueryUnit::GetLastQueryUnitIndexCollection()
{
	return dynamic_cast<CQueryUnitIndexCollection*>(GetLastChild());
}

IQueryUnitIndexCollection* CQueryUnit::GetQueryUnitIndexCollection(SS::Interface::Core::BlackBox::EIndexType eit)
{
	for(IQueryUnitIndexCollection* pColl = GetFirstQueryUnitIndexCollection(); pColl; pColl = pColl->GetNextQueryUnitIndexCollection())
	{
		if(eit == pColl->GetIndexCollectionType())
			return pColl;
	}
	return NULL;
}

bool CQueryUnit::IsEmpty()
{
	return IsItemEmpty();
}

void CQueryUnit::SetSequenceID(unsigned int uiSequenceID)
{
	m_uiSequenceID = uiSequenceID;
}

unsigned int CQueryUnit::GetSequenceID()
{
	return m_uiSequenceID;
}

const wchar_t* CQueryUnit::GetWord()
{
	return m_strWord.c_str();
}

void CQueryUnit::SetWord(const wchar_t* szWord)
{
	m_strWord = szWord;
}

bool CQueryUnit::IsNumber(int iNumber)
{
	return m_sNumbers.find(iNumber) != m_sNumbers.end();
}

void CQueryUnit::AddNumber(int iNumber)
{
	m_sNumbers.insert(iNumber);
}

void CQueryUnit::ClearNumbers()
{
	m_sNumbers.clear();
}

}
}
}
