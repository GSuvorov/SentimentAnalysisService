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

class CQuery : 
		public CRootItem,
		public IQuery
{
public:
	CQuery();
	virtual ~CQuery();

public:
	virtual void AddTransformation(ITransformation* pTransformation);
	virtual ITransformation* GetFirstTransformation();
	virtual ITransformation* GetLastTransformation();
	virtual bool IsEmpty();
	virtual void Clear();
	virtual void Release();
};

}
}
}
