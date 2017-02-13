#pragma once

#include <stack>

#include "IBlackBoxUnit.h"
using namespace SS::Interface::Core::BlackBox;


namespace SS
{
namespace Iterators
{

/////////////////////////////////////////////////////////////////////////////////////
// CSentenceIterator

class CSentenceIterator
{
public:
	CSentenceIterator(IUnit* pFirstUnit = NULL) : m_pUnit(pFirstUnit)
	{
	}

	IUnit* operator*()
	{
		return m_pUnit;
	}

	CSentenceIterator& operator++()
	{
		MoveNextUnit();
		return *this;
	}

	CSentenceIterator operator++(int)
	{
		CSentenceIterator it = *this;
		MoveNextUnit();
		return it;
	}

	CSentenceIterator& operator--()
	{
		MovePrevUnit();
		return *this;
	}

	CSentenceIterator operator--(int)
	{
		CSentenceIterator it = *this;
		MovePrevUnit();
		return it;
	}

protected:
	void MoveNextUnit()
	{
		IUnit* pAuxUnit = m_pUnit->GetLeftChildUnit();
		if(!pAuxUnit)
			pAuxUnit = GetNextSibling();

		m_pUnit = pAuxUnit;
	}

	void MovePrevUnit()
	{
		if(m_pUnit->GetLeftUnit())
			m_pUnit = GetRightmostChild(m_pUnit->GetLeftUnit());
		else
			m_pUnit = m_pUnit->GetParentUnit();
	}

private:
	IUnit* GetNextSibling()
	{
		for(IUnit* pUnit = m_pUnit; pUnit; pUnit = pUnit->GetParentUnit())
		{
			if(pUnit->GetRightUnit())
				return pUnit->GetRightUnit();
		}

		return NULL;
	}
public:
	IUnit* GetRightmostChild(IUnit* pParentUnit)
	{
		for(IUnit* pUnit = pParentUnit; pUnit; pUnit = pUnit->GetRightChildUnit())
		{
			if(!pUnit->HasChilds())
				return pUnit;
		}

		return pParentUnit;
	}

protected:
	IUnit* m_pUnit;
};

/////////////////////////////////////////////////////////////////////////////////////
// CLeafIterator - goes only through ISentence's low level units

class CLeafIterator : public CSentenceIterator
{
public:
	CLeafIterator(IUnit* pFirstUnit = NULL) : CSentenceIterator(pFirstUnit)
	{
		if(m_pUnit && m_pUnit->HasChilds())
			MoveNextLeaf();
	}

	CLeafIterator& operator++()
	{
		MoveNextLeaf();
		return *this;
	}

	CLeafIterator operator++(int)
	{
		CLeafIterator it = *this;
		MoveNextLeaf();
		return it;
	}

	CLeafIterator& operator--()
	{
		MovePrevLeaf();
		return *this;
	}

	CLeafIterator operator--(int)
	{
		CLeafIterator it = *this;
		MovePrevLeaf();
		return it;
	}

private:
	void MoveNextLeaf()
	{
		do
		{
			MoveNextUnit();
		}
		while(m_pUnit && m_pUnit->HasChilds());
	}

	void MovePrevLeaf()
	{
		do
		{
			MovePrevUnit();
		}
		while(m_pUnit && m_pUnit->HasChilds());
	}
};

}
}
