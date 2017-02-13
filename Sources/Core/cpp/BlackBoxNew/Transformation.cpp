#include "stdafx.h"
#include "Transformation.h"

#include "Group.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CTransformation::CTransformation()
{
}

CTransformation::~CTransformation()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// ITransformation interface

void CTransformation::AddGroup(IGroup* pGroup)
{
	AddChild(dynamic_cast<CGroup*>(pGroup));
}

ITransformation* CTransformation::GetNextTransformation()
{
	return dynamic_cast<CTransformation*>(GetRight()); 
}

ITransformation* CTransformation::GetPrevTransformation()
{
	return dynamic_cast<CTransformation*>(GetLeft()); 
}

IGroup* CTransformation::GetFirstGroup()
{
	return dynamic_cast<CGroup*>(GetFirstChild());
}

IGroup* CTransformation::GetLastGroup()
{
	return dynamic_cast<CGroup*>(GetLastChild());
}

void CTransformation::Clear()
{
	ReleaseChildren();
}

ITransformation* CTransformation::Release()
{
	return dynamic_cast<CTransformation*>(ReleaseItem());
}

bool CTransformation::IsEmpty()
{
	return IsItemEmpty();
}

}
}
}
