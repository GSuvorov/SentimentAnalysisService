#include "StdAfx.h"
#include ".\rqkakoi.h"

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


CRQKakoi::CRQKakoi(void)
{
}

CRQKakoi::~CRQKakoi(void)
{
}
bool CRQKakoi::AnalyseQuestion(EQuestionType eQType)
{
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	Types::SyntaxCategories oSyntaxCategories;
	list<UINT> l_Categories;
	list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;

	IUnit  *pUnit, * pNoun=NULL ;
	TBoxUnits l_NounGroup;


	if (m_DInfo._pMainEntity)
	{
		if(m_DInfo._pMainEntity->HasChilds())
		{
			pNoun=m_DInfo._pMainEntity->GetLeftChildUnit();
			while(pNoun )
			{
				//l_QuestionGroup.push_back(pNoun);
				pUnit=pNoun->Pop();
				if(m_DInfo._pQuestion->HasChilds() && (IsContainFeature(efcNoun,pNoun)||
					IsContainFeature(efcSemanticCommon,pNoun))) 
				{
					m_DInfo._pQuestion->GetRightChildUnit()->AddUnitHorizontal(pNoun);
					l_NounGroup.push_back(pNoun);
					break;
				}else
				{
					pNoun->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.scAjunct);
					m_pCurrentSentence->AddUnit(pNoun);
				}

				/*if(IsContainFeature(efcNoun,pNoun)||
					IsContainFeature(efcSemanticCommon,pNoun)) break;*/

				if(pUnit)
					pNoun=pUnit;
				else break;
			}
		}else 
		{
			pNoun=m_DInfo._pMainEntity;
		}

		CollectSemanticFeature(oSemanticFeature,pNoun);

		if (m_DInfo._pAdditionalEntity && !m_DInfo._pMainEntity->HasChilds())
		{
			m_DInfo._pMainEntity->ReleaseUnitHorizontal();
			m_DInfo._pMainEntity=m_DInfo._pAdditionalEntity;
			CheckSubjectFin(m_DInfo._pMainEntity);
		}
		if(! m_DInfo._pMainEntity)
			SetSyntaxCategory(m_DInfo._pMainEntity,Values::SyntaxCategories.scMainEntity);
	}else if(m_DInfo._pAdditionalEntity)
	{
		SetSyntaxCategory(m_DInfo._pAdditionalEntity,Values::SyntaxCategories.scMainEntity);
		m_DInfo._pQuestion->GetSyntaxFeature()->SetIsNotQuestion();
		m_DInfo._pQuestion->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->Undefine();
		return true;
	}

	if (m_DInfo._pPrep)
	{
		if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval) &&
			oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure))
		{
			SetSyntaxFiltered(m_DInfo._pQuestion,m_DInfo._pPrep,false);
			SetSyntaxCategory(m_DInfo._pQuestion,Values::SyntaxCategories.scAjunctMeasure);
			RemoveSemanticIndicesFromUnit(l_Sem,pNoun);
			SetSemanticIndices(l_Sem,m_DInfo._pQuestion);
		}
		else if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure))
		{
			SetSyntaxCategory(m_DInfo._pQuestion,Values::SyntaxCategories.scAjunctMeasure);

			RemoveSemanticIndicesFromUnit(l_Sem,pNoun);
			KeepSpecifedSemanticTypes(Values::TSemanticType.smtMeasure,l_Sem);
			SetSemanticIndices(l_Sem,m_DInfo._pQuestion);
		}
		else if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval))
		{
			SetSyntaxFiltered(m_DInfo._pQuestion,m_DInfo._pPrep,false);

			RemoveSemanticIndicesFromUnit(l_Sem,pNoun);
			KeepSpecifedSemanticTypes(Values::TSemanticType.smtTimeInterval,l_Sem);
			SetSemanticIndices(l_Sem,m_DInfo._pQuestion);
		}
		else if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPlace))
		{
			SetSyntaxFiltered(m_DInfo._pQuestion,m_DInfo._pPrep,true);

			RemoveSemanticIndicesFromUnit(l_Sem,pNoun);
			KeepSpecifedSemanticTypes(Values::TSemanticType.smtPlace,l_Sem);
			SetSemanticIndices(l_Sem,m_DInfo._pQuestion);
		}else
		{
			m_DInfo._pQuestion->GetSyntaxFeature()->SetIsNotQuestion();
			m_DInfo._pQuestion->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->Undefine();

			oSyntaxCategories=LegacyCategoryToEntity(pNoun,m_DInfo._pVerb);
			//SetSyntaxInfo(pNoun,l_NounGroup,usntEntity2);
			if (l_NounGroup.empty() == false)
			{
				pUnit=JoinUpUnits(l_NounGroup);
				pUnit->Pop();
				m_DInfo._pQuestion->AddUnitHorizontal(pUnit);
				SetSyntaxCategory(pUnit,oSyntaxCategories);
			}

			if(oSyntaxCategories.Equal(Values::SyntaxCategories.scMainEntity) && m_DInfo._pMainEntity)
			{
				SetSyntaxCategory(m_DInfo._pMainEntity,Values::SyntaxCategories.scAdditionalEntity);
			}
		}
	}else if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure))
	{
		if(oSemanticFeature.m_SMeasureType.GetValue()>1)
		{
			SetSyntaxCategory(m_DInfo._pQuestion,Values::SyntaxCategories.scAjunctMeasure);

			RemoveSemanticIndicesFromUnit(l_Sem,pNoun);
			KeepSpecifedSemanticTypes(Values::TSemanticType.smtMeasure,l_Sem);
			SetSemanticIndices(l_Sem,m_DInfo._pQuestion);

			SetSingleSyntax(pNoun);

			if (m_DInfo._pVerb)
			{
				CollectSemanticFeature(oSemanticFeature,m_DInfo._pVerb);
				if(oSemanticFeature.m_SVerbType.IsContain(Values::TSVerbType.semvtAcquirePossess)||
					oSemanticFeature.m_SVerbType.IsContain(Values::TSVerbType.semvtBe))
				{
					if(m_DInfo._pPredicate)m_DInfo._pPredicate->ReleaseUnitHorizontal();
					else m_DInfo._pVerb->ReleaseUnitHorizontal();
				}
			}
		}else
		{
			AddSemanticIndex((SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24)+m_semNumber.GetDictionaryIndex(),m_DInfo._pQuestion);

			pUnit=JoinUpUnits(l_NounGroup);
			pUnit->Pop();
			m_DInfo._pQuestion->AddUnitHorizontal(pUnit);

			SetSyntaxCategory(pUnit,Values::SyntaxCategories.scAjunct);
		}
	}else if (oSemanticFeature.m_SemanticType.GetValue()<=1||
		oSemanticFeature.m_SPlaceType.GetValue()<=1 && 
		oSemanticFeature.m_SOrganizationType.GetValue()<=1&&
		oSemanticFeature.m_SPersonType.GetValue()<=1
		)
	{
		m_DInfo._pQuestion->GetSyntaxFeature()->SetIsNotQuestion();
		m_DInfo._pQuestion->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->Undefine();

		//SetSyntaxCategory(m_DInfo._pQuestion,Values::SyntaxCategories.scMainEntity);
		//AddSyntaxCategoryIndex(Values::SyntaxCategories.scAttribute,m_DInfo._pQuestion->GetLeftChildUnit());

		oSyntaxCategories=LegacyCategoryToEntity(pNoun,m_DInfo._pVerb);
		if (l_NounGroup.empty()==false)
		{
			pUnit=JoinUpUnits(l_NounGroup);
			pUnit->Pop();
			m_DInfo._pQuestion->AddUnitHorizontal(pUnit);
			SetSyntaxCategory(pUnit,oSyntaxCategories);
		}

		if(oSyntaxCategories.Equal(Values::SyntaxCategories.scMainEntity) && m_DInfo._pMainEntity)
		{
			SetSyntaxCategory(m_DInfo._pMainEntity,Values::SyntaxCategories.scAdditionalEntity);
		}
		m_DInfo._pQuestion->ReleaseUnitHorizontal();

	}else if (oSemanticFeature.m_SemanticType.GetValue()>1&&
		(oSemanticFeature.m_SPlaceType.GetValue()>1 || 
		oSemanticFeature.m_SOrganizationType.GetValue()>1||
		oSemanticFeature.m_SPersonType.GetValue()>1)
		)
	{
		oSyntaxCategories=LegacyCategoryToEntity(pNoun,m_DInfo._pVerb);
		SetSyntaxCategory(m_DInfo._pQuestion,oSyntaxCategories);
		if(oSyntaxCategories.Equal(Values::SyntaxCategories.scMainEntity) && !m_DInfo._pMainEntity)
		{
			SetSyntaxCategory(m_DInfo._pMainEntity,Values::SyntaxCategories.scAdditionalEntity);
		}

		RemoveSyntaxCategoryIndices(l_Categories,pNoun);
		AddSyntaxCategoryIndices(l_Categories,m_DInfo._pQuestion);

		if(oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
			/*oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)||*/
			oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession))
		{
			//l_Sem.clear();
			l_Sem.push_back(m_semName);
			l_Sem.push_back(m_semLastName);
			l_Sem.push_back(m_semFirstName);
			SetSingleSyntax(pNoun);
		}else if(oSemanticFeature.m_SPlaceType.GetValue()>1)
		{
			RemoveSemanticIndicesFromUnit(l_Sem,pNoun);
			KeepSpecifedSemanticTypes(Values::TSemanticType.smtPlace,l_Sem);
		}
		//else SetSingleSyntax(l_QuestionGroup.back());


		SetSemanticIndices(l_Sem,m_DInfo._pQuestion);
	}
	return true;

}


				}
			}
		}
	}
}