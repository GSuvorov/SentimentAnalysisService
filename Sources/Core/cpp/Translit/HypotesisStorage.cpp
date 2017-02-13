#include "stdafx.h"
#include "HypotesisStorage.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CHypotesisStorage::CHypotesisStorage(int iMaxCount) :
		m_iMaxCount(iMaxCount),
		m_iCount(0),
		m_itCurrent()
{
	m_rgHypotesis = new CHypotesis[m_iMaxCount];
}

CHypotesisStorage::~CHypotesisStorage()
{
	delete [] m_rgHypotesis;
}

/////////////////////////////////////////////////////////////////////////////////////
// CHypotesisStorage interface

void CHypotesisStorage::Clear()
{
	SS_TRY;

	m_mHypotesis.clear();
	m_iCount = 0;

	SS_CATCH(L"");
}

void CHypotesisStorage::AddHypotesis(const TLetterSet* pLetters, int iLetterPos)
{
	SS_TRY;

	if(m_iCount == 0)
	{
		for(TLetterSet::const_iterator it = pLetters->begin(); it != pLetters->end(); ++it)
			AddNew(it->strLetter.c_str(), NULL, -1, iLetterPos, it->iPatternLength, it->iWeight);
	}
	else
	{
		CHypotesis* pPrev = NULL;

		int iCount = m_iCount;
		for(TLetterSet::const_iterator it = pLetters->begin(); it != pLetters->end(); ++it)
		{
			for(int i = 0; i < iCount; i++)
			{
				Add(it->strLetter.c_str(), pPrev, i, iLetterPos, it->iPatternLength, it->iWeight);
				pPrev = &m_rgHypotesis[m_iCount - 1];
			}
		}
	}

	SS_CATCH(L"");
}

void CHypotesisStorage::AddSymbol(wchar_t wc, int iLetterPos)
{
	wchar_t sz[] = { wc, 0 };
	SS_TRY;

	if(m_iCount == 0)
		AddNew(sz, NULL, -1, iLetterPos, 1, 0);
	else
	{
		int iCount = m_iCount;
		for(int i = 0; i < iCount; i++)
			Add(sz, NULL, i, iLetterPos, 1, 0);
	}

	SS_CATCH(L"Failed adding symbol" AND sz);
}

LPCWSTR CHypotesisStorage::GetFirstHypotesis()
{
	SS_TRY;

	if(m_mHypotesis.empty())
	{
		for(int i = 0; i < m_iCount; i++)
		{
			try
			{
				m_mHypotesis.insert(THypotesisMap::value_type(m_rgHypotesis[i].GetWeight(), &m_rgHypotesis[i]));
			}
			catch(...)
			{
				SAVE_LOG(L"Ошибка при добавлении элемента в m_mHypotesis");
			}
		}
	}

	m_itCurrent = m_mHypotesis.begin();
	if(m_itCurrent == m_mHypotesis.end())
		return NULL;
	return (*m_itCurrent).second->GetString();

	SS_CATCH(L"");
	return NULL;
}

LPCWSTR CHypotesisStorage::GetNextHypotesis()
{
	SS_TRY;

	if(++m_itCurrent == m_mHypotesis.end())
		return NULL;
	return (*m_itCurrent).second->GetString();

	SS_CATCH(L"");
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////
// CHypotesisStorage functions

bool CHypotesisStorage::AddNew(LPCWSTR sz, CHypotesis* pCompare, int iSource, int iLetterPos, int iPatternLength, int iWeight)
{
	SS_TRY;

	if(!DeleteIrrelevantHypotesis())
		return false;

	if(iSource >= 0)
		m_rgHypotesis[m_iCount] = m_rgHypotesis[iSource];
	else
		m_rgHypotesis[m_iCount].Clear();

	for(int i = iLetterPos - iPatternLength + 1; i < iLetterPos; i++)
		m_rgHypotesis[m_iCount].PushSkipSymbol(i);

	m_rgHypotesis[m_iCount].Push(sz, iWeight, iLetterPos);
	if(!pCompare || *pCompare != m_rgHypotesis[m_iCount])
		m_iCount++;

	SS_CATCH(L"");
	return true;
}

bool CHypotesisStorage::Add(LPCWSTR sz, CHypotesis* pCompare, int iSource, int iLetterPos, int iPatternLength, int iWeight)
{
	SS_TRY;

	if((int) m_rgHypotesis[iSource].GetSize() <= iLetterPos)
	{
		m_rgHypotesis[iSource].Push(sz, iWeight, iLetterPos);
		for(int i = 0; i < iPatternLength - 1; i++)
			m_rgHypotesis[iSource].PushSkipSymbol(iLetterPos - i);
	}
	else
	{
		if(!AddNew(sz, pCompare, iSource, iLetterPos, iPatternLength, iWeight))
			return false;
	}

	SS_CATCH(L"");
	return true;
}

bool CHypotesisStorage::DeleteIrrelevantHypotesis()
{
	SS_TRY;

	if(m_iCount < m_iMaxCount)
		return true;

	int iTotalWeight = 0;
	for(int i = 0; i < m_iCount; i++)
		iTotalWeight += m_rgHypotesis[i].GetWeight();

	int iThreshold = iTotalWeight / m_iCount;
	for(int i = m_iCount - 1; i >= 0; i--)
	{
		if(m_rgHypotesis[i].GetWeight() < iThreshold)
		{
			if(i < m_iCount - 1)
				m_rgHypotesis[i] = m_rgHypotesis[m_iCount - 1];
			m_iCount--;
		}
	}

	SS_CATCH(L"");
	return m_iCount < m_iMaxCount;
}
