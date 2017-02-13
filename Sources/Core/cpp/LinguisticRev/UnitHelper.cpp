#include "stdafx.h"
#include "UnitHelper.h"

#define REV_MASK 0x00ff0000

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;

#include "QueryIterator.h"
using namespace SS::Core::Types::IndexationSearch;

#include "..\ASSInterface\TMainAnalyseTypes.h"
using namespace SS::Core::Types;


namespace SS
{
namespace LinguisticRev
{

bool FindMorphology(IText* pText, IQueryUnit* pUnit, bool bSynonyms)
{
	SS_TRY;

	for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		if(FindMorphology(pPara, pUnit, bSynonyms))
			return true;
	}

	SS_CATCH(L"");
	return false;
}

bool FindMorphology(IParagraph* pPara, IQueryUnit* pUnit, bool bSynonyms)
{
	SS_TRY;

	for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
	{
		for(CLeafIterator it = pSent->GetFirstUnit(); *it; ++it)
		{
			if(CompareMorphology<false>(*it, pUnit, bSynonyms))
				return true;
		}
	}

	SS_CATCH(L"");
	return false;
}

bool CompareIDForms(IQueryUnitIndex* pQIndex, IIndex* pIndex)
{
	// if pQUIndex has no id forms, return true
	short shIDForm;
	if(!pQIndex->GetFirstIDForm(&shIDForm))
		return true;

	for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
	{
		if(pQIndex->ContainsIDForm(pFeature->GetMorphoIndex()))
			return true;
	}
	return false;
}

bool IsAnswerUnit(IQueryUnit* pUnit)
{
	SS_TRY;

	if(!pUnit->GetQueryUnitIndexCollection(eitMorphoIndex))
	{
		// answer unit must have either semantics or syntax index collection
		return 	pUnit->GetQueryUnitIndexCollection(eitSemanticIndex) ||
				pUnit->GetQueryUnitIndexCollection(eitSyntaxIndex);
	}

	SS_CATCH(L"");
	return false;
}

void SetRelevance(ISentence* pSent, UINT uiRev)
{
	UINT uiOld = pSent->GetRelevance() - (pSent->GetRelevance() / RELEVANCE_MULTIPLIER) * RELEVANCE_MULTIPLIER;
	pSent->SetRelevance(uiOld + uiRev * RELEVANCE_MULTIPLIER);
}

void SetRelevance(IParagraph* pPara, UINT uiRev)
{
	UINT uiOld = pPara->GetRelevance() - (pPara->GetRelevance() / RELEVANCE_MULTIPLIER) * RELEVANCE_MULTIPLIER;
	pPara->SetRelevance(uiOld + uiRev * RELEVANCE_MULTIPLIER);
}

UINT GetRelevance(ISentence* pSent)
{
	return (pSent->GetRelevance() & REV_MASK) >> 16;
}

IUnit* GetRoot(IUnit* pUnit)
{
	SS_TRY;

	for(IUnit* pRoot = pUnit; pRoot; pRoot = pRoot->GetParentUnit())
	{
		if(!pRoot->GetParentUnit())
			return pRoot;
	}

	SS_CATCH(L"");
	return pUnit;
}

IUnit* GetRightmostSibling(IUnit* pUnit)
{
	SS_TRY;

	for(IUnit* pChild = GetRoot(pUnit); pChild; pChild = pChild->GetRightChildUnit())
	{
		if(!pChild->HasChilds())
			return pChild;
	}

	SS_CATCH(L"");
	return pUnit;
}

IUnit* GetLeftmostSibling(IUnit* pUnit)
{
	SS_TRY;

	for(IUnit* pChild = GetRoot(pUnit); pChild; pChild = pChild->GetLeftChildUnit())
	{
		if(!pChild->HasChilds())
			return pChild;
	}

	SS_CATCH(L"");
	return pUnit;
}

void ClearHighlightType(ISentence* pSent)
{
	SS_TRY;

	for(CLeafIterator it = pSent->GetFirstUnit(); *it; ++it)
	{
		(*it)->SetTransPosition(0);
		(*it)->SetHighlightType(hltNone);
	}

	SS_CATCH(L"");
}

void HighlightKeywords(ISentence* pSent, ITransformation* pTransform, bool bSynonyms)
{
	SS_TRY;

	for(CLeafIterator itSent = pSent->GetFirstUnit(); *itSent; ++itSent)
	{
		(*itSent)->SetHighlightType(hltNone);
		for(CQueryIterator<true> itQuery = pTransform->GetFirstGroup(); *itQuery; ++itQuery)
		{
			if(CompareMorphology<false>(*itSent, *itQuery, bSynonyms))
			{
				(*itSent)->SetHighlightType(hltQueryWord);
				break;
			}
		}
	}

	SS_CATCH(L"");
}

bool IsSkipUnit(IQueryUnit* pUnit, int iParagraphNumber)
{
	return	pUnit->IsNumber(0) || 
			pUnit->IsNumber(iParagraphNumber) || 
			!pUnit->IsObligatory();
}

}
}
