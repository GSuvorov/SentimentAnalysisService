#include "stdafx.h"
#include "Query.h"

#include "Transformation.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CQuery::CQuery()
{
}

CQuery::~CQuery()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IQuery interface

void CQuery::AddTransformation(ITransformation* pTransformation)
{
	AddChild(dynamic_cast<CTransformation*>(pTransformation));
}

ITransformation* CQuery::GetFirstTransformation()
{
	return dynamic_cast<CTransformation*>(GetFirstChild());
}

ITransformation* CQuery::GetLastTransformation()
{
	return dynamic_cast<CTransformation*>(GetLastChild());
}

bool CQuery::IsEmpty()
{
	return IsItemEmpty();
}

void CQuery::Clear()
{
	ReleaseChildren();
}

void CQuery::Release()
{
	ReleaseItem();
}

}
}
}
