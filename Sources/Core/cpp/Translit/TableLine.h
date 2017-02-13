#pragma once

#include "State.h"

#include <map>
using namespace std;


class CLine
{
public:
	CLine();
	~CLine();

public:
	int AddTransaction(int iStateFrom, int& iStateTo);
	void AddFinalTransition(int iStateFrom, LPCWSTR szExitLetter, int iPatternLength, int iWeight);

	void GetNextState(DWORD& dwCurrState, int iLetterPos, CHypotesisStorage* pHypotesis) const;

	void Print(wchar_t wc, int iStatesCount);

private:
	typedef map<int, CState*> TStatesMap;
	TStatesMap m_mStates;
};
