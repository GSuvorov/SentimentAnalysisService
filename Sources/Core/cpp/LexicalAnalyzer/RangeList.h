#pragma once


namespace SS
{
namespace LexicalAnalyzer
{

enum ERangeType
{
	rtEpigraph,
	rtHeader,
	rtStyle,
};

enum EFontSize
{
	fsSmallest,
	fsSmall,
	fsMedium,
	fsLarge,
	fsLargest,
};

struct SStyle
{
	ERangeType RangeType;
	union
	{
		int iLevel;
		EFontSize FontSize;
	};

	int nWordBegin;
	int nWordEnd;
};

class CRange
{
public:
	CRange(int nBegin, int nEnd) :
			m_nBegin(nBegin),
			m_nEnd(nEnd)
	{
	}

	~CRange()
	{
	}

public:
	bool operator<=(const CRange& rng) const
	{
		return m_nBegin <= rng.m_nBegin;
	}

	bool operator>=(const CRange& rng) const
	{
		return rng.m_nEnd >= m_nEnd;
	}

	int GetBegin() const
	{
		return m_nBegin;
	}

	int GetEnd() const
	{
		return m_nEnd;
	}

private:
	int m_nBegin;
	int m_nEnd;
};

class CRangeList
{
public:
	CRangeList()
	{
		m_iMinLevel = 7;
		m_itBegins = m_mRangeBegins.end();
		m_itEnds = m_mRangeEnds.end();
	}

	~CRangeList()
	{
		Clear();
	}

	typedef map<CRange, SStyle*, less_equal<CRange> > TRangeMapLess;
	typedef map<CRange, SStyle*, greater_equal<CRange> > TRangeMapGreater;

// filling RangeList
public:
	void Clear()
	{
		SS_TRY;

		for(TRangeMapLess::iterator it = m_mRangeBegins.begin(); it != m_mRangeBegins.end(); it++)
			delete (*it).second;

		m_mRangeBegins.clear();
		m_mRangeEnds.clear();

		m_itBegins = m_mRangeBegins.end();
		m_itEnds = m_mRangeEnds.end();

		m_iMinLevel = 7;

		SS_CATCH(L"");
	}

	void AddRange(int nBegin, int nEnd, SStyle st)
	{
		SS_TRY;

		SStyle* pst = new SStyle;
		*pst = st;

		if(st.RangeType == rtHeader)
			m_iMinLevel = min(m_iMinLevel, st.iLevel);

		m_mRangeBegins.insert(TRangeMapLess::value_type(CRange(nBegin, nEnd), pst));
		m_mRangeEnds.insert(TRangeMapGreater::value_type(CRange(nBegin, nEnd), pst));

		SS_CATCH(L"");
	}

// using RangeList
public:
	int GetMinLevel()
	{
		return m_iMinLevel;
	}

	///resets internal state, should be called everytime before using filled RangeList
	void MoveFirst()
	{
		SS_TRY;

		m_itBegins = m_mRangeBegins.begin();
		m_itEnds = m_mRangeEnds.begin();

		SS_CATCH(L"");
	}

	///sets beginning word number for all ranges which begin is less than nCharEnd
	SStyle* SetWordBegin(int nWord, int nCharEnd)
	{
		SS_TRY;

		if(m_itBegins == m_mRangeBegins.end())
			return NULL;

		int n = (*m_itBegins).first.GetBegin();
		if(n < nCharEnd)
		{
			(*m_itBegins).second->nWordBegin = nWord;
			return (*m_itBegins++).second;
		}

		SS_CATCH(L"");
		return NULL;
	}

	///returns styles of all ranges which end is within [nCharBegin, nCharEnd]
	SStyle* SetWordEnd(int nWord, int nCharBegin, int nCharEnd)
	{
		SS_TRY;

		if(m_itEnds == m_mRangeEnds.end())
			return NULL;

		int n = (*m_itEnds).first.GetEnd();
		if(n <= nCharBegin)
		{
			(*m_itEnds).second->nWordEnd = nWord;
			return (*m_itEnds++).second;
		}

		SS_CATCH(L"");
		return NULL;
	}

private:
	TRangeMapLess m_mRangeBegins;
	TRangeMapLess::iterator m_itBegins;

	TRangeMapGreater m_mRangeEnds;
	TRangeMapGreater::iterator m_itEnds;

	int m_iMinLevel;
};

}
}
