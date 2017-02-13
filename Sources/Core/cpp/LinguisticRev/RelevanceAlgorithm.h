#pragma once

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;

#include <math.h>

#include <map>
using namespace std;

#include "QueryIterator.h"


namespace SS
{
namespace LinguisticRev
{

/// non-linear function to calculate relevance by distance
class CRelevanceCurve
{
public:
	CRelevanceCurve(double dPower, double dChange, double dRevMin, double dRevMax) :
			m_dPower(dPower),
			m_dChange(dChange)
	{
		// calculate coefficients
		m_dB = (dRevMax - dRevMin) / (AntiPow(1. - dChange, dPower) + AntiPow(dChange, dPower));
		m_dA = dRevMin + m_dB * AntiPow(dChange, dPower);
	}

public:
	double Calculate(double d)
	{
		return AntiPow(d - m_dChange, m_dPower) * m_dB + m_dA;
	}

	double AntiPow(double x, double power)
	{
		if(x < 0)
			return -pow(-x, power);
		return pow(x, power);
	}

private:
	double m_dA;
	double m_dB;
	double m_dChange;
	double m_dPower;
};

/// struct used to choose the best transform, according to greater_equal operator
struct STransRev
{
	STransRev(int iFuzzyLevel = 0, int iRev = 0, int iSkip = INT_MAX, int iWeightSkip = 100) : 
			iFuzzyLevel(iFuzzyLevel),
			iRelevance(iRev), 
			iSkippedCount(iSkip),
			iWeightSkipPercentage(iWeightSkip)
	{
	}

	bool operator>=(const STransRev& oRev) const
	{
		if(oRev.iFuzzyLevel != iFuzzyLevel)
			return iFuzzyLevel >= oRev.iFuzzyLevel;

		if(oRev.iSkippedCount == iSkippedCount)
			return iRelevance >= oRev.iRelevance;
		else
			return iSkippedCount <= oRev.iSkippedCount;
	}

	int iRelevance;
	int iFuzzyLevel;
	int iSkippedCount;
	int iWeightSkipPercentage;
};

class CRelevanceAlgorithm : public CAnalyseParams
{
public:
	CRelevanceAlgorithm();
	~CRelevanceAlgorithm();

public:
	void Init(ISentence* pSent, int iParagraphNumber);
	/// Calculate relevance for current sentence
	STransRev CalcRelevance(ITransformation* pTransform, bool bSequence);

private:
	/// Leaf iterator extension
	class CLeafIteratorEx
	{
	public:
		explicit CLeafIteratorEx(IUnit* pUnit = NULL) : 
				m_it(pUnit),
				m_iPos(0)
		{
		}

		CLeafIteratorEx& operator++()
		{
			++m_it;
			++m_iPos;
			return *this;
		}

		CLeafIteratorEx operator++(int)
		{
			CLeafIteratorEx it = *this;
			++m_it;
			++m_iPos;
			return it;
		}

		IUnit* operator*()
		{
			return *m_it;
		}

		int GetPos() const
		{
			return m_iPos;
		}

		operator CLeafIterator()
		{
			return m_it;
		}

	private:
		CLeafIterator m_it;
		int m_iPos;
	};

	/// Answer combination, and it's compare predicate
	typedef vector<CLeafIteratorEx> TCombination;
	struct CPosComparer
	{
		bool operator()(const TCombination::value_type& t1, const TCombination::value_type& t2)
		{
			return t1.GetPos() < t2.GetPos();
		}
	};

private:
	/// Calculates relevance of the combination, stored in m_mPos
	int GetCombinationRelevance();
	/// Finds the next combination of query words (beginning with itFirst), and fills m_oCombination
	bool FindCombination(const CLeafIteratorEx& itFirst);
	/// Finds unit, coincident with qit (by its trans position), beginning with lit
	bool FindCoincidentUnit(CLeafIteratorEx& lit, const CQueryIterator<>& qit);

	/// Calculates number of highlighted words within [itBegin, itEnd]
	int GetHighlightedWordCount(CLeafIterator itBegin, CLeafIterator itEnd);
	/// Clears highlight type in [itBegin, itEnd) interval
	void ClearHighlightType(CLeafIterator itBegin, CLeafIterator itEnd);

private:
	ITransformation* m_pTransform;
	ISentence* m_pSent;
	TCombination m_oCombination;
	CRelevanceCurve m_oCurve;

	int m_iTopSkipped;
	int m_iTopAdded;
	int m_iCurrSkipped;
	int m_iCurrAdded;
	int m_iUnobligSkipped;

	bool m_bIDFormNotCoincident;
	bool m_bSequence;

	int m_iParagraphNumber;
};

}
}
