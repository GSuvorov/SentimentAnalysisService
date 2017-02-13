#include "StdAfx.h"
#include ".\smadverbialmodifiere.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{


CSMAdverbialModifierE::CSMAdverbialModifierE(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMAdverbialModifierE::~CSMAdverbialModifierE(void)
{
}
bool CSMAdverbialModifierE::AnalizeSentence()
{
	SS_TRY
	{
		TBoxUnits l_Units;
		IUnit * pPrep;
		do{
			if(IsSyntax() || IsContainFeature(efECaseGenetive)) continue;

			if (m_pCurrentUnit->GetInfo()->_PlaceType.GetValue()>0 && !IsQuestionMode() && !IsContainFeature(efcPretext))
			{
				if ( (pPrep=IsPrepositionBeforeE(6,m_pCurrentUnit,true)) )
				{
					l_Units.push_back(m_pCurrentUnit);
					AddPlaceUnit(l_Units);
					//SetSyntaxInfo(m_Unit,l_Units,usntComplexProperNameSyntax);
					if(ProcessPreposition(l_Units,pPrep,false))
					{
						//SetSpecialType(usntComplexProperNameSyntax);
						SetSyntax();
					}
					l_Units.clear();
				}
			}else if ( m_pCurrentUnit->GetInfo()->_TimeType.GetValue()>0 && !IsQuestionMode())
				{
					if (IsPrepositionSyntax()	)
						continue;
					if ( (pPrep=IsPrepositionBeforeE(6,m_pCurrentUnit,true)) )
					{
						l_Units.push_back(m_pCurrentUnit);
						Mem();
						if (GetNextUnit())
						{
							if(m_pCurrentUnit->GetInfo()->_TimeType.GetValue()>0 && !IsQuestionMode())
								l_Units.push_back(m_pCurrentUnit);
							else GotoMem();
						}
						if(ProcessPreposition(l_Units,pPrep,true))
						{
							SetSyntax();
						}
						l_Units.clear();
					}
				}else if (m_LexemType.IsDigits() && m_pCurrentUnit->GetInfo()->_TimeType.GetValue()>0)
				{
					if ( (pPrep=IsPrepositionBeforeE(6,m_pCurrentUnit,true)) )
					{
						l_Units.push_back(m_pCurrentUnit);						
						//SetSyntaxInfo(m_Unit,l_Units,usntComplexProperNameSyntax);
						if(ProcessPreposition(l_Units,pPrep,true))
						{
							SetSyntax();
							SS::Core::Features::CSemanticFeature oSemanticFeature;
							oSemanticFeature.m_SemanticType=Values::TSemanticType.smtTimeInterval;
							oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateYear;
							SS::Dictionary::Types::TDictionaryIndex lVal;
							m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
							lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
							AddSemanticIndex(lVal);
						}
						l_Units.clear();
					}
				}
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}

void CSMAdverbialModifierE::AddPlaceUnit(TBoxUnits & l_Units)
{
	while(GetNextUnit() && m_pCurrentUnit->GetInfo()->_PlaceType.GetValue()>0 && !IsContainFeature(efcPretext))//Ironically, in light of the Thomas and Enzi criticism, Devils Tower National Monument in northeast Wyoming was the first national monument to be created in this natio
	{
		l_Units.push_back(m_pCurrentUnit);
	}
}


			}
		}
	}
}