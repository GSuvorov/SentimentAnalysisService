#pragma once


namespace SS
{
namespace LinguisticRev
{

/////////////////////////////////////////////////////////////////////////////////////
// CQueryIterator

/// iterator for ITransformation
template<bool bOmitAnswerWord = false>
class CQueryIterator
{
public:
	CQueryIterator(IGroup* pGroup) : 
			m_pGroup(pGroup),
			m_uiPrevSeqID(-1),
			m_iCount(0),
			m_pUnit(NULL)
	{
		if(m_pGroup)
		{
			m_pUnit = m_pGroup->GetFirstQueryUnit();
			m_iCount = 1;
			OmitWords();
		}
	}

	IQueryUnit* operator*() const
	{
		return m_pUnit;
	}

	operator bool() const
	{
		return m_pUnit != NULL;
	}

	CQueryIterator& operator++()
	{
		if(!m_pUnit)
			return *this;

		m_uiPrevSeqID = m_pUnit->GetSequenceID();

		GotoNext();
		OmitWords();

		return *this;
	}

	bool SequenceIDChanged() const
	{
		return m_pUnit && (m_uiPrevSeqID != m_pUnit->GetSequenceID());
	}

	int GetPosition() const
	{
		return m_iCount;
	}

private:
	void GotoNext()
	{
		m_pUnit = m_pUnit->GetNextQueryUnit();
		while(!m_pUnit)
		{
			m_pGroup = m_pGroup ? m_pGroup->GetNextGroup() : NULL;
			if(!m_pGroup)
				return;
			m_pUnit = m_pGroup->GetFirstQueryUnit();
		}

		m_iCount++;
	}

	void OmitWords()
	{
		while(m_pUnit)
		{
			if(bOmitAnswerWord && IsAnswerUnit(m_pUnit))
			{
				GotoNext();
				continue;
			}
			break;
		}
	}

private:
	IQueryUnit* m_pUnit;
	IGroup* m_pGroup;
	UINT m_uiPrevSeqID;
	int m_iCount;
};

}
}
