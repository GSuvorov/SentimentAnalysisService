#include "stdafx.h"
#include "QueryStorage.h"

#include "Query.h"
#include "Transformation.h"
#include "Group.h"
#include "QueryUnit.h"
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

CQueryStorage::CQueryStorage()
{
}

CQueryStorage::~CQueryStorage()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CQueryStorage::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID == IID_BlackBoxQueryStorage)
		*pBase = (IBlackBoxQueryStorage*) this;
	else if(pIID == IID_Base)
		*pBase = (SS::Interface::IBase*) this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CQueryStorage::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// IBlackBoxQueryStorage interface

IQuery* CQueryStorage::CreateQuery()
{
	return new CQuery;
}

ITransformation* CQueryStorage::CreateTransformation()
{
	return new CTransformation;
}

IGroup* CQueryStorage::CreateGroup()
{
	return new CGroup;
}

IQueryUnit* CQueryStorage::CreateQueryUnit()
{
	return new CQueryUnit;
}

IQueryUnitIndex* CQueryStorage::CreateQueryUnitIndex()
{
	return new CQueryUnitIndex;
}

IQueryUnitIndexCollection* CQueryStorage::CreateQueryUnitIndexCollection()
{
	return new CQueryUnitIndexCollection;
}

}
}
}
