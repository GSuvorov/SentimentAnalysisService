#include "stdafx.h"
#include "RelevanceAlgorithm.h"

using namespace SS::Core::Types::IndexationSearch;

#include <atlbase.h>


namespace SS
{
namespace LinguisticRev
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CRelevanceAlgorithm::CRelevanceAlgorithm() : 
		m_pSent(NULL),
		m_pTransform(NULL),
		m_bSequence(true),
		m_oCurve(0.4, 0.35, 0., 99.),
		m_iTopSkipped(0),
		m_iTopAdded(0),
		m_iCurrSkipped(0),
		m_iCurrAdded(0),
		m_iUnobligSkipped(0),
		m_bIDFormNotCoincident(false),
		m_iParagraphNumber(-1)
{
	m_oCombination.reserve(16);
}

CRelevanceAlgorithm::~CRelevanceAlgorithm()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CRelevanceAlgorithm interface

void CRelevanceAlgorithm::Init(ISentence* pSent, int iParagraphNumber)
{
	m_pSent = pSent;
	m_iParagraphNumber = iParagraphNumber;
}

STransRev CRelevanceAlgorithm::CalcRelevance(ITransformation* pTransform, bool bSequence)
{
	SS_TRY;

	m_pTransform = pTransform;
	m_bSequence = bSequence;
	m_bIDFormNotCoincident = false;
	CLeafIterator itMaxBegin, itMaxEnd;

#ifdef COUNT_SKIP_UNITS
	int iTransformSize = 0;
	for(CQueryIterator<> qit = pTransform->GetFirstGroup(); *qit; ++qit)
		iTransformSize++;
#endif

	int iDistance = INT_MAX;
	STransRev oRev;
	for(CLeafIteratorEx lit(m_pSent->GetFirstUnit()); *lit; )
	{
		if(!FindCombination(lit))
			break;

		CLeafIteratorEx
			itFirst = *min_element(m_oCombination.begin(), m_oCombination.end(), CPosComparer()),
			itLast = *max_element(m_oCombination.begin(), m_oCombination.end(), CPosComparer());

		int iTotalSkipped = m_iCurrSkipped + m_iTopSkipped + m_iUnobligSkipped;
		int iAux = itLast.GetPos() - itFirst.GetPos() + iTotalSkipped * 150;
		if(iAux < iDistance)
		{
			iDistance = iAux;
			oRev.iRelevance = GetCombinationRelevance();
			oRev.iSkippedCount = iTotalSkipped;
#ifdef COUNT_SKIP_UNITS
			oRev.iWeightSkipPercentage = 100 * m_iSkippedByWeight / iTransformSize;
#endif
			itMaxBegin = GetLeftmostSibling(*itFirst);
			itMaxEnd = GetRightmostSibling(*itLast);
		}

		lit = ++itFirst;
	}

	if(oRev.iRelevance > 0)
	{
		ClearHighlightType(m_pSent->GetFirstUnit(), itMaxBegin);
		if(*itMaxEnd)
			ClearHighlightType(++itMaxEnd, NULL);
	}

	return oRev;

	SS_CATCH(L"");
	return STransRev();
}

/////////////////////////////////////////////////////////////////////////////////////
// CRelevanceAlgorithm functions

int CRelevanceAlgorithm::GetCombinationRelevance()
{
	SS_TRY;

	// find first and last words in the combination
	CLeafIteratorEx	itBegin = *min_element(m_oCombination.begin(), m_oCombination.end(), CPosComparer()),
					itEnd = *max_element(m_oCombination.begin(), m_oCombination.end(), CPosComparer());

	// calculate combination's basic relevance
	double dDist =	(double) GetHighlightedWordCount(itBegin, itEnd) / 
					(double) (itEnd.GetPos() - itBegin.GetPos() + 1);
	int iRelevance = (int) m_oCurve.Calculate(dDist);

	// check if a title's present
	int i = 0;
	for(i = 0; i < 	m_iTopSkipped; i++)
		iRelevance -= iRelevance / 3;
	for(i = 0; i < m_iTopAdded; i++)
		iRelevance += (100 - iRelevance) / 3;
	for(i = 0; i < 	m_iCurrSkipped; i++)
		iRelevance -= iRelevance / 4;
	for(i = 0; i < m_iCurrAdded; i++)
		iRelevance += (100 - iRelevance) / 2;

	// check unobligatory
	for(i = 0; i < m_iUnobligSkipped; i++)
		iRelevance = (3 * iRelevance) / 5;

	// check if some unit is not coincident by idforms
	if(m_bIDFormNotCoincident)
		iRelevance /= 10;

	// apply transformation's weight
	float fWeight = m_pTransform->GetWeight();
	if(fWeight)
		iRelevance = (int) (iRelevance * fWeight);

	return iRelevance;

	SS_CATCH(L"");
	return 0;
}

bool CRelevanceAlgorithm::FindCombination(const CLeafIteratorEx& itFirst)
{
	SS_TRY;

	CQueryIterator<> itQuery(m_pTransform->GetFirstGroup());
	if(!*itQuery)
		return false;

	m_oCombination.erase(m_oCombination.begin(), m_oCombination.end());
	m_iTopSkipped = 0;
	m_iTopAdded = 0;
	m_iCurrSkipped = 0;
	m_iCurrAdded = 0;
	m_iUnobligSkipped = 0;
	m_bIDFormNotCoincident = false;

	CLeafIteratorEx itSent = itFirst;
	while(true)
	{
		bool bSkipped = false;

		CLeafIteratorEx itTmp = itSent;
		if(!FindCoincidentUnit(itSent, itQuery))
		{
			itSent = itTmp;
			if(IsSkipUnit(*itQuery, m_iParagraphNumber))
			{
				if((*itQuery)->IsNumber(0))
					m_iTopSkipped++;
				else if((*itQuery)->IsNumber(m_iParagraphNumber))
					m_iCurrSkipped++;
				else if(!(*itQuery)->IsObligatory())
					m_iUnobligSkipped++;
				bSkipped = true;
			}
			else
				return false;
		}
		else
		{
			// compare id forms
			EHighlightType e = (*itSent)->GetHighlightType();
			if(e == hltQueryWord && !CompareMorphology<true>(*itSent, *itQuery, true))
				m_bIDFormNotCoincident = true;

			// check titles
			m_iTopAdded += (*itQuery)->IsNumber(0) ? 1 : 0;
			if(m_iParagraphNumber != 0)
				m_iCurrAdded += (*itQuery)->IsNumber(m_iParagraphNumber) ? 1 : 0;

			// push to combination
			m_oCombination.push_back(itSent);
		}

		if(!++itQuery)
			return !m_oCombination.empty();

		if(!m_bSequence || itQuery.SequenceIDChanged())
			itSent = itFirst;
		else if(!bSkipped)
			++itSent;
	}

	SS_CATCH(L"");
	return false;
}

bool CRelevanceAlgorithm::FindCoincidentUnit(CLeafIteratorEx& lit, const CQueryIterator<>& qit)
{
	for(; *lit; ++lit)
	{
#ifdef _DEBUG
		LPCWSTR szUnit = (*lit)->GetWord();
		LPCWSTR szGroup = (*qit)->GetWord();
		int iTransPos = (*lit)->GetTransPosition();
#endif

		if((*lit)->GetHighlightType() == hltNone)
			continue;
		if((*lit)->GetTransPosition() == qit.GetPosition())
			return true;
	}

	return false;
}

int CRelevanceAlgorithm::GetHighlightedWordCount(CLeafIterator itBegin, CLeafIterator itEnd)
{
	SS_TRY;

	++itEnd;

	int iCount = 0;
	for(CLeafIterator it = itBegin; *it != *itEnd; ++it)
	{
		if((*it)->GetHighlightType() != hltNone)
			iCount++;
	}

	return iCount;

	SS_CATCH(L"");
	return 0;
}

void CRelevanceAlgorithm::ClearHighlightType(CLeafIterator itBegin, CLeafIterator itEnd)
{
	SS_TRY;

	for(CLeafIterator it = itBegin; *it != *itEnd; ++it)
		(*it)->SetHighlightType(hltNone);

	SS_CATCH(L"");
}

}
}
