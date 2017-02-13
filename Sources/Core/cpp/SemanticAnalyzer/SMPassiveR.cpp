#include "StdAfx.h"
#include ".\smpassiver.h"
#include "..\ASSInterface\TDictionaryNames.h"
namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{
			
		
CSMPassiveR::CSMPassiveR(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealRussian;
	m_sDescription=_T(__FUNCTION__);
}

CSMPassiveR::~CSMPassiveR(void)
{
}

bool CSMPassiveR::AnalizeSentence()
{
	SS_TRY
	{
		do{
			if (
				IsContainFeature(efRPassive)&&
				!IsContainFeature(efcRepresentBecome)
				)
			{
				Mem();
				FindLeft();
				GotoMem();
				if(!FindRight())
					GotoMem();
			}		
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}
bool CSMPassiveR::FindLeft()
{

	CSMPassiveR::EWordType eWType=CSMPassiveR::ewtSkip;
	IUnit * pNoun=NULL;
	while( eWType!=CSMPassiveR::ewtStop && GetPrevUnit() )
	{
		eWType=GetWordType(m_pCurrentUnit);
		if (eWType==CSMPassiveR::ewtProcess)
		{
			if (IsContainFeature(efcPretext)&&pNoun)
			{
				if(m_pCurrentUnit!=pNoun &&
					!IsCoordinatedPrepWithNounByCase(m_pCurrentUnit,pNoun)
					)
					SetIndex(pNoun);
				pNoun=NULL;
			}else
			{
				SetIndex(pNoun);
				pNoun=m_pCurrentUnit;
			}
		}

	}
	SetIndex(pNoun);

	return false;
}

bool CSMPassiveR::FindRight()
{
	CSMPassiveR::EWordType eWType=CSMPassiveR::ewtSkip;
	while( eWType!=CSMPassiveR::ewtStop && GetNextUnit() )
	{
		eWType=GetWordType(m_pCurrentUnit);
		if (eWType==CSMPassiveR::ewtProcess)
		{
			if (IsContainFeature(efcPretext))
			{
				IUnit * pPretext=m_pCurrentUnit;
				while(GetNextUnit())
				{
					eWType=GetWordType(m_pCurrentUnit);
					if(eWType!=CSMPassiveR::ewtSkip)
						break;
				}
				if(eWType==CSMPassiveR::ewtProcess)
				{
					if(m_pCurrentUnit!=pPretext &&
						!IsContainFeature(efcPretext) &&
						IsContainFeature(efcNoun)&&
						!IsCoordinatedPrepWithNounByCase(pPretext,m_pCurrentUnit)
						)
						SetIndex(m_pCurrentUnit);
				}
			}else
			{
				SetIndex(m_pCurrentUnit);
			}
		}
	}
	return false;
}
bool CSMPassiveR::IsCoordinatedPrepWithNounByCase(IUnit * pPretext, IUnit * pNoun)
{
	if(!pPretext || !pNoun) return false;
	m_MorphoFeature.Undefine();
	Types::CaseType oPretextCaseType;
	//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,pPretext->m_MorphoInfo,Values::PartOfSpeechTypes.postOfficial,m_MorphoFeature);
	CollectMorphoFeature(m_MorphoFeature,pPretext);
	oPretextCaseType.Assign(m_MorphoFeature.m_CaseType);
	m_MorphoFeature.Undefine();
	//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,pNoun->m_MorphoInfo,Values::PartOfSpeechTypes.postNoun,m_MorphoFeature);
	CollectMorphoFeature(m_MorphoFeature,pNoun);
	oPretextCaseType.AssignAND(&m_MorphoFeature.m_CaseType);
	return oPretextCaseType.GetValue()>1;
}
CSMPassiveR::EWordType CSMPassiveR::GetWordType(IUnit * pWord)
{
	if(/*!SyntaxTypeEquals(usntUndefined,pWord)*/IsSyntax(pWord)/*||
		!SpecialTypeEquals(ustUndefined,pWord)&&
		!SpecialTypeEquals(ustNumeral,pWord)&&
		!SpecialTypeEquals(ustWordCombinationLex,pWord)*/
		) 
		return CSMPassiveR::ewtSkip;
	if (IsContainFeature(efcNoun,pWord)/*&&!pWord->m_MorphoInfo.IsContainFeature(efRNotOnlyNoun)*/||
		IsContainFeature(efcPretext,pWord)/*||
		SpecialTypeEquals(ustWordCombinationLex,pWord)*/
		)
		return CSMPassiveR::ewtProcess;
	if(IsContainFeature(efcAdverbAdjective/*|efRDetPronounAdjective*/,pWord)||
		m_LexemType.IsDigits(0,pWord)
		//SpecialTypeEquals(ustNumeral,pWord)
		)
		return CSMPassiveR::ewtSkip;
	if (IsContainFeature(efcVerb,pWord)
		//&&!pWord->m_MorphoInfo.IsContainFeature(efRRepresentBecome)
		)
	{
		if(IsContainFeature(efRPassive,pWord)) return CSMPassiveR::ewtStop;
		if(IsContainFeature(efcRepresentBecome,pWord)) return CSMPassiveR::ewtSkip;

		m_MorphoFeature.Undefine();
		//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,pWord->m_MorphoInfo,m_MorphoFeature);
		CollectMorphoFeature(m_MorphoFeature,pWord);
		if(m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfPersonal)&&
			!(m_MorphoFeature.m_PhaseVerbType.IsContain(Values::PhaseVerbType.pvPhaseAll)||
			m_MorphoFeature.m_PhaseVerbType.IsContain(Values::PhaseVerbType.pvPhaseInf))
			)
			return CSMPassiveR::ewtStop;
		if (m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple)&&
			!m_MorphoFeature.m_ShortType.IsContain(Values::ShortType.stShort)
			)
			return CSMPassiveR::ewtSkip;
	}
	return CSMPassiveR::ewtProcess;
}


void CSMPassiveR::SetIndex(IUnit * pUnit)
{
	if (!pUnit) return;

	if(IsContainFeature(efRCaseNominative,pUnit))
		AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pUnit);
	if (IsContainFeature(efRCaseInstrumental,pUnit))
		AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pUnit);
	AddFeature(efcDetectedNoun,pUnit);
}

}
}
}
}