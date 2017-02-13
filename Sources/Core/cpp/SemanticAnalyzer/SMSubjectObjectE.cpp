#include "StdAfx.h"
#include ".\smsubjectobjecte.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{


CSMSubjectObjectE::CSMSubjectObjectE(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMSubjectObjectE::~CSMSubjectObjectE(void)
{
}

bool CSMSubjectObjectE::AnalizeSentence()
{
	SS_TRY
	{
		do{
			if((IsContainFeature(efcSemanticCommon) || IsContainFeature(efEDNounNominative))&&
				!IsContainFeature(efcDetectedNoun) && !IsContainFeature(efECaseGenetive) && !IsContainFeature(efcPretext))
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject);
				AddFeature(efcDetectedNoun);
			}
			else if(IsContainFeature(efEDNounPrep)&&
				!IsContainFeature(efcDetectedNoun)
				)
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scPrepositionObject);
				AddFeature(efcDetectedNoun);
			}
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}

			}
		}
	}
}