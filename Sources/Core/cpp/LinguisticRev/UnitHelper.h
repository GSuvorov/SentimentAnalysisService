#pragma once

using namespace SS::Core::BlackBox;

#include "..\ASSInterface\Constants.h"
using namespace SS::Constants;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

#include "..\ASSInterface\ICommon.h"
using namespace SS::Interface::Core::Common;

#include "..\ASSInterface\IMainAnalyse.h"
using namespace SS::Interface::Core::MainAnalyse;


namespace SS
{
namespace LinguisticRev
{

/// compare mode used in comparing query and text units
enum ECompareMode
{
	cmNone,
	cmMorpho,		// morphology
	cmSemantics,	// morphology & semantics
	cmSyntax,		// morphology & semantics & syntax
};

enum EIndexCompareMode
{
	icNormal,			// compare index' SIGNIFICANT_INDEX_PART
	icCompareIDForms,	// compare index' SIGNIFICANT_INDEX_PART and it's id forms
	icSemanticIndex,	// compare full index
};

/// checks if pUnit and pQueryUnitIndexCollection have coincident index
template<EIndexCompareMode Mode>
bool IsIndexInColl(IQueryUnitIndexCollection* pColl, IUnit* pUnit)
{
	SS_TRY;

	if(!pColl)
		return true;

	bool bRes;
	IQueryUnitIndex* pQueryUnitIndex;
	for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		switch(Mode)
		{
			case icNormal:
				bRes = pColl->ContainsIndex(pIndex->GetDictionaryIndex()->GetFirst()) != NULL;
				break;
			case icCompareIDForms:
				pQueryUnitIndex = pColl->ContainsIndex(pIndex->GetDictionaryIndex()->GetFirst());
				bRes = pQueryUnitIndex && CompareIDForms(pQueryUnitIndex, pIndex);
				break;
			case icSemanticIndex:
				bRes = pColl->ContainsIndexFull(pIndex->GetDictionaryIndex()->GetFirst()) != NULL;
				break;
			default:
				assert(false);
		}
		if(bRes)
			return true;
	}

	SS_CATCH(L"");
	return false;
}

/// compares IUnit and IQueryUnit by morphology and synonyms (if bSynonyms is true)
template<bool bCompareIDForms>
bool CompareMorphology(IUnit* pUnit, IQueryUnit* pQueryUnit, bool bSynonyms)
{
	SS_TRY;

	if(bSynonyms)
	{
		IQueryUnitIndexCollection* pSynonyms = pQueryUnit->GetQueryUnitIndexCollection(eitSynonymIndex);
		if(pSynonyms)
			return IsIndexInColl<bCompareIDForms ? icCompareIDForms : icNormal>(pSynonyms, pUnit);
	}

	IQueryUnitIndexCollection* pMorpho = pQueryUnit->GetQueryUnitIndexCollection(eitMorphoIndex);
	return pMorpho && IsIndexInColl<bCompareIDForms ? icCompareIDForms : icNormal>(pMorpho, pUnit);

	SS_CATCH(L"");
	return false;
}

/// checks if there is a unit in pText, that is coincident with pUnit by morphology
bool FindMorphology(IText* pText, IQueryUnit* pUnit, bool bSynonyms);
bool FindMorphology(IParagraph* pPara, IQueryUnit* pUnit, bool bSynonyms);
/// checks index IDForms for coincidence
bool CompareIDForms(IQueryUnitIndex* pQIndex, IIndex* pIndex);

/// checks is pUnit is a question word
bool IsAnswerUnit(IQueryUnit* pUnit);

/// sets 16-24 bits in sentence relevance
void SetRelevance(ISentence* pSent, UINT uiRev);
/// sets 16-24 bits in paragraph relevance
void SetRelevance(IParagraph* pPara, UINT uiRev);

/// returns 16-24 bits of sentence relevance
UINT GetRelevance(ISentence* pSent);

/// returns the topmost unit above pUnit
IUnit* GetRoot(IUnit* pUnit);
/// returns rightmost child of pUnit's root
IUnit* GetRightmostSibling(IUnit* pUnit);
/// returns leftmost child of pUnit's root
IUnit* GetLeftmostSibling(IUnit* pUnit);

/// resets all unit's highlight type
void ClearHighlightType(ISentence* pSent);
/// highlights pSent's units, that coincide with pTransform by morphology
void HighlightKeywords(ISentence* pSent, ITransformation* pTransform, bool bSynonyms);

/// checks if unit is obligatory
bool IsSkipUnit(IQueryUnit* pUnit, int iParagraphNumber);

}
}
