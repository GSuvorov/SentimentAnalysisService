#pragma once

#define MAX_CHAR_COUNT		0xFFFF
#define MAX_STATES_COUNT	(8 * sizeof(DWORD))

#include "TableLine.h"


class CTransitionTable
{
public:
	CTransitionTable();
	~CTransitionTable();

public:
	void Process(LPWSTR szWord, CHypotesisStorage* pHypotesis) const;

	void Clear();
	bool InitByDefault(LPCWSTR* pszTable);
	bool AddLat(LPCWSTR szLat, LPCWSTR szRus, int iWeight);

private:
	CLine* GetLine(wchar_t wc);
	bool Verify(LPCWSTR szRus, int iWeight) const;
	void Print() const;

private:
	int m_iStatesCount;
	CLine* m_rgLines[MAX_CHAR_COUNT];
};
