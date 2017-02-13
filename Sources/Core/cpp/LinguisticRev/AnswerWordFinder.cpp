#include "stdafx.h"
#include "AnswerWordFinder.h"

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
/////////////////////////////////////////////////////////////////////////////////////

CAnswerWordFinder::CAnswerWordFinder() :
		m_pSent(NULL),
		m_pSemanticFeature(NULL),
		m_pSemanticConverter(NULL)
{
}

CAnswerWordFinder::~CAnswerWordFinder()
{
	if(m_pSemanticFeature)
		delete m_pSemanticFeature;
}

/////////////////////////////////////////////////////////////////////////////////////
// CAnswerWordFinder interface

void CAnswerWordFinder::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY;

	if(GetLoadManager())
		return;

	CBaseCoreClass::SetLoadManager(pLoadManager);
	m_pSemanticFeature = new CSemanticFeature;
	m_pSemanticConverter = (IAMConverterSemantic*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSemantic, IID_AMConverterSemantic);

	SS_CATCH(L"");
}

void CAnswerWordFinder::Init(ISentence* pSent)
{
	m_pSent = pSent;
}

bool CAnswerWordFinder::FindAnswerWord(ITransformation* pTransform, ECompareMode eMode, bool bSequence)
{
	bool bFound = false;
	SS_TRY;

	if(!m_oInterval.Init(m_pSent, pTransform, bSequence))
		return false;

	IQueryUnit* pAnswerUnit = m_oInterval.GetAnswerUnit();
	int iTransPosition = m_oInterval.GetTransPosition();

	for(IUnit* pUnit = m_oInterval.GetFirstUnit(); pUnit; pUnit = m_oInterval.GetNextUnit())
	{
		if(pUnit->GetHighlightType() != hltNone)
			continue;

		if(Compare(pUnit, pAnswerUnit, eMode))
		{
			bFound = true;
			pUnit->SetHighlightType(hltAnswerWord);
			pUnit->SetTransPosition(iTransPosition);
		}
	}

	if(!bFound)
		return false;

	// if answer word's subtree contains query word, change highlight
	bFound = false;
	for(IUnit* pUnit = m_oInterval.GetFirstUnit(); pUnit; pUnit = m_oInterval.GetNextUnit())
	{
		if(pUnit->GetHighlightType() == hltAnswerWord)
		{
			if(IsSubtreeHighlighted(pUnit))
			{
				pUnit->SetHighlightType(hltNone);
				pUnit->SetTransPosition(-1);
			}
			else
				bFound = true;
		}
	}

	SS_CATCH(L"");
	return bFound;
}

float CAnswerWordFinder::GetAnswerWordWeight(ITransformation* pTransform)
{
	SS_TRY;

	IQueryUnit* pQueryWord = GetQueryWord(pTransform);
	if(!pQueryWord)
		return 0.;

	IQueryUnitIndexCollection* pColl = pQueryWord->GetQueryUnitIndexCollection(eitSemanticIndex);
	if(!pColl)
		return 0.;

	float fWeight = 0.;
	for(IUnit* pUnit = m_oInterval.GetFirstUnit(); pUnit; pUnit = m_oInterval.GetNextUnit())
	{
		if(pUnit->GetHighlightType() != hltAnswerWord)
			continue;

		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			for(IQueryUnitIndex* pQIndex = pColl->GetFirstQueryUnitIndex(); pQIndex; pQIndex = pQIndex->GetNextQueryUnitIndex())
			{
				if((pQIndex->GetIndex().GetDictionaryIndexWithoutFlags()) == 
					pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndexWithoutFlags())
				{
					fWeight = max(fWeight, pQIndex->GetWeight());
				}
			}
		}

		if(fWeight > 0)
			RecalcTimeWeight(pUnit, &fWeight);
	}

	return fWeight;

	SS_CATCH(L"");
	return 0.;
}

/////////////////////////////////////////////////////////////////////////////////////
// CAnswerWordFinder functions

IQueryUnit* CAnswerWordFinder::GetQueryWord(ITransformation* pTrans)
{
	SS_TRY;

	for(IGroup* pGroup = pTrans->GetFirstGroup(); pGroup; pGroup = pGroup->GetNextGroup())
	{
		if(pGroup->GetSyntaxFeature()->IsQuestion())
			return pGroup->GetFirstQueryUnit();
	}

	SS_CATCH(L"");
	return NULL;
}

bool CAnswerWordFinder::Compare(IUnit* pUnit, IQueryUnit* pQueryUnit, ECompareMode eMode)
{
	SS_TRY;

	IQueryUnitIndexCollection* pSemColl = pQueryUnit->GetQueryUnitIndexCollection(eitSemanticIndex);
	switch(eMode)
	{
		case cmSemantics:
			return	pSemColl &&
					IsIndexInColl<icSemanticIndex>(pSemColl, pUnit);
		case cmSyntax:
			return	IsIndexInColl<icSemanticIndex>(pSemColl, pUnit) &&
					IsIndexInColl<icNormal>(pQueryUnit->GetQueryUnitIndexCollection(eitSyntaxIndex), pUnit);
		default:
			assert(false);
	}

	SS_CATCH(L"");
	return false;
}

bool CAnswerWordFinder::IsSubtreeHighlighted(IUnit* pUnit)
{
	SS_TRY;

	IUnit	*pRoot = GetRoot(pUnit),
			*pRight = pRoot ? pRoot->GetRightUnit() : NULL;
	for(CSentenceIterator it = pRoot; *it != pRight; ++it)
	{
		if((*it)->GetHighlightType() == hltQueryWord)
			return true;
	}

	SS_CATCH(L"");
	return false;
}

void CAnswerWordFinder::RecalcTimeWeight(IUnit* pUnit, float* pfWeight)
{
	SS_TRY;

	bool	bYear = false,
			bMonth = false,
			bDay = false;
	for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		if(pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() != eitSemanticIndex)
			continue;

		m_pSemanticFeature->Undefine();
		m_pSemanticConverter->EnCode(pIndex->GetDictionaryIndex()->GetFirst().GetId(), m_pSemanticFeature);
		if(!m_pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtTimeInterval))
			continue;

		if(m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.semtitDateYear))
			bYear = true;
		else if(m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.semtitDateMonth))
			bMonth = true;
		else if(m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.semtitDateNumber))
			bDay = true;
	}

	if(bYear && bDay && bMonth)
		*pfWeight = 1.;
	else if(bYear && bMonth)
		*pfWeight = max(*pfWeight, 0.95f);
	else if(bMonth && bDay)
		*pfWeight = max(*pfWeight, 0.75f);

	SS_CATCH(L"");
}

}
}
