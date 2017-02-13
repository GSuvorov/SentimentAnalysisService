#include "StdAfx.h"
#include ".\smsyntaxcategoriespulldown.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace PostSyntaxAnalysis
			{		
		CSMSyntaxCategoriesPullDown::CSMSyntaxCategoriesPullDown(void) : m_uiCurrentBitNumber(0)
		{
			m_uiValue=0;
			m_AnalyseLanguage=SS::Core::Types::ealUndefined;

			m_sDescription=_T(__FUNCTION__);
		}

		CSMSyntaxCategoriesPullDown::~CSMSyntaxCategoriesPullDown(void)
		{
		}
		bool CSMSyntaxCategoriesPullDown::AnalizeSentence()
		{
			SS_TRY
			{
				SS::Core::Features::CSyntaxFeatureAposteriori * oSyntaxFeatureAposteriori;

				if (GetFirstUnit())
				{
					do{
						if (m_pCurrentUnit->GetSyntaxFeature()  )
						{
							oSyntaxFeatureAposteriori=m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori();
							m_uiValue=oSyntaxFeatureAposteriori->m_SyntaxCategories.GetValue();
							if (GetFirstCategoryValue())
							{
								do{
									if( m_pCurrentUnit->GetSyntaxFeature()->IsQuestion() || IsCategory(m_uiCurrentBitNumber))
										AddSyntaxIndex(m_uiCurrentBitNumber);
								}while(GetNextCategoryValue());
							}

						}
					}while(GetNextUnit());
				}

				return true;
			}
			SS_CATCH(L"")
		}
		UINT CSMSyntaxCategoriesPullDown::GetFirstCategoryValue()
		{
			m_uiCurrentBitNumber=0;
			UINT uiMask=0,uiResult=0;
			while(uiResult==0 && m_uiCurrentBitNumber++<31)
			{
				uiMask=1<<m_uiCurrentBitNumber;
				uiResult=m_uiValue&uiMask;
			}
			return uiResult;
		}
		UINT CSMSyntaxCategoriesPullDown::GetNextCategoryValue()
		{
			UINT uiMask=0,uiResult=0;
			while(uiResult==0 && m_uiCurrentBitNumber++<31)
			{
				uiMask=1<<m_uiCurrentBitNumber;
				uiResult=m_uiValue&uiMask;
			}
			return uiResult;
		}

		bool CSMSyntaxCategoriesPullDown::IsCategory(UINT uiSingleValue)
		{
			Types::SyntaxCategories oCaregory;
			Types::SyntaxCategories oSyntaxCategories=Values::SyntaxCategories.scAdditionalEntity;
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunct);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceSource);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceFixedPoint);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceEnd);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeSource);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeFixedPoint);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeEnd);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeDuration);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scPredicate);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scMainEntity);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scWhoDefinition);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scWhatDefinition);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scNameDefinition);

			oCaregory.SetSingleValue(uiSingleValue);

			return oSyntaxCategories.IsIntersectsWith(&oCaregory);
		}

//////////////////////////////////////////////////////////////////////////
			}
		}
	}
}