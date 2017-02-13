#pragma once

#include <vector>
using namespace std;


struct SHypotesisSymbol
{
	SHypotesisSymbol(LPCWSTR sz, int wt) :
			iWeight(wt)
	{
		SetSymbol(sz);
	}

	void SetSymbol(LPCWSTR sz)
	{
		lstrcpyW(szSymbol, sz);
	}

	wchar_t szSymbol[MAX_RUS_LEN + 1];
	int iWeight;
};

class CHypotesis
{
public:
	CHypotesis();
	virtual ~CHypotesis();

public:
	void operator=(const CHypotesis& oHypotesis);
	bool operator==(const CHypotesis& oHypotesis) const;
	bool operator!=(const CHypotesis& oHypotesis) const;

	void Push(LPCWSTR sz, int iWeight, int iPosition);
	void PushSkipSymbol(int iPosition);
	void Clear();
	int GetSize();

	int GetWeight();
	LPCWSTR GetString();

private:
	wstring m_strResult;

	typedef vector<SHypotesisSymbol> THypotesis;
	THypotesis m_vHypotesis;
};
