#pragma once

#include <string>
#include <set>
#include <map>
#include <functional>
using namespace std;

#include "Hypotesis.h"


struct SWeightString
{
	SWeightString(int i = 1, int iLen = 0, LPCWSTR sz = L"") :
			iWeight(i),
			iPatternLength(iLen),
			strLetter(sz)
	{
	}

	bool operator <(const SWeightString& _rl) const
	{
		return strLetter < _rl.strLetter;
	}

	int iWeight;
	int iPatternLength;
	wstring strLetter;
};

typedef set<SWeightString> TLetterSet;
typedef multimap<int, CHypotesis*, greater<int> > THypotesisMap;


class CHypotesisStorage
{
public:
	CHypotesisStorage(int iMaxCount);
	~CHypotesisStorage();

public:
	void Clear();
	void AddHypotesis(const TLetterSet* pLetters, int iLetterPos);
	void AddSymbol(wchar_t wc, int iLetterPos);
	LPCWSTR GetFirstHypotesis();
	LPCWSTR GetNextHypotesis();

private:
	bool AddNew(LPCWSTR sz, CHypotesis* pCompare, int iSource, int iLetterPos, int iPatternLength, int iWeight);
	bool Add(LPCWSTR sz, CHypotesis* pCompare, int iSource, int iLetterPos, int iPatternLength, int iWeight);

	bool DeleteIrrelevantHypotesis();

private:
	const int m_iMaxCount;
	int m_iCount;
	CHypotesis* m_rgHypotesis;

	THypotesisMap m_mHypotesis;
	THypotesisMap::iterator m_itCurrent;
};
