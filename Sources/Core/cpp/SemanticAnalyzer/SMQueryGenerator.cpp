#include "StdAfx.h"
#include ".\smquerygenerator.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace QueryGeneration
			{
		
		CSMQueryGenerator::CSMQueryGenerator(void) : m_pDictionaryInterface(NULL)
		{
			m_bInit = false;
			m_sDescription=_T(__FUNCTION__);
		}

		CSMQueryGenerator::~CSMQueryGenerator(void)
		{
		}

		bool CSMQueryGenerator::AnalizeSentence()
		{
			SS_TRY
			{
				ToStringAll();
				m_pSMManager->SetQueryString(m_sResultQuery);
				return true;
			}
			SS_CATCH(L"")
		}

		bool CSMQueryGenerator::Init(IText * pIText)
		{
			if (CSMExpert::Init(pIText)==false) return false;

			m_sResultQuery.clear();
			
			if(!m_bInit)
			{
				m_bInit = true;

				m_pDictionaryInterface=m_pSMManager->GetDictionaryInterface();
				if (m_ToBeForms.empty())
				{
					m_ToBeForms.insert(wstring(_T("is")));
					m_ToBeForms.insert(wstring(_T("are")));
					m_ToBeForms.insert(wstring(_T("was")));
					m_ToBeForms.insert(wstring(_T("were")));
				}
				if (m_StopWords.empty())
				{
					m_StopWords.insert(wstring(_T("what")));
					m_StopWords.insert(wstring(_T("how")));
					m_StopWords.insert(wstring(_T("many")));
					m_StopWords.insert(wstring(_T("much")));

					m_StopWords.insert(wstring(_T("какой")));
					m_StopWords.insert(wstring(_T("каких")));
					m_StopWords.insert(wstring(_T("какая")));
					m_StopWords.insert(wstring(_T("какую")));
					m_StopWords.insert(wstring(_T("каков")));
					m_StopWords.insert(wstring(_T("какова")));
					m_StopWords.insert(wstring(_T("каковы")));
					m_StopWords.insert(wstring(_T("каком")));
					m_StopWords.insert(wstring(_T("что")));
				}
			}
			
			return true;

		}

		///Проверка юнита что он вопросительное слово
		bool CSMQueryGenerator::IsQuestion(IUnit* pUnit)
		{
			//Находим самый верхний юнит у текущего
			IUnit* pCurUnit = NULL;
			IUnit* pParUnit = pUnit;
			do 
			{
				pCurUnit = pParUnit;
				pParUnit = pCurUnit->GetParentUnit();
			}
			while (pParUnit);
			//Проверка			
			if (pCurUnit->GetSyntaxFeature()) 
			{
				return (pCurUnit->GetSyntaxFeature()->IsQuestion() != 0);
			}
			return false;
		}

		///Формирование строки запроса
		void CSMQueryGenerator::ToStringAll()
		{
			SS_TRY
			{
				m_sResultQuery.clear();
				IUnit* pCurrUnit = GetFirstUnit();
				IUnit* pRightUnit = NULL;
				IUnit* pChildUnit = NULL;
				while (pCurrUnit)
				{
					pChildUnit = pCurrUnit->GetLeftChildUnit();
					if (pChildUnit)
					{
						pCurrUnit = pChildUnit;
						continue;
					}
					{
						//Текущий юнит
						if (!IsQuestion(pCurrUnit))
						{
							m_sResultQuery += pCurrUnit->GetWord();
							m_sResultQuery += L" ";
						}
					}
					pRightUnit = pCurrUnit->GetRightUnit();
					while (pRightUnit == NULL)
					{
						pCurrUnit = pCurrUnit->GetParentUnit();
						if (pCurrUnit == NULL)
						{ break; }
						{
							//Текущий юнит
							if (!IsQuestion(pCurrUnit))
							{
								m_sResultQuery += pCurrUnit->GetWord();
								m_sResultQuery += L" ";
							}
						}
						pRightUnit = pCurrUnit->GetRightUnit();
					}
					pCurrUnit = pRightUnit;
				}
			}
			SS_CATCH(L"")
		}

		void CSMQueryGenerator::ToString()
		{
			bool bRusDef = false;
			map<UINT,wstring> mCatString;
			list<wstring> l_QueryBack,l_QueryFront,l_QueryRusDef;
			list<wstring>::iterator its;
			map<UINT,wstring>::iterator iF;
			IUnit * pUnit, *pMemUnit=NULL;
			ISyntaxFeature * pSyntaxFeature;
			wstring str;
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			Types::TSemanticType oSemanticType=Values::TSemanticType.smtPlace;
			oSemanticType.AssignOR(&Values::TSemanticType.smtTimeInterval);
			
			Types::SyntaxCategories oSyntaxCategories=Values::SyntaxCategories.scAdditionalEntity;
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunct);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceEnd);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceFixedPoint);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceSource);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeDuration);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeEnd);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeFixedPoint);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeSource);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scPredicate);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scMainEntity);
			oSyntaxCategories.AssignOR(&Values::SyntaxCategories.scChain);
			if (pUnit=GetFirstUnit())
			{
				do{
					if (pSyntaxFeature=pUnit->GetSyntaxFeature())
					{
						if( IsSyntax() || ((pSyntaxFeature->IsQuestion()==false) &&
							oSyntaxCategories.IsContain((pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories)))
							)
						{
							str=_T("+");
							if (pUnit->HasChilds())
							{
								pUnit=pUnit->GetLeftChildUnit();
								if (pUnit->HasChilds())
								{
									IUnit * pUnit3 = pUnit->GetLeftChildUnit();

									if(bRusDef && IsRusDef(pUnit3) && (pMemUnit==NULL))
										pMemUnit = pUnit3;

									if(m_StringService.StrEqual(pUnit3->GetWord(),_T("")))
										FillWord(pUnit3,str);
									else
										str+=pUnit3->GetWord();
									while(pUnit3=pUnit3->GetRightUnit())
									{
										str+=_T(" +");
										if(m_StringService.StrEqual(pUnit3->GetWord(),_T("")))
											FillWord(pUnit3,str);
										else
											str+=pUnit3->GetWord();
									}
								}else
								{
									if(bRusDef && IsRusDef(pUnit) && (pMemUnit==NULL))
										pMemUnit=pUnit;

									if(m_StringService.StrEqual(pUnit->GetWord(),_T("")))
										FillWord(pUnit,str);
									else
										str+=pUnit->GetWord();
									while(pUnit=pUnit->GetRightUnit())
									{
										if(bRusDef && IsRusDef(pUnit) && (pMemUnit==NULL))
											pMemUnit=pUnit;

										if (!pUnit->HasChilds())
										{
											str+=_T(" +");
											if(m_StringService.StrEqual(pUnit->GetWord(),_T("")))
												FillWord(pUnit,str);
											else
												str+=pUnit->GetWord();
										}else{
											IUnit * pUnit2=pUnit->GetLeftChildUnit();

											str+=_T(" +");
											if(m_StringService.StrEqual(pUnit2->GetWord(),_T("")))
												FillWord(pUnit2,str);
											else
												str+=pUnit2->GetWord();
											while(pUnit2=pUnit2->GetRightUnit())
											{
												str+=_T(" +");
												if(m_StringService.StrEqual(pUnit2->GetWord(),_T("")))
													FillWord(pUnit2,str);
												else
													str+=pUnit2->GetWord();
											}
										}
									};
								}
							}else str+=pUnit->GetWord();
							if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scMainEntity))
								mCatString.insert(map<UINT,wstring>::value_type(Values::SyntaxCategories.scMainEntity.GetSingleValue(),str));
							else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAdditionalEntity))
								mCatString.insert(map<UINT,wstring>::value_type(Values::SyntaxCategories.scAdditionalEntity.GetSingleValue(),str));
							else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scPredicate))
								mCatString.insert(map<UINT,wstring>::value_type(Values::SyntaxCategories.scPredicate.GetSingleValue(),str));
							else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scChain))
								mCatString.insert(map<UINT,wstring>::value_type(Values::SyntaxCategories.scChain.GetSingleValue(),str));
							else{
								l_QueryBack.push_back(str);
							//	mCatString.insert(map<UINT,wstring>::value_type(Values::SyntaxCategories.scAjunct.GetSingleValue(),str));
							}
						}else if ( pSyntaxFeature->IsQuestion())
						{
							
							if(!bRusDef && (bRusDef=IsRusDef(pUnit))) 
							{
								str=_T(" +");
								str+=pUnit->GetLeftChildUnit()->GetWord();
								l_QueryRusDef.push_back(str);
							}
							//QuestionProcess(pUnit)
							/*if (str.empty()==false)
							{
								if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scMainEntity))
									mCatString.insert(map<UINT,wstring>::value_type(Values::SyntaxCategories.scMainEntity.GetSingleValue(),str));
								else if (pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAdditionalEntity))
									mCatString.insert(map<UINT,wstring>::value_type(Values::SyntaxCategories.scAdditionalEntity.GetSingleValue(),str));
								else l_QueryBack.push_back(str);

							}*/
						}
