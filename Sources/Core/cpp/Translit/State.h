#pragma once

#include "HypotesisStorage.h"


class CState
{
public:
	CState(int iState, int iDirection);
	~CState();

public:
	void AddTransaction(int iDirection);
	void AddFinalTransition(LPCWSTR szExitLetter, int iPatternLength, int iWeight);
	BOOL IsActive(DWORD dwCurrState) const;
	DWORD GetState() const;
	void FillHypotesis(int iLetterPos, CHypotesisStorage* pHypotesis) const;

	void PrintDirections();

private:
	DWORD m_dwState;
	DWORD m_dwDirection;
	TLetterSet m_sLetters;
};
