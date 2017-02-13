#pragma once

#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Interface::Core::BlackBox::Query;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CQueryStorage : public IBlackBoxQueryStorage
{
public:
	CQueryStorage();
	~CQueryStorage();

// IBase
public:
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	ULONG Release();

// IBlackBoxQueryStorage
public:
	virtual IQuery* CreateQuery();
	virtual ITransformation* CreateTransformation();
	virtual IGroup* CreateGroup();
	virtual IQueryUnit* CreateQueryUnit();
	virtual IQueryUnitIndex* CreateQueryUnitIndex();
	virtual IQueryUnitIndexCollection* CreateQueryUnitIndexCollection();
};

}
}
}
