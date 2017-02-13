#pragma once

#include "RootItem.h"

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;

#include <set>
using namespace std;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CQueryUnit : 
		public CMiddleItem,
		public IQueryUnit
{
public:
	CQueryUnit();
	virtual ~CQueryUnit();

public:
	virtual IQueryUnit* GetNextQueryUnit();
	virtual IQueryUnit* GetPrevQueryUnit();
	virtual void Clear();
	virtual IQueryUnit* ReleaseQueryUnit();
	virtual void AddQueryUnitIndexCollection(IQueryUnitIndexCollection* pQueryUnitIndexCollection);
	virtual IQueryUnitIndexCollection* GetFirstQueryUnitIndexCollection();
	virtual IQueryUnitIndexCollection* GetLastQueryUnitIndexCollection();
	virtual IQueryUnitIndexCollection* GetQueryUnitIndexCollection(SS::Interface::Core::BlackBox::EIndexType IndexCollectionType);
	virtual bool IsEmpty();
	virtual void SetSequenceID(unsigned int uiSequenceID);
	virtual unsigned int GetSequenceID();
	virtual const wchar_t* GetWord();
	virtual void SetWord(const wchar_t* szWord);
	virtual bool IsNumber(int iNumber);
	virtual void AddNumber(int iNumber);
	virtual void ClearNumbers();

private:
	wstring m_strWord;
	unsigned int m_uiSequenceID;
	set<int> m_sNumbers;
};

}
}
}
