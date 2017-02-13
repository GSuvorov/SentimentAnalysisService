#include "StdAfx.h"
#include ".\smpassivee.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{


CSMPassiveE::CSMPassiveE(void)
{
	m_bInit = false;
	m_uiBeSourceIndex=0;
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMPassiveE::~CSMPassiveE(void)
{
}

bool CSMPassiveE::AnalizeSentence()
{
	SS_TRY
	{
		Types::OfficialType oOficialSkip;
		oOficialSkip.Assign(Values::OfficialType.otPretext);
		oOficialSkip.AssignOR(&Values::OfficialType.otConjunction);
		oOficialSkip.AssignOR(&Values::OfficialType.otParticle);
	//	SS::Core::Features::CMorphoFeature oMorphoFeature;
		bool bGood=false, bPassive=false;
		UINT uiCount;

		do{
			if(IsContainFeature(efcPretext) && 
				m_LexemType.GetLexemLength()==2 &&
				(m_LexemType.WordEqualsTo(_T("by"))|| m_LexemType.WordEqualsTo(_T("By"))||m_LexemType.WordEqualsTo(_T("BY")))
				)
			{
				Mem();
				while(GetNextUnit()&&!IsSyntax() )
				{
					if(IsContainFeature(efcNoun)&&
						!IsContainFeature(efECaseGenetive)
						)
						break;
					if( IsContainFeature(efcVerb)
						)
						break;
					if(m_LexemType.SymbolEqualsTo(_T("."))||m_LexemType.IsSComma()) break;
				}
				IUnit * pSubject, * pSubject2=NULL, *pObject=NULL;
				if(IsContainFeature(efcNoun)&&
					!IsContainFeature(efECaseGenetive)&&
					!IsSyntax()
					)
				{
					pSubject=m_pCurrentUnit;
					if(GetNextUnit() && IsContainFeature(efcNoun)&&
						!IsContainFeature(efECaseGenetive)&&
						!IsSyntax()
						)
						pSubject2 = m_pCurrentUnit;
				}else continue;
				GotoMem();
				bGood=false;
				if ((GetPrevUnit() && !IsSyntax())) 
					GetNextUnit();
				uiCount = 0;
				while(GetPrevUnit()&&!IsSyntax())
				{
					m_MorphoFeature.Undefine();
					//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,m_MorphoFeature);
					CollectMorphoFeature(m_MorphoFeature);
					if(m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
						m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple)&&
						m_MorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast))
					{
						bGood=true;
						break;
					}
					if(
						(m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdverb)||
						m_MorphoFeature.m_OfficialType.IsContain(oOficialSkip))&&
						!(IsContainFeature(efcNoun) || IsContainFeature(efcVerb) ) 
						)
						continue;
					if(uiCount++<=2) 
						continue;
					else 
						break;

					if(m_LexemType.SymbolEqualsTo(_T("."))||m_LexemType.IsSComma())
						break;
				}
				if(bGood)
				{
					if(GetPrevUnit()&& (IsBeForm(m_pCurrentUnit) || IsContainFeature(efcRepresentBecome)))
					{

						while(GetPrevUnit()&& !IsSyntax())
						{
							if(IsContainFeature(efcNoun)&&
								IsContainFeature(efEDNounNominative)
								)
							{
								if(IsContainFeature(efcVerb))
								{
									m_MorphoFeature.Undefine();
									//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,Values::PartOfSpeechTypes.postVerb,oMorphoFeature);
									CollectMorphoFeature(m_MorphoFeature);
									if(m_MorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtAux))
										continue;
								}
								break;
							}
						}
						if(!IsSyntax()&&
							IsContainFeature(efcNoun)&&
							IsContainFeature(efEDNounNominative)
							)
						{
							if(!IsPrepositionBeforeE(4,m_pCurrentUnit))
							{
								pObject=m_pCurrentUnit;
								AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pSubject);
								AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pObject);
								AddFeature(efcDetectedNoun,pObject);
								AddFeature(efcDetectedNoun,pSubject);
								if(pSubject2!=NULL)
								{
									AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pSubject2);
									AddFeature(efcDetectedNoun,pSubject2);
								}
								bPassive=true;
							}
						}
					}
				}
				GotoMem();
			}
			
		}while(GetNextUnit());
		if(!bPassive)
		{
			if(!GetFirstUnit())return true;
			do{
				if((/*m_LexemType.IsENG()|| m_LexemType.IsEng()|| m_LexemType.Iseng()*/m_LexemType.IsWord())&&
					IsBeForm(m_pCurrentUnit)
					)
				{
					Mem();
					bGood=false;
					uiCount=0;
					while(GetNextUnit() && !IsSyntax() && uiCount++<=2)
					{
						m_MorphoFeature.Undefine();
						//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,m_MorphoFeature);
						CollectMorphoFeature(m_MorphoFeature);
						if(m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
							m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple)&&
							m_MorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast))
						{
							bGood=true;
							break;
						}else if (!m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdverb))
						{
							break;
						}
					}
					if(bGood)
					{
						Mem1();
						GotoMem();
						while(GetPrevUnit()&& !IsSyntax() )
						{
							if(IsContainFeature(efcNoun)&&
								IsContainFeature(efEDNounNominative)
								)
							{
								if(IsContainFeature(efcVerb))
								{
									m_MorphoFeature.Undefine();
									//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,Values::PartOfSpeechTypes.postVerb,oMorphoFeature);
									CollectMorphoFeature(m_MorphoFeature);
									if(m_MorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtAux))
										continue;
								}
								break;
							}
						}
						if(!IsSyntax()&&
							IsContainFeature(efcNoun)&&
							IsContainFeature(efEDNounNominative)
							)
						{
							if(!IsPrepositionBeforeE(4,m_pCurrentUnit))
							{
								AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject);
								AddFeature(efcDetectedNoun);
							}
						}
						GotoMem1();
					}
				}
			}while(GetNextUnit());
		}

		return true;
	}
	SS_CATCH(L"")
}
bool CSMPassiveE::Init(IText * pIText)
{
	if(!CSMSentenceExpert::Init(pIText)) return false;

	if(!m_bInit)
	{
		m_bInit = true;

		list<UINT> l_Source;
		if( m_uiBeSourceIndex==0 && GetWordSourceIndices(_T("be"),l_Source))
		{
			m_uiBeSourceIndex=l_Source.front();
		}
	}
	
	return true;
}

bool CSMPassiveE::IsBeForm(IUnit * pUnit)
{
	if(!pUnit || !IsContainFeature(efcVerb,pUnit)) return false;
	IIndex * pIndex;
	if(!(pIndex=pUnit->GetFirstIndex())) return false;
	do{
		if( pIndex->GetDictionaryIndex() && m_uiBeSourceIndex == pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex())
			return true;
	}while(pIndex=pUnit->GetNextIndex());

	return false;
}

			}
		}
	}
}