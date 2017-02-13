#pragma once

#include "LeafItem.h"

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

#include <set>
using namespace std;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CQueryUnitIndex : 
		public CLeafItem,
		public IQueryUnitIndex
{
public:
	CQueryUnitIndex();
	virtual ~CQueryUnitIndex();

public:
	virtual IQueryUnitIndex* GetNextQueryUnitIndex();
	virtual IQueryUnitIndex* GetPrevQueryUnitIndex();
	virtual void SetIndex(TDictionaryIndex oIndex);
	virtual TDictionaryIndex& GetIndex();
	virtual IQueryUnitIndex* Release();
	virtual void AddIDForm(short shForm);
	virtual bool GetFirstIDForm(short* pshForm);
	virtual bool GetNextIDForm(short* pshForm);
	virtual bool ContainsIDForm(short idForm);

private:
	typedef set<short> TFormSet;
	TFormSet m_stIDForms;
	TFormSet::iterator m_itIDForm;

	TDictionaryIndex m_oIndex;
};

}
}
}
