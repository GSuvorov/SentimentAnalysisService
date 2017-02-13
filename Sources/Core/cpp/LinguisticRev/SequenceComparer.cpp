#include "stdafx.h"
#include "SequenceComparer.h"

#include "QueryIterator.h"

using namespace SS;
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Core::Types::IndexationSearch;


namespace SS
{
namespace LinguisticRev
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CSequenceComparer::CSequenceComparer() :
		m_pSent(NULL),
		m_pTransform(NULL),
		m_eMode(cmNone),
		m_bSynonyms(false),
		m_bSequence(true),
		m_iParagraphNumber(0),
		m_eResult(ESearchResult::srFound)
{
}

CSequenceComparer::~CSequenceComparer()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CSequenceComparer interface

void CSequenceComparer::Init(ISentence* pSent, int iParagraphNumber, bool bSynonyms)
{
	m_pSent = pSent;
	m_pTransform = NULL;
	m_eMode = cmNone;
	m_bSynonyms = bSynonyms;
	m_bSequence = true;
	m_iParagraphNumber = iParagraphNumber;
}

bool CSequenceComparer::HasCoincidences(ITransformation* pTransform, ECompareMode eMode, bool bSequence)
{
	SS_TRY;

	m_eMode = eMode;
	m_bSequence = bSequence;
	m_pTransform = pTransform;
	ClearHighlightType(m_pSent);

	bool bRet = false;
	for(CLeafIterator it = m_pSent->GetFirstUnit(); *it; )
	{
		if(!FindCoincidence(it, m_pSent->GetFirstUnit()))
			return bRet;
		else
			bRet = true;
	}

	return bRet;

	SS_CATCH(L"");
	return false;
}

bool CSequenceComparer::HighlightKeywords(ITransformation* pTransform)
{
	SS_TRY;

	for(CLeafIterator itSent = m_pSent->GetFirstUnit(); *itSent; ++itSent)
	{
		(*itSent)->SetHighlightType(hltNone);
		for(CQueryIterator<true> itQuery = pTransform->GetFirstGroup(); *itQuery; ++itQuery)
		{
			if(CompareMorphology<false>(*itSent, *itQuery, m_bSynonyms))
			{
				(*itSent)->SetHighlightType(hltQueryWord);
				break;
			}
		}
	}

	SS_CATCH(L"");
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// CSequenceComparer functions

bool CSequenceComparer::FindCoincidence(CLeafIterator& itSent, IUnit* pFirstUnit)
{
	SS_TRY;

	CQueryIterator<true> itQuery = m_pTransform->GetFirstGroup();
	if(!*itQuery)
		return false;

	// true, when first time searching for this word
	bool bQueryUnitChanged = true;
	bool bSomeUnitFound = false;

	CLeafIterator itAux = itSent;
	while(true)
	{
		IUnit* pUnit = FindCoincidentUnit(*itSent, *itQuery);
		if(m_eResult == srNotFound && bQueryUnitChanged)
			return false;

		if(pUnit)
		{
			if(bQueryUnitChanged)
				itAux = pUnit;

			bQueryUnitChanged = false;
			bSomeUnitFound = true;

			assert(!IsAnswerUnit(*itQuery));
			pUnit->SetHighlightType(hltQueryWord);
			pUnit->SetTransPosition(itQuery.GetPosition());

			itSent = pUnit;
			++itSent;
		}
		else
		{
			if(!++itQuery)
				return bSomeUnitFound;

			if(!m_bSequence || itQuery.SequenceIDChanged())
				itSent = pFirstUnit;
			else
				itSent = bQueryUnitChanged ? itAux : ++itAux;

			bQueryUnitChanged = true;
		}
	}

	SS_CATCH(L"");
	return false;
}

IUnit* CSequenceComparer::FindCoincidentUnit(IUnit* pStartUnit, IQueryUnit* pQueryUnit)
{
	m_eResult = IsSkipUnit(pQueryUnit, m_iParagraphNumber) ? srSkipped : srNotFound;
	for(CLeafIterator it = pStartUnit; *it; ++it)
	{
#ifdef _DEBUG
		LPCWSTR szUnit = (*it)->GetWord();
		LPCWSTR szGroup = pQueryUnit->GetWord();
#endif

		if((*it)->GetHighlightType() != hltNone)
			continue;
		if(Compare(*it, pQueryUnit))
		{
			m_eResult = srFound;
			return *it;
		}
	}

	return NULL;
}

bool CSequenceComparer::Compare(IUnit* pUnit, IQueryUnit* pQueryUnit)
{
	SS_TRY;

	switch(m_eMode)
	{
		case cmMorpho:
			return	CompareMorphology<false>(pUnit, pQueryUnit, m_bSynonyms);

		case cmSemantics:
			if(IsSkipUnit(pQueryUnit, m_iParagraphNumber) && CompareMorphology<false>(pUnit, pQueryUnit, m_bSynonyms))
			{
				if(!IsIndexInColl<icSemanticIndex>(pQueryUnit->GetQueryUnitIndexCollection(eitSemanticIndex), pUnit))
				{
					m_eResult = srNotFound;
					return false;
				}
				return true;
			}

			return	CompareMorphology<false>(pUnit, pQueryUnit, m_bSynonyms) &&
					IsIndexInColl<icSemanticIndex>(pQueryUnit->GetQueryUnitIndexCollection(eitSemanticIndex), pUnit);

		case cmSyntax:
			if(IsSkipUnit(pQueryUnit, m_iParagraphNumber) && CompareMorphology<false>(pUnit, pQueryUnit, m_bSynonyms))
			{
				if(	!IsIndexInColl<icSemanticIndex>(pQueryUnit->GetQueryUnitIndexCollection(eitSemanticIndex), pUnit) ||
					!IsIndexInColl<icNormal>(pQueryUnit->GetQueryUnitIndexCollection(eitSyntaxIndex), pUnit))
				{
					m_eResult = srNotFound;
					return false;
				}
				return true;
			}

			return	CompareMorphology<false>(pUnit, pQueryUnit, m_bSynonyms) &&
					IsIndexInColl<icSemanticIndex>(pQueryUnit->GetQueryUnitIndexCollection(eitSemanticIndex), pUnit) &&
					IsIndexInColl<icNormal>(pQueryUnit->GetQueryUnitIndexCollection(eitSyntaxIndex), pUnit);

		default:
			assert(false);
	}

	SS_CATCH(L"");
	return false;
}

}
}
