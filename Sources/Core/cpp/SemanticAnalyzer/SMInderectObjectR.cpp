#include "StdAfx.h"
#include ".\sminderectobjectr.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{


CSMInderectObjectR::CSMInderectObjectR(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealRussian;
	m_sDescription=_T(__FUNCTION__);
}

CSMInderectObjectR::~CSMInderectObjectR(void)
{
}
bool CSMInderectObjectR::AnalizeSentence()
{
	SS_TRY
	{
		IUnit * pPrep;
		const UINT CaseMask= efRCaseAccusative|efRCaseGenetive|efRCaseInstrumental|efRCaseNominative|efRCaseLocative|efRCaseDative ;
		UINT eFeature ;
		do{
			if ( !!IsSyntax()) 
				continue;
			if ( IsContainFeature(efcNoun) /*&& SpecialTypeEquals(ustUndefined)*/)
			{
				pPrep=IsPrepositionBeforeR(4,m_pCurrentUnit);
				if(pPrep)
				{
					eFeature = pPrep->GetInfo()->_uiFeature & CaseMask  ;
					if( IsContainFeature(eFeature) )
					{
						//AddFeature(efRDNounPrep);
						//AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject);
						AddSyntaxCategoryIndex(Values::SyntaxCategories.scPrepositionObject);
						AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunct,pPrep);
					}
				}else if (IsContainFeature(efRCaseGenetive)|| IsContainFeature(efRCaseDative)|| IsContainFeature(efRCaseInstrumental))
				{
					/*Mem();
					eFeature=m_pCurrentUnit->GetInfo()->_uiFeature & CaseMask;
					while ( GetPrevUnit()  )
					{
						if(IsContainFeature(efcVerb) && m_pCurrentUnit->GetSyntaxFeature())
						{
							if((eFeature&efRCaseDative) && 
								//m_pCurrentUnit->m_MorphoInfo.m_SyntaxApriori.m_TypeOfEndingObject.IsContain(Values::TypeOfEndingObject.toeoCaseDat)||
								m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureApriori()->m_TypeOfEndingObject.IsContain(Values::TypeOfEndingObject.toeoCaseDat)||
								(eFeature&efRCaseGenetive) && 
								m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureApriori()->m_TypeOfEndingObject.IsContain(Values::TypeOfEndingObject.toeoCaseGen)||
								(eFeature&efRCaseInstrumental) && 
								m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureApriori()->m_TypeOfEndingObject.IsContain(Values::TypeOfEndingObject.toeoCaseInstr)
								)
							{
								GotoMem();
								//m_pCurrentUnit->m_MorphoInfo.m_SynonimsCollection.insert(Values::SyntaxCategories.scObject.GetSingleValue()+(SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24));
								//AddSyntaxCategoryIndex(Values::SyntaxCategories.scPrepositionObject);
								AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectInfin);
								AddFeature(efcDetectedNoun);
								break;
							}
						}
					}
					GotoMem();*/
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectInfin);
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