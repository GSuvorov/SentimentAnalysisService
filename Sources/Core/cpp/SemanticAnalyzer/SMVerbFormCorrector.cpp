#include "StdAfx.h"
#include ".\smverbformcorrector.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace QueryGeneration
			{
			
		
		CSMVerbFormCorrector::CSMVerbFormCorrector(void) : m_pDictionaryInterface(NULL)
		{
			m_bInit = false;
			m_sDescription=_T(__FUNCTION__);
		}

		CSMVerbFormCorrector::~CSMVerbFormCorrector(void)
		{
		}

		bool CSMVerbFormCorrector::AnalizeSentence()
		{
			SS_TRY
			{
				IUnit * pUnit;
				ISyntaxFeature * pSyntaxFeature;
				IIndex * pIndex;
				IFeature * pFeature;
				SS::Core::Features::CMorphoFeature oMorphoFeature;
				//wstring str;

				if (pUnit=GetFirstUnit())
				{
					//str = m_pCurrentUnit->GetWord();
					do{
						if (pSyntaxFeature=pUnit->GetSyntaxFeature())
						{
							if(pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scChain))
							{
								//str = m_pCurrentUnit->GetWord();
								break;
							}
						}
					}while(pUnit=GetNextUnit());
				}
				if (!pUnit) return true;


				set<wstring>::iterator itF=m_ToBeForms.find(pUnit->GetWord());
				if (itF!=m_ToBeForms.end()){
					return true;
				}
				pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.Undefined);
				if (pUnit )
				{
					bool bPast=false;
					bool bPresent=false;
					if (pIndex=pUnit->GetFirstIndex())
					{
						do{
							if (pFeature=pIndex->GetFirstFeature())
							{
								do{
									m_pIAMCMorpho->EnCode(/*(*/pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/,pFeature->GetMorphoInfo(),
										pFeature->GetPartOfSpeech(),&oMorphoFeature);
									if(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
										oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtAux)&&
										oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast))
									{
										bPast=true;
										break;
									}else if(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
										oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtAux)&&								
										oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent)&&
										oMorphoFeature.m_PersonType.Equal(Values::PersonType.ptThirdFace)
										)
									{
										bPresent=true;
										break;
									}
								}while(pFeature=pIndex->GetNextFeature());
							}
						}while(pIndex=pUnit->GetNextIndex());
					}
					if (bPast || bPresent)
					{
						IUnit * pAux=pUnit;

						if (pUnit=GetFirstUnit())
						{
							do{
								//str = m_pCurrentUnit->GetWord();
								if (pSyntaxFeature=pUnit->GetSyntaxFeature())
								{
									if(pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scPredicate))
									{
										if(pUnit->HasChilds())
										{
											pUnit=pUnit->GetLeftChildUnit();
											if(pUnit)
											{
												//str = pUnit->GetWord();
												if(IsVerb(pUnit)) break;
												else
												{
													while(pUnit=pUnit->GetRightUnit())
													{
														if(IsVerb(pUnit)) break;
													}
												}
											}
										}
										break;
									}
								}
							}while(pUnit=GetNextUnit());
						}
						/*while(pUnit=GetNextUnit())
						{
							str = m_pCurrentUnit->GetWord();
							if (pSyntaxFeature=pUnit->GetSyntaxFeature())
							{
								if(pSyntaxFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scPredicate))
								{
									break;
								}
							}
						}*/
						CorrectForm(pUnit,pAux,bPast);
					}
				}
				return true;
			}
			SS_CATCH(L"")
		}

		bool CSMVerbFormCorrector::IsVerb(IUnit * pUnit)
		{
			if(!pUnit) return false;

			IFeature * pFeature;
			IIndex * pIndex;
			SS::Core::Features::CMorphoFeature oMorphoFeature;
			//UINT uiIndex;
			if (pIndex=pUnit->GetFirstIndex())
			{
				do{
					//uiIndex=pIndex->GetDictionaryIndex()->GetFirst();
					if (pFeature=pIndex->GetFirstFeature())
					{
						do{
							m_pIAMCMorpho->EnCode(/*uiIndex>>24*/pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber(),pFeature->GetMorphoInfo(),
								pFeature->GetPartOfSpeech(),&oMorphoFeature);
							if(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
								oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtMain))
							{
								return true;
							}
						}while(pFeature=pIndex->GetNextFeature());
					}
				}while(pIndex=pUnit->GetNextIndex());
			}
			
			return false;
		}

		void CSMVerbFormCorrector::CorrectForm(IUnit* pUnit, IUnit * pAux, bool bPast)
		{
			if (pUnit && pAux)
			{
				IIndex * pIndex;
				IFeature * pFeature;
				SS::Core::Features::CMorphoFeature oMorphoFeature;

				if (pIndex=pUnit->GetFirstIndex())
				{
					do{
						if (pFeature=pIndex->GetFirstFeature())
						{
							do{
								m_pIAMCMorpho->EnCode(/*(*/pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/,pFeature->GetMorphoInfo(),
									pFeature->GetPartOfSpeech(),&oMorphoFeature);
								if(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
									oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtMain)										
								)
								{

									SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
									oDictionaryIndex.SetDictionaryIndex(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex());
									list<SS::Dictionary::Types::TWordInfo> l_Morpho;
									list<SS::Dictionary::Types::TWordInfo>::iterator i;
									if(m_pDictionaryInterface->GetWordFormsByIDSource( 
										/*in*/&oDictionaryIndex, /*out*/&l_Morpho)
	
	/*GetFormsByIDSource(pIndex->GetDictionaryIndex()->GetFirst(),&l_Morpho)*/)
									{
										for (i=l_Morpho.begin();i!=l_Morpho.end();i++) 
										{
											oMorphoFeature.Undefine();
											m_pIAMCMorpho->EnCode((i->front().m_WordIndex.m_DictIndex.GetDictionaryIndex())>>24,
												i->front().m_MorphoTypeInfo.m_MorphoInfo,
												i->front().m_SpecialMorpho,
												&oMorphoFeature);
											/*if(
												(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
												oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtMain)) &&
												(oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast) ||
												oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent))
												)
											{
												//wchar_t buf[20];
												//mbstowcs(buf,i->m_source_data.m_sWord.c_str(),20);
												pUnit->SetWord(i->front().m_wWord.c_str());
												pAux->ReleaseUnitHorizontal();
												return;
											}*/
											if (!bPast)
											{
												if(
													oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
													oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtMain)&&
													oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent)
													)
												{
													pUnit->SetWord(i->front().m_wWord.c_str());
													pAux->ReleaseUnitHorizontal();
													return;
												}

											}
											else
											{

												if(
													oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
													oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtMain)&&
													oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast)
													)
												{
													pUnit->SetWord(i->front().m_wWord.c_str());
													pAux->ReleaseUnitHorizontal();
													return;
												}
											}
										}
									}
									break;
								}
							}while( (pFeature=pIndex->GetNextFeature()));
						}
					}while((pIndex=pUnit->GetNextIndex()));
				}
			}
		}

		bool CSMVerbFormCorrector::Init(IText * pIText)
		{
			if (CSMExpert::Init(pIText)==false) return false;
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
			}
	
			return true;

		}

//////////////////////////////////////////////////////////////////////////
}
}
}
}