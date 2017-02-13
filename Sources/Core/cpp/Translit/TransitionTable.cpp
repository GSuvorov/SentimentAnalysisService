#include "stdafx.h"
#include "TransitionTable.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CTransitionTable::CTransitionTable() :
		m_iStatesCount(1)
{
	memset(m_rgLines, 0, sizeof(m_rgLines));
}

CTransitionTable::~CTransitionTable()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////
// CTransitionTable interface

void CTransitionTable::Process(LPWSTR szWord, CHypotesisStorage* pHypotesis) const
{
	SS_TRY;

	int iPos = 0;
	DWORD dwCurrState = 1;
	szWord = CharLowerW(szWord);

	pHypotesis->Clear();
	for(LPCWSTR sz = szWord; *sz; sz++, iPos++)
	{
		CLine* pLine = m_rgLines[*sz];
		if(pLine)
			pLine->GetNextState(dwCurrState, iPos, pHypotesis);
		else
			pHypotesis->AddSymbol(*sz, iPos); // symbol, that is not present in table
	}

	SS_CATCH(L"");
}

void CTransitionTable::Clear()
{
	SS_TRY;

	m_iStatesCount = 1;
	for(int i = 0; i < MAX_CHAR_COUNT; i++)
	{
		delete m_rgLines[i];
		m_rgLines[i] = NULL;
	}

	SS_CATCH(L"");
}

bool CTransitionTable::InitByDefault(LPCWSTR* pszTable)
{
	SS_TRY;

	Clear();
	for(int i = 0; pszTable[i]; i++)
	{
		LPCWSTR szRus = pszTable[i];

		int iWeight = MAX_SYMBOL_WEIGHT;
		for(i++; pszTable[i]; i++)
			AddLat(pszTable[i], szRus, iWeight--);
	}

	SS_CATCH(L"");
	return true;
}

bool CTransitionTable::AddLat(LPCWSTR szLat, LPCWSTR szRus, int iWeight)
{
	if(!Verify(szRus, iWeight))
		return false;

	int iLen = (int) wcslen(szLat),
		iStateFrom = 0;
	for(int i = 0; i < iLen - 1; i++)
	{
		CLine* pLine = GetLine(szLat[i]);
		if(!pLine)
		{
			SAVE_LOG(SS_ERROR AND L"Таблица содержит символы с кодом больше 0xFFFF");
			return false;
		}

		iStateFrom = pLine->AddTransaction(iStateFrom, m_iStatesCount);
		if(m_iStatesCount == MAX_STATES_COUNT)
		{
			SAVE_LOG(SS_ERROR AND L"Таблица слишком сложная. Автомат не может содержать более 32 состояний");
			return false;
		}
	}

	CLine* pLine = GetLine(szLat[iLen - 1]);
	if(!pLine)
	{
		SAVE_LOG(SS_ERROR AND L"Таблица содержит символы с кодом больше 0xFFFF");
		return false;
	}

	pLine->AddFinalTransition(iStateFrom, szRus, iLen, iWeight);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// CTransitionTable functions

CLine* CTransitionTable::GetLine(wchar_t wc)
{
	if(wc >= MAX_CHAR_COUNT)
		return NULL; // incorrect symbol
	if(!m_rgLines[wc])
		m_rgLines[wc] = new CLine;
	return m_rgLines[wc];
}

bool CTransitionTable::Verify(LPCWSTR szRus, int iWeight) const
{
	if(wcslen(szRus) > MAX_RUS_LEN)
	{
		wchar_t szMessage[64] = { 0 };
		swprintf(szMessage, L"Слишком длинное русское буквосочетание '%s', максимальная длина - %d", szRus, iWeight);
		SAVE_LOG(SS_ERROR AND szMessage);
		return false;
	}

	if(iWeight > MAX_SYMBOL_WEIGHT)
	{
		wchar_t szMessage[64] = { 0 };
		swprintf(szMessage, L"Слишком большой вес %d, максимальное значение - %d", iWeight, MAX_SYMBOL_WEIGHT);
		SAVE_LOG(SS_ERROR AND szMessage);
		return false;
	}

	return true;
}

void CTransitionTable::Print() const
{
	wprintf(L"\t");
	for(int i = -1; i < m_iStatesCount; i++)
		wprintf(L"  %d  ", i);

	wprintf(L"\n\n");
	for(int i = 0; i < MAX_CHAR_COUNT; i++)
	{
		if(m_rgLines[i])
			m_rgLines[i]->Print(i, m_iStatesCount);
	}
}
