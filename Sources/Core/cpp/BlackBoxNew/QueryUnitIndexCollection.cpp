#include "stdafx.h"
#include "QueryUnitIndexCollection.h"

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

CQueryUnitIndexCollection::CQueryUnitIndexCollection() :
		m_eType(eitUndefinedIndex)
{
}

CQueryUnitIndexCollection::~CQueryUnitIndexCollection()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IQueryUnitIndexCollection interface

IQueryUnitIndexCollection* CQueryUnitIndexCollection::GetNextQueryUnitIndexCollection()
{
	return dynamic_cast<CQueryUnitIndexCollection*>(GetRight());
}

IQueryUnitIndexCollection* CQueryUnitIndexCollection::GetPrevQueryUnitIndexCollection()
{
	return dynamic_cast<CQueryUnitIndexCollection*>(GetLeft());
}

void CQueryUnitIndexCollection::AddQueryUnitIndex(IQueryUnitIndex* pQueryUnitIndex)
{
	m_mSignificantIndexes.insert(TIndexMap::value_type(pQueryUnitIndex->GetIndex().GetDictionaryIndexWithoutFlags(), pQueryUnitIndex));
	m_mFullIndexes.insert(TIndexMap::value_type(pQueryUnitIndex->GetIndex(), pQueryUnitIndex));

	AddChild(dynamic_cast<CQueryUnitIndex*>(pQueryUnitIndex));
}

IQueryUnitIndex* CQueryUnitIndexCollection::GetFirstQueryUnitIndex()
{
	return dynamic_cast<CQueryUnitIndex*>(GetFirstChild());
}

IQueryUnitIndex* CQueryUnitIndexCollection::GetLastQueryUnitIndex()
{
	return dynamic_cast<CQueryUnitIndex*>(GetLastChild());
}

IQueryUnitIndex* CQueryUnitIndexCollection::ContainsIndexFull(TDictionaryIndex oIndex)
{
	TIndexMap::iterator it = m_mFullIndexes.find(oIndex);
	if(it == m_mFullIndexes.end())
		return NULL;
	return it->second;
}

IQueryUnitIndex* CQueryUnitIndexCollection::ContainsIndex(TDictionaryIndex oIndex)
{
	TIndexMap::iterator it = m_mSignificantIndexes.find(oIndex.GetDictionaryIndexWithoutFlags());
	if(it == m_mSignificantIndexes.end())
		return NULL;
	return it->second;
}

void CQueryUnitIndexCollection::Clear()
{
	m_eType = eitUndefinedIndex;
	ReleaseChildren();
}

IQueryUnitIndexCollection* CQueryUnitIndexCollection::Release()
{
	return dynamic_cast<CQueryUnitIndexCollection*>(ReleaseItem());
}

void CQueryUnitIndexCollection::SetIndexCollectionType(EIndexType eit)
{
	m_eType = eit;
}

EIndexType CQueryUnitIndexCollection::GetIndexCollectionType()
{
	return m_eType;
}

bool CQueryUnitIndexCollection::IsEmpty()
{
	return IsItemEmpty();
}

/////////////////////////////////////////////////////////////////////////////////////
// CLeafItem interface

void CQueryUnitIndexCollection::OnPopItem(CLeafItem* pItem)
{
	IQueryUnitIndex* pIndex = dynamic_cast<CQueryUnitIndex*>(pItem);

	m_mSignificantIndexes.erase(pIndex->GetIndex().GetDictionaryIndexWithoutFlags());
	m_mFullIndexes.erase(pIndex->GetIndex());
}

}
}
}
