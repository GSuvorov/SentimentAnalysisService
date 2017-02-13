#pragma once

#include "RootItem.h"
#include "SyntaxFeature.h"

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CGroup : 
		public CMiddleItem,
		public IGroup
{
public:
	CGroup();
	virtual ~CGroup();

public:
	virtual void AddQueryUnit(IQueryUnit* pQueryUnit);
	virtual IGroup* GetNextGroup();
	virtual IGroup* GetPrevGroup();
	virtual IQueryUnit* GetFirstQueryUnit();
	virtual IQueryUnit* GetLastQueryUnit();
	virtual void Clear();
	virtual IGroup* Release();
	virtual ISyntaxFeature* GetSyntaxFeature();
	virtual bool IsEmpty();
	virtual void SetSequenceID(unsigned int uiSequenceID);
	virtual unsigned int GetSequenceID();

private:
	CSyntaxFeature* m_pSyntaxFeature;
};

}
}
}
