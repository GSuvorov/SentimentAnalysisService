#include "stdafx.h"
#include "State.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CState::CState(int iState, int iDirection) :
		m_dwState(1 << iState),
		m_dwDirection(1 << iDirection)
{
}

CState::~CState()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CState interface

void CState::AddTransaction(int iDirection)
{
	m_dwDirection |= 1 << iDirection;
}

void CState::AddFinalTransition(LPCWSTR szExitLetter, int iPatternLength, int iWeight)
{
	m_sLetters.insert(SWeightString(iWeight, iPatternLength, szExitLetter));
}

BOOL CState::IsActive(DWORD dwCurrState) const
{
	return dwCurrState & m_dwState;
}

DWORD CState::GetState() const
{
	return m_dwDirection;
}

void CState::FillHypotesis(int iLetterPos, CHypotesisStorage* pHypotesis) const
{
	pHypotesis->AddHypotesis(&m_sLetters, iLetterPos);
}

void CState::PrintDirections()
{
	for(int i = 0; i < 32; i++)
	{
		if((1 << i) & m_dwDirection)
			wprintf(L"%d,", i);
	}
}
