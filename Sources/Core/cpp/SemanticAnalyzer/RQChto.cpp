#include "StdAfx.h"
#include ".\rqchto.h"

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


CRQChto::CRQChto(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CRQChto::~CRQChto(void)
{
}
bool CRQChto::AnalyseQuestion(EQuestionType eQType)
{
	IUnit * pQuestion=NULL, * pUnit, * pQWord;
	for(GetFirstUnit(); m_pCurrentUnit ; GetNextUnit())
	{
		if(m_pCurrentUnit->GetSyntaxFeature() &&
			m_pCurrentUnit->GetSyntaxFeature()->IsQuestion())
		{
			pQuestion=m_pCurrentUnit;
			break;
		}
	}
	if(!pQuestion) return false;

	SMorphoDescription * pD;
	
	DeleteSemantic(pQuestion);
	if(m_DInfo._pVerb)
	{
		SetSyntaxCategory(pQuestion,Values::SyntaxCategories.scAdditionalEntity);
		//pHead->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scObject);
		pQWord=NULL;
		if(pQuestion->HasChilds())
		{
			for (pUnit=pQuestion->GetLeftChildUnit(); pUnit ; pUnit=pUnit->GetRightUnit()) 
			{
				pD=GetUnitDescription(pUnit);
				if(pD && 
					pD->_ePOS==eposQuestion
					)
					break;
			}
			if(!pUnit)pQWord=pUnit;
		}else 
		{
			pD=GetUnitDescription(pQuestion);
			pQWord=pQuestion;
		}
		if (pD && pD->_ePOS==eposQuestion)
		{
			if(!pD->_oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctAccusative)&&
				!pD->_oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctNominative)||
				m_DInfo._pPrep)
			{
				AddSyntaxCategory(pQuestion,Values::SyntaxCategories.scPrepositionObject);
				if(m_DInfo._pPrep)
				{
					//SetSingleSyntax(m_DInfo._pPrep);
					RemoveFeature(efcPretext,m_DInfo._pPrep);
					MoveUnitToPredicate(m_DInfo._pPrep,m_DInfo._pVerb);
				}
				return true;
			}
		}

		for(GetFirstUnit(), pUnit=NULL; m_pCurrentUnit ; GetNextUnit())
		{
			if(IsContainSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scMainEntity))
			{
				pUnit=m_pCurrentUnit;
				break;
			}
		}

		if(!pUnit) return false;

		if(pUnit->HasChilds())
		{
			IUnit *pU2 = NULL;
			for (pU2 = pUnit->GetLeftChildUnit(); pU2 ; pU2=pU2->GetRightUnit() ) {
				if(IsContainFeature(efcNoun,pU2))
					break;
			}
			if(!pU2) pUnit = pU2;
			else pUnit=NULL;
		}else if (!IsContainFeature(efcNoun,pUnit))
			pUnit=NULL;
		bool bGood=false;
		if(pUnit)
		{
			if(IsMorphologyCoordinated(pUnit,m_DInfo._pVerb,false,true,true))
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pQWord);
				RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pQWord);
				bGood=true;
			}
		}
		if(!bGood)
		{
			if(IsMorphologyCoordinated(m_DInfo._pVerb,pQWord,false,true,true))
			{
				RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pQWord);
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pQWord);
				bGood=true;
			}
		}
		//if(!CheckSubjectFin(l_Entity1.front(),l_Entity1))
		//{
		//	l_QuestionGroup.front()->m_SyntaxInfo.SetSyntaxCategory(Values::SyntaxCategories.scMainEntity);
		//	//l_QuestionGroup.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scSubjectFin);

		//}
	}
	else
	{
		if(m_DInfo._pMainEntity)
		{
			SS::Core::Features::CSemanticFeature oF;
			if (m_DInfo._pMainEntity->HasChilds())
			{
				pUnit=m_DInfo._pMainEntity->GetLeftChildUnit();
				do{

					oF.Undefine();
					CollectSemanticFeature(oF,pUnit);

					if ((IsContainFeature(efcSemanticCommon,pUnit)||
						IsContainFeature(efEDNounNominative,pUnit)))
					{
						AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhatDefinition,pUnit);
						AddSyntaxCategory(pUnit,Values::SyntaxCategories.scWhatDefinition);
						break;
					}
				}while(pUnit=pUnit->GetRightUnit());
			}else
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhatDefinition,m_DInfo._pMainEntity);
				AddSyntaxCategory(m_DInfo._pMainEntity,Values::SyntaxCategories.scWhatDefinition);
			}
		}


		SetSyntaxCategory(pQuestion,Values::SyntaxCategories.scWhatDefinition);
		/*RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
		RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject);*/
		RemoveSubjectObjectIndicesFromUnit();
		SetDefQuestion();
		//pHead=l_Entity1.front();
		//pHead->m_SyntaxInfo.SetSyntaxCategory(Values::SyntaxCategories.scMainEntity);		
	}

}

void CRQChto::RemoveSubjectObjectIndicesFromUnit()
{
	if (!m_pCurrentUnit) return;

	IUnit * pUnit = NULL;
	RemoveSubjectObjectIndices(m_pCurrentUnit);
	if(m_pCurrentUnit->HasChilds() && (pUnit=m_pCurrentUnit->GetLeftChildUnit()))
	{
		do{
			RemoveSubjectObjectIndices(pUnit);										
		}while(pUnit=pUnit->GetRightUnit());
	}
	return;
}

void CRQChto::RemoveSubjectObjectIndices(IUnit * pUnit)
{
	if(!pUnit) return;

	RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin, pUnit);
	RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject, pUnit);
	return;
}

void CRQChto::DeleteSemantic(IUnit * pUnit)
{
	if(!pUnit) return ;
	list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
	RemoveSemanticIndicesFromUnit(l_Sem,pUnit);
	if(!pUnit->HasChilds()) return;
	for(IUnit * pU=pUnit->GetLeftChildUnit(); pU ; pU=pU->GetRightUnit())
		DeleteSemantic(pU);
}


				}
			}
		}
	}
}