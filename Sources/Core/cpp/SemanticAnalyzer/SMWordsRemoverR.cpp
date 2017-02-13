#include "StdAfx.h"
#include ".\smwordsremoverr.h"

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
					CSMWordRemoverR::CSMWordRemoverR(void)
					{
						m_bInit = false;
						m_AnalyseLanguage=SS::Core::Types::ealRussian;
						m_sDescription=_T(__FUNCTION__);
					}

					CSMWordRemoverR::~CSMWordRemoverR(void)
					{
					}

					bool CSMWordRemoverR::Init(IText * pIText)
					{
						if (!CSMSentenceExpert::Init(pIText)) return false;

						if(!m_bInit)
						{
							m_bInit = true;

							m_Words.clear();
							m_Words.insert(wstring(_T("что")));
							m_Words.insert(wstring(_T("где")));
							m_Words.insert(wstring(_T("когда")));
							m_Words.insert(wstring(_T("как")));
							m_Words.insert(wstring(_T("какой")));
							m_Words.insert(wstring(_T("кто")));
							m_Words.insert(wstring(_T("сколько")));
						}

						return true;
					}

					bool CSMWordRemoverR::AnalizeSentence()
					{
						SS_TRY
						{
							SS::Core::Types::EAnalyseLanguage eAnalyseLanguage;
							eAnalyseLanguage = m_pCurrentSentence->GetLanguage();
							if(eAnalyseLanguage == SS::Core::Types::ealUndefined) return true;
							IUnit * pUnit, * pPop;

							if (!GetFirstUnit()) return true;
							do{

								//wstring str = m_pCurrentUnit->GetWord();

								if (m_pCurrentUnit->GetSyntaxFeature() &&
									m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.GetValue()>1)
								{
									if (pUnit=m_pCurrentUnit->GetLeftChildUnit())
									{
										do{

											//str = pUnit->GetWord();
											if (pUnit->GetSyntaxFeature())
											{
												if (
													pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scDeterminer)||
													IsPartOfSpeech(pUnit)
													||pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scChain)
													)
												{
													pPop=pUnit;
													pUnit=pUnit->Pop();
													if(pPop->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scChain) || IsAjunct(pPop))
													{
														m_pCurrentSentence->AddUnit(pPop);
													}else
													{
														pPop->ReleaseUnitHorizontal();
													}

												}else pUnit=pUnit->GetRightUnit();
											}else 
											{
												wstring sz_Word = pUnit->GetWord();
												if((!IsLexem(pUnit) && !IsPartOfSpeech(pUnit)) ||
													IsWord(sz_Word.c_str()) || 
													(pUnit->HasChilds() && pUnit->GetChildCount()>1))
													pUnit=pUnit->GetRightUnit();
												else 
												{
													if(!IsAjunct(pUnit))
														pUnit = pUnit->ReleaseUnitHorizontal();
													else
													{
														pPop=pUnit;
														pUnit=pUnit->Pop();
														m_pCurrentSentence->AddUnit(pPop);
													}
												}
											}
										}while(pUnit);
									}
									if(!IsDeleteCurrentUnit())
										GetNextUnit();
								}else
								{
									if((m_pCurrentUnit->HasChilds()) && (pUnit=m_pCurrentUnit->GetLeftChildUnit()))
									{
										//str = pUnit->GetWord();

										if(IsLexem(pUnit))
										{
											m_pCurrentUnit=m_pCurrentUnit->ReleaseUnitHorizontal();
										}else if(IsPartOfSpeech(pUnit))
										{
											pPop=pUnit;
											pUnit->Pop();
											pPop->ReleaseUnitHorizontal();
											GetNextUnit();
										}else
											GetNextUnit();

									}else
										m_pCurrentUnit=m_pCurrentUnit->ReleaseUnitHorizontal();
								}
							}while(m_pCurrentUnit);

							return true;
						}
						SS_CATCH(L"")
					}

					bool CSMWordRemoverR::IsAjunct(IUnit * pUnit)
					{
						if(pUnit==NULL) return false;

						if(IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scAjunct,pUnit) && m_pCurrentUnit->GetSyntaxFeature()->IsQuestion())
						{
							SetSyntaxCategory(pUnit,Values::SyntaxCategories.scAjunct);
							RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scAjunct,pUnit);
							//pUnit->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.scAjunct);
							return true;
						}
						return false;
					}

					bool CSMWordRemoverR::IsDeleteCurrentUnit()
					{
						SS_TRY
						{
							UINT uiChildCount;
							uiChildCount = m_pCurrentUnit->GetChildCount();
							if((uiChildCount == 0) && m_StringService.StrEqual(m_pCurrentUnit->GetWord(),_T("")))
							{
								m_pCurrentUnit=m_pCurrentUnit->ReleaseUnitHorizontal();
								return true;
							}

							return false;
						}
						SS_CATCH(L"")
					}

					bool CSMWordRemoverR::IsLexem(IUnit* pUnit)
					{
						TCHAR * sz_Word;
						sz_Word=(TCHAR*)pUnit->GetWord();
						if (!sz_Word || sz_Word[0]==0 ||
							sz_Word[0]=='\"' || sz_Word[0]=='\'' || sz_Word[0]=='.' || sz_Word[0]==',' || 
							sz_Word[0]==';' || sz_Word[0]=='?' || sz_Word[0]==':' || sz_Word[0]=='!' || 
							(m_LexemType.IsSymbol(pUnit) && IsContainFeature(efcRepresentBecome,pUnit))) return true;
						return false;
					}

					bool CSMWordRemoverR::IsPartOfSpeech(IUnit * pUnit)
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
										if(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun)&&
											oMorphoFeature.m_PronounGroup.IsContain(Values::PronounGroup.pngAdjective)&&
											oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncPossessive)||
											oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)||
											oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otConjunction)||
											oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otDeterminers)
											)
											return true;
									}while(pFeature=pIndex->GetNextFeature());
								}
							}while(pIndex=pUnit->GetNextIndex());
						}
						return false;

					}



					bool CSMWordRemoverR::IsWord(const TCHAR * sWord)
					{
						wstring sz_Word(sWord);
						m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
						set<wstring>::iterator i=m_Words.find(sz_Word.c_str());
						return (i!=m_Words.end());
					}

				}
			}
		}
	}
}