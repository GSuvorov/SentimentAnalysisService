#include "StdAfx.h"
#include ".\smsubjectr.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{
			

CSMSubjectR::CSMSubjectR(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealRussian;
	m_sDescription=_T(__FUNCTION__);
}

CSMSubjectR::~CSMSubjectR(void)
{
}

bool CSMSubjectR::AnalizeSentence()
{
	SS_TRY
	{
		do{
			if ( !IsSyntax() )	
			{
				if ( (IsContainFeature(efRCaseNominative)|| IsContainFeature(efRCaseAccusative)) &&
					IsContainFeature(efcNoun)&& !IsContainFeature(efcDetectedNoun)
					)
				{
					if( !IsPrepositionBeforeR(7,m_pCurrentUnit) )
					{
						if (IsContainFeature(efRCaseNominative))
							AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
						if (IsContainFeature(efRCaseAccusative)
							//&&IsIndexationMode()
							)
							AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject);
					}
				}
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