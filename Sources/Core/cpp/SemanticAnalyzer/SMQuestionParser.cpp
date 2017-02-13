#include "StdAfx.h"
#include ".\smquestionparser.h"
#include ".\smmanager.h"
#include "..\assinterface\TDictionaryNames.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace PostSyntaxAnalysis
			{

		CSMQuestionParser::CSMQuestionParser(void)
		{
			m_bInit = false;
			m_sDescription=_T(__FUNCTION__);
			m_AnalyseLanguage=SS::Core::Types::ealEnglish;
		}

		CSMQuestionParser::~CSMQuestionParser(void)
		{
		}
		bool CSMQuestionParser::AnalizeSentence()
		{
			SS_TRY
			{
				SelectQuestionEnglish(m_pCurrentSentence);

				return true;
			}
			SS_CATCH(L"")
		}
		bool CSMQuestionParser::Init(IText * pIText)
		{
			if (!CSMExpert::Init(pIText)) return false;

			SS::Core::Features::CSemanticFeature oSemanticFeature;

			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtMeasure;
			oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtNumber;
			m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_questSemNumber);

			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstFirstName;
			m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_questSemFirstName);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstLastName;
			m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_questSemLastName);

			if(!m_bInit)
			{
				m_bInit = true;
				
				if (m_StopWords.empty())
				{
					m_StopWords.insert(wstring(_T("many")));
					m_StopWords.insert(wstring(_T("much")));
				}

				m_QuestionTypeEng.insert(map<wstring,CSMQuestionParser::EQuestionType>::value_type(wstring(_T("what")),CSMQuestionParser::eqtWhat));
				m_QuestionTypeEng.insert(map<wstring,CSMQuestionParser::EQuestionType>::value_type(wstring(_T("which")),CSMQuestionParser::eqtWhat));
				m_QuestionTypeEng.insert(map<wstring,CSMQuestionParser::EQuestionType>::value_type(wstring(_T("who")),CSMQuestionParser::eqtWho));
				m_QuestionTypeEng.insert(map<wstring,CSMQuestionParser::EQuestionType>::value_type(wstring(_T("whom")),CSMQuestionParser::eqtWho));
				m_QuestionTypeEng.insert(map<wstring,CSMQuestionParser::EQuestionType>::value_type(wstring(_T("when")),CSMQuestionParser::eqtWhen));
				m_QuestionTypeEng.insert(map<wstring,CSMQuestionParser::EQuestionType>::value_type(wstring(_T("where")),CSMQuestionParser::eqtWhere));
				m_QuestionTypeEng.insert(map<wstring,CSMQuestionParser::EQuestionType>::value_type(wstring(_T("how")),CSMQuestionParser::eqtHow));
				m_QuestionTypeEng.insert(map<wstring,CSMQuestionParser::EQuestionType>::value_type(wstring(_T("why")),CSMQuestionParser::eqtWhy));


				m_ToBeLiteral.insert(wstring(_T("is")));
				m_ToBeLiteral.insert(wstring(_T("are")));
				m_ToBeLiteral.insert(wstring(_T("was")));
				m_ToBeLiteral.insert(wstring(_T("were")));
			}

			return true;
		}
		bool CSMQuestionParser::SelectQuestionEnglish(ISentence * pSentence)
		{
			if(GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamOnlyKeyWords) return false;
			CSMQuestionParser::EQuestionType eQType=CSMQuestionParser::eqtNon;
			map<wstring,CSMQuestionParser::EQuestionType>::iterator iF;
			IUnit * pUnit;
			UINT uiDepth=0;
			UINT uiPosition=0;
			wstring str;
			if ( (pUnit=pSentence->GetFirstUnit()) )
			{
				do{
					if (pUnit->HasChilds())
					{
						IUnit * pUnit2=pUnit->GetLeftChildUnit();
						do{
							str=pUnit2->GetWord();
							m_StringService.LowerSz((TCHAR*)str.c_str());
							iF=m_QuestionTypeEng.find(str);
							if(iF!=m_QuestionTypeEng.end())
							{
								uiPosition = pUnit2->GetPosition();
								if(uiPosition > 2) continue;

								eQType=iF->second;
								if (pUnit->GetSyntaxFeature())
								{
									pUnit->GetSyntaxFeature()->SetIsQuestion();
								}
								break;
							}
						}while(pUnit2=pUnit2->GetRightUnit());
					}else 
					{
						str=pUnit->GetWord();
						m_StringService.LowerSz((TCHAR*)str.c_str());
						iF=m_QuestionTypeEng.find(str);
						if(iF!=m_QuestionTypeEng.end())
						{
							eQType=iF->second;
							break;
						}
					}
					uiDepth++;
				}while(	eQType==CSMQuestionParser::eqtNon && uiDepth<20 && (pUnit=pUnit->GetRightUnit()) );
			}
			if (eQType==CSMQuestionParser::eqtNon) return false;
			ProcessSentence(pSentence);
			if (pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunctMeasure))
			{
				AdjunctMeasureProcess(pSentence,pUnit,eQType);
			}else if (pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectFin))
			{
				SubjectFinProcess(pSentence,pUnit,eQType);
			}else if (pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectComplement))
			{
				SubjectComplementProcess(pUnit,eQType);
			}else if (pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObject) ||
				pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObjectComplement))
			{
				ObjectProcess(pSentence,pUnit,eQType);
			}else if (pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunctTimeDuration))
			{
				AjunctTimeDurationProcess(pSentence,pUnit,eQType);
			}else if (!pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunct))
			{
				PopSemantic(pUnit);
			}else if (pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunct))
			{
				AjunctProcess(pSentence,pUnit,eQType);
			}

			return true;
		}
		void CSMQuestionParser::PopSemantic(IUnit * pGroup)
		{
			IUnit * pUnit;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
			if (pUnit=pGroup->GetLeftChildUnit())
			{
				do{
					if(
						pUnit->GetSyntaxFeature() && 
						pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scDeterminer)
						)
						continue;

					oSemanticFeature.Undefine();
					if( CollectSemanticFeature(oSemanticFeature,pUnit) )
					{
						Types::TSemanticType oSemanticType=Values::TSemanticType.smtTimeInterval;
						oSemanticType.AssignOR(&Values::TSemanticType.smtPlace);
						oSemanticType.AssignOR(&Values::TSemanticType.smtPerson);

						RemoveSemanticIndicesFromUnit(l_Semantic, pUnit);
						KeepSpecifedSemanticTypes(oSemanticType,l_Semantic);
						SetSemanticIndices(l_Semantic, pGroup);
						break;
					}
				}while(pUnit=pUnit->GetRightUnit());
			}
		}

		void CSMQuestionParser::AjunctTimeDurationProcess(ISentence * pSentence,IUnit * pGroup,CSMQuestionParser::EQuestionType eQType)
		{
			AddAllTimeIntervalSemantic(pGroup);
			AddSyntaxCategory(pGroup,Values::SyntaxCategories.scAjunctTimeEnd);
			if (eQType==CSMQuestionParser::eqtHow)
			{
				SS::Core::Features::CSemanticFeature oSemanticFeature;
				list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
				IUnit * pUnit,*pUnitNext;
				pUnit=pGroup->GetLeftChildUnit();

				if (pUnit)
				{
					do{
						//wstring str = pUnit->GetWord();
						oSemanticFeature.Undefine();
						CollectSemanticFeature(oSemanticFeature,pUnit);
						if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure))
						{
							RemoveSemanticIndicesFromUnit(l_Semantic, pUnit);
							KeepSpecifedSemanticTypes(Values::TSemanticType.smtMeasure,l_Semantic);
							SetSemanticIndices(l_Semantic, pGroup);

							pUnitNext=pUnit->Pop();
							pUnit->SetObligatory(false);
							MoveToAgjunct(pSentence,pUnit);
							break;
						}
					}while(pUnit=pUnit->GetRightUnit());
				}
			}
		}

		void CSMQuestionParser::ObjectProcess(ISentence * pSentence, IUnit * pGroup,CSMQuestionParser::EQuestionType eQType)
		{
			if (!pGroup || eQType==CSMQuestionParser::eqtNon) return;

			if(CheckAimSemSyntAndMoveToAjunct(pSentence,pGroup,Values::SyntaxCategories.scObject,eQType)) return;
			if(CheckAimSemSyntAndMoveToAjunct(pSentence,pGroup,Values::SyntaxCategories.scObjectComplement,eQType)) return;

			if (eQType==CSMQuestionParser::eqtWho)
			{
				AddAllPersonSemantic(pGroup);
			}else if (eQType==CSMQuestionParser::eqtWhat)
			{
				SuperstructureSupport(pGroup);
			}
		}

		bool CSMQuestionParser::SuperstructureSupport(IUnit * pGroup)
		{
			IUnit * pUnit;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
			if ( pGroup->GetChildCount()>1 && (pUnit=pGroup->GetLeftChildUnit())) 
			{
				do{
					if(
						pUnit->GetSyntaxFeature() && 
						pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scDeterminer)
						)
						continue;

					oSemanticFeature.Undefine();
					CollectSemanticFeature(oSemanticFeature,pUnit);
					if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval)||
						oSemanticFeature.m_SPlaceType.GetValue()>1||
						oSemanticFeature.m_SOrganizationType.GetValue()>1||
						oSemanticFeature.m_SPersonType.GetValue()>1||
						oSemanticFeature.m_SServiceTypes.GetValue()>1
						)
					{
						Types::TSemanticType oSemanticType=Values::TSemanticType.smtTimeInterval;
						oSemanticType.AssignOR(&Values::TSemanticType.smtPlace);
						oSemanticType.AssignOR(&Values::TSemanticType.smtOrganization);
						oSemanticType.AssignOR(&Values::TSemanticType.smtService);
						//							oSemanticType.AssignOR(&Values::TSemanticType.smtPerson);

						RemoveSemanticIndicesFromUnit(l_Semantic, pUnit);
						KeepSpecifedSemanticTypes(oSemanticType,l_Semantic);
						if(oSemanticFeature.m_SemanticType.IsIntersectsWith(&oSemanticType))
							SetSemanticIndices(l_Semantic, pGroup);
						else if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
						{
							//if(!oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstActor))
								AddAllPersonNameSemantic(pGroup);
							/*else
							{
								SS::Dictionary::Types::TDictionaryIndex lVal=0;
								oSemanticFeature.Undefine();
								oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
								oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstActor;
								m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
								AddSemanticIndex(pGroup,lVal);
							}*/
							return true;
						}

						break;
					}else if (oSemanticFeature.m_SemanticType.GetValue()<=1 && IsNoun(pUnit))
					{
						IUnit * pU=pGroup->GetLeftChildUnit();
						while(pU)
						{
							if(IsContainSyntaxCategory(pU,Values::SyntaxCategories.scAttribute))
							{
								pU->ReleaseUnitHorizontal();
								break;
							}
							pU=pU->GetRightUnit();
						}

						pGroup->GetSyntaxFeature()->SetIsNotQuestion();
						break;
					}else if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure) && 
						oSemanticFeature.m_SMeasureType.GetValue()<=1
						)
					{
						if(!IsContainSemanticIndex(m_questSemNumber.GetDictionaryIndex(),pGroup))
						{
							AddSemanticIndex(pGroup,m_questSemNumber);
						}
					}
				}while(pUnit=pUnit->GetRightUnit());
			}
			return false;
		}

		bool CSMQuestionParser::IsNoun(IUnit * pUnit)
		{
			IIndex * pIndex;
			IFeature * pFeature;
			//UINT uiIndex;
			SS::Core::Features::CMorphoFeature oMorphoFeature;

			if (pIndex=pUnit->GetFirstIndex())
			{
				do{
					//uiIndex=pIndex->GetDictionaryIndex()->GetFirst();
					if ( pFeature=pIndex->GetFirstFeature() )
					{
						do{
							m_pIAMCMorpho->EnCode(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*uiIndex>>24*/,pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&oMorphoFeature);
							if(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun))
								return true;
						}while(pFeature=pIndex->GetNextFeature());
					}
				}while(pIndex=pUnit->GetNextIndex());
			}
			return false;
		}

		void CSMQuestionParser::SubjectComplementProcess(IUnit * pGroup,CSMQuestionParser::EQuestionType eQType)
		{
			if (!pGroup || eQType==CSMQuestionParser::eqtNon) return;
			
			CSMQuestionParser::TCategoryUnitTable::iterator i=m_CategoryUnitTable.find(Values::SyntaxCategories.scSubjectFin.GetSingleValue());
			if (i==m_CategoryUnitTable.end()) return;

			if (eQType==CSMQuestionParser::eqtWho)
			{
		
				AddAllPersonSemantic(pGroup);
				if (CheckWhoCategory(pGroup,i->second))
				{
					AddSyntaxCategory(pGroup,Values::SyntaxCategories.scAdditionalEntity);
					SetDefQuestion();
				}
			}else if (eQType==CSMQuestionParser::eqtWhat)
			{
				CSMQuestionParser::TCategoryUnitTable::iterator it=m_CategoryUnitTable.find(Values::SyntaxCategories.scMain.GetSingleValue());
				if(it!=m_CategoryUnitTable.end() && (IsFindCall(it->second)))
					return;
				if (CheckWhatCategory(pGroup,i->second))
				{
					AddSyntaxCategory(pGroup,Values::SyntaxCategories.scAdditionalEntity);
					SetDefQuestion();
				}

				SuperstructureSupport(pGroup);
			}
		}

		void CSMQuestionParser::SubjectFinProcess(ISentence * pSentence,IUnit * pGroup,CSMQuestionParser::EQuestionType eQType)
		{
			if (!pGroup || eQType==CSMQuestionParser::eqtNon) return;

			if(CheckAimSemSyntAndMoveToAjunct(pSentence,pGroup,Values::SyntaxCategories.scSubjectFin,eQType)) return;

			if (eQType==CSMQuestionParser::eqtWho)
			{
				AddAllPersonSemantic(pGroup);
			}else if (eQType==CSMQuestionParser::eqtWhat && pGroup->GetChildCount()>=1)
			{
				bool bPerson= SuperstructureSupport(pGroup);
				CSMQuestionParser::TCategoryUnitTable::iterator i=m_CategoryUnitTable.find(Values::SyntaxCategories.scSubjectComplement.GetSingleValue());
				if(i!=m_CategoryUnitTable.end())
				{
					if(bPerson)
						SetSyntaxCategory(pGroup,Values::SyntaxCategories.scWhoDefinition);
					else SetSyntaxCategory(pGroup,Values::SyntaxCategories.scWhatDefinition);
					SetSyntaxCategory(i->second,Values::SyntaxCategories.scMainEntity);
				}
			}
		}

		void CSMQuestionParser::AdjunctMeasureProcess(ISentence * pSentence, IUnit * pGroup,CSMQuestionParser::EQuestionType eQType)
		{
			if (!pGroup || eQType==CSMQuestionParser::eqtNon) return;

			if(CheckAimSemSyntAndMoveToAjunct(pSentence,pGroup,Values::SyntaxCategories.scAjunctMeasure,eQType) && (eQType==CSMQuestionParser::eqtHow)) return;

			IUnit * pUnit,*pUnitNext;
			if (eQType==CSMQuestionParser::eqtWhat)
			{
				CSMQuestionParser::TCategoryUnitTable::iterator i=m_CategoryUnitTable.find(Values::SyntaxCategories.scSubjectFin.GetSingleValue());
				if (i==m_CategoryUnitTable.end()) return;
				pUnit=i->second->GetLeftChildUnit();
			}else if (eQType==CSMQuestionParser::eqtHow) pUnit=pGroup->GetLeftChildUnit();
			if(eQType==CSMQuestionParser::eqtWhat || eQType==CSMQuestionParser::eqtHow)
			{
				/*if (CheckManyMuch(pGroup))
				{
					if(!IsContainSemanticIndex(m_questSemNumber,pGroup))
					{
						AddSemanticIndex(pGroup,m_questSemNumber);
					}
				}*/

				SS::Core::Features::CSemanticFeature oSemanticFeature;
				list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
				if (pUnit)
				{
					do{
						if(
							pUnit->GetSyntaxFeature() && 
							pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Equal(Values::SyntaxCategories.scDeterminer)
							)
							continue;
						
						//wstring str = pUnit->GetWord();
						oSemanticFeature.Undefine();
						CollectSemanticFeature(oSemanticFeature,pUnit);
						if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure))
						{
							if(oSemanticFeature.m_SMeasureType.GetValue()<=1) 
							{
								DeleteSyntaxCategory(pGroup,Values::SyntaxCategories.scAjunctMeasure);
								if(!IsContainSemanticIndex(m_questSemNumber.GetDictionaryIndex(),pGroup))
									AddSemanticIndex(pGroup,m_questSemNumber);//What is South Africa's total population? South Africa has a total population of about 42 million, among which about 75 percent are the blacks....
							}else if (!(eQType==CSMQuestionParser::eqtHow))
							{
								RemoveSemanticIndicesFromUnit(l_Semantic, pUnit);
								KeepSpecifedSemanticTypes(Values::TSemanticType.smtMeasure,l_Semantic);
								SetSemanticIndices(l_Semantic, pGroup);
								SetSemanticIndices(l_Semantic, pUnit);
							}

							if(!oSemanticFeature.m_SMeasureType.IsContain(Values::TSMeasureType.msrtPrice))
							{
								pUnitNext=pUnit->Pop();
								pUnit->SetObligatory(false);
								MoveToAgjunct(pSentence,pUnit);
							}
							break;
						}
					}while(pUnit=pUnit->GetRightUnit());
				}
				if(l_Semantic.empty())
				{// verb
					CSMQuestionParser::TCategoryUnitTable::iterator it;

					if (m_CategoryUnitTable.find(Values::SyntaxCategories.scMain.GetSingleValue())!=m_CategoryUnitTable.end())
						it=m_CategoryUnitTable.find(Values::SyntaxCategories.scMain.GetSingleValue());
					else if (m_CategoryUnitTable.find(Values::SyntaxCategories.scAjunctGoal.GetSingleValue())!=m_CategoryUnitTable.end())
						it=m_CategoryUnitTable.find(Values::SyntaxCategories.scAjunctGoal.GetSingleValue());//How much is the international space stations expected to cost?
					else if (m_CategoryUnitTable.find(Values::SyntaxCategories.scPredicate.GetSingleValue())!=m_CategoryUnitTable.end())
						it=m_CategoryUnitTable.find(Values::SyntaxCategories.scPredicate.GetSingleValue());
					else
						return;

					pUnit=it->second->GetLeftChildUnit();
					do{
						if(pUnit->GetSyntaxFeature() && !pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scChain))
						{
							oSemanticFeature.Undefine();
							CollectSemanticFeature(oSemanticFeature,pUnit);
							SS::Dictionary::Types::TDictionaryIndex lSem=0;
							if( oSemanticFeature.m_SVerbType.IsContain(Values::TSVerbType.semvtCost)||
								oSemanticFeature.m_SVerbType.IsContain(Values::TSVerbType.semvtWeigh))
							{
								oSemanticFeature.m_SemanticType=Values::TSemanticType.smtMeasure;
								if(oSemanticFeature.m_SVerbType.IsContain(Values::TSVerbType.semvtCost))
									oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtPrice;
								else oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtWeight;
								m_pIAMCSemantic->Code(&oSemanticFeature,&lSem);
								AddSemanticIndex(pGroup,lSem);
							}
						}
					}while(pUnit=pUnit->GetRightUnit());
				}
			}
		}

		void CSMQuestionParser::AjunctProcess(ISentence * pSentence,IUnit * pGroup,CSMQuestionParser::EQuestionType eQType)
		{
			if (!pGroup || eQType==CSMQuestionParser::eqtNon) return;
			
			IUnit * pUnit=NULL;

			if(CheckAimSemSyntAndMoveToAjunct(pSentence,pGroup,Values::SyntaxCategories.scObjectOfPreposition,eQType)) 
				return;

			if (pUnit=pGroup->GetLeftChildUnit())
			{
				do{
					if(
						pUnit->GetSyntaxFeature() && 
						pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Equal(Values::SyntaxCategories.scAjunct))
					{
						if (eQType==CSMQuestionParser::eqtHow)
						{
							//SetSyntaxCategory(pUnit,Values::SyntaxCategories.scAjunctManner);
							//if(!IsFindDie(pSentence))
							//{
								pGroup->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.scAjunctManner);
								break;
							//}
						}
						else if(eQType==CSMQuestionParser::eqtWhy)
						{
							//SetSyntaxCategory(pUnit,Values::SyntaxCategories.scAjunctCause);
							pGroup->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.scAjunctCause);
							break;
						}	
					}
				}while(pUnit=pUnit->GetRightUnit());
	
			}

		}

		bool CSMQuestionParser::IsFindCall(IUnit * pUnit)
		{
			if(!pUnit) return false;
			wstring str;
			if(pUnit->HasChilds())
			{
				IUnit * pUnit2=pUnit->GetLeftChildUnit();
				do{
					 str = pUnit2->GetWord();
					 m_StringService.LowerSz((TCHAR*)str.c_str());
					 if(m_StringService.StrEqual(_T("call"),str.c_str())||m_StringService.StrEqual(_T("calls"),str.c_str())||
						 m_StringService.StrEqual(_T("called"),str.c_str())|| m_StringService.StrEqual(_T("calling"),str.c_str()))
								return true;
				}while(pUnit2=pUnit2->GetRightUnit());
			}

			return false;
		}


		void CSMQuestionParser::FindAndMovePretext(ISentence * pSentence,IUnit * pGroup)
		{
			if(!pSentence || !pGroup) return;

			IUnit * pUnit=NULL;

			if (pUnit=pGroup->GetLeftChildUnit())
			{
				do{
					if(IsPretext(pUnit))
					{
						pUnit->Pop();
						MoveToAgjunct(pSentence,pUnit);
					}
				}while(pUnit=pUnit->GetRightUnit());
			}
		}

		bool CSMQuestionParser::CheckAimSemSyntAndMoveToAjunct(ISentence * pSentence,IUnit * pGroup, const Types::SyntaxCategories & oCategory,CSMQuestionParser::EQuestionType eQType)
		{
			if(!pSentence || !pGroup) return false;

			IUnit * pUnit=NULL;

			if(eQType==CSMQuestionParser::eqtWhat || eQType==CSMQuestionParser::eqtWho)
			{
				if ((pUnit=pGroup->GetLeftChildUnit()) && (pGroup->GetChildCount()<3))
				do{
					if((pUnit->GetSyntaxFeature() &&
						pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(oCategory)) &&
						oCategory.IsContain(Values::SyntaxCategories.scObjectOfPreposition))
					{
						if(eQType==CSMQuestionParser::eqtWho) AddAllPersonSemantic(pGroup);
						FindAndMovePretext(pSentence,pGroup);
						if(IsFindMainDie())
							pGroup->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.scAjunctManner); 
						return true;
					}
				}while(pUnit=pUnit->GetRightUnit());
			}

			IUnit * pPop=NULL;
			TBoxUnits l_Units;
			TBoxUnits::iterator it;
			SS::Core::Features::CSemanticFeature oF;
			map<wstring,CSMQuestionParser::EQuestionType>::iterator iF;
			set<wstring>::iterator i;
			wstring str;
			bool bMuchMany = false;
			bool bSemUndef = false;
			bool bSemPersonUndef = false, bMeasureUndef;
			bool bSemPerson = false;
			bool bSemDef = false;
			bool bGood = false;
			SS::Core::Features::CSyntaxFeatureAposteriori * oSyntaxFeatureAposteriori;
			Types::SyntaxCategories oSOCategory=Values::SyntaxCategories.scAjunct;
			oSOCategory.AssignOR(&Values::SyntaxCategories.scObject);
			list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
			UINT uiDepth=0;

			oF.Undefine();
			CollectSemanticFeature(oF,pGroup);
			if(!IsSemanticUndef(oF,bSemPersonUndef,bMeasureUndef))
				return false;
			if (CheckManyMuch(pGroup))
			{
				if(!IsContainSemanticIndex(m_questSemNumber.GetDictionaryIndex(),pGroup))
					AddSemanticIndex(pGroup,m_questSemNumber);
				DeleteSyntaxCategory(pGroup,oCategory);
				bMuchMany = true;
			}
			if (pUnit=pGroup->GetLeftChildUnit())
			do{
				bMeasureUndef=false;
				oF.Undefine();
				str = pUnit->GetWord();
 				m_StringService.LowerSz((TCHAR*)str.c_str());
				pUnit->SetWeight(float(0.5));
				iF = m_QuestionTypeEng.find(str);
				i = m_StopWords.find(str);

				if(iF == m_QuestionTypeEng.end() && i == m_StopWords.end())
					bGood = true;
				else
					bGood = false;

				CollectSemanticFeature(oF,pUnit);
				if((pUnit->GetSyntaxFeature() &&
					pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(oCategory))
					&& bGood && (IsSemanticUndef(oF,bSemPersonUndef,bMeasureUndef) || bMuchMany)
					&& (!m_StringService.StrEqual(_T("person"),str.c_str())))
				{
					bSemUndef = true;
					if (bSemPersonUndef && ((pGroup->GetChildCount() < 3) || uiDepth > 1)&&
						(oF.m_SPersonType.IsContain(Values::TSPersonType.semprstAgent)||
						oF.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession) ||
						oF.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality)))
							bSemPerson = true;
					if (bMeasureUndef) 
					{
						if(!IsContainSemanticIndex(m_questSemNumber.GetDictionaryIndex(),pGroup))
							AddSemanticIndex(pGroup,m_questSemNumber);
						DeleteSyntaxCategory(pGroup,oCategory);
					}
					oSyntaxFeatureAposteriori=pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori();
					if(oSyntaxFeatureAposteriori->m_SyntaxCategories.IsIntersectsWith(&oSOCategory))
						FindAndMovePretext(pSentence,pGroup);
					l_Units.push_back(pUnit);
				}else if((bGood && !IsSemanticUndef(oF,bSemPersonUndef,bMeasureUndef))  &&
					!pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAttribute)
					)
				{
					if(!bSemDef)
					{
						bSemDef = true;
						//if(!oCategory.IsContain(Values::SyntaxCategories.scObjectOfPreposition) || IsContainFeature(efEClearProper,pUnit))
						{
							RemoveSemanticIndicesFromUnit(l_Sem,pUnit);
							if(oCategory.IsContain(Values::SyntaxCategories.scAjunctMeasure))
								KeepSpecifedSemanticTypes(Values::TSemanticType.smtMeasure,l_Sem);	
							SetSemanticIndices(l_Sem,pGroup);
							SetSemanticIndices(l_Sem,pUnit);
						}
					}
				}else
				{
					if(bGood && IsSemanticUndef(oF,bSemPersonUndef,bMeasureUndef) && !IsPretext(pUnit))
							l_Units.push_back(pUnit);
				}
				pUnit = pUnit->GetRightUnit();
				if(!pUnit) break;
				uiDepth++;
			}while(true);

			if (!bSemUndef) return false;

			if(l_Units.empty()==false)
			{
				pUnit=m_pIBlackBoxFactory->CreateUnit();
				for(it=l_Units.begin(); it!=l_Units.end(); it++)
				{
					(*it)->SetObligatory(false);
					pPop = (*it);
					(*it)->Pop();
					pUnit->AddUnitVertical(pPop);
					//(*it)->Pop();
					//MoveToAgjunct(pSentence,(*it));

				}
				MoveToAgjunct(pSentence,pUnit);
				l_Units.clear();
			}
			if(bSemPersonUndef && !bMuchMany && !bSemDef && !bMeasureUndef)
			{
				/*if(bSemPersonNationality)	
					CSMExpert::AddSemanticIndex(m_questSemNationality,pGroup);
				else*/
				if(bSemPerson)
				{
					AddSemanticIndex(pGroup,m_questSemFirstName);
					AddSemanticIndex(pGroup,m_questSemLastName);
				}
			}
			/*if(!bMuchMany && !bSemPersonUndef && !bSemDef)
				pGroup->ReleaseUnitHorizontal();*/

			return true;
		}

		bool CSMQuestionParser::IsSemanticUndef(SS::Core::Features::CSemanticFeature oSemanticFeature, bool & bPersonUndef, bool & bMeasureUndef)
		{
			bool bSemUndef = false;
			if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure)){
				bSemUndef = (oSemanticFeature.m_SMeasureType.GetValue()<=1);
				if (bSemUndef) bMeasureUndef = true;
				else return false;
			}
			if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)) 
			{
				bSemUndef = (oSemanticFeature.m_SPlaceType.GetValue()<=1);
				if(!bSemUndef) return false;
			}
			if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtTimeInterval))
			{
				bSemUndef = (oSemanticFeature.m_STimeIntervalType.GetValue()<=1);
				if(!bSemUndef) return false;
			}
			if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization))
			{
				bSemUndef = (oSemanticFeature.m_SOrganizationType.GetValue()<=1);
				if(!bSemUndef) return false;
			}
			if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtService))
			{
				bSemUndef = (oSemanticFeature.m_SServiceTypes.GetValue()<=1);
				if(!bSemUndef) return false;
			}
			if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
			{
				if(!oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstActor))
				{
					bPersonUndef = true;
					return true;
				}else return false;
			}
			
			return true;
		}

		bool CSMQuestionParser::IsFindMainDie()
		{
			CSMQuestionParser::TCategoryUnitTable::iterator it=m_CategoryUnitTable.find(Values::SyntaxCategories.scMain.GetSingleValue());
			if((it->second)->HasChilds())
			{
				wstring str;
				IUnit * pUnit=(it->second)->GetLeftChildUnit();
				do{
					str = pUnit->GetWord();
					m_StringService.LowerSz((TCHAR*)str.c_str());
					if(m_StringService.StrEqual(_T("die"),str.c_str())||m_StringService.StrEqual(_T("dies"),str.c_str())||
						m_StringService.StrEqual(_T("died"),str.c_str())|| m_StringService.StrEqual(_T("dying"),str.c_str()))
						return true;
				}while(pUnit=pUnit->GetRightUnit());
			}
			return false;
		}

		bool CSMQuestionParser::CheckManyMuch(IUnit * pGroup)
		{
			if(!pGroup) return false;

			//bool bMuchMany;
			IUnit * pUnit;
			wstring str;

			if (pUnit=pGroup->GetLeftChildUnit())
				do{
					str=pUnit->GetWord();
					m_StringService.LowerSz((TCHAR*)str.c_str());
					if(m_StopWords.find(str) != m_StopWords.end()) return true;
					//bMuchMany=m_StringService.StrEqual(str.c_str(),_T("many")) ||m_StringService.StrEqual(str.c_str(),_T("much"));
					//if (bMuchMany) return true;
				}while(pUnit=pUnit->GetRightUnit());
			return false;
		}		

		void CSMQuestionParser::MoveToAgjunct(ISentence * pSentence,IUnit * pUnit)
		{
			pUnit->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.scAjunct);
			pSentence->AddUnit(pUnit);
		}

		bool CSMQuestionParser::KeyQuestionProcess()
		{
			list<IUnit*> l_Units;
			IUnit * pUnit;
			if ( (pUnit=GetFirstUnit()) )
			{	
				do{
					l_Units.push_back(pUnit);
				}while( (pUnit=GetNextUnit()) );
			}
			if (l_Units.size()>1)
			{
				pUnit=m_pSMManager->GetBlackBoxTextStorage()->CreateUnit();
				pUnit->JoinUpInterval(l_Units.front(),l_Units.back());
			}else pUnit=GetFirstUnit();
			if(!pUnit) return false;

			SetSyntaxCategory(pUnit,Values::SyntaxCategories.scMainEntity);
			return true;
		}
		void CSMQuestionParser::ProcessSentence(ISentence * pSentence)
		{
			m_CategoryUnitTable.clear();
			IUnit * pUnit; 
			ISyntaxFeature * pSyntaxFeature;

			if ( (pUnit=pSentence->GetFirstUnit()) )
			{
				do{
					if(pSyntaxFeature=pUnit->GetSyntaxFeature())
					{

					    if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunctGoal))
							m_CategoryUnitTable.insert(map<UINT,IUnit*>::value_type(Values::SyntaxCategories.scAjunctGoal.GetSingleValue(),pUnit));

						else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scPredicate))
							m_CategoryUnitTable.insert(map<UINT,IUnit*>::value_type(Values::SyntaxCategories.scPredicate.GetSingleValue(),pUnit));

						else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObject))
							m_CategoryUnitTable.insert(map<UINT,IUnit*>::value_type(Values::SyntaxCategories.scObject.GetSingleValue(),pUnit));

						else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectFin))
							m_CategoryUnitTable.insert(map<UINT,IUnit*>::value_type(Values::SyntaxCategories.scSubjectFin.GetSingleValue(),pUnit));

						else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectComplement))
							m_CategoryUnitTable.insert(map<UINT,IUnit*>::value_type(Values::SyntaxCategories.scSubjectComplement.GetSingleValue(),pUnit));

						else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunct))
							m_CategoryUnitTable.insert(map<UINT,IUnit*>::value_type(Values::SyntaxCategories.scAjunct.GetSingleValue(),pUnit));

						else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scMain))
							m_CategoryUnitTable.insert(map<UINT,IUnit*>::value_type(Values::SyntaxCategories.scMain.GetSingleValue(),pUnit)); 
					}
				}while(	pUnit=pUnit->GetRightUnit() );
			}
		}
		void CSMQuestionParser::AddAllTimeIntervalSemantic(IUnit * pUnit)
		{
			SS::Dictionary::Types::TDictionaryIndex lVal=0;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtTimeInterval;

			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.Undefined;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateYear;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);
			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateDay;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);
			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateMonth;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);
			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDayTime;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);
			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitCentury;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);
		}
		void CSMQuestionParser::AddAllPlaceSemantic(IUnit * pUnit)
		{
			SS::Dictionary::Types::TDictionaryIndex lVal=0;
			SS::Core::Features::CSemanticFeature oSemanticFeature;

			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;

			oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltState;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltCity;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltContinent;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltCountry;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltProvince;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPlaceType.Undefine();
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

		}

		void CSMQuestionParser::AddAllPersonSemantic(IUnit * pUnit)
		{
			SS::Dictionary::Types::TDictionaryIndex lVal=0;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstFirstName;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstLastName;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstMiddleName;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstAgent;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstNationality;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstProfession;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstName;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstActor;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);
		}
		void CSMQuestionParser::AddAllPersonNameSemantic(IUnit * pUnit)
		{
			SS::Dictionary::Types::TDictionaryIndex lVal=0;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstFirstName;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstLastName;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstMiddleName;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstName;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);

			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstActor;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			AddSemanticIndex(pUnit,lVal);
		}
		void CSMQuestionParser::AddSemanticIndex(IUnit * pUnit, SS::Dictionary::Types::TDictionaryIndex & oSemType)
		{
			if (!pUnit) return;
			IIndex *pIndex=NULL;
			oSemType.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
			pIndex=m_pIBlackBoxFactory->CreateIndex();
			pIndex->SetIndexationFlag(true);
			pIndex->GetDictionaryIndex()->AppendIndex(oSemType);			
			pUnit->AddIndex(pIndex);
		}

		bool CSMQuestionParser::CheckWhoCategory(IUnit * pHead, IUnit * pGroup)
		{
			if (pGroup->HasChilds()==false) return false;

			IUnit * pUnit;
			IUnit * pUnit2;
			wstring str;
			 
			pUnit=pGroup->GetLeftChildUnit();
			do{
				str = pUnit->GetWord();
				m_StringService.LowerSz((TCHAR*)str.c_str());
				if(m_StringService.StrEqual(_T("a"),str.c_str()) ||
					(IsContainFeature(efECaseGenetive,pUnit)))
				{
					continue;
				}

				if(pUnit->HasChilds())
				{
					if(CheckWhoCategoryProcess(pHead,pUnit))
						return true;
					pUnit2 = pUnit->GetLeftChildUnit();
					do{
						if(CheckWhoCategoryProcess(pHead,pUnit2,pGroup))
							return true;
					}while(pUnit2=pUnit2->GetRightUnit());				
				}else
				{
					if(CheckWhoCategoryProcess(pHead,pUnit))
						return true;
				}
				
			}while(pUnit=pUnit->GetRightUnit());

			SetSyntaxCategory(pHead,Values::SyntaxCategories.scAdditionalEntity);
			return false;
		}

		bool CSMQuestionParser::CheckWhoCategoryProcess(IUnit * pHead, IUnit * pUnit, IUnit * pGroup)
		{
			if (!pHead || !pUnit) return false;
			if(IsContainFeature(efECaseGenetive,pUnit)) return false;

			SS::Core::Features::CSemanticFeature oF;

			oF.Undefine();
			CollectSemanticFeature(oF,pUnit);

			if(oF.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure)) 
				return false;

			if (oF.m_SPersonType.IsContain(Values::TSPersonType.semprstName)||
				oF.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName)||
				oF.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)
				)
			{
				SetSyntaxCategory(pHead,Values::SyntaxCategories.scNameDefinition);
				if(pGroup==NULL)
				{
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pUnit);
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pUnit);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition,pUnit);	
				}else
				{
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pGroup);
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pGroup);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition,pGroup);
					//AddSyntaxCategory(pGroup,Values::SyntaxCategories.scWhoDefinition);
				}
				return true;
			}else if (
				oF.m_SemanticType.IsContain(Values::TSemanticType.smtPlace) || 
				oF.m_SemanticType.IsContain(Values::TSemanticType.smtPerson)
				)
			{
				SetSyntaxCategory(pHead,Values::SyntaxCategories.scWhoDefinition);
				if(pGroup==NULL)
				{
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pUnit);
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pUnit);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition,pUnit);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition,pUnit);
				}else
				{
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pGroup);
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pGroup);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition,pGroup);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scNameDefinition,pGroup);
				}

				return true;
			}
			return false;
		}		
		
		bool CSMQuestionParser::CheckWhatCategory(IUnit * pHead, IUnit * pGroup)
		{
			if (pGroup->HasChilds()==false) return false;

			IUnit * pUnit;
			IUnit * pUnit2;
			wstring str;

			pUnit=pGroup->GetLeftChildUnit();
			do{
				str = pUnit->GetWord();
				m_StringService.LowerSz((TCHAR*)str.c_str());
				if(m_StringService.StrEqual(_T("a"),str.c_str()))
				{
					continue;
				}

				if(pUnit->HasChilds())
				{
					if(CheckWhatCategoryProcess(pHead,pUnit))
						return true;
					pUnit2 = pUnit->GetLeftChildUnit();
					do{
						//str = pUnit2->GetWord();
						if(CheckWhatCategoryProcess(pHead,pUnit2,pGroup))
							return true;
					}while(pUnit2=pUnit2->GetRightUnit());				
				}else
				{
					if(CheckWhatCategoryProcess(pHead,pUnit))
						return true;
				}
			}while(pUnit=pUnit->GetRightUnit());

			SetSyntaxCategory(pHead,Values::SyntaxCategories.scAdditionalEntity);
			return false;
		}

		bool CSMQuestionParser::CheckWhatCategoryProcess(IUnit * pHead, IUnit * pUnit, IUnit * pGroup)
		{
			SS::Core::Features::CSemanticFeature oF;
			bool bGood = false;

			oF.Undefine();
			CollectSemanticFeature(oF,pUnit);
			bGood = !IsContainFeature(efECaseGenetive,pUnit) && !IsDeterminer(pUnit) &&
				pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectFin)||
				pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObject);

			if(oF.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure) || 
				pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAttribute))
				return false;
			else if(oF.m_SPersonType.GetValue()>1 && bGood)
			{
				CorrectFuncForPersonServiceSem(pHead, pUnit,oF);
				return false;
			}
			else if (bGood && !oF.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
			{
				if (oF.m_SPlaceType.GetValue()>1 ||
					oF.m_STimeIntervalType.GetValue()>1 ||
					oF.m_SServiceTypes.GetValue()>1)
				{
					Types::TSemanticType oSemanticType=Values::TSemanticType.smtTimeInterval;
					oSemanticType.AssignOR(&Values::TSemanticType.smtPlace);
					oSemanticType.AssignOR(&Values::TSemanticType.smtService);
					list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
					RemoveSemanticIndicesFromUnit(l_Semantic, pUnit);
					KeepSpecifedSemanticTypes(oSemanticType,l_Semantic);
					SetSemanticIndices(l_Semantic, pHead);
				}
				SetSyntaxCategory(pHead,Values::SyntaxCategories.scWhatDefinition);
				if(pGroup==NULL)
				{
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pUnit);
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pUnit);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhatDefinition,pUnit);
				}else
				{
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pGroup);
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pGroup);
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scWhatDefinition,pGroup);
				}
				
				return true;
			}
			return false;
		}

		void CSMQuestionParser::CorrectFuncForPersonServiceSem(IUnit * pHead, IUnit * pUnit, SS::Core::Features::CSemanticFeature oSemanticFeature)
		{
			if(!pHead || !pUnit) return;

			Types::TSPersonType oSemanticPersonType=Values::TSPersonType.semprstProfession;
			oSemanticPersonType.AssignOR(&Values::TSPersonType.semprstNationality);

			if(oSemanticFeature.m_SPersonType.IsIntersectsWith(&oSemanticPersonType))
			{
				list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
				RemoveSemanticIndicesFromUnit(l_Semantic, pUnit);
				KeepSpecifedSemanticTypes(Values::TSemanticType.smtPerson,l_Semantic);
				SetSemanticIndices(l_Semantic, pHead);

				pUnit->Pop();
				pUnit->SetObligatory(false);
				MoveToAgjunct(m_pCurrentSentence,pUnit);
			}

			return;
		}
//////////////////////////////////////////////////////////////////////////////
}
}
}
}