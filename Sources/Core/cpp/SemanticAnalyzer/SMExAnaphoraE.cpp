#include "StdAfx.h"
#include ".\smexanaphorae.h"

namespace SS
{
namespace Semantic
{
namespace AnalysisExperts
{
namespace IndexationExperts
{
namespace English
{


CSMExAnaphoraE::CSMExAnaphoraE(void)
{
	m_bInit = false;
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMExAnaphoraE::~CSMExAnaphoraE(void)
{
}
bool CSMExAnaphoraE::Init(IText * pIText)
{
	SS_TRY
	{
		if(!CSMExAnaphora::Init(pIText)) return false;

		if(!m_bInit)
		{
			m_bInit = true;

			m_AimWords.clear();
			m_AimWords.insert(wstring(_T("he")));
			m_AimWords.insert(wstring(_T("she")));
			//m_AimWords.insert(wstring(_T("they")));
		}
	}
	SS_CATCH(L"");
	return true;
}

bool CSMExAnaphoraE::IsNameAgent()
{
	SS_TRY
	{
		bool bName=(
			!IsSyntax() &&
			//(SpecialTypeEquals(ustComplexProperName) || SpecialTypeEquals(ustUndefined)) &&
			(IsContainFeature(efcSemanticCommon) || IsContainFeature(efEDNounNominative)) &&
			(m_LexemType.IsWord()) &&
			IsContainFeature(efcDetectedNoun)&& 
			IsContainFeature(efcSemanticName)
			) ;
	//if (bName && m_Unit->m_SpecialUnitType==ustComplexProperName)
	//{
	//	if ((WT_Eng() || WT_ENG()||WT_eng()|| m_Unit->m_UnitType==utEnG) &&!m_Unit->m_MorphoInfo.IsContainFeature(efDetectedNoun) )
	//	{
	//		bName=false;
	//	}
	//}

		return bName;
	}
	SS_CATCH(L"");
}

}
}
}
}
}