#pragma once

#include "RootItem.h"

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;
using namespace SS::Interface::Core::BlackBox;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

#include <map>
using namespace std;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CQueryUnitIndexCollection :
		public CMiddleItem,
		public IQueryUnitIndexCollection
{
public:
	CQueryUnitIndexCollection();
	virtual ~CQueryUnitIndexCollection();

// IQueryUnitIndexCollection
public:
	virtual IQueryUnitIndexCollection* GetNextQueryUnitIndexCollection();
	virtual IQueryUnitIndexCollection* GetPrevQueryUnitIndexCollection();
	virtual void AddQueryUnitIndex(IQueryUnitIndex* pQueryUnitIndex);
	virtual IQueryUnitIndex* GetFirstQueryUnitIndex();
	virtual IQueryUnitIndex* GetLastQueryUnitIndex();
	virtual IQueryUnitIndex* ContainsIndexFull(TDictionaryIndex oIndex);
	virtual IQueryUnitIndex* ContainsIndex(TDictionaryIndex oIndex);
	virtual void Clear();
	virtual IQueryUnitIndexCollection* Release();
	virtual void SetIndexCollectionType(EIndexType IndexType);
	virtual EIndexType GetIndexCollectionType();
	virtual bool IsEmpty();

// CLeafItem
public:
	virtual void OnPopItem(CLeafItem* pItem);

private:
	SS::Interface::Core::BlackBox::EIndexType m_eType;

	typedef map<TDictionaryIndex, IQueryUnitIndex*> TIndexMap;
	TIndexMap m_mFullIndexes;
	TIndexMap m_mSignificantIndexes;
};

}
}
}