//						else 
//						{
////							if(CollectSemanticFeature(pUnit,oSemanticFeature))
//							{
//								str=_T(" +");
//								str+=pUnit->GetWord();
//								l_QueryFront.push_back(str);
//								oSemanticFeature.Undefine();
//							}
//							str.clear();
//						}
					}
					else 
					{
//						if(CollectSemanticFeature(pUnit,oSemanticFeature))
						{
							str=_T(" +");

							if (pUnit->HasChilds())
							{
								pUnit=pUnit->GetLeftChildUnit();
								if (pUnit->HasChilds())
								{
									pUnit=pUnit->GetLeftChildUnit();
									if(m_StringService.StrEqual(pUnit->GetWord(),_T("")))
										FillWord(pUnit,str);
									else
										str+=pUnit->GetWord();
									while(pUnit=pUnit->GetRightUnit())
									{
										str+=_T(" +");
										if(m_StringService.StrEqual(pUnit->GetWord(),_T("")))
											FillWord(pUnit,str);
										else
											str+=pUnit->GetWord();
									}
								}else
								{
									if(m_StringService.StrEqual(pUnit->GetWord(),_T("")))
										FillWord(pUnit,str);
									else
										str+=pUnit->GetWord();
									while(pUnit=pUnit->GetRightUnit())
									{
										str+=_T(" +");
										if(m_StringService.StrEqual(pUnit->GetWord(),_T("")))
											FillWord(pUnit,str);
										str+=pUnit->GetWord();
									}
								}
							}else
							{
								if(m_StringService.StrEqual(pUnit->GetWord(),_T("")))
									FillWord(pUnit,str);
								else
									str+=pUnit->GetWord();
							}
							l_QueryFront.push_back(str);
							oSemanticFeature.Undefine();
						}
						
					}
				}while(pUnit=GetNextUnit());
			}
			for ( its=l_QueryFront.begin();its!=l_QueryFront.end();its++) 
			{
				m_sResultQuery+=_T(" ");
				m_sResultQuery+=*its;
			}

			if (l_QueryFront.empty()==false) m_sResultQuery+=_T(" ");

			iF=mCatString.find(Values::SyntaxCategories.scMainEntity.GetSingleValue());
			if ( iF!=mCatString.end())
			{
				/*if(bRusDef)
				{
					m_sResultQuery+=_T(" +");
					m_sResultQuery+=_T("-");
					m_sResultQuery+=_T(" +");
					m_sResultQuery+=_T("это");
				}*/
				if(bRusDef && (l_QueryRusDef.empty()==false))
				{
					SS::Core::Features::CMorphoFeature oMorphoFeature;
					for (its=l_QueryRusDef.begin();its!=l_QueryRusDef.end();its++) 
					{
						m_sResultQuery+=_T(" ");
						m_sResultQuery+=*its;
					}
					if(pMemUnit)
					{
						CollectMorphoFeature(oMorphoFeature,pMemUnit);
						bool bRCaseNominative = IsContainFeature(efRCaseNominative,pMemUnit);
						bool bRCaseGenetive = IsContainFeature(efRCaseGenetive,pMemUnit);
						bool bRMasculineGender = oMorphoFeature.m_GenderType.IsContain(Values::GenderType.gtMasculine);
						bool bRFemaleGender = oMorphoFeature.m_GenderType.IsContain(Values::GenderType.gtFemale);
						if(bRCaseNominative && bRFemaleGender)
						{
							m_sResultQuery+=_T(" +");
							m_sResultQuery+=_T("такая");
						}
						else if(bRMasculineGender && bRCaseNominative)
						{
							m_sResultQuery+=_T(" +");
							m_sResultQuery+=_T("такой");
						}
						else
						{
							m_sResultQuery+=_T(" +");
							m_sResultQuery+=_T("такое");
						}
						m_sResultQuery+=_T(" ");
					}
				}
				m_sResultQuery+=iF->second;

			}
			iF=mCatString.find(Values::SyntaxCategories.scChain.GetSingleValue());
			if ( iF!=mCatString.end())
			{
				m_sResultQuery+=_T(" ");
				m_sResultQuery+=iF->second;
			}
			iF=mCatString.find(Values::SyntaxCategories.scPredicate.GetSingleValue());
			if ( iF!=mCatString.end())
			{
				m_sResultQuery+=_T(" ");
				m_sResultQuery+=iF->second;
			}
			iF=mCatString.find(Values::SyntaxCategories.scAdditionalEntity.GetSingleValue());
			if ( iF!=mCatString.end())
			{
				m_sResultQuery+=_T(" ");
				m_sResultQuery+=iF->second;
			}
			//m_sResultQuery+=_T("\"");
			//iF=mCatString.find(Values::SyntaxCategories.scAjunct.GetSingleValue());
			//if ( iF!=mCatString.end())
			//{
			//	m_sResultQuery+=_T(" ");
			//	m_sResultQuery+=iF->second;
			//}
			for ( its=l_QueryBack.begin();its!=l_QueryBack.end();its++) 
			{
				m_sResultQuery+=_T(" ");
				m_sResultQuery+=*its;
			}

		}

		/*void CSMQueryGenerator:: QuestionProcess(IUnit * pUnit)
		{
			if(!pUnit) return;
		
			SS::Core::Features::CSyntaxFeatureAposteriori * pSyntaxFeatureAposteriori=pSyntaxFeature->GetSyntaxFeatureAposteriori();
			CollectSemanticFeature(oSemanticFeature,pUnit);
			if (pUnit->HasChilds())
			{
				pUnit=pUnit->GetLeftChildUnit();
				str=pUnit->GetWord();
				if (!pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunctMeasure)
					&& m_pOfficialVirtualSyntax->IsPrepositionSyntaxExist(&str)||
					(pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunctMeasure)||
					oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
					&&!IsStopWord(str.c_str())
					)
				{
					str=_T("+");
					str+=pUnit->GetWord();								
				}else str.clear();
				while(pUnit=pUnit->GetRightUnit())
				{
					wstring stemp=pUnit->GetWord();
					if (!pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunctMeasure)&&
						m_pOfficialVirtualSyntax->IsPrepositionSyntaxExist(&str)||
						(pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scAjunctMeasure)||
						oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))&&
						!IsStopWord(stemp.c_str())
						)
					{
						str+=_T(" +");
						str+=pUnit->GetWord();								
					}
				};
			}
		}*/

		bool CSMQueryGenerator::IsStopWord(const TCHAR * sWord)
		{
			CStringService oStringService;
			oStringService.LowerSz((TCHAR*)sWord);
			set<wstring>::iterator i=m_StopWords.find(sWord);
			return i!=m_StopWords.end();
		}

		void CSMQueryGenerator::FillWord(IUnit* pUnit, wstring &wWord)
		{
			SS::Dictionary::Types::SWordInfo oInfo;
			SS::Dictionary::Types::SWordIndex oIndex;
			for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
			{
				for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
				{
					oIndex.m_IDForm = pFeature->GetMorphoIndex();
					oIndex.m_DictIndex = pIndex->GetDictionaryIndex()->GetFirst();

					if(!m_pDictionaryInterface->GetWordInfo(&oIndex, &oInfo))
						continue;
					
					wWord+=oInfo.m_wWord;
					return;
				}
			}
		}

		bool CSMQueryGenerator::IsRusDef(IUnit* pUnit)
		{
			if(!pUnit || !m_pCurrentSentence->GetLanguage()==SS::Core::Types::ealRussian) return false;

			if(pUnit->GetSyntaxFeature())
			{
				SS::Core::Features::CSyntaxFeatureAposteriori * pSyntaxFeatureAposteriori=pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori();

				if(pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scNameDefinition) ||
					pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scWhoDefinition) ||
					pSyntaxFeatureAposteriori->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scWhatDefinition))
					return true;
			}

			return false;
		}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
}