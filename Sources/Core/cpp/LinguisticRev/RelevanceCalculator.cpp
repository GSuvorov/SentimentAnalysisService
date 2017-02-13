#include "stdafx.h"
#include "RelevanceCalculator.h"

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;


namespace SS
{
namespace LinguisticRev
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CRelevanceCalculator::CRelevanceCalculator() :
		m_pQuery(NULL),
		m_bSynonyms(false)
{
}

CRelevanceCalculator::~CRelevanceCalculator()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CRelevanceCalculator interface

void CRelevanceCalculator::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	CBaseCoreClass::SetLoadManager(pLoadManager);
	m_oFinder.SetLoadManager(pLoadManager);
}

void CRelevanceCalculator::SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
{
	CAnalyseParams::SetAnalyseParams(pAnalyseParams);
	m_oComparer.SetAnalyseParams(pAnalyseParams);
	m_oComparer.SetAnalyseParams(pAnalyseParams);
	m_oRevAlg.SetAnalyseParams(pAnalyseParams);
}

void CRelevanceCalculator::Init(bool bSynonyms, IQuery* pQuery)
{
	m_pQuery = pQuery;
	m_bSynonyms = bSynonyms;
}

void CRelevanceCalculator::UpdateRelevance(IText* pText)
{
	SS_TRY;

	SetTitleToQuery(pText);
	for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		if(pPara->GetType() == tctTable)
			continue;

		// percentage should be recalculated for each new paragraph
		m_mPercentage.clear();

		int iRelevance = 0;
		for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		{
			if(GetRelevance(pSent) == 0)
				continue;

			STransRev oRev = UpdateRelevance(pSent, pPara);

#ifdef COUNT_SKIP_UNITS
			int iSentRev = (100 - oRev.iWeightSkipPercentage) * 1000000 + 
							oRev.iFuzzyLevel * 1000 +
							oRev.iRelevance;
			pSent->SetRelevance(iSentRev);
#else
			int iSentRev = oRev.iFuzzyLevel * 100 + oRev.iRelevance;
			SetRelevance(pSent, iSentRev);
#endif

			iRelevance = max(iRelevance, iSentRev);
		}

#ifdef COUNT_SKIP_UNITS
		pPara->SetRelevance(iRelevance);
#else
		SetRelevance(pPara, iRelevance);
#endif
	}

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CRelevanceCalculator functions

STransRev CRelevanceCalculator::UpdateRelevance(ISentence* pSent, IParagraph* pPara)
{
	STransRev oResult;
	SS_TRY;

	// reset variables
	int iParagraphNumber = pPara->GetNumber();
	m_mRev.clear();
	m_oComparer.Init(pSent, iParagraphNumber, m_bSynonyms);
	m_oFinder.Init(pSent);
	m_oRevAlg.Init(pSent, iParagraphNumber);

	// calculate relevance for all transformations
	ITransformation* pTransform = m_pQuery->GetFirstTransformation();
	for(; pTransform; pTransform = pTransform->GetNextTransformation())
	{
		if(!pTransform->IsObligatory())
			continue;

		int iFuzzyLevel = m_mRev.empty() ? 0 : m_mRev.begin()->first.iFuzzyLevel;
		if(SentenceSucceeded(7, pTransform))
		{
			STransRev oRev = m_oRevAlg.CalcRelevance(pTransform, true);
			float fWeight = m_oFinder.GetAnswerWordWeight(pTransform);
			if(fWeight)
				oRev.iRelevance = (int)(oRev.iRelevance * fWeight);
			oRev.iFuzzyLevel = 7;
			m_mRev.insert(TRevMap::value_type(oRev, pTransform));
		}
		//else if(iFuzzyLevel <= 6 && SentenceSucceeded(6, pTransform))
		//{
		//	STransRev oRev = m_oRevAlg.CalcRelevance(pTransform, true);
		//	float fWeight = m_oFinder.GetAnswerWordWeight(pTransform);
		//	if(fWeight)
		//		oRev.iRelevance = (int)(oRev.iRelevance * fWeight);
		//	oRev.iFuzzyLevel = 6;
		//	m_mRev.insert(TRevMap::value_type(oRev, pTransform));
		//}
		//else if(iFuzzyLevel <= 5 && SentenceSucceeded(5, pTransform))
		//{
		//	STransRev oRev = m_oRevAlg.CalcRelevance(pTransform, false);
		//	float fWeight = m_oFinder.GetAnswerWordWeight(pTransform);
		//	if(fWeight)
		//		oRev.iRelevance = (int)(oRev.iRelevance * fWeight);
		//	oRev.iFuzzyLevel = 5;
		//	m_mRev.insert(TRevMap::value_type(oRev, pTransform));
		//}
		//else if(iFuzzyLevel <= 4 && SentenceSucceeded(4, pTransform))
		//{
		//	STransRev oRev = m_oRevAlg.CalcRelevance(pTransform, false);
		//	float fWeight = m_oFinder.GetAnswerWordWeight(pTransform);
		//	if(fWeight)
		//		oRev.iRelevance = (int)(oRev.iRelevance * fWeight);
		//	oRev.iFuzzyLevel = 4;
		//	m_mRev.insert(TRevMap::value_type(oRev, pTransform));
		//}
		//else if(iFuzzyLevel <= 3 && SentenceSucceeded(3, pTransform))
		//	m_mRev.insert(TRevMap::value_type(STransRev(3), pTransform));
		//else if(iFuzzyLevel <= 2 && SentenceSucceeded(2, pTransform))
		//	m_mRev.insert(TRevMap::value_type(STransRev(2), pTransform));
		//else if(iFuzzyLevel <= 1 && SentenceSucceeded(1, pTransform))
		//	m_mRev.insert(TRevMap::value_type(STransRev(1), pTransform));
	}

	if(!m_mRev.empty())
	{
		// if some transformations succeeded, choose the best one
		oResult = m_mRev.begin()->first;

		ITransformation* pBestTransform = m_mRev.begin()->second;
		if(pBestTransform != GetLastObligTransform())
		{
			// reset highlight
			ClearHighlightType(pSent);
			SentenceSucceeded(oResult.iFuzzyLevel, pBestTransform);
			if(oResult.iFuzzyLevel == 7 || oResult.iFuzzyLevel == 6)
				m_oRevAlg.CalcRelevance(pBestTransform, true);
			else if(oResult.iFuzzyLevel == 5 || oResult.iFuzzyLevel == 4)
				m_oRevAlg.CalcRelevance(pBestTransform, false);
		}

		if(oResult.iFuzzyLevel >= 4)
		{
			for(CLeafIterator it = pSent->GetFirstUnit(); *it; ++it)
				(*it)->SetIsInAnswerSentence(true);
		}
		else if(oResult.iFuzzyLevel == 3 || oResult.iFuzzyLevel == 2)
		{
			int iCount = CountPresentWordsPercentage(pPara, pBestTransform);
			if(iCount < 100)
			{
				oResult.iFuzzyLevel = 1;
				oResult.iRelevance = iCount;
			}
		}

		// remove green highlight
		if(oResult.iFuzzyLevel <= 5)
		{
			for(CLeafIterator it = pSent->GetFirstUnit(); *it; ++it)
			{
				if((*it)->GetHighlightType() == hltAnswerWord)
					(*it)->SetHighlightType(hltNone);
			}
		}
	}
	else
	{
		// else, highlight keywords
		if(m_pQuery->GetFirstTransformation())
			HighlightKeywords(pSent, m_pQuery->GetFirstTransformation(), m_bSynonyms);
	}

	SS_CATCH(L"");
	return oResult;
}

bool CRelevanceCalculator::SentenceSucceeded(int iFuzzyLevel, ITransformation* pTransform)
{
	SS_TRY;

	switch(iFuzzyLevel)
	{
		case 7:
			return	m_oComparer.HasCoincidences(pTransform, cmSyntax, true) &&
					m_oFinder.FindAnswerWord(pTransform, cmSyntax, true);
		case 6:
			return	m_oComparer.HasCoincidences(pTransform, cmMorpho, true) && 
					m_oFinder.FindAnswerWord(pTransform, cmSyntax, true);
		case 5:
			return	m_oComparer.HasCoincidences(pTransform, cmMorpho, false) && 
					m_oFinder.FindAnswerWord(pTransform, cmSyntax, false);
		case 4:
			return	m_oComparer.HasCoincidences(pTransform, cmMorpho, false) && 
					m_oFinder.FindAnswerWord(pTransform, cmSemantics, false);
		case 3:
			return	m_oComparer.HighlightKeywords(pTransform) && 
					m_oFinder.FindAnswerWord(pTransform, cmSyntax, false);
		case 2:
			return	m_oComparer.HighlightKeywords(pTransform) && 
					m_oFinder.FindAnswerWord(pTransform, cmSemantics, false);
		case 1:
			return	m_oComparer.HasCoincidences(pTransform, cmMorpho, true);
		default:
			assert(false);
	}

	SS_CATCH(L"");
	return false;
}

void CRelevanceCalculator::SetTitleToQuery(IText* pText)
{
	SS_TRY;

	// clear title numbers from old searches
	for(ITransformation* pTrans = m_pQuery->GetFirstTransformation(); pTrans; pTrans = pTrans->GetNextTransformation())
	{
		for(CQueryIterator<true> it = pTrans->GetFirstGroup(); *it; ++it)
			(*it)->ClearNumbers();
	}

	for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		if(pPara->GetType() != tctHeader)
			continue;

		// set title indicators to query units
		for(ITransformation* pTrans = m_pQuery->GetFirstTransformation(); pTrans; pTrans = pTrans->GetNextTransformation())
		{
			for(CQueryIterator<true> it = pTrans->GetFirstGroup(); *it; ++it)
			{
				if(FindMorphology(pPara, *it, false))
					(*it)->AddNumber(pPara->GetNumber());
			}
		}
	}

	SS_CATCH(L"");
}

ITransformation* CRelevanceCalculator::GetLastObligTransform()
{
	SS_TRY;

	for(ITransformation* pTrans = m_pQuery->GetLastTransformation(); pTrans; pTrans = pTrans->GetPrevTransformation())
	{
		if(pTrans->IsObligatory())
			return pTrans;
	}

	SS_CATCH(L"");
	return NULL;
}

int CRelevanceCalculator::CountPresentWordsPercentage(IParagraph* pPara, ITransformation* pTrans)
{
	SS_TRY;

	TPercentageMap::iterator it = m_mPercentage.find(pTrans);
	if(it != m_mPercentage.end())
		return (*it).second;

	int iRes = 100;
	for(CQueryIterator<true> qit = pTrans->GetFirstGroup(); *qit; ++qit)
	{
		if( !IsSkipUnit(*qit, pPara->GetNumber()) &&
			!FindMorphology(pPara, *qit, m_bSynonyms))
		{
			iRes = (int)((float) iRes * (1. - (*qit)->GetWeight()));
		}
	}

	m_mPercentage.insert(TPercentageMap::value_type(pTrans, iRes));
	return iRes;

	SS_CATCH(L"");
	return 100;
}

}
}
