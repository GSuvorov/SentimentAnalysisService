#include "StdAfx.h"
#include ".\smwordsremover.h"

namespace SS
{
namespace Semantic
{
namespace PostAnalysisExperts
{
namespace PostSyntaxAnalysis
{

CSMWordsRemover::CSMWordsRemover(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMWordsRemover::~CSMWordsRemover(void)
{
}


bool CSMWordsRemover::IsContainSyntaxCategories(IUnit * pUnit, Types::SyntaxCategories oSyntaxCategories)
{
	SS_TRY
	{
		if(oSyntaxCategories.IsUndefined())
			return true;

		if(pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsIntersectsWith(&oSyntaxCategories) )
			return true;
		
		return false;
	}
	SS_CATCH(L"")
}


void CSMWordsRemover::RemoveWord(IUnit * pUnit, const wchar_t * wzRemoveWord, Types::SyntaxCategories oSyntaxCategories)
{
	SS_TRY
	{
		for(;pUnit; pUnit = pUnit->GetRightUnit())
		{
			if(pUnit->HasChilds())
			{
				RemoveWord(pUnit->GetLeftChildUnit(), wzRemoveWord, oSyntaxCategories);
				if(!pUnit->HasChilds())
				{
					pUnit = pUnit->ReleaseUnitHorizontal();
					if(!pUnit)
						return;
				}
			}

			const TCHAR * str = pUnit->GetWord();
			m_StringService.LowerSz((TCHAR*)str);		
			if(m_StringService.StrEqual(str,wzRemoveWord) && IsContainSyntaxCategories(pUnit, oSyntaxCategories) )
			{
				IUnit * pPop;
				pPop = pUnit;
				pUnit = pUnit->Pop();
				pPop->ReleaseUnitHorizontal();
				if(!pUnit)
					return;
			}
		}
		return;
	}
	SS_CATCH(L"")
}

void CSMWordsRemover::RemoveWord(const wchar_t * wzRemoveWord, Types::SyntaxCategories oSyntaxCategories)
{
	SS_TRY
	{
		RemoveWord(GetFirstUnit(), wzRemoveWord, oSyntaxCategories);
	}
	SS_CATCH(L"")
}

bool CSMWordsRemover::Remove()
{
	SS_TRY
	{

		IUnit * pUnit, * pPop;
		bool bCurrentUnitRemove;

		if (!GetFirstUnit()) return true;
		do
		{
			bCurrentUnitRemove = false;

			if (m_pCurrentUnit->GetSyntaxFeature() && 
				m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.GetValue()>1)
			{
				if (pUnit=m_pCurrentUnit->GetLeftChildUnit())
				{
					do{
						const TCHAR * str = pUnit->GetWord();
						m_StringService.LowerSz((TCHAR*)str);
						if(!(GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamOnlyKeyWords) &&
							(IsLegalLexem(pUnit)||
							m_StringService.StrEqual(str,_T("s"))||
							m_StringService.StrEqual(str,_T("name"))||
							m_StringService.StrEqual(str,_T("call"))||
							m_StringService.StrEqual(str,_T("there"))||
							m_StringService.StrEqual(str,_T("nickname"))||
							m_StringService.StrEqual(str,_T("surname")))
							)
						{
							if(m_StringService.StrEqual(str,_T("name")))
								CheckAndRemoveAdjectiveForNames(pUnit);
							pPop = pUnit;
							pUnit = pUnit->Pop();
							pPop->ReleaseUnitHorizontal();
							bCurrentUnitRemove = true;
							if(!pUnit) break;
						}
						if (pUnit->GetSyntaxFeature())
						{
							SetObligatory(pUnit);
							if (
								pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scDeterminer)||
								IsPartOfSpeech(pUnit)
								||pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scChain)||
								pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scVerbModifier)
								)
							{
								pPop=pUnit;
								pUnit=pUnit->Pop();
								if(pPop->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scChain))
								{
									m_pCurrentSentence->AddUnit(pPop);
								}else
								{
									pPop->ReleaseUnitHorizontal();
									bCurrentUnitRemove = true;
								}
								
							}else pUnit=pUnit->GetRightUnit();
						}else 
						{
							if(!m_StringService.StrEqual(str/*pUnit->GetWord()*/,_T("?")))
								pUnit=pUnit->GetRightUnit();
							else 
							{
								pUnit->ReleaseUnitHorizontal();
								bCurrentUnitRemove = true;
							}
						}
					}while(pUnit);
				}
				if(!IsDeleteCurrentUnit(bCurrentUnitRemove))
					GetNextUnit();
			}else
			{
				if((m_pCurrentUnit->HasChilds()) && (pUnit=m_pCurrentUnit->GetLeftChildUnit()))
				{
					if(IsLegalLexem(pUnit))
					{
						m_pCurrentUnit=m_pCurrentUnit->ReleaseUnitHorizontal();
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

bool CSMWordsRemover::IsNotMostInDegree(SS::Core::Features::CMorphoFeature & oMorphoFeature)
{
	if (oMorphoFeature.m_DegreeType.Equal(Values::DegreeType.dtSuperlative)) 
		return false;
	return true;
}

void CSMWordsRemover::CheckAndRemoveAdjectiveForNames(const IUnit * pUnit)
{
	if(!pUnit) return;
	Mem();
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	if (!GetFirstUnit()) return;
	do{

		CollectSemanticFeature(oSemanticFeature);
		const TCHAR * str = m_pCurrentUnit->GetWord();
		m_StringService.LowerSz((TCHAR*)str);
		if (!oSemanticFeature.m_SemanticType.Equal(Values::TSemanticType.smtPerson) &&
			IsContainFeature(efcAdverbAdjective) && (m_pCurrentUnit!=pUnit) && !IsContainFeature(efEClearProper) &&
			!m_StringService.StrEqual(str,_T("what")) && !m_StringService.StrEqual(str,_T("who")) && !m_StringService.StrEqual(str,_T("which")) &&
			!m_StringService.StrEqual(str,_T("how")))
		{
			IUnit * pPop;
			pPop = m_pCurrentUnit;
			m_pCurrentUnit->Pop();
			pPop->ReleaseUnitHorizontal();
			GotoMem();
			return;
		}
	}while(GetNextUnitWithChild());

	GotoMem();
	return;
}


bool CSMWordsRemover::AnalizeSentence()
{
	SS_TRY
	{
		Remove();
		std::wstring wsRemoveWord = L"get";
		RemoveWord(wsRemoveWord.c_str(), Values::SyntaxCategories.Undefined);
		wsRemoveWord = L"&";
		RemoveWord(wsRemoveWord.c_str(), Values::SyntaxCategories.Undefined);
		return true;
	}
	SS_CATCH(L"")
}

bool CSMWordsRemover::IsDeleteCurrentUnit(bool bDel)
{

	UINT uiChildCount;
	uiChildCount = m_pCurrentUnit->GetChildCount();
	if((uiChildCount == 0) && bDel)
	{
		m_pCurrentUnit=m_pCurrentUnit->ReleaseUnitHorizontal();
		return true;
	}

	return false;
}

bool CSMWordsRemover::IsLegalLexem(IUnit* pUnit)
{
	TCHAR * sz_Word;
	sz_Word=(TCHAR*)pUnit->GetWord();
	if (!sz_Word || /*sz_Word[0]==0 ||*/
		sz_Word[0]=='\"' || sz_Word[0]=='.' || sz_Word[0]==',' || sz_Word[0]==';' || 
		sz_Word[0]=='?' || sz_Word[0]==':' || sz_Word[0] == '\'' || sz_Word[0] == '\\' ||
		sz_Word[0] == '/' || sz_Word[0] == '(' || sz_Word[0] == ')') 
		return true;
	return false;
}

bool CSMWordsRemover::IsPartOfSpeech(IUnit * pUnit)
{
	IIndex * pIndex;
	IFeature * pFeature;
	//UINT uiIndex;
	SS::Core::Features::CMorphoFeature oMorphoFeature;
	bool IsVerb = false;
	bool IsAimPartOfSpeech = false;

	if (pIndex=pUnit->GetFirstIndex())
	{
		do{
			//uiIndex=pIndex->GetDictionaryIndex()->GetFirst();
			if ( pFeature=pIndex->GetFirstFeature() )
			{
				do{
					m_pIAMCMorpho->EnCode(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*uiIndex>>24*/,pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&oMorphoFeature);
					if(
						!IsAimPartOfSpeech && /*IsNotMostInDegree(oMorphoFeature)&&*/
						(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun)&&
						((oMorphoFeature.m_PronounGroup.IsContain(Values::PronounGroup.pngAdjective)&&
						!oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncInterrogative))||
						(oMorphoFeature.m_PronounGroup.IsContain(Values::PronounGroup.pngNoun)&&
						oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncIndividual)&&
						(oMorphoFeature.m_PersonType.Equal(Values::PersonType.ptThirdFace)||
						oMorphoFeature.m_PersonType.Equal(Values::PersonType.ptSecondFace))))||
						oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)||
						oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otDeterminers))
						)
							IsAimPartOfSpeech = true;
					else if(oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb))
							IsVerb = true;
						
				}while(pFeature=pIndex->GetNextFeature());
			}
		}while(pIndex=pUnit->GetNextIndex());
	}
	if(IsAimPartOfSpeech && !IsVerb) return true;
	else return false;
}

void CSMWordsRemover::SetObligatory(IUnit * pUnit)
{
	if (!pUnit) return;
	bool bFindSemName = false;
	if(IsContainFeature(efcSemanticName,pUnit) &&
		(pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectFin) ||
		pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObject) ||
		pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scObjectComplement))
		)
	{
		if(pUnit->HasChilds())
		{
			IUnit * pUnit2 = pUnit->GetLeftChildUnit();
			do{
				CheckNameSemantic(pUnit2);
			}while(pUnit2=pUnit2->GetRightUnit());
		}else
		{
			CheckNameSemantic(pUnit);
		}		
	}
	return;
}

void CSMWordsRemover::CheckNameSemantic(IUnit * pUnit)
{
	if(!pUnit) return;
	if (!pUnit->GetRightUnit())
		return;
	else if(IsNameSemantic(pUnit))
	{
		//wstring str = pUnit->GetWord();
		pUnit->SetObligatory(false);
		return;
	}
	return;
}


bool CSMWordsRemover::IsNameSemantic(IUnit * pUnit)
{
	if (!pUnit) return false;
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	if (CollectSemanticFeature(oSemanticFeature,pUnit))
	{
		if (oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName)||
			oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
			oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName))
		{
			IUnit * pUnit2 = pUnit->GetRightUnit();
			CollectSemanticFeature(oSemanticFeature,pUnit2);
			if(pUnit2 && 
				(oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName)||
				oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
				oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)))
			{
				return true;
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
}
}
}
}