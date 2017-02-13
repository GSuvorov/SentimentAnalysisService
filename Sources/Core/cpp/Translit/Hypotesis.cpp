#include "stdafx.h"
#include "Hypotesis.h"

#define SKIP_SYMBOL 1
static const wchar_t s_szSkipString[] = { SKIP_SYMBOL, 0 };


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CHypotesis::CHypotesis()
{
}

CHypotesis::~CHypotesis()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CHypotesis interface

void CHypotesis::operator =(const CHypotesis& oHypotesis)
{
	m_vHypotesis = oHypotesis.m_vHypotesis;
}

bool CHypotesis::operator ==(const CHypotesis& oHypotesis) const
{
	if(oHypotesis.m_vHypotesis.size() != m_vHypotesis.size())
		return false;

	for(int i = 0; i < (int) m_vHypotesis.size(); i++)
	{
		if(wcscmp(oHypotesis.m_vHypotesis[i].szSymbol, m_vHypotesis[i].szSymbol) != 0)
			return false;
	}

	return true;
}

bool CHypotesis::operator !=(const CHypotesis& oHypotesis) const
{
	return !(*this == oHypotesis);
}

void CHypotesis::Push(LPCWSTR sz, int iWeight, int iPosition)
{
	int iLength = (int) m_vHypotesis.size();
	if(iPosition >= iLength)
	{
		m_vHypotesis.insert(m_vHypotesis.end(), iPosition - iLength, SHypotesisSymbol(s_szSkipString, 0));
		m_vHypotesis.push_back(SHypotesisSymbol(sz, iWeight));
	}
	else
	{
		m_vHypotesis[iPosition].SetSymbol(sz);
		m_vHypotesis[iPosition].iWeight = iWeight;
	}
}

void CHypotesis::PushSkipSymbol(int iPosition)
{
	Push(s_szSkipString, MAX_SYMBOL_WEIGHT, iPosition);
}

void CHypotesis::Clear()
{
	m_vHypotesis.erase(m_vHypotesis.begin(), m_vHypotesis.end());
}

int CHypotesis::GetSize()
{
	return (int) m_vHypotesis.size();
}

int CHypotesis::GetWeight()
{
	SS_TRY;

	int iWeight = 0;
	for(THypotesis::iterator it = m_vHypotesis.begin(); it != m_vHypotesis.end(); ++it)
		iWeight += it->iWeight;
	return iWeight;

	SS_CATCH(L"");
	return 0;
}

LPCWSTR CHypotesis::GetString()
{
	SS_TRY;

	m_strResult.erase(m_strResult.begin(), m_strResult.end());
	for(THypotesis::iterator it = m_vHypotesis.begin(); it != m_vHypotesis.end(); ++it)
	{
		if(*it->szSymbol != SKIP_SYMBOL)
			m_strResult += it->szSymbol;
	}

	SS_CATCH(L"");
	return m_strResult.c_str();
}
