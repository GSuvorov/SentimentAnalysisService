#include "StdAfx.h"
#include ".\smtreetransformation.h"

namespace SS
{
namespace Semantic
{
namespace PostAnalysisExperts
{
	namespace PostSyntaxAnalysis
	{		

//const float WeightArray[] = {1,0.5,0.3,0.2,0.1};
CSMTreeTransformation::CSMTreeTransformation(void) :
	m_uiRrecursionDepth(0xffffffff),/*m_PositionOfHead(greater),m_RemoveAll(0),m_CombineLevAndPos(1),*/m_uiLevel(1),
	m_fWeight(0.0), m_bIsFirstLastName(false)
{
	m_bInit = false;	
	m_sDescription=_T("трансфорация иерархического дерева синтаксического разбора в линейное с учётом уровня иерархии");
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
}

CSMTreeTransformation::~CSMTreeTransformation(void)
{
}

bool CSMTreeTransformation::Init(IText * pIText)
{
	if (!CSMExpert::Init(pIText)) return false;

	if(!m_bInit)
	{
		m_bInit = true;

		if (m_Words.empty())
		{
			m_Words.insert(wstring(_T("what")));
			m_Words.insert(wstring(_T("where")));
			m_Words.insert(wstring(_T("how")));
			m_Words.insert(wstring(_T("when")));
			m_Words.insert(wstring(_T("why")));
			m_Words.insert(wstring(_T("which")));
			m_Words.insert(wstring(_T("who")));
		}
		if (m_OtherWords.empty())
		{
			m_OtherWords.insert(wstring(_T("usually")));
			m_OtherWords.insert(wstring(_T("currently")));
			m_OtherWords.insert(wstring(_T("other")));
		}
	}

	return true;
}

bool IUnitPtrLess(IUnit * pLeft,IUnit * pRight)
{
	SS_TRY
	{
		if(!pLeft || !pRight) return false;

		IUnit * pChildLeft;
		IUnit * pChildRight;

		pChildLeft=pLeft->GetLeftChildUnit();
		pChildRight=pRight->GetLeftChildUnit();

		if (pChildLeft && pChildRight)
			return ((IUnit*)pChildLeft)->GetPosition()<((IUnit*)pChildRight)->GetPosition();
		else if(pChildLeft && !pChildRight)
			return ((IUnit*)pChildLeft)->GetPosition()<((IUnit*)pRight)->GetPosition();
		else if(!pChildLeft && pChildRight)
			return ((IUnit*)pLeft)->GetPosition()<((IUnit*)pChildRight)->GetPosition();
		else
			return ((IUnit*)pLeft)->GetPosition()<((IUnit*)pRight)->GetPosition();
	}
	SS_CATCH(L"")
}

bool CSMTreeTransformation::AnalizeSentence()
{
	SS_TRY
	{

		list<IUnit*> l_UnitList, l_UnitSentenceList;
		list<IUnit*>::iterator it;
		IUnit * pUnit;
		Types::SyntaxCategories oSyntaxCategoriesUp;

		if (!GetFirstUnit()) return true;
				do{
					l_UnitList.clear();
					pUnit=m_pCurrentUnit;
					m_pCurrentUnit=pUnit->Pop();
					if(pUnit->GetSyntaxFeature())
					{
						ProcessUnit(pUnit,l_UnitList,pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories,1);
						oSyntaxCategoriesUp=pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories;
					}else
					{
						SAVE_LOG(SS_ERROR AND pUnit->GetWord() AND __WFUNCTION__);
					}


					if (l_UnitList.empty()==false)
					{
						l_UnitList.sort(IUnitPtrLess);
						pUnit=m_pIBlackBoxFactory->CreateUnit();
						pUnit->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories=oSyntaxCategoriesUp;
						for(it=l_UnitList.begin(); it!=l_UnitList.end(); it++)
						{
							pUnit->AddUnitVertical(*it);
						}
						l_UnitSentenceList.push_back(pUnit);
						l_UnitList.clear();
					}

				}while(m_pCurrentUnit);
				for (it=l_UnitSentenceList.begin(); it!=l_UnitSentenceList.end(); it++) 
				{
					m_pCurrentSentence->AddUnit(*it);
				}
				l_UnitSentenceList.clear();

		return true;
	}
	SS_CATCH(L"")
	
}

bool CSMTreeTransformation::ProcessUnit(IUnit * pUnit, list<IUnit*> & l_Units, const Types::SyntaxCategories & oSyntaxCategories, UINT uiLevel)
{
	SS_TRY
	{
		if(uiLevel>m_uiRrecursionDepth) return true;

		l_Units.push_back(pUnit);
		if(pUnit->GetSyntaxFeature())
		{
			pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.AssignOR(&oSyntaxCategories);
		}
		//wstring str = pUnit->GetWord();

		if(!FillUnitsInQuotationsList(pUnit))
		{
			SetWeightFirstLastName(pUnit);
			pUnit->SetWeight(m_fWeight/*WeightArray[uiLevel-1]*/);
		}
		
		IUnit * pChild,*pChild2;
		if (!(pChild=pUnit->GetLeftChildUnit())) return true;

		do{
			pChild2=pChild;
			if(pChild->GetSyntaxFlag() && pUnit->GetSyntaxFeature() )
			{	
				pChild=pChild2->Pop();
				if(!IsContainRejectUnimportant(pChild2,/*pUnit,*/uiLevel+1))
				{
					ProcessUnit(pChild2,l_Units,pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories,uiLevel+1);
				}
			}else pChild=pChild->GetRightUnit();
		}while(pChild);

		return true;
	}
	SS_CATCH(L"")
}

bool CSMTreeTransformation::IsContainRejectUnimportant(IUnit* pChild, /*IUnit * pHead ,*/UINT uiLevel)
{
	SS_TRY
	{
		if (/*!pHead ||*/ !pChild) return false;

		if(GetAnalyseParams())
		{
			if((GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamRejectUnimportant) && (uiLevel > m_uiLevel)
				&& !DetectQuestWords(pChild))
				return true;
		}else
		{
			SAVE_LOG(SS_ERROR AND L"GetAnalyseParams() == NULL" AND __WFUNCTION__);
		}
	}
	SS_CATCH(L"")
	return false;
}

bool CSMTreeTransformation::DetectQuestWords(IUnit* pUnit)
{
	if (!pUnit) return false;

	if (pUnit->HasChilds())
	{
		IUnit * pChild;
		if (pChild=pUnit->GetLeftChildUnit())
		{
			do{
				if (IsWord(pChild, true)) return true;
			}while(pChild = pChild->GetRightUnit());
		}
	}else
		if (IsWord(pUnit, true)) return true;

	return false;
}

bool CSMTreeTransformation::IsWord(IUnit * pUnit, bool bQWords)
{
	wstring sWord=pUnit->GetWord();
	m_StringService.LowerSz((TCHAR*)sWord.c_str());
	set<wstring>::iterator i;
	if (bQWords)
	{
		i=m_Words.find(sWord.c_str());
		return (i!=m_Words.end());
	}
	else 
	{
		i=m_OtherWords.find(sWord.c_str());
		return (i!=m_OtherWords.end());
	}
}

bool CSMTreeTransformation::IsFirstLastNameSemantic()
{
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	if (!GetFirstUnit()) return false;
	do{
		if (CollectSemanticFeature(oSemanticFeature))
		{
			if (oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName))
			{
				return true;
			}else if (oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName))
			{
				return true;
			}
		}
	}while(GetNextUnitWithChild());

	return false;
}

bool CSMTreeTransformation::FillUnitsInQuotationsList(IUnit* pUnit)
{
	if(!pUnit) return false;

	SS::Core::Features::CSemanticFeature oSemanticFeature;
	//wstring str = pUnit->GetWord();
	if (CollectSemanticFeature(oSemanticFeature,pUnit) && oSemanticFeature.m_SServiceTypes.IsContain(Values::TSServiceTypes.sstAcronymCitation))
	{
		FillUnits(pUnit);
		return true;
	}

	return false;
}

void CSMTreeTransformation::FillUnits(IUnit * pUnit)
{
	if (!pUnit) return;

	IUnit * pUnitLocal = NULL;


	if((pUnit) && (pUnit->HasChilds()))
	{
		pUnit->SetWeight(1);
		pUnitLocal = pUnit->GetLeftChildUnit();
		if(pUnitLocal)
		{
			do{
				//wstring str = pUnitLocal->GetWord();
				FillUnits(pUnitLocal);
			}while(pUnitLocal = pUnitLocal->GetRightUnit());
		}
	}else{
			//wstring str = pUnit->GetWord();
			pUnit->SetWeight(1);
	}

	return;
}

void CSMTreeTransformation::SetWeightFirstLastName(IUnit * pUnit)
{
	if (!pUnit) return;

	if (IsContainFeature(efcSemanticName,pUnit) &&
		!IsSetWeightFirstLastNameChilds(pUnit) && 
		pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Equal(Values::SyntaxCategories.scSubjectFin))
	{
		m_fWeight = 1;
	}else
		SetWeightNoFirstLastName(pUnit);

	return;
}

void CSMTreeTransformation::SetWeightNoFirstLastName(IUnit * pUnit)
{
	if (!pUnit) return;

	//wstring str = pUnit->GetWord();

	bool bUpper=m_LexemType.IsUpperCase(pUnit);
	if(IsWord(pUnit,false))
		m_fWeight = 0;
	else if(IsSetWeightFirstLastNameChilds(pUnit))
		m_fWeight = float(0.5);
	else if(pUnit->GetSyntaxFeature() && 
		pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAttribute))
		m_fWeight = float(0.33);
	else if (bUpper ||
		(pUnit->GetSyntaxFeature() && 
		pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Equal(Values::SyntaxCategories.scSubjectFin)))
	{
		m_fWeight = 1;
	}else
		m_fWeight = float(0.5);

	return;
}

bool CSMTreeTransformation::IsSetWeightFirstLastNameChilds(IUnit * pUnit)
{
	if (!pUnit) return false;

	if(!pUnit->HasChilds()) return false;

	bool bSubjFin = false;

	if(pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Equal(Values::SyntaxCategories.scSubjectFin)
		/*&& !pUnit->GetSyntaxFlag()*/)
		bSubjFin = true;

	SS::Core::Features::CSemanticFeature oSemanticFeature;
	bool bGood=false,bVal=false;
	IUnit * pChild = pUnit->GetLeftChildUnit();
	do{
		if(bSubjFin)
		{
			CollectSemanticFeature(oSemanticFeature,pChild);
			if(oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName) && !bGood)
			{
				pChild->SetWeight(float(0.5));
				bGood = true;
			}else if(oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName) && bGood)
			{
				pChild->SetWeight(float(1));
				bVal = true;
			}else
				pChild->SetWeight(float(0.5));
		}else
			pChild->SetWeight(float(0.5));

	}while(pChild=pChild->GetRightUnit());	
		
	return bVal;
}

//////////////////////////////////////////////////////////////////////
}
}
}
}