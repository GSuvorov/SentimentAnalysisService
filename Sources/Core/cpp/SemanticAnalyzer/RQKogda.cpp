#include "StdAfx.h"
#include ".\rqkogda.h"
#include "smquestionanalysisr.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace Russian
				{


CRQSimpleAdjunct::CRQSimpleAdjunct(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CRQSimpleAdjunct::~CRQSimpleAdjunct(void)
{
}
bool CRQSimpleAdjunct::AnalyseQuestion(EQuestionType eQType)
{
	if(!GetFirstUnit()) return false;

	do{
		if(m_pCurrentUnit->GetSyntaxFeature() && 
			m_pCurrentUnit->GetSyntaxFeature()->IsQuestion())
		{
			if(eQType==eqtRKogda)
			{
				SetSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctTimeFixedPoint);
				AddSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctTimeEnd);
				AddSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctTimeDuration);
				AddSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctTimeSource);

				AddAllTimeIntervalSemantic(m_pCurrentUnit);
			}else if (eQType==eqtRGde)
			{
				SetSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctPlaceFixedPoint);
				AddSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctPlaceEnd);
				AddSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctPlaceSource);

				AddAllPlaceSemantic(m_pCurrentUnit);
			}else if (eQType==eqtRKuda)
			{
				SetSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctPlaceEnd);

				AddAllPlaceSemantic(m_pCurrentUnit);
			}else if (eQType==eqtROtkuda)
			{
				SetSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctPlaceSource);

				AddAllPlaceSemantic(m_pCurrentUnit);
			}else if (eQType==eqtROther)
			{
				m_pCurrentUnit->GetSyntaxFeature()->SetIsNotQuestion();
			}else if (eQType==eqtRPochemu)
			{
				SetSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctCause);
			}else if (eQType==eqtRKak)
			{
				SetSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctTimeDuration);

				AddAllTimeIntervalSemantic(m_pCurrentUnit);
			}else if (eQType==eqtRKakManner)
			{
				SetSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunctManner);
			}

			return true;
		}
	}while(GetNextUnit());

	return false;
}

				}
			}
		}
	}
}