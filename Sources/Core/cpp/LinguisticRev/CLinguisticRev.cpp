#include "StdAfx.h"
#include ".\clinguisticrev.h"


namespace SS
{
namespace LinguisticRev
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CLinguisticRev::CLinguisticRev() :
		m_pQuery(NULL),
		m_bSynonyms(false)
{
}

CLinguisticRev::~CLinguisticRev()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

ULONG CLinguisticRev::Release()
{
	SS_TRY;

	delete this;

	SS_CATCH(L"");
	return NULL;
}

HRESULT CLinguisticRev::QueryInterface(REFIID pIID, void** pBase)
{
	SS_TRY;

	if(IID_LinguisticRelevance == pIID)
		*pBase = (ILinguisticRelevance*) this;
	else if(IID_Base == pIID)
		*pBase = (SS::Interface::IBase*) this;
	else 
		*pBase = NULL;

	SS_CATCH(L"");
	return S_OK;
}

void CLinguisticRev::SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
{
	SS_TRY;

	CAnalyseParams::SetAnalyseParams(pAnalyseParams);
	m_bSynonyms = GetAnalyseParams()->GetAnalyseDepthParams()->IsSynonims();

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// ILinguisticRelevance interface

void CLinguisticRev::SetQuery(IQuery* pQuery)
{
	m_pQuery = pQuery;
}

bool CLinguisticRev::IsExistAnswerWord(IText* pText)
{
	SS_TRY;

	if(!m_pQuery)
		return true;

	CSequenceComparer oComparer;
	for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		{
			oComparer.Init(pSent, pPara->GetNumber(), m_bSynonyms);
			SetRelevance(pSent, 0);

			ITransformation* pTransform = m_pQuery->GetFirstTransformation();
			for(; pTransform; pTransform = pTransform->GetNextTransformation())
			{
				ClearHighlightType(pSent);
				if(oComparer.HasCoincidences(pTransform, cmMorpho, true))
				{
					SetRelevance(pSent, 1);
					break;
				}
			}
		}
	}

	SS_CATCH(L"");
	return true;
}

__int32 CLinguisticRev::UpdateRelevance(IText* pText, IText* pTextTitle)
{
	SS_TRY;

	if(!m_pQuery)
		return 0;

	m_oRevCalc.Init(m_bSynonyms, m_pQuery);
	m_oRevCalc.SetLoadManager(GetLoadManager());
	m_oRevCalc.SetAnalyseParams(GetAnalyseParams());
	m_oRevCalc.UpdateRelevance(pText);

	SS_CATCH(L"");
	return 0;
}

void CLinguisticRev::HighlightKeywords(IText* pText)
{
	SS_TRY;

	if(!m_pQuery)
		return;

	ITransformation* pTransform = m_pQuery->GetFirstTransformation();
	if(!pTransform)
		return;

	for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		{
			if(GetRelevance(pSent) != 0)
				continue;
			SS::LinguisticRev::HighlightKeywords(pSent, pTransform, m_bSynonyms);
		}
	}

	SS_CATCH(L"");
}

}
}
