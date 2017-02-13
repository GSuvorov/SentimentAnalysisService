#pragma once

#include "RootItem.h"

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CTransformation : 
		public CMiddleItem,
		public ITransformation
{
public:
	CTransformation();
	virtual ~CTransformation();

public:
	virtual void AddGroup(IGroup* pGroup);
	virtual ITransformation* GetNextTransformation();
	virtual ITransformation* GetPrevTransformation();
	virtual IGroup* GetFirstGroup();
	virtual IGroup* GetLastGroup();
	virtual void Clear();
	virtual ITransformation* Release();
	virtual bool IsEmpty();
};

}
}
}
