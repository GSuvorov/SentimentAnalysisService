#include "StdAfx.h"
#include ".\smspecifycategory.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace PostSyntaxAnalysis
			{

		CSMSpecifyCategory::CSMSpecifyCategory(void)
		{
			m_bInit = false;
			m_AnalyseLanguage=SS::Core::Types::ealEnglish;
		}

		CSMSpecifyCategory::~CSMSpecifyCategory(void)
		{
		}

		bool CSMSpecifyCategory::Init(IText * pIText)
		{
			if (!CSMExpert::Init(pIText)) return false;
			
			if(!m_bInit)
			{
				m_bInit = true;

				SS::Core::Features::CSemanticFeature oSemanticFeature;
				oSemanticFeature.m_SemanticType=Values::TSemanticType.smtMeasure;
				oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtNumber;
				m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semNumber);

				oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;

				oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstFirstName;
				m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_questSemFirstName);

				oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstLastName;
				m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_questSemLastName);
			}

			return true;
		}

		bool CSMSpecifyCategory::AnalizeSentence()
		{
			SS_TRY
			{
				PassiveProcess();

				IUnit * pUnit, *pBeUnit=NULL;
				ISyntaxFeature * pFeature;
				bool bSubject=false, bComplement=false, bNameSurname=false;
				SS::Core::Features::CSyntaxFeatureAposteriori * pSyntaxFeatureAposteriori;
				m_CategoryUnitTable.clear();

				if (pUnit=GetFirstUnit())
				{
					do{
						pFeature=pUnit->GetSyntaxFeature();
						if (pFeature )//&& pFeature->IsQuestion()==false )
						{
							pSyntaxFeatureAposteriori=pFeature->GetSyntaxFeatureAposteriori();

							if ( pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectFin) )
							{
								AddSyntaxCategory(pUnit,Values::SyntaxCategories.scMainEntity);
								bSubject = true;
								m_CategoryUnitTable.insert(map<UINT,IUnit*>::value_type(Values::SyntaxCategories.scSubjectFin.GetSingleValue(),pUnit));
							}
							else if (pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObject) ||
								pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObjectComplement))
							{
								AddSyntaxCategory(pUnit,Values::SyntaxCategories.scAdditionalEntity);
							}
							else if ( pSyntaxFeatureAposteriori->m_SyntaxCategories.Equal(Values::SyntaxCategories.scMain) )
							{
								AddSyntaxCategory(pUnit,Values::SyntaxCategories.scPredicate);
								if(pUnit->HasChilds() && IsContainFeature(efcRepresentBecome,pUnit->GetLeftChildUnit()))
									pBeUnit = pUnit->GetLeftChildUnit();//(pUnit->GetLeftChildUnit())->SetObligatory(false);
							}		
							else if ( pSyntaxFeatureAposteriori->m_SyntaxCategories.Equal(Values::SyntaxCategories.scSubjectComplement) )
							{
								DeleteSyntaxCategory(pUnit,Values::SyntaxCategories.scSubjectComplement);
								AddSyntaxCategory(pUnit,Values::SyntaxCategories.scAdditionalEntity);
							}
							else if ( pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunct) )
							{
								SpecifyAjunct(pUnit);
							}
							else if (IsNotEmptyWordsAndSymbols(pUnit) && !IsCategory(pSyntaxFeatureAposteriori))
							{
								AddSyntaxCategory(pUnit,Values::SyntaxCategories.scAjunct);
								MoveToAjunctChilds(pUnit);
							}else if((pFeature->IsQuestion()))
							{
								std::map<UINT,IUnit*>::iterator i=m_CategoryUnitTable.find(Values::SyntaxCategories.scSubjectFin.GetSingleValue());
								if (!(i==m_CategoryUnitTable.end()) && CheckNameAndSetNameSemantic(i->second,pUnit))
									bNameSurname = true;
								if((pUnit->GetLeftChildUnit())->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectComplement))
									bComplement = true;
							}
						}
					}while(pUnit=GetNextUnit());
				}

				if(bSubject && bComplement && pBeUnit && !bNameSurname)
				{
					pBeUnit->GetParentUnit()->SetObligatory(false);
					pBeUnit->SetObligatory(false);
				}

				return true;
			}
			SS_CATCH(L"")
		}

		bool CSMSpecifyCategory::CheckNameAndSetNameSemantic(IUnit * pUnit, IUnit * pQuestion)
		{
			if(!pUnit) return false;

			IUnit * pChild;
			if(pUnit->HasChilds() && (pChild = pUnit->GetLeftChildUnit()))
			{
				do
				{
					const TCHAR * str = pChild->GetWord();
					m_StringService.LowerSz((TCHAR*)str);
					if(m_StringService.StrEqual(str,_T("name")) || m_StringService.StrEqual(str,_T("surname")) ||
						m_StringService.StrEqual(str,_T("motto")) || m_StringService.StrEqual(str,_T("nickname")))
					{
						AddPersonSemantic(pQuestion);
						//DeleteSyntaxCategory(pQuestion,Values::SyntaxCategories.scWhatDefinition);
						return true;
					}
				}while(pChild=pChild->GetRightUnit());
			}

			return false;
		}

		void CSMSpecifyCategory::AddPersonSemantic(IUnit * pUnit)
		{
			AddSemanticIndex(m_questSemFirstName.GetDictionaryIndex()+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24),pUnit);
			AddSemanticIndex(m_questSemLastName.GetDictionaryIndex()+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24),pUnit);
		}
		

		void CSMSpecifyCategory::PassiveProcess()
		{
			IUnit * pUnit;

			if(pUnit = GetFirstUnit())
			do{
				if(IsPassive(pUnit))
				{
					SpecifyCategories();
					break;
				}
			}while(pUnit = GetNextUnit());
		}

		void CSMSpecifyCategory::SpecifyCategories()
		{
			SetSyntaxQuestion();
			SetSyntaxSubjectObject();
		}

		void CSMSpecifyCategory::SetSyntaxQuestion()
		{
			IUnit * pUnit = NULL;

			if (pUnit=GetFirstUnit())
			{
				do{
					if (pUnit->GetSyntaxFeature() && pUnit->GetSyntaxFeature()->IsQuestion())
					{
						if(pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectFin))
						{
							/*if (CheckManyMuch(pUnit))
							{
								if(!IsContainSemanticIndex(m_semNumber,pUnit))
								{
									AddSemanticIndex(m_semNumber,pUnit);
								}
							}*/
							if(!IsContainSemanticIndex(m_semNumber.GetDictionaryIndex(),pUnit))
								SetSyntaxCategory(pUnit,Values::SyntaxCategories.scObject);
							break;
						}
					}
				}while(pUnit = GetNextUnit());
			}			
		}

		void CSMSpecifyCategory::SetSyntaxSubjectObject()
		{
			IUnit * pUnit;

			if(pUnit = GetFirstUnit())
			do{
				if(pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectFin))
				{
					if (CheckSubjectInChilds(pUnit))
					{
						
					}
				}else if(pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunct))
				{
					CheckObjectPreposition(pUnit, true);
				}
			}while(pUnit = GetNextUnit());
		}

		bool CSMSpecifyCategory::CheckSubjectInChilds(IUnit * pHead)
		{
			if ((!pHead->HasChilds()) /*|| (pHead->GetChildCount() == 1)*/)  return false;

			IUnit * pUnit = pHead->GetLeftChildUnit();
			do{
				//wstring str = pUnit->GetWord();
				if(IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pUnit))
				{
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pUnit);
					if(!IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pUnit))
					{
						AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pUnit);
					}
					return true;
				}
			}while(pUnit=pUnit->GetRightUnit());
			return false;
		}

		void CSMSpecifyCategory::SpecifyAjunct(IUnit * pHead)
		{
			if (!pHead->HasChilds())
			{
				//SetSyntaxCategory(pHead,Values::SyntaxCategories.scAdditionalEntity);
				return;
			}
			
			IUnit * pUnit=pHead->GetLeftChildUnit();
			bool bTime=false, bPlace=false,bMeasure=false;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
//			CollectSemanticFeature(pHead,oSemanticFeature);
			do{
				//wstring str = pUnit->GetWord();

				if (!IsPretext(pUnit) && CollectSemanticFeature(oSemanticFeature,pUnit))
				{
					if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPlace))
						bPlace=true;
					else if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval))
						bTime=true;
					else if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure))
						bMeasure=true;
					
					oSemanticFeature.Undefine();
				}
			}while( !bTime && !bPlace && !bMeasure && (pUnit=pUnit->GetRightUnit()) );
			IUnit * pSemanticUnit=pUnit;
			if (bPlace || bTime)
			{
				wstring str;
				list<SS::Core::Features::Types::SyntaxCategories>  l_SyntaxCategories;
				list<Types::SyntaxCategories>::iterator itS;
				SS::Core::Features::Types::SyntaxCategories oSyntaxCategories;

				oSyntaxCategories.SetValue(0);
				pUnit=pHead->GetLeftChildUnit();

				do{
					str=pUnit->GetWord();
					m_StringService.LowerSz((TCHAR*)str.c_str());
					/*char buf[50];
					wcstombs(buf,str.c_str(),50);
					string str2(buf);*/
					if ( m_pOfficialVirtualSyntax->GetByFormPrepositionSyntaxCategories(&str,&l_SyntaxCategories) )
					{
						for (itS=l_SyntaxCategories.begin() ; itS!=l_SyntaxCategories.end() ; itS++)
						{
							if ( bTime==true &&((*itS).Equal(Values::SyntaxCategories.scAjunctTimeDuration) || (*itS).Equal(Values::SyntaxCategories.scAjunctTimeEnd) || (*itS).Equal(Values::SyntaxCategories.scAjunctTimeFixedPoint) ||(*itS).Equal(Values::SyntaxCategories.scAjunctTimeSource)) ||
								bTime==false &&((*itS).Equal(Values::SyntaxCategories.scAjunctPlaceEnd) || (*itS).Equal(Values::SyntaxCategories.scAjunctPlaceFixedPoint)|| (*itS).Equal(Values::SyntaxCategories.scAjunctPlaceSource)) )
							{
								oSyntaxCategories.AssignOR(&(*itS));
							}
						}
					}
				}while( oSyntaxCategories.GetValue()<=1 && (pUnit=pUnit->GetRightUnit()) ) ;
				if(oSyntaxCategories.GetValue()>1)
				{
					SetSyntaxCategory(pHead,oSyntaxCategories);
					//if (pHead->GetSyntaxFeature()->IsQuestion())
					//{
						RemoveSemanticIndicesFromUnit(l_Semantic,pSemanticUnit);
						if (bTime) KeepSpecifedSemanticTypes(Values::TSemanticType.smtTimeInterval,l_Semantic);
						else KeepSpecifedSemanticTypes(Values::TSemanticType.smtPlace,l_Semantic);
						SetSemanticIndices(l_Semantic, pHead);
					//}
				}//else SetSyntaxCategory(pHead,Values::SyntaxCategories.scAdditionalEntity);
			}else if (bMeasure)
			{
				//SetSyntaxCategory(pHead,Values::SyntaxCategories.scAjunctMeasure);

				RemoveSemanticIndicesFromUnit(l_Semantic, pSemanticUnit);
				KeepSpecifedSemanticTypes(Values::TSemanticType.smtMeasure,l_Semantic);
				SetSemanticIndices(l_Semantic,pHead);
				
			}else
				CheckObjectPreposition(pHead, false);
		}

		void CSMSpecifyCategory::CheckObjectPreposition(IUnit * pHead, bool bPassive)
		{
			if (!pHead) return;
			IUnit * pUnit;

			if(pUnit=pHead->GetLeftChildUnit())
			{
				do{
					if(MoveToPrepositionObject(pUnit, bPassive) && bPassive)
					{
						ProcessUnit(pUnit,pHead);
					}
				}while(pUnit=pUnit->GetRightUnit());
			}
		
			return;
		}

		bool CSMSpecifyCategory::MoveToPrepositionObject(IUnit * pUnit, bool bPassive)
		{
			if (!pUnit) return false;

			if(pUnit->GetSyntaxFeature() &&
				pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObjectOfPreposition))
			{
				if (!bPassive)
				{
					SetSyntaxCategory(pUnit,Values::SyntaxCategories.scPrepositionObject);
					if(!IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scPrepositionObject,pUnit))
						AddSyntaxCategoryIndex(Values::SyntaxCategories.scPrepositionObject,pUnit);
				}
				return true;
			}
			return false;
		}

		void CSMSpecifyCategory::ProcessUnit(IUnit * pUnit, IUnit * pHead)
		{
			const TCHAR * str;
			do{
				if (IsPretext(pUnit))
				{
					str=pUnit->GetWord();
					m_StringService.LowerSz((TCHAR*)str);
					if (m_StringService.StrEqual(_T("by"),str))
					{
						SetSyntaxCategory(pHead,Values::SyntaxCategories.scSubjectFin);
						if(!IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pHead))
							AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pHead);
					} 

					/*if (pHead->GetSyntaxFeature() && pHead->GetSyntaxFeature()->IsQuestion())
					{
						pUnit->Pop();
						pUnit->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.scAjunct);
						m_pCurrentSentence->AddUnit(pUnit);
					}*/
					break;
				}
			}while(pUnit=pUnit->GetLeftUnit());
		}

		bool CSMSpecifyCategory::IsPassive(IUnit * pHead)
		{
			if (!pHead->HasChilds()) return false;

			IUnit * pUnit;
			IIndex * pIndex;
			IFeature * pFeature;
			//UINT uiIndex;
			SS::Core::Features::CMorphoFeature oMorphoFeature;

			pUnit=pHead->GetLeftChildUnit();
			do{
				//wstring str = pUnit->GetWord();

				if (pIndex=pUnit->GetFirstIndex())
				{
					do{
						//uiIndex=pIndex->GetDictionaryIndex()->GetFirst();
						if ( pFeature=pIndex->GetFirstFeature() )
						{
							do{
								m_pIAMCMorpho->EnCode(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*uiIndex>>24*/,pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&oMorphoFeature);
								if(oMorphoFeature.m_VoiceType.IsContain(Values::VoiceType.vtPassive))
									return true;
							}while(pFeature=pIndex->GetNextFeature());
						}
					}while(pIndex=pUnit->GetNextIndex());
				}
			}while(pUnit=pUnit->GetRightUnit());

			return false;
		}


		bool CSMSpecifyCategory::IsLegalLexem(IUnit* pUnit)
		{
			if (!pUnit) return false;
			TCHAR * sz_Word;
			sz_Word=(TCHAR*)pUnit->GetWord();
			if (!sz_Word ||  sz_Word[0]=='?' || sz_Word[0]==0 ||
				sz_Word[0]=='\"' || sz_Word[0]=='.' || sz_Word[0]==',' || sz_Word[0]==';' || sz_Word[0]==':' ||
				sz_Word[0] == '\''
				)return false;
			return true;
		}

		bool CSMSpecifyCategory::IsNotEmptyWordsAndSymbols(IUnit* pUnit)
		{
			if (!pUnit) return false;

			if(pUnit->HasChilds())
			{
				IUnit * pUnit2=pUnit->GetLeftChildUnit();
				if(pUnit2->HasChilds())
				{
					if (IsNotEmptyWordsAndSymbols(pUnit2))
						return true;
				}
				else
				{
					do{
						if(IsLegalLexem(pUnit2) /*&& !m_LexemType.IsSymbol(pUnit2)*/)
							return true;
					}while(pUnit2=pUnit2->GetRightUnit());
				}
			}
			return false;
		}

		bool CSMSpecifyCategory::IsCategory(SS::Core::Features::CSyntaxFeatureAposteriori * pSyntaxFeatureAposteriori)
		{
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
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scSubjectFin);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scObject);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scObjectComplement);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctGoal);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctCause);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctMeasure);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctManner);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scVerbModifier);

			return pSyntaxFeatureAposteriori->m_SyntaxCategories.IsIntersectsWith(&oSyntaxCategories);
			//return oSyntaxCategories.IsIntersectsWith(&(pSyntaxFeatureAposteriori->m_SyntaxCategories));
		}

		bool CSMSpecifyCategory::MoveToAjunctChilds(IUnit * pGroup)
		{
			if(!pGroup->HasChilds())
			{
				return false;
			}

			IUnit * pUnit=pGroup->GetLeftChildUnit();
			do{
				if (!MoveToAjunctChilds(pUnit))
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunct,pUnit);
			}while(pUnit=pUnit->GetRightUnit());

			return true;

		}

//////////////////////////////////////////////////////////////////////////
}
}
}
}