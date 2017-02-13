#include "StdAfx.h"
#include ".\rqskolko.h"

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


CRQSkolko::CRQSkolko(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CRQSkolko::~CRQSkolko(void)
{
}
bool CRQSkolko::AnalyseQuestion(EQuestionType eQType)
{
	IUnit * pQuestion=NULL, *pMain=NULL,*pAdd=NULL, /*pUnit,*/ * pNoun=NULL , *pPredicate=NULL;

	SS::Core::Features::CSemanticFeature oSemanticFeature;
	list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;

	for (GetFirstUnit(); m_pCurrentUnit ; GetNextUnit()) 
	{
		if(m_pCurrentUnit->GetSyntaxFeature())
		{
			if(m_pCurrentUnit->GetSyntaxFeature()->IsQuestion())
				pQuestion=m_pCurrentUnit;
			else if (IsContainSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scMainEntity))
				pMain=m_pCurrentUnit;
			else if (IsContainSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAdditionalEntity))
				pAdd=m_pCurrentUnit;
			else if (IsContainSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scPredicate))
				pAdd=m_pCurrentUnit;
		}else if(!m_pCurrentUnit->HasChilds())
		{
			SetSingleSyntax(m_pCurrentUnit);
			GetPrevUnit();
		}
	}

	if(pMain && pMain->HasChilds()) 
		pNoun=pMain->GetLeftChildUnit();
	else pNoun=pMain;

	if (m_DInfo._pVerb)
	{
		if(ProcessVerbSemantic(m_DInfo._pVerb,oSemanticFeature,l_Sem))
		{
			SetSemanticIndices(l_Sem,pQuestion);
			SetSyntaxCategory(pQuestion,Values::SyntaxCategories.scAjunctMeasure);
		}else
		{
			oSemanticFeature.Undefine();
			if(CollectSemanticFeature(oSemanticFeature,pNoun))
			{
				RemoveSemanticIndicesFromUnit(l_Sem,pNoun);
				if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval))
				{
					//KeepSpecifedSemanticTypes(Values::TSemanticType.smtTimeInterval,l_Sem);
					//SetSemanticIndices(l_Sem,pQuestion);
					SetSyntaxCategory(pQuestion,Values::SyntaxCategories.scAjunctTimeDuration);
					AddAllTimeIntervalSemantic(pQuestion);

					//l_Entity1.front()->m_SyntaxUnitType=usntUndefined;
					if(pMain->HasChilds()) 
					{
						//pMain->GetLeftChildUnit()->ReleaseUnitHorizontal();
						SetSingleSyntax(pMain->GetLeftChildUnit()); //Сколько длится период акклиматизации в Египте?
						if(!pMain->HasChilds())
						{
							pMain->ReleaseUnitHorizontal();
							pMain=NULL;

						}
					}
					else {
						pMain->ReleaseUnitHorizontal();
						pMain=NULL;
					}
					//SetSingleSyntax(pNoun);
					//l_Entity1.pop_front();
					if (pAdd && !pMain )
					{
						SetSyntaxCategory(pAdd,Values::SyntaxCategories.scMainEntity);
						//l_Entity1.front()->m_SyntaxInfo.SetSyntaxCategory(Values::SyntaxCategories.scAdditionalEntity);
						//l_Entity1.front()->m_SyntaxInfo.SetSyntaxCategory(Values::SyntaxCategories.scMainEntity);
						//l_Entity1.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scSubjectFin);
					}
					if(pMain)SetSyntaxCategory(pMain,Values::SyntaxCategories.scMainEntity);
				}else
				{
					//pNoun->m_SyntaxInfo.SetSyntaxCategory(Values::SyntaxCategories.scAjunct);
					SetSingleSyntax(pNoun);
					//pNoun->m_MorphoInfo.m_SynonimsCollection.erase(Values::SyntaxCategories.scSubjectFin.GetSingleValue()+(SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24));
					SetSyntaxCategory(pQuestion,Values::SyntaxCategories.scMainEntity);
					//pHead->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scSubjectFin);
					AddSemanticIndex(m_semNumber.GetDictionaryIndex()+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24),pQuestion);
				}

			}else
			{
				SetSingleSyntax(pNoun);
				SetSyntaxCategory(pQuestion,Values::SyntaxCategories.scMainEntity);
				//pHead->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scSubjectFin);
				AddSemanticIndex(m_semNumber.GetDictionaryIndex()+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24),pQuestion);
			}

		}

	}else
	{
		SetSyntaxCategory(pQuestion,Values::SyntaxCategories.scAjunct);
		AddSemanticIndex(m_semNumber.GetDictionaryIndex()+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24),pQuestion);
	}
	return true;

}


				}
			}
		}
	}
}