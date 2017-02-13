#include "StdAfx.h"
#include ".\smadverbialmodifierr.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{


CSMAdverbialModifierR::CSMAdverbialModifierR(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealRussian;
	m_sDescription=_T(__FUNCTION__);
}

CSMAdverbialModifierR::~CSMAdverbialModifierR(void)
{
}

bool CSMAdverbialModifierR::AnalizeSentence()
{
	SS_TRY
	{
		TBoxUnits l_Units;
		IUnit * pPrep;
		do{
			if(IsSyntax() ) continue;

			if (  m_pCurrentUnit->GetInfo()->_PlaceType.GetValue()>0
				)
			{

				if ( (pPrep=IsPrepositionBeforeR(6,m_pCurrentUnit)) )
				{
					l_Units.push_back(m_pCurrentUnit);
					AddGenetiveRussian(l_Units);
					//SetSyntaxInfo(m_Unit,l_Units,usntComplexProperNameSyntax);
					if(ProcessPreposition(l_Units,pPrep,false))
					{
						
					}
					l_Units.clear();
				}
			}else if ( m_pCurrentUnit->GetInfo()->_TimeType.GetValue()>0 
				)
			{
				if (IsPrepositionSyntax()	)
					continue;

				if ( (pPrep=IsPrepositionBeforeR(6,m_pCurrentUnit)) )
				{
					l_Units.push_back(m_pCurrentUnit);
					AddGenetiveRussian(l_Units);
					if(ProcessPreposition(l_Units,pPrep,true))
					{
					}
					l_Units.clear();
				}
			}else if (m_LexemType.IsDigits())
			{
				if ( (pPrep=IsPrepositionBeforeR(6,m_pCurrentUnit)) )
				{
					l_Units.push_back(m_pCurrentUnit);
					//SetSyntaxInfo(m_Unit,l_Units,usntComplexProperNameSyntax);
					if(ProcessPreposition(l_Units,pPrep,true))
					{
						//SetSyntaxType(usntDateSyntax);
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
void CSMAdverbialModifierR::AddGenetiveRussian(TBoxUnits & l_Units)
{
	while(GetNextUnit() && IsContainFeature(efRCaseGenetive) && !IsContainFeature(efcPretext))//По данным проведенных исследований, в 2003 году от нападения акул пострадали 74 человека; десять жертв скончалось.
	{
			l_Units.push_back(m_pCurrentUnit);
	}
}

			}
		}
	}
}