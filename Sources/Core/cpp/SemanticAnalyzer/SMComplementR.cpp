#include "StdAfx.h"
#include ".\smcomplementr.h"
#include "../ASSInterface/TDictionaryNames.h"
namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{
CSMComplementR::CSMComplementR(void)
{
	m_bInit = false;
	m_AnalyseLanguage=SS::Core::Types::ealRussian;
	m_sDescription=_T(__FUNCTION__);
}

CSMComplementR::~CSMComplementR(void)
{
}

bool CSMComplementR::AnalizeSentence()
{
	SS_TRY
	{
		if(m_AnalyseLanguage!=m_pCurrentSentence->GetLanguage()) return true;

		bool bGood;

		do{
			if (IsContainFeature(efcRepresentBecome))
			{
				DetectVerbDefinitionConstructions();
			}else if (IsContainFeature(efcSemanticName) && !IsSyntax() && !IsQuestionMode())
			{
				bGood=false;
				Mem();
				if (BeforePersonSimple(m_pCurrentUnit) )
				{
					bGood=true;
				}else{
					GotoMem();
					if ( !(bGood=AfterPersonInComma(m_pCurrentUnit))) 
					{
						GotoMem();
						bGood=BeforePersonInComma(m_pCurrentUnit);
					}
				}
				if (bGood)
				{
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition);
				}
			}else if(m_LexemType.IsSymbol())
			{
				bGood=false;
				if (m_LexemType.SymbolEqualsTo(_T("-")))
					bGood=true;
				else
				{
					Mem();
					GetNextUnit();
					if (m_LexemType.WordEqualsTo(_T("это")))
					{
						bGood=true;	
					}
					GotoMem();
				}
				if (bGood)
				{
					//AddSemanticIndex(m_semRepresent+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24));
					DetectVerbDefinitionConstructions(true);
				}
			}	
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}
bool CSMComplementR::Init(IText * pIText)
{
	if (!CSMSentenceExpert::Init(pIText)) return false;

	if(!m_bInit)
	{
		m_bInit = true;

		SS::Core::Features::CSemanticFeature oF;
		oF.m_SemanticType=Values::TSemanticType.smtVerb;
		oF.m_SVerbType=Values::TSVerbType.semvtBe;
		oF.m_SVBeType=Values::TSVBeType.tsvbtRepresent;
		m_pIAMCSemantic->Code(&oF,&m_semRepresent);

		/*oF.m_SemanticType=Values::TSemanticType.smtPerson;
		oF.m_SPersonType=Values::TSPersonType.semprstName;
		m_pIAMCSemantic->Code(&oF,&m_semName);*/
	}

	return true;

}

void CSMComplementR::DetectVerbDefinitionConstructions(bool bTire)
{
	TBoxUnits l_WhoDef;
	SS::Core::Features::CSemanticFeature  oSemanticFeature;
	Mem();
	if(!PersonBefore(l_WhoDef,oSemanticFeature))
	{
		GotoMem();
		return;
	}
	GotoMem();
	ProcessToBeCompliment(oSemanticFeature,l_WhoDef,!bTire);
}

bool CSMComplementR::ProcessToBeCompliment(SS::Core::Features::CSemanticFeature & oSemanticFeatureFirst,TBoxUnits & l_UnitsFirst,bool bVerbRB)
{
	IUnit* pMem = m_pCurrentUnit;
	SS::Core::Features::CMorphoFeature morphoFeature;
	SS::Core::Features::CSemanticFeature oSemanticFeature2;
	UINT uiAdverbCount=0,uiWordsCount=0,uiDet=0;
	bool bGood=false, bRusETO=false;
	list<long> l_SemValues;
	TBoxUnits  l_Second;

	if(GetNextUnit())
	{
		if(m_LexemType.WordEqualsTo(_T("это"))) bRusETO=true;
		GetPrevUnit();
	}

	while ( GetNextUnit() )
	{
		if(	! (
			/*(SpecialTypeEquals(ustUndefined) || 
			SpecialTypeEquals(ustComplexProperName) && IsContainSemanticIndex(m_semName)||
			SpecialTypeEquals(ustCitation)||
			SpecialTypeEquals(ustNumeral)||
			SpecialTypeEquals(ustPlaceSpecification))&&*/ (uiWordsCount++<4 || bRusETO)&&
			(!IsSyntax())&& 
			(!m_LexemType.IsSComma()|| bRusETO )&& !m_LexemType.IsSDot() && !m_LexemType.IsSTire() && !IsContainFeature(efcPretext)
			)
			)
		{
			if(bRusETO) continue;
			else break;
		}
		//if (SpecialTypeEquals(ustComplexProperName) && IsContainSemanticIndex(m_semName))
		//{
		//	if(!IsCoordinatedCaseGenderNumber(l_UnitsFirst.back(),m_pCurrentUnit,true,bVerbRB) ) continue;
		//	oSemanticFeature2.m_SemanticType=Values::TSemanticType.smtPerson;
		//	oSemanticFeature2.m_SPersonType=Values::TSPersonType.semprstName;
		//	//l_Second.assign(m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.begin(),
		//	//	m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.end());
		//	l_Second.push_back(m_pCurrentUnit);
		//	break;

		/*/}else*/ if (IsContainFeature(efcSemanticCommon)&&
			( IsContainFeature(efRCaseNominative) || IsContainFeature(efRCaseInstrumental) )
			)
		{
			//m_pMorphoService->CollectSemanticFeature(m_pIAMCSemantic,m_pCurrentUnit->m_MorphoInfo,oSemanticFeature2,l_SemValues);
			CollectSemanticFeature(oSemanticFeature2);
			l_Second.push_back(m_pCurrentUnit);
			if (oSemanticFeature2.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure)){
				oSemanticFeature2.Undefine();
				continue;
			}
			if( !IsCoordinatedCaseGenderNumber(l_UnitsFirst.back(),m_pCurrentUnit,false,bVerbRB) ) continue;
			l_Second.push_back(m_pCurrentUnit);
			break;		
		//}else if (SpecialTypeEquals(ustCitation))
		//{
		//	//l_Second.assign(m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.begin(),
		//	//	m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.end());
		//	l_Second.push_back(m_pCurrentUnit);
		//	oSemanticFeature2.m_SemanticType=Values::TSemanticType.smtService;
		//	oSemanticFeature2.m_SServiceTypes=Values::TSServiceTypes.sstAcronymCitation;
		//	break;
		}
		else
		{
			morphoFeature.Undefine();
			//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,morphoFeature);
			CollectMorphoFeature(morphoFeature);
			if (morphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) &&
				morphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent) ||
				morphoFeature.m_OfficialType.IsContain(Values::OfficialType.otConjunction)
				)
			{
				break;
			}
		}
	}
	if (l_Second.empty()==false)
	{
		if (
			(oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstName)||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName))&&
			( IsContainFeature(efRAnimate)&&IsCoordinatedCaseGenderNumber(l_UnitsFirst.back(),m_pCurrentUnit,true,bVerbRB))
			)
		{
			if(oSemanticFeature2.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
			{
				//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);
				//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
				AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scNameDefinition);
				AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
			} else 
			{
				AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhatDefinition);
				AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhatDefinition);
				//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
				//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
			}
		}
		if (
			(oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)||
			IsContainFeature(efRAnimate,l_UnitsFirst.front()))&&
			(  IsContainFeature(efRAnimate))
			)
		{
			AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhoDefinition);
			if( !oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstName) )
				AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
			else AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scNameDefinition);
			//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
			//if( !oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstName) )l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
			//else l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);
		}
		if( (!oSemanticFeatureFirst.m_SemanticType.IsContain(Values::TSemanticType.smtPerson)||
			oSemanticFeatureFirst.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)||
			oSemanticFeatureFirst.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization))
			&&(!IsContainFeature(efRAnimate,l_UnitsFirst.front())|| 
			oSemanticFeature2.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)||
			oSemanticFeature2.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization))
			)
		{
			AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhatDefinition);
			AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhatDefinition);
			//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
			//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
		}
		if (oSemanticFeatureFirst.m_SemanticType.GetValue()<=1 && 
			(!IsContainFeature(efRAnimate,l_UnitsFirst.front()))
			//				&&(l_UnitsFirst.back()->m_UnitType==utEng ||  l_UnitsFirst.back()->m_UnitType==utRus )
			)
		{
			if ((oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
				oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
				oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality))&&
				( IsContainFeature(efRAnimate)&&IsCoordinatedCaseGenderNumber(l_UnitsFirst.back(),m_pCurrentUnit,true,bVerbRB))
				)
			{
				AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scNameDefinition);
				AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
				//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);
				//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
			}
			if ((oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstName)||
				oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
				oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)||
				oSemanticFeature2.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName))&&
				(IsContainFeature(efRAnimate))
				)
			{
				AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scNameDefinition);
				AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
				//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);
				//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
			}
			if (oSemanticFeature2.m_SServiceTypes.IsContain(Values::TSServiceTypes.sstAcronymCitation))
			{
				AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhatDefinition);
				AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhatDefinition);
				//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
				//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
			}

		}
	}

	uiWordsCount=0;
	GotoMem();
	while ( GetNextUnit() )
	{
		if( !(/*SpecialTypeEquals(ustUndefined) &&*/ (uiAdverbCount < 3 || bRusETO) && (uiWordsCount++ < 3 || bRusETO ) &&
			(!IsSyntax())&& 
			(!m_LexemType.IsSComma()|| bRusETO) && !m_LexemType.IsSDot() && !m_LexemType.IsSTire())
		  )
		{
			if (bRusETO) continue;
			else break;
		}
		morphoFeature.Undefine();
		//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,morphoFeature);
		CollectMorphoFeature(morphoFeature);

		if ((morphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)||
			 morphoFeature.m_OfficialType.IsContain(Values::OfficialType.otConjunction)) && !bRusETO
		   ) 
		{
			break;
		}
		else if (morphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple)&& 
			     morphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent) 
			    )
		{
			break;
		}
		else if ( !IsContainFeature(efcSemanticCommon) && IsContainFeature(efcNoun) &&		
				  !morphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNumeric) && 
				  !morphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postOfficial) &&
				  (IsContainFeature(efRCaseNominative) || IsContainFeature(efRCaseInstrumental))
			    )
		{
			if(!IsCoordinatedCaseGenderNumber(l_UnitsFirst.back(),m_pCurrentUnit,false,bVerbRB) ) continue;
			bGood=true;
			break;
		}
	}
	if (bGood)
	{
		if ((oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstName)||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName))&&
			(IsContainFeature(efRAnimate)&&IsCoordinatedCaseGenderNumber(l_UnitsFirst.back(),m_pCurrentUnit,true,bVerbRB))
			)
		{
			AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition);
			AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
			//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);
			//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
		}
		if ((oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
			oSemanticFeatureFirst.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality))&&
			IsContainFeature(efRAnimate)
			)
		{
			AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition);
			AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
			//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
			//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
		}
		if(oSemanticFeatureFirst.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)||
			oSemanticFeatureFirst.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization)||
			oSemanticFeatureFirst.m_SemanticType.GetValue()<=1
			)
		{
			AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhatDefinition);
			AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhatDefinition);
			//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
			//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
		}
	}
	return false;
}
bool CSMComplementR::PersonBefore(TBoxUnits & l_Units,SS::Core::Features::CSemanticFeature & oSemanticFeature)
{
	if (GetPrevUnit())
	{
		UINT uiCount=0;
		if(m_LexemType.IsSComma())
		{
			if( GetPrevUnit() && !(m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("))")) ) )
			{
				return false;
			}
		}
		if (m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("))")))
		{
			while (GetPrevUnit() )
			{
				if  ( m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("(_T(")) ) 
				{
					break;
				}
			}
			if ( !(m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("(_T("))) ) return false;
			if ( !GetPrevUnit() ) return false;
		}else if (m_LexemType.IsSymbol() ) return false;
		if(IsPrepositionBeforeR(4,m_pCurrentUnit)) return false;
		uiCount=0;
		do {
			if ( IsSyntax() ) break;

			//if ( //SpecialTypeEquals(ustComplexProperName)  &&
			//	IsContainFeature(efRCaseNominative) && 
			//	IsContainFeature(efcSemanticName)
			//	) 
			//{
			//	//l_Units.assign(m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.begin(),m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.end());
			//	l_Units.push_back(m_pCurrentUnit);
			//	if (IsContainSemanticIndex(m_semName))
			//	{
			//		oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
			//		oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstName;
			//	}else
			//	{
			//		oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;
			//	}
			//	return true;
			//}

			if (
				IsContainFeature(efcNoun)&&
				IsContainFeature(efRCaseNominative)
				||
				IsContainFeature(efcSemanticCommon) &&
				IsContainFeature(efRCaseNominative)

				)
			{
				l_Units.push_back(m_pCurrentUnit);
				if(IsContainFeature(efcSemanticCommon))
					//m_pMorphoService->CollectSemanticFeature(m_pIAMCSemantic,m_pCurrentUnit->m_MorphoInfo,oSemanticFeature);
					CollectSemanticFeature(oSemanticFeature);
				return true;
			}
			if (IsContainFeature(efcVerb)
				) return false;
		} while(uiCount++<4 && GetPrevUnit() && !m_LexemType.IsSComma() );
	}
	return false;
}
bool CSMComplementR::AfterPersonInComma(IUnit * pPerson)
{
	return false;
	if ( pPerson==NULL) return false;
	IUnit * pMem=m_pCurrentUnit;
	GotoUnit(pPerson);
	if (GetNextUnit() && m_LexemType.IsSComma() )
	{
		TBoxUnits l_Add;
		bool bName=false,bWasPrep=false;
		while (GetNextUnit() && m_LexemType.IsSComma()==false && m_LexemType.IsSDot()==false /*&& !SyntaxTypeEquals(usntSubjectComplement)*/)
		{
			if ( (IsContainFeature(efcSemanticName) /*|| 
				SpecialTypeEquals(ustComplexProperName)  && IsContainSemanticIndex(m_semName)*/)
				&& bWasPrep==false
				)
			{
				GotoUnit(pMem);
				return false;
			}else if(IsContainFeature(efcPretext)) bWasPrep=true;
			else bWasPrep=false;
			l_Add.push_back(m_pCurrentUnit);
		};
		GotoUnit(pMem);

		if (l_Add.size()<=6)
		{
			TBoxUnits::iterator it;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			UINT uiCount=0;
			for (it=l_Add.begin();uiCount<3 && it!=l_Add.end(); it++,uiCount++)
			{
				if (IsSyntax(*it)) continue;

					if( !IsCoordinatedCaseGenderNumber(pPerson,(*it),true))
						continue;

					oSemanticFeature.Undefine();
					if ( CollectSemanticFeature(oSemanticFeature,*it) )
					{
						if (
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)
							)						
						{
							IUnit * pHead=JoinUpUnits(l_Add);
							AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition,pHead);
							m_pCurrentUnit=pHead;
							return true;
						}
					}
				}			
		}
	}
	GotoUnit(pMem);
	return false;
}

bool CSMComplementR::BeforePersonInComma(IUnit * pPerson)
{
	if ( pPerson==NULL) return false;
	IUnit * pMem=m_pCurrentUnit;
	GotoUnit(pPerson);
	UINT uiCount=0;

	while (GetPrevUnit() && !m_LexemType.IsSComma() && !IsContainFeature(efcVerb)) uiCount++;
	if ( m_LexemType.IsSComma() && uiCount<3)
	{
		TBoxUnits l_Add;

		while ( GetPrevUnit() && m_LexemType.IsSComma()==false)
		{
			//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,m_MorphoFeature);
			m_MorphoFeature.Undefine();
			CollectMorphoFeature(m_MorphoFeature);
			if (m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) && 
				m_MorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast)||
				IsContainFeature(efcRepresentBecome)
				)
			{
				GotoUnit(pMem);
				return false;
			}
			//l_Add.push_back(m_pCurrentUnit);
			l_Add.push_front(m_pCurrentUnit);
		};
		GotoUnit(pMem);

		if (l_Add.size()<=6)
		{
			TBoxUnits::iterator it;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			for (it=l_Add.begin(); it!=l_Add.end(); it++)
			{
//				if (SyntaxTypeEquals(usntSubjectComplement,*it)) break;

				if( !IsCoordinatedCaseGenderNumber(pPerson,(*it),true))
					continue;

				oSemanticFeature.Undefine();
				if ( CollectSemanticFeature(oSemanticFeature,*it) )
				{
					if (
						oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
						oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
						oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)
						)						
					{
						//							SetSyntaxInfo(l_Add.front(),l_Add,usntSubjectComplement);
						//							l_Add.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);
						//(*it)->m_SyntaxUnitType=usntSubjectComplement;
						//(*it)->m_SyntaxInfo.CreateSyntaxInfo();
						//(*it)->m_SyntaxInfo.m_pHeadUnit=(*it);
						//(*it)->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);
						IUnit * pHead=JoinUpUnits(l_Add);
						AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition,pHead);
						//SetSyntaxType(usntSubjectComplement);
						m_pCurrentUnit=pHead;
						return true;
					}
				}
			}
		}
	}
	GotoUnit(pMem);
	return false;
}

bool CSMComplementR::BeforePersonSimple(IUnit * pPerson)
{

	if ( pPerson==NULL) return false;
	IUnit * pMem=m_pCurrentUnit;
	GotoUnit(pPerson);
	if (GetPrevUnit())
	{
		if(!IsCoordinatedCaseGenderNumber(pPerson,m_pCurrentUnit,true)){
			GotoUnit(pMem);
			return false;
		}

		SS::Core::Features::CSemanticFeature oSemanticFeature;
		if ( CollectSemanticFeature(oSemanticFeature) )
		{
			if (
				oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
				oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
				oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)
				)
			{
				TBoxUnits l_Units;
				//l_Units.push_back(m_pCurrentUnit);
				if (!IsSyntax() )
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition);
				GotoUnit(pMem);
				return true;
			}
		}
	}
	GotoUnit(pMem);
	return false;
}

bool CSMComplementR::IsCoordinatedCaseGenderNumber(IUnit * pFirst, IUnit * pSecond,bool bCheckGender,bool bSkipInstrumentalCaseSecond)
{
	if(pFirst->GetFirstIndex() &&
		(/*(*/pFirst->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/)==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus||
		pSecond->GetFirstIndex() &&
		(/*(*/pSecond->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/)==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus
		)
		return true;
	SS::Core::Features::CMorphoFeature oMorphoFeature2;
	m_MorphoFeature.Undefine();
	CollectMorphoFeature(m_MorphoFeature,pFirst);
	CollectMorphoFeature(oMorphoFeature2,pSecond);

	if(m_MorphoFeature.m_NumberType.GetValue()<=1) 
		m_MorphoFeature.m_NumberType.FillUp();
	if(oMorphoFeature2.m_NumberType.GetValue()<=1) 
		oMorphoFeature2.m_NumberType.FillUp();

	if (m_MorphoFeature.m_GenderType.IsContain(Values::GenderType.gtGeneral)) 
		m_MorphoFeature.m_GenderType.FillUp();
	if (oMorphoFeature2.m_GenderType.IsContain(Values::GenderType.gtGeneral)) 
		oMorphoFeature2.m_GenderType.FillUp();

	m_MorphoFeature.m_CaseType.AssignAND(&oMorphoFeature2.m_CaseType);
	m_MorphoFeature.m_GenderType.AssignAND(&oMorphoFeature2.m_GenderType);
	m_MorphoFeature.m_NumberType.AssignAND(&oMorphoFeature2.m_NumberType);

	if( m_MorphoFeature.m_CaseType.GetValue()<=1 &&
		(!bSkipInstrumentalCaseSecond || !oMorphoFeature2.m_CaseType.IsContain(Values::CaseType.ctInstrumental)) ||
		bCheckGender && m_MorphoFeature.m_GenderType.GetValue()<=1 || 
		m_MorphoFeature.m_NumberType.GetValue()<=1 
		)
		return false;
	return true;
}
void CSMComplementR::AddSyntaxCategoryIndices(TBoxUnits & l_Units,const Types::SyntaxCategories & oSyntaxCategories)
{
	TBoxUnits::iterator i;
	for (i=l_Units.begin(); i!=l_Units.end(); i++) {
		AddSyntaxCategoryIndex(oSyntaxCategories,*i);
	}
}

			}
		}
	}
}