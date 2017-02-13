#include "StdAfx.h"
#include ".\rqkto.h"

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



CRQKto::CRQKto(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CRQKto::~CRQKto(void)
{
}

bool CRQKto::AnalyseQuestion(EQuestionType eQType)
{
	IUnit  *pUnit, * pNoun=NULL;

	SMorphoDescription * pD;

	if (m_DInfo._pVerb)
	{
		SetSyntaxCategory(m_DInfo._pQuestion,Values::SyntaxCategories.scMainEntity);
		AddAllPersonSemantic(m_DInfo._pQuestion);

		IUnit * pQWord=NULL;
		if(m_DInfo._pQuestion->HasChilds())
		{
			for (pUnit=m_DInfo._pQuestion->GetLeftChildUnit(); pUnit ; pUnit=pUnit->GetRightUnit()) 
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
			pD=GetUnitDescription(m_DInfo._pQuestion);
			pQWord=m_DInfo._pQuestion;
		}
		if (pD && pD->_ePOS==eposQuestion)
		{
			if(!pD->_oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctAccusative)&&
				!pD->_oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctNominative)||
				m_DInfo._pPrep)
			{
				AddSyntaxCategory(m_DInfo._pQuestion,Values::SyntaxCategories.scPrepositionObject);
				if(m_DInfo._pPrep)
				{
					//SetSingleSyntax(m_DInfo._pPrep);
					RemoveFeature(efcPretext,m_DInfo._pPrep);
					MoveUnitToPredicate(m_DInfo._pPrep,m_DInfo._pVerb);
				}
				return true;
			}
		}


		if (m_DInfo._pMainEntity)
		{
			SetSyntaxCategory(m_DInfo._pMainEntity,Values::SyntaxCategories.scAdditionalEntity);
		}
	}else
	{
		SetDefQuestion();
		if (m_DInfo._pMainEntity)
		{
			SetSyntaxCategory(m_DInfo._pMainEntity,Values::SyntaxCategories.scMainEntity);
			CheckSubjectFin(m_DInfo._pMainEntity);
		}
		bool bName=false;
		if(CheckWhoCategory(m_DInfo._pQuestion,m_DInfo._pMainEntity,bName))
		{
			if(m_DInfo._pQuestion->HasChilds()) RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,m_DInfo._pQuestion->GetLeftChildUnit());
			else RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,m_DInfo._pQuestion);
			if(m_DInfo._pMainEntity->HasChilds())
			{
				for (pUnit=m_DInfo._pMainEntity->GetLeftChildUnit();pUnit;pUnit=pUnit->GetParentUnit()) 
				{
					if(IsContainFeature(efcNoun,pUnit))
					{
						if(!bName)
						{
							SetSyntaxCategory(pUnit,Values::SyntaxCategories.scNameDefinition);
							AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition,pUnit);//(*i)->m_MorphoInfo.m_SynonimsCollection.insert(Values::SyntaxCategories.scNameDefinition.GetSingleValue()+(SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24));
						}
						else
						{
							SetSyntaxCategory(pUnit,Values::SyntaxCategories.scWhoDefinition);
							AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition,pUnit);//(*i)->m_MorphoInfo.m_SynonimsCollection.insert(Values::SyntaxCategories.scWhoDefinition.GetSingleValue()+(SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24));
						}
						RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,m_DInfo._pQuestion);
						RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,m_DInfo._pQuestion);
						break;
					}
				}

			} else 
			{
				pUnit=m_DInfo._pMainEntity;
				if(IsContainFeature(efcNoun,pUnit))
				{
					if(!bName)AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition,pUnit);//(*i)->m_MorphoInfo.m_SynonimsCollection.insert(Values::SyntaxCategories.scNameDefinition.GetSingleValue()+(SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24));
					else AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition,pUnit);//(*i)->m_MorphoInfo.m_SynonimsCollection.insert(Values::SyntaxCategories.scWhoDefinition.GetSingleValue()+(SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24));
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,m_DInfo._pQuestion);
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,m_DInfo._pQuestion);
				}

			}
			if(m_DInfo._pMainEntity->HasChilds()) RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,m_DInfo._pMainEntity->GetLeftChildUnit());
			else RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,m_DInfo._pMainEntity);
			//!!l_Entity1.front()->m_MorphoInfo.m_SynonimsCollection.erase(Values::SyntaxCategories.scSubjectFin.GetSingleValue()+(SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24));
		}
		//		pHead->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
	}

	return true;

}


				}
			}
		}
	}
}