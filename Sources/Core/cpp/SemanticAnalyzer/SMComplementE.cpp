#include "StdAfx.h"
#include ".\smcomplemente.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{


CSMComplementE::CSMComplementE(void)
{
	m_bInit = false;
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMComplementE::~CSMComplementE(void)
{
}

bool CSMComplementE::Init(IText * pIText)
{
	SS_TRY
	{
		if (!CSMSentenceExpert::Init(pIText)) return false;

		if(!m_bInit)
		{
			m_bInit = true;

			SS::Core::Features::CSemanticFeature oF;
			oF.m_SemanticType=Values::TSemanticType.smtPerson;
			oF.m_SPersonType=Values::TSPersonType.semprstName;
			m_pIAMCSemantic->Code(&oF,&m_semName);
		}

		return true;
	}
	SS_CATCH(L"")

}


bool CSMComplementE::AnalizeSentence()
{
	SS_TRY
	{
		if(m_AnalyseLanguage!=m_pCurrentSentence->GetLanguage()) return true;

		bool bGood;
		do{
			//wstring str = m_pCurrentUnit->GetWord();

			if (IsContainFeature(efcRepresentBecome))
			{
				DetectVerbDefinitionConstructions();
			}else if (IsContainFeature(efcSemanticName) && !IsContainFeature(efECaseGenetive))
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
				if (bGood && m_pCurrentUnit)
				{
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition);
				}
			}
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}

void CSMComplementE::DetectVerbDefinitionConstructions()
{
	SS_TRY
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
		ProcessToBeCompliment(oSemanticFeature,l_WhoDef);
	}
	SS_CATCH(L"")
}

bool CSMComplementE::ProcessToBeCompliment(SS::Core::Features::CSemanticFeature & oSemanticFeatureLeft,TBoxUnits & l_UnitsFirst)
{
	SS_TRY
	{
		//IUnit * pMem=m_pCurrentUnit;
	//	SS::Core::Features::CMorphoFeature m_MorphoFeature;
		SS::Core::Features::CSemanticFeature oSemanticFeatureRight;
		UINT uiAdverbCount=0,uiWordsCount=0,uiDet=0;
		bool bGood=false, bOneOf=false, bArticle=false;

		TBoxUnits  l_Second;
		wstring str;

		while (
			GetNextUnit() 
			)
		{
			str = m_pCurrentUnit->GetWord();
			m_StringService.LowerSz((TCHAR*)str.c_str());
			if(	m_StringService.StrEqual(_T("as"),str.c_str()) ||
				m_StringService.StrEqual(_T("most"),str.c_str()))
			{
				continue;
			}else if(m_StringService.StrEqual(_T("the"),str.c_str())||
				m_StringService.StrEqual(_T("a"),str.c_str()))
			{
				bArticle=true;
				continue;
			}else if(m_StringService.StrEqual(_T("no"),str.c_str()) ||
				m_StringService.StrEqual(_T("not"),str.c_str()))
			{
				break;
			}

			SkipBrackets();

			if(	! (
				/*(SpecialTypeEquals(ustUndefined) || 
				SpecialTypeEquals(ustComplexProperName) && IsContainSemanticIndex(m_semName)||
				SpecialTypeEquals(ustCitation)||
				SpecialTypeEquals(ustNumeral)||
				SpecialTypeEquals(ustPlaceSpecification) )
				&&*/ uiWordsCount++<=4&&
				(!IsSyntax()/*||SyntaxTypeEquals(usntSubjectComplement)*/ )&& 
				!m_LexemType.IsSComma()&& !m_LexemType.IsSDot() && !m_LexemType.IsSTire() && !IsContainFeature(efcPretext)
				)
				)
			{
				break;
			}

			m_MorphoFeature.Undefine();
			CollectMorphoFeature(m_MorphoFeature);
			if (m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) 
				&& m_MorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast) &&
				m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) &&
				!IsContainFeature(efcRepresentBecome) && !bOneOf && !bArticle)
			{
				break;
			}
			if(m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun))
				continue;

			if (SpecialTypeEquals(ustComplexProperName) && IsContainSemanticIndex(m_semName.GetDictionaryIndex()))
			{
				oSemanticFeatureRight.m_SemanticType=Values::TSemanticType.smtPerson;
				oSemanticFeatureRight.m_SPersonType=Values::TSPersonType.semprstName;
				//l_Second.assign(m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.begin(),
				//m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.end());
				l_Second.push_back(m_pCurrentUnit);
				break;

			}else if ((IsContainFeature(efcSemanticCommon)||
				IsContainFeature(efEDNounNominative)) &&
				!IsContainFeature(efECaseGenetive)
				/*!IsContainFeature(efcAdverbAdjective)*/
				)
			{
				//str = m_pCurrentUnit->GetWord();
	//			m_pMorphoService->CollectSemanticFeature(m_pIAMCSemantic,m_pCurrentUnit->m_MorphoInfo,oSemanticFeatureRight,l_SemValues);
				CollectSemanticFeature(oSemanticFeatureRight);
				if (oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure) && 
					(oSemanticFeatureRight.m_SMeasureType.IsContain(Values::TSMeasureType.msrtNumber))
					)
				{
					if(m_StringService.StrEqual(_T("one"),str.c_str()))
					{
						oSemanticFeatureRight.Undefine();
						Mem();
						if(GetNextUnit())
						{
							str = m_pCurrentUnit->GetWord();
							if(!m_StringService.StrEqual(_T("of"),str.c_str()))
								GotoMem();
							else
								bOneOf = true;
						}
						continue;
					}else
					{
						oSemanticFeatureRight.Undefine();
						continue;
					}
				}else if(oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval) &&
					oSemanticFeatureLeft.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
				{
						oSemanticFeatureRight.Undefine();
						continue;
				}
				l_Second.push_back(m_pCurrentUnit);
				//Mem();
				if (GetNextUnit() && ((IsContainFeature(efcSemanticCommon) ||
					IsContainFeature(efEDNounNominative))) && !IsContainFeature(efECaseGenetive))
				{
					//str = m_pCurrentUnit->GetWord();
					oSemanticFeatureRight.Undefine();
					
					list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
					list<UINT> l_Categories;
					RemoveSyntaxCategoryIndices(l_Categories,l_Second.back());
					RemoveSemanticIndicesFromUnit(l_Sem,l_Second.back());
					l_Sem.clear();
					RemoveSemanticIndicesFromUnit(l_Sem,m_pCurrentUnit);
					RemoveSyntaxCategoryIndices(l_Categories,m_pCurrentUnit);

					l_Second.push_back(m_pCurrentUnit);
					IUnit * pHead=JoinUpUnits(l_Second);
					if (pHead)
					{
						m_pCurrentUnit=pHead;
						SetSemanticIndices(l_Sem);
					}
					l_Second.clear();
					l_Second.push_back(m_pCurrentUnit);
					CollectSemanticFeature(oSemanticFeatureRight);
					//GotoMem();
					//continue;
				}
				//GotoMem();
				//if(!IsLatinWord(l_UnitsFirst.back()) && !IsCoordinatedCaseGenderNumber(l_UnitsFirst.back(),m_pCurrentUnit,false,bVerbRB) ) continue;
				//l_Second.push_back(m_pCurrentUnit);
				break;
			}else if ( SpecialTypeEquals(ustCitation) )
			{
	//			l_Second.assign(m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.begin(),
	//				m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.end());
				l_Second.push_back(m_pCurrentUnit);
				oSemanticFeatureRight.m_SemanticType=Values::TSemanticType.smtService;
				oSemanticFeatureRight.m_SServiceTypes=Values::TSServiceTypes.sstAcronymCitation;
				break;
			}else
			{

				//m_MorphoFeature.Undefine();
				//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,m_MorphoFeature);
				//CollectMorphoFeature(m_MorphoFeature);
				if (!IsContainFeature(efcRepresentBecome) && !bOneOf &&
					m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) 
					&& m_MorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent)||
					m_MorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otConjunction))
					break;
			}
		}
		if (l_Second.empty()==false)
		{
			if (
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstName)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName)
				)
			{
				//if(oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
				//{
					AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scNameDefinition);
					AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
				/*} else 
				{
					AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhatDefinition);
					AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhatDefinition);
					//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
					//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhatDefinition);
				}*/
			}
			if (
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstActor)
				)
			{
				AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhoDefinition);
				if(!(oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstName)||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName)))
					AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
				else AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scNameDefinition);
			}
			if( (!oSemanticFeatureLeft.m_SemanticType.IsContain(Values::TSemanticType.smtPerson)||
				oSemanticFeatureLeft.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)||
				oSemanticFeatureLeft.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization) ||
				oSemanticFeatureLeft.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval))
				//&& (!oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
				/*&&(	oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)||
				oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization))*/
				)
			{
				if(!oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
				{
					AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhatDefinition);
					AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhatDefinition);
				}
			}
			//if (oSemanticFeatureLeft.m_SemanticType.GetValue()<=1)
			//{
				if (oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstActor)
					)
				{
					//AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scNameDefinition);
					AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhoDefinition);
					AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
				}
				if ((oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstName)||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)||
					oSemanticFeatureRight.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName))
					)
				{
					if(oSemanticFeatureLeft.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
					{
						AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhoDefinition);
						AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scNameDefinition);
					}else
					{
						AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhatDefinition);
						AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhatDefinition);
					}
				}
				if ((oSemanticFeatureRight.m_SServiceTypes.IsContain(Values::TSServiceTypes.sstAcronymCitation)||
					oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)||
					oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization)||
					oSemanticFeatureRight.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval)) ||
					oSemanticFeatureRight.m_SemanticType.GetValue()<=1 )
				{

					AddSyntaxCategoryIndices(l_Second,Values::SyntaxCategories.scWhatDefinition);
					AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhatDefinition);
				}

			//}
		}

		uiWordsCount=0;
		GotoMem();
		while ( GetNextUnit() )
		{
			if ( !(SpecialTypeEquals(ustUndefined) && (uiAdverbCount < 3) && (uiWordsCount++ < 3 ) &&
				(!IsSyntax()/*||SyntaxTypeEquals(usntSubjectComplement)*/ )&& 
				!m_LexemType.IsSComma() && !m_LexemType.IsSDot() && !m_LexemType.IsSTire()
			   ))
			{
				break;
			}
			if (IsContainFeature(efEDetPronounAdjective))
			{
				uiDet++;
				continue;
			}
			m_MorphoFeature.Undefine();
			//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,m_MorphoFeature);
			CollectMorphoFeature(m_MorphoFeature);

			if ( (m_MorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)||
				m_MorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otConjunction)) 
				) 
			{
				break;
			}else if (
				uiDet==0 && 
				m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple)&& 
				m_MorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent) 
				)
			{
				break;
			}else if ( !IsContainFeature(efcSemanticCommon) && IsContainFeature(efcNoun) &&
				IsContainFeature(efEDNounNominative) && 
				!m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNumeric) && 
				!m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postOfficial) 
				)
			{
				bGood=true;
				break;
			}
		}
		if (bGood)
		{

			if (oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstName)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName)
				)
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition);
				AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
				//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);
				//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
			}
			if ((oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)||
				oSemanticFeatureLeft.m_SPersonType.IsContain(Values::TSPersonType.semprstActor))
				)
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition);
				AddSyntaxCategoryIndices(l_UnitsFirst,Values::SyntaxCategories.scWhoDefinition);
				//m_pCurrentUnit->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
				//l_UnitsFirst.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scWhoDefinition);
			}
			if(oSemanticFeatureLeft.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)||
				oSemanticFeatureLeft.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization)||
				oSemanticFeatureLeft.m_SemanticType.GetValue()<=1
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
	SS_CATCH(L"")
}
bool CSMComplementE::PersonBefore(TBoxUnits & l_Units,SS::Core::Features::CSemanticFeature & oSemanticFeature)
{
	SS_TRY
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
			if (m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("))")) )
			{
				while (GetPrevUnit() )
				{
					if  (  m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("(_T(")) ) 
					{
						break;
					}
				}
				if ( !(m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("(_T(")) ) ) return false;
				if ( !GetPrevUnit() ) return false;
			}else if (m_LexemType.IsSymbol() ) return false;

			if(IsPrepositionBeforeE(4,m_pCurrentUnit)) return false;
			uiCount=0;
			wstring str;
			do {
				if ( !!IsSyntax()) break;

				if ( SpecialTypeEquals(ustComplexProperName)  &&
					!IsContainFeature(efECaseGenetive) 
					) 
				{
					//l_Units.assign(m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.begin(),m_pCurrentUnit->m_EntityInfo.m_pHeadUnit->m_EntityInfo.m_pExtra->m_l_UnitGroup.end());
					l_Units.push_back(m_pCurrentUnit);
					if (IsContainSemanticIndex(m_semName.GetDictionaryIndex()))
					{
						oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
						oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstName;
					}else
					{
						oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;
					}
					return true;
				}

				if(m_pCurrentUnit)
				{
					str=m_pCurrentUnit->GetWord();
					m_StringService.LowerSz((TCHAR*)str.c_str());
					if(m_StringService.StrEqual(_T("there"),str.c_str()) ||m_StringService.StrEqual(_T("this"),str.c_str())||m_StringService.StrEqual(_T("these"),str.c_str())) break;	
				}

				if (
					IsContainFeature(efcNoun)&&
					IsContainFeature(efEDNounNominative)||
					IsContainFeature(efcSemanticCommon) &&
					(!IsContainFeature(efECaseGenetive) && !IsContainFeature(efcPretext) &&
					!IsContainFeature(efcVerb))
					)
				{
					l_Units.push_back(m_pCurrentUnit);
					if(IsContainFeature(efcSemanticCommon))
						CollectSemanticFeature(oSemanticFeature);
					return true;
				}
				if (IsContainFeature(efcVerb))
				{
					Mem1();
					if (GetPrevUnit())
					{
						if (
							IsContainFeature(efcNoun)&&
							IsContainFeature(efEDNounNominative)||
							IsContainFeature(efcSemanticCommon) &&
							(!IsContainFeature(efECaseGenetive) )
							)
						{
							l_Units.push_back(m_pCurrentUnit);
							if(IsContainFeature(efcSemanticCommon))
								CollectSemanticFeature(oSemanticFeature);
							GotoMem1();
							//l_Units.push_back(m_pCurrentUnit);
							return true;
						}
					}	
					GotoMem1();
					return false;
				}
			} while(uiCount++<4 && GetPrevUnit() && !m_LexemType.IsSComma() );
		}
		return false;
	}
	SS_CATCH(L"")
}
bool CSMComplementE::AfterPersonInComma(IUnit * pPerson)
{
	SS_TRY
	{
		return false;
		if ( pPerson==NULL) return false;
		IUnit * pMem=m_pCurrentUnit;
		GotoUnit(pPerson);
		//TCHAR buf[100];
		if (GetNextUnit() && m_LexemType.IsSComma() )
		{
			TBoxUnits l_Add;
			bool bName=false,bWasPrep=false;
			while (GetNextUnit() && m_LexemType.IsSComma()==false && m_LexemType.IsSDot()==false /*&& !SyntaxTypeEquals(usntSubjectComplement)*/ )
			{
				if ( (IsContainFeature(efcSemanticName) || 
					 SpecialTypeEquals(ustComplexProperName)  && IsContainSemanticIndex(m_semName.GetDictionaryIndex()))
					 && !bWasPrep
				   )
				{
					GotoUnit(pMem);
					return false;
				}
				else if(IsContainFeature(efcPretext)) bWasPrep=true;
				else if(!SpecialTypeEquals(ustComplexProperName) ) bWasPrep=false;
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
					if (/*!SyntaxTypeEquals(usntUndefined,*it)*/IsSyntax(*it)) continue;
					//if (SyntaxTypeEquals(usntSubjectComplement,*it)) break;

					oSemanticFeature.Undefine();
					if ( CollectSemanticFeature(oSemanticFeature,*it) )
					{
						if (
							(oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)||
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstActor)) &&
							(!oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName) &&
							!oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName))
							)						
						{
							//SetSyntaxInfo(l_Add.front(),l_Add,usntSubjectComplement);
							//l_Add.front()->m_SyntaxInfo.AddSyntaxCategory(Values::SyntaxCategories.scNameDefinition);

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
	SS_CATCH(L"")
}

bool CSMComplementE::BeforePersonInComma(IUnit * pPerson)
{
	SS_TRY
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
				//wstring str = m_pCurrentUnit->GetWord();
				//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,m_MorphoFeature);
				m_MorphoFeature.Undefine();
				CollectMorphoFeature(m_MorphoFeature);
				if ((m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) && 
					m_MorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast)) ||
					IsContainFeature(efcRepresentBecome)
					)
				{
					GotoUnit(pMem);
					return false;
				}
				l_Add.push_front(m_pCurrentUnit);
			};
			GotoUnit(pMem);

			if (l_Add.size()<=6)
			{
				TBoxUnits::iterator it;
				SS::Core::Features::CSemanticFeature oSemanticFeature;
				for (it=l_Add.begin(); it!=l_Add.end(); it++)
				{
					//if (SyntaxTypeEquals(usntSubjectComplement,*it)) break;
					oSemanticFeature.Undefine();
					if ( CollectSemanticFeature(oSemanticFeature,*it) )
					{
						if (
							(oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)||
							oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstActor)) &&
							(!oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName) &&
							!oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName))
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
	SS_CATCH(L"")
}


bool CSMComplementE::BeforePersonSimple(IUnit * pPerson)
{
	SS_TRY
	{
		if (pPerson==NULL || IsContainFeature(efECaseGenetive)) return false;
		IUnit * pMem=m_pCurrentUnit;
		GotoUnit(pPerson);
		if ( GetPrevUnit() )
		{
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			if ( CollectSemanticFeature(oSemanticFeature) )
			{
				if ( oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
					 oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
					 oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)||
					 oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstActor) )
				{
					TBoxUnits l_Units;
					//l_Units.push_back(m_pCurrentUnit);
					/*---COMMENTED--- if ( !IsSyntax() )*/
						//SetSyntaxInfo(m_pCurrentUnit,l_Units,usntSubjectComplement);
						//SetSyntaxType(usntSubjectComplement);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition);
					GotoUnit(pMem);
					return true;
				}
			}
		}
		GotoUnit(pMem);
		return false;
	}
	SS_CATCH(L"")
}

void CSMComplementE::AddSyntaxCategoryIndices(TBoxUnits & l_Units,const Types::SyntaxCategories & oSyntaxCategories)
{
	SS_TRY
	{
		TBoxUnits::iterator i;
		for (i=l_Units.begin(); i!=l_Units.end(); i++) 
		{
			AddSyntaxCategoryIndex(oSyntaxCategories,*i);
		}
	}
	SS_CATCH(L"")
}

void CSMComplementE::SkipBrackets()
{
	SS_TRY
	{
		if (m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("(")))
		{
			while(GetNextUnit())
			{
				if(m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T(")")))
				{
					GetNextUnit();
					break;
				}
			}
		}
	}
	SS_CATCH(L"")
}



			}
		}
	}
}