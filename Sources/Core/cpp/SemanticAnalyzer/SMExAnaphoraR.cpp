#include "StdAfx.h"
#include ".\smexanaphorar.h"

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
				

CSMExAnaphoraR::CSMExAnaphoraR(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealRussian;
	m_sDescription=_T(__FUNCTION__);
}

CSMExAnaphoraR::~CSMExAnaphoraR(void)
{
}
bool CSMExAnaphoraR::Init(IText * pIText)
{
	if(!CSMExAnaphora::Init(pIText)) return false;

	m_AimWords.clear();

	m_AimWords.insert(wstring(_T("он")));
	m_AimWords.insert(wstring(_T("она")));
	m_AimWords.insert(wstring(_T("они")));
	m_AimWords.insert(wstring(_T("оно")));

	return true;
}

bool CSMExAnaphoraR::IsNameAgent()
{
	bool bName=(
		(!IsSyntax()/*||SyntaxTypeEquals(usntSubjectComplement)*/ )&&
		//( SpecialTypeEquals(ustComplexProperName) || SpecialTypeEquals(ustUndefined)	) &&
		(IsContainFeature(efRCaseNominative))&&
		(/*m_LexemType.IsRus()||m_LexemType.Isrus()||m_LexemType.IsRUS()*/m_LexemType.IsWord()) && 
		(IsContainFeature(efcSemanticName)|| IsContainFeature(efRAnimate))
		) ;
	return bName;
}

				}
			}
		}
	}
}