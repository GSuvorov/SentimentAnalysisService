#include "stdafx.h"
#include "TableLine.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CLine::CLine()
{
}

CLine::~CLine()
{
	for(TStatesMap::iterator it = m_mStates.begin(); it != m_mStates.end(); ++it)
		delete (*it).second;
}

/////////////////////////////////////////////////////////////////////////////////////
// CLine interface

int CLine::AddTransaction(int iStateFrom, int& iStateTo)
{
	TStatesMap::iterator it = m_mStates.find(iStateFrom);
	if(it == m_mStates.end())
	{
		CState* pState = new CState(iStateFrom, iStateTo);
		m_mStates.insert(TStatesMap::value_type(iStateFrom, pState));
		return iStateTo++;
	}
	(*it).second->AddTransaction(iStateTo);
	return iStateTo++;
}

void CLine::AddFinalTransition(int iStateFrom, LPCWSTR szExitLetter, int iPatternLength, int iWeight)
{
	int iPlaceInMap = iStateFrom ? iStateFrom : -1;

	TStatesMap::iterator it = m_mStates.find(iPlaceInMap);
	if(it == m_mStates.end())
	{
		CState* pState = new CState(iStateFrom, 0);
		m_mStates.insert(TStatesMap::value_type(iPlaceInMap, pState));
		pState->AddFinalTransition(szExitLetter, iPatternLength, iWeight);
	}
	else
	{
		CState* pState = (*it).second;
		pState->AddFinalTransition(szExitLetter, iPatternLength, iWeight);
	}
}

void CLine::GetNextState(DWORD& dwCurrState, int iLetterPos, CHypotesisStorage* pHypotesis) const
{
	DWORD dwNewState = 1;
	for(TStatesMap::const_iterator it = m_mStates.begin(); it != m_mStates.end(); ++it)
	{
		CState* pState = (*it).second;
		if(pState->IsActive(dwCurrState))
		{
			dwNewState |= pState->GetState();

			// fill hypotesis only when returning to the initial state
			if(pState->GetState() == 1)
				pState->FillHypotesis(iLetterPos, pHypotesis);
		}
	}
	dwCurrState = dwNewState;
}

void CLine::Print(wchar_t wc, int iStatesCount)
{
	wprintf(L"%c\t", wc);
	for(int i = -1; i < iStatesCount; i++)
	{
		TStatesMap::iterator it = m_mStates.find(i);
		if(it != m_mStates.end())
		{
			wprintf(L"%d: ", (*it).first);
			(*it).second->PrintDirections();
		}
		else
			wprintf(L"-  , ");
	}

	wprintf(L"\n");
}
