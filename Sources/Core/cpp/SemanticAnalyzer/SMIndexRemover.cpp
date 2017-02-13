#include "StdAfx.h"
#include ".\smindexremover.h"
#include "..\assinterface\TDictionaryNames.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace PostSyntaxAnalysis
			{
			
		
		CSMIndexRemover::CSMIndexRemover(void) : m_uiObject(0), m_uiSubject(0)
		{
			m_bInit = false;
			m_AnalyseLanguage=SS::Core::Types::ealEnglish;
			m_sDescription=_T(__FUNCTION__);
		}

		CSMIndexRemover::~CSMIndexRemover(void)
		{
		}

		bool CSMIndexRemover::AnalizeSentence()
		{
			SS_TRY
			{
				IUnit * pUnit;
				//const TCHAR * str;
				SS::Core::Features::CSyntaxFeatureAposteriori * oSyntaxFeatureAposteriori;
				Types::SyntaxCategories oSORemoveCategory=Values::SyntaxCategories.scPredicate;
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunct);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scWhoDefinition);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scWhatDefinition);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctPlaceEnd);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctPlaceFixedPoint);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctPlaceSource);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctTimeDuration);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctTimeEnd);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctTimeFixedPoint);
				oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctTimeSource);

				if (!GetFirstUnit()) return true;;
				do{
					if (m_pCurrentUnit->GetSyntaxFeature()  )
					{
						oSyntaxFeatureAposteriori=m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori();

						if(oSyntaxFeatureAposteriori->m_SyntaxCategories.IsIntersectsWith(&oSORemoveCategory))
						{
							RemoveSubjectObjectIndices(m_pCurrentUnit);
							if(m_pCurrentUnit->HasChilds() && (pUnit=m_pCurrentUnit->GetLeftChildUnit()))
							{
								do{
									RemoveSubjectObjectIndices(pUnit);										
								}while(pUnit=pUnit->GetRightUnit());
							}
						}
					}
					if (!(m_pCurrentUnit->GetSyntaxFeature() && m_pCurrentUnit->GetSyntaxFeature()->IsQuestion()))
					{
						if(!m_pCurrentUnit->HasChilds()) RemoveSemanticIndices(m_pCurrentUnit);
						else RemoveSemanticFromUnits(m_pCurrentUnit);
						RemoveSyntIndexFromUnits(m_pCurrentUnit,Values::SyntaxCategories.scPrepositionObject);
					}
				}while(GetNextUnit());

				return true;
			}
			SS_CATCH(L"")
		}

		bool CSMIndexRemover::RemoveSemanticFromUnits(IUnit * pGroup)
		{
			if(!pGroup->HasChilds())
				return false;

			RemoveSemanticIndices(pGroup);

			IUnit * pUnit=pGroup->GetLeftChildUnit();
			do
			{
				//wstring str = pUnit->GetWord();
				if (!RemoveSemanticFromUnits(pUnit))
					RemoveSemanticIndices(pUnit);
				else
					RemoveSemanticIndices(pUnit);
			}
			while( pUnit = pUnit->GetRightUnit() );

			return true;
		}

		bool CSMIndexRemover::RemoveSyntIndexFromUnits(IUnit * pGroup, const Types::SyntaxCategories & oCategory)
		{
			if(!pGroup->HasChilds())
				return false;

			RemoveSyntaxCategoryIndex(oCategory,pGroup);
			DeleteSyntaxCategory(pGroup,oCategory);

			IUnit * pUnit=pGroup->GetLeftChildUnit();
			do
			{
				DeleteSyntaxCategory(pUnit,oCategory);
				if (!RemoveSyntIndexFromUnits(pUnit,oCategory))
					RemoveSyntaxCategoryIndex(oCategory,pUnit);
				else
					RemoveSyntaxCategoryIndex(oCategory,pUnit);
			}
			while( pUnit = pUnit->GetRightUnit() );

			return true;
		}

		bool CSMIndexRemover::Init(IText * pIText)
		{
			if (!CSMExpert::Init(pIText)) return false;

			if(!m_bInit)
			{
				m_bInit = true;

				m_uiSubject=Values::SyntaxCategories.scSubjectFin.GetSingleValue() + (SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24);
				m_uiObject=Values::SyntaxCategories.scObject.GetSingleValue() + (SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24);
			}

			return true;
		}

		void CSMIndexRemover::RemoveSubjectObjectIndices(IUnit * pUnit)
		{
			IIndex * pIndex;
			UINT uiIndex;
			if (pIndex=pUnit->GetFirstIndex())
			{
				do{
					uiIndex=pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex();
					if (uiIndex==m_uiObject || uiIndex==m_uiSubject )
					{
						pIndex=pIndex->ReleaseIndex();
					}else pIndex=pUnit->GetNextIndex();
				}while(pIndex);
			}
		}
		void CSMIndexRemover::RemoveSemanticIndices(IUnit * pUnit)
		{
			IIndex * pIndex;
			if (pIndex=pUnit->GetFirstIndex())
			{
				do{
					/*uiIndex=pIndex->GetDictionaryIndex()->GetFirst();
					if ( (uiIndex>>24) == SS::Dictionary::DATA_TYPE::NAMES::ednSemantic )*/
					if((pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() == SS::Interface::Core::BlackBox::eitSemanticIndex) ||
						!(pIndex->GetDictionaryIndex()))
					{
						pIndex=pIndex->ReleaseIndex();
					}else pIndex=pUnit->GetNextIndex();
				}while(pIndex);
			}
		}
}
}
}
}