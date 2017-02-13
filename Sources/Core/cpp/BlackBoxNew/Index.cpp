#include "stdafx.h"
#include "Index.h"

#include "Feature.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CIndex::CIndex() :
		m_bIndexationFlag(false)
{
}

CIndex::~CIndex()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IIndex interface

void CIndex::SetIndexationFlag(bool bl)
{
	m_bIndexationFlag = bl;
}

bool CIndex::GetIndexationFlag()
{
	return m_bIndexationFlag;
}

IDictionaryIndex* CIndex::GetDictionaryIndex()
{
	return &m_oDictionaryIndex;
}

void CIndex::AddFeature(IFeature* pFeature)
{
	AddChild(dynamic_cast<CFeature*>(pFeature));
}

IFeature* CIndex::GetFirstFeature()
{
	return dynamic_cast<CFeature*>(GetFirstChild());
}

IFeature* CIndex::GetNextFeature()
{
	return dynamic_cast<CFeature*>(GetNextChild());
}

IIndex* CIndex::Pop()
{
	return dynamic_cast<CIndex*>(PopItem());
}

IIndex* CIndex::ReleaseIndex()
{
	return dynamic_cast<CIndex*>(ReleaseItem());
}

void CIndex::ClearIndex()
{
	m_oDictionaryIndex.Clear();
	m_bIndexationFlag = false;
	ReleaseChildren();
}

}
}
}
