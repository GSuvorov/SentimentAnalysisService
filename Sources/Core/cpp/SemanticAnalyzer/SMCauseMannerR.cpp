#include "StdAfx.h"
#include ".\smcausemannerr.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{
				namespace Russian
				{
				


CSMCauseMannerR::CSMCauseMannerR(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealRussian;
	m_sDescription=_T(__FUNCTION__);
}

CSMCauseMannerR::~CSMCauseMannerR(void)
{
}
bool CSMCauseMannerR::AnalizeSentence()
{
	if(m_AnalyseLanguage!=m_pCurrentSentence->GetLanguage()) return true;

	UINT uiWordCount=0;
	wstring str;

	do{
		uiWordCount++;
		str = m_pCurrentUnit->GetWord();
		m_StringService.LowerSz((TCHAR*)str.c_str());
		
		if (uiWordCount==1 && str==wstring(_T("чтобы")))
		{
			AddMannerCategory();
		}
		
		if( !IsSyntax() )
		{
			if(IsContainFeature(efRVerbDeepr))
			{
				AddMannerCategory();
			}else if (IsContainFeature(efRCaseInstrumental)&&
				!(IsContainFeature(efcDetectedNoun)|| IsContainFeature(efRAnimate) ) 
				)
			{
				AddMannerCategory();
			}
		}

	}while(GetNextUnit());
	return true;

}

				}
			}
		}
	}
}