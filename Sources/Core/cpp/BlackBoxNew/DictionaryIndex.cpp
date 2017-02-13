#include "StdAfx.h"
#include "DictionaryIndex.h"

#include "..\Assinterface\TDictionaryNames.h"
#include "..\ASSInterface\TDictionaryManager.h"
#include "..\ASSInterface\Constants.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CDictionaryIndex::CDictionaryIndex()
{
}

CDictionaryIndex::~CDictionaryIndex()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IDictionaryIndex interface

void CDictionaryIndex::SetIsComposite(bool blValue)
{
}

bool CDictionaryIndex::IsComposite()
{
	return false;
}

void CDictionaryIndex::AppendIndex(TDictionaryIndex oIndex)
{
	m_oIndex = oIndex;
}

void CDictionaryIndex::Clear()
{
	m_oIndex = 0;
}

void CDictionaryIndex::RemoveIndex()
{
	m_oIndex = 0;
}

bool CDictionaryIndex::GetFirst(TDictionaryIndex* pIndex)
{
	*pIndex = m_oIndex;
	return true;
}

bool CDictionaryIndex::GetNext(TDictionaryIndex* pIndex)
{
	return false;
}

TDictionaryIndex CDictionaryIndex::GetFirst()
{
	return m_oIndex;
}

}
}
}
