#include "stdafx.h"
#include "AnswerWordInterval.h"

#include "QueryIterator.h"

using namespace SS;
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Core::Types::IndexationSearch;


namespace SS
{
namespace LinguisticRev
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CAnswerWordInterval::CAnswerWordInterval() :
		m_pTransform(NULL),
		m_pSent(NULL),
		m_pAnswerUnit(NULL),
		m_iTransPosition(-1)
{
}

CAnswerWordInterval::~CAnswerWordInterval()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CAnswerWordInterval interface

bool CAnswerWordInterval::Init(ISentence* pSent, ITransformation* pTransform, bool bSequence)
{
	SS_TRY;

	m_pAnswerUnit = NULL;
	m_pSent = pSent;
	m_pTransform = pTransform;

	int iBegin = -1,
		iEnd = -1;
	for(CQueryIterator<> it = m_pTransform->GetFirstGroup(); *it; ++it)
	{
		if(IsAnswerUnit(*it))
		{
			m_pAnswerUnit = *it;
			m_iTransPosition = it.GetPosition();

			if(bSequence)
			{
				int iPos = it.GetPosition();
				if(!it.SequenceIDChanged())
					iBegin = iPos - 1;
				if(!(++it).SequenceIDChanged())
					iEnd = *it ? (iPos + 1) : -1;
			}
			else
			{
				iBegin = -1;
				iEnd = -1;
			}
			break;
		}
	}

	// check transformation correctness
	if(!m_pAnswerUnit)
		return false;

	m_itBegin = GetFirstUnit(iBegin);
	m_itEnd = GetLastUnit(iEnd);
	return true;

	SS_CATCH(L"");
	return false;
}

IQueryUnit* CAnswerWordInterval::GetAnswerUnit()
{
	return m_pAnswerUnit;
}

int CAnswerWordInterval::GetTransPosition()
{
	return m_iTransPosition;
}

IUnit* CAnswerWordInterval::GetFirstUnit()
{
	m_itPos = m_itBegin;
	return *m_itPos;
}

IUnit* CAnswerWordInterval::GetNextUnit()
{
	if(*m_itPos == *m_itEnd)
		return NULL;
	return *(++m_itPos);
}

/////////////////////////////////////////////////////////////////////////////////////
// CAnswerWordInterval functions

CLeafIterator CAnswerWordInterval::GetFirstUnit(int iPos)
{
	if(iPos >= 0)
	{
		for(CLeafIterator it = m_pSent->GetFirstUnit(); *it; ++it)
		{
			if((*it)->GetTransPosition() == iPos)
				return ++it;
		}
	}

	return CLeafIterator(m_pSent->GetFirstUnit());
}

CLeafIterator CAnswerWordInterval::GetLastUnit(int iPos)
{
	if(iPos < 0)
		return CLeafIterator(NULL);

	CLeafIterator itRes(NULL);
	for(CLeafIterator it = m_pSent->GetFirstUnit(); *it; ++it)
	{
		if((*it)->GetTransPosition() == iPos)
			itRes = it;
	}

	return itRes;
}

}
}
