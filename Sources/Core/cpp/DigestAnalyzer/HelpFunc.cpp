#include "stdafx.h"
#include "HelpFunc.h"
//#include "boost/regex.hpp"

using namespace SS::Interface::Core;
using namespace SS::Interface::Core::MainAnalyse;
using namespace SS::Interface::Core::Dictionary;
using namespace SS::Interface::Core::BlackBox;

static const int nSuffixCount = 108;

static const SRusLNSuffixInfo sRusLNSiffixes[] =
{
	L"ов", L"ов", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"ова", L"ов", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"ова", L"ов", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"ову", L"ов", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"овым", L"ов", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"ове", L"ов", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"ov", L"ov", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"оva", L"оva", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,

	L"ev", L"ev", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"eva", L"eva", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,

	L"in", L"in", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"ina", L"ina", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,

	L"ова", L"ова", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"овой", L"ова", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"ову", L"ова", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"овой", L"ова", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"овой", L"ова", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"овой", L"ова", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,

	L"ев", L"ев", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"ева", L"ев", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"ева", L"ев", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"еву", L"ев", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"евым", L"ев", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"еве", L"ев", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"ева", L"ева", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"евой", L"ева", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"еву", L"ева", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"евой", L"ева", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"евой", L"ева", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"евой", L"ева", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,

	L"ин", L"ин", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"ина", L"ин", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"ина", L"ин", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"ину", L"ин", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"иным", L"ин", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"ине", L"ин", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"ина", L"ина", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"иной", L"ина", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"ину", L"ина", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"иной", L"ина", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"иной", L"ина", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"иной", L"ина", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,

	L"ский", L"ский", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"ского", L"ский", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"ского", L"ский", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"скому", L"ский", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"ским", L"ский", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"ского", L"ский", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"ская", L"ская", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"ской", L"ская", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"скую", L"ская", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"ской", L"ская", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"ской", L"ская", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"ской", L"ская", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,

	// абцахские фамилии. Арцынба, Шамба
	L"нба", L"нба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"нбы", L"нба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"нбу", L"нба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"нбе", L"нба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"нбой", L"нба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"нбе", L"нба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"нба", L"нба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"нбы", L"нба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"нбу", L"нба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"нбе", L"нба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"нбой", L"нба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"нбе", L"нба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,

	L"мба", L"мба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"мбы", L"мба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"мбу", L"мба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"мбе", L"мба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"мбой", L"мба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"мбе", L"мба", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"мба", L"мба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"мбы", L"мба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"мбу", L"мба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"мбе", L"мба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"мбой", L"мба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"мбе", L"мба", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,

	//* финские - Мяйконен
	L"нен", L"нен", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"нена", L"нен", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"нена", L"нен", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"нену", L"нен", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"неным", L"нен", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"нене", L"нен", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"нен", L"нен", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"нен", L"нен", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"нен", L"нен", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"нен", L"нен", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"нен", L"нен", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"нен", L"нен", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,

	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"швили", L"швили", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,

	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctNominative,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctGenitive,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctAccusative,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctDative,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtMasculine, SS::Core::Features::Values::CaseType.ctLocative,

	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctNominative,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctGenitive,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctAccusative,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctDative,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctInstrumental,
	L"дзе", L"дзе", SS::Core::Features::Values::GenderType.gtFemale, SS::Core::Features::Values::CaseType.ctLocative,
};

MorphoInformationList UnitPropertiesOperator::m_morphoInfo;

bool SortByPosInSentFull(IUnit* u1, IUnit* u2)
{
	return u1->GetFirstCharPositionNew() < u2->GetFirstCharPositionNew();
}

int NumOfPointsInString(wstring str)
{
	int res = 0;
	for(unsigned int i = 0; i < str.size(); ++i)
		if (str[i] == L'.')
			++res;
	return res;
}

bool UnitPropertiesOperator::IsPseudoAdjective(SS::Interface::Core::BlackBox::IUnit* pUnit,
		SS::Core::Features::Types::CaseType caseType/* = SS::Core::Features::Values::CaseType.Undefined*/)
{
    wstring word = pUnit->GetWord();
    if(word == L"верх")
        return false;

    SS::Core::Features::CMorphoFeature oMorfoPatternAdj;
	oMorfoPatternAdj.Undefine();
	oMorfoPatternAdj.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
	oMorfoPatternAdj.m_CaseType = caseType;

	bool isPseudoAdj = IsMorpho(pUnit, oMorfoPatternAdj) || isEvristicNumericAdjective(pUnit);
	if (!isPseudoAdj)
	{
		SS::Core::Features::CMorphoFeature oMorfoPatternParticiple;
		oMorfoPatternParticiple.Undefine();
		oMorfoPatternParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;	
		oMorfoPatternParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;
		oMorfoPatternParticiple.m_CaseType = caseType;
		isPseudoAdj = IsMorpho(pUnit, oMorfoPatternParticiple);
	}
    return isPseudoAdj;
}

bool UnitPropertiesOperator::IsMorpho(IUnit* pUnit, SS::Core::Features::CMorphoFeature oMorfoAnfr)
{
	SS::Core::Features::CMorphoFeature oMorphoFeature;
	if (GetMorphoInformation(pUnit, oMorphoFeature, 0)) /// есть сохраненная информация
		return oMorfoAnfr.IsIntersectsWith(oMorphoFeature);
	else
	{
		SS::Interface::Core::BlackBox::IIndex* pIndex;
		SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex;		

		wstring value = pUnit->GetWord();
		for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			pDictionaryIndex = pIndex->GetDictionaryIndex();
			if (!pDictionaryIndex)
				continue;

			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
			SS::Interface::Core::BlackBox::EIndexType oIndexType;
			oIndexType = oDictionaryIndex.GetIndexType();	

			if ((oIndexType == SS::Interface::Core::BlackBox::eitMorphoIndex) ||
				(value.empty() && (oIndexType == SS::Interface::Core::BlackBox::eitCombinatoryIndex)))
			{
				for(IFeature * pFeature = pIndex->GetFirstFeature();pFeature; pFeature = pIndex->GetNextFeature())
				{
					oMorphoFeature.Undefine();

					m_pAMConverter->EnCode(oDictionaryIndex.GetDictionaryNumber(), 
						pFeature->GetMorphoInfo(),
						pFeature->GetPartOfSpeech(), &oMorphoFeature);

					if (oMorfoAnfr.IsIntersectsWith(oMorphoFeature))
						return true;
				}
			}
		}
	}
	return false;
}


bool UnitPropertiesOperator::IsSemantic(IUnit* pUnit, SS::Core::Features::Types::TSemanticType semType,
										SS::Core::Features::Types::TSPersonType persType, SS::Core::Features::Types::TSPlaceType placeType)
{
	if (!pUnit)
		return false;
	SS::Interface::Core::BlackBox::IIndex* pIndex;
	SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex;
	SS::Interface::Core::BlackBox::EIndexType oIndexType;

	if (semType.Equal(SS::Core::Features::Values::TSemanticType.Undefined) && persType.Equal(SS::Core::Features::Values::TSPersonType.Undefined))
		return true;
	for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		pDictionaryIndex = pIndex->GetDictionaryIndex();

		if(pDictionaryIndex)
		{
			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
			oIndexType = oDictionaryIndex.GetIndexType();
			if (oIndexType == SS::Interface::Core::BlackBox::eitSemanticIndex)
			{
				SS::Core::Features::CSemanticFeature oSemanticFeature;
				m_pAMConverterSemantic->EnCode(oDictionaryIndex.GetDictionaryIndex(), &oSemanticFeature);
				if ((oSemanticFeature.m_SemanticType.Equal(semType) || semType.Equal(SS::Core::Features::Values::TSemanticType.Undefined)) &&
                    (oSemanticFeature.m_SPersonType.IsIntersectsWith(&persType) || persType.Equal(SS::Core::Features::Values::TSPersonType.Undefined)) &&
					(oSemanticFeature.m_SPlaceType.Equal(placeType) || placeType.Equal(SS::Core::Features::Values::TSPlaceType.Undefined)))
				{
					return true;
				}
			}
		}
	}

	return false;
}


void UnitPropertiesOperator::GetUnitSemanticFeatures(SS::Interface::Core::BlackBox::IUnit* pUnit,SS::Core::Features::CSemanticFeature& resSemanticFeature)
{
	if (!pUnit)
		return;
	bool first = true;
	SS::Interface::Core::BlackBox::EIndexType oIndexType;
	SS::Interface::Core::BlackBox::IIndex* pIndex;
	SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex;
	resSemanticFeature.Undefine();
	for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		pDictionaryIndex = pIndex->GetDictionaryIndex();

		if(pDictionaryIndex)
		{
			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
			oIndexType = oDictionaryIndex.GetIndexType();
			if (oIndexType == SS::Interface::Core::BlackBox::eitSemanticIndex)
			{
				int index = oDictionaryIndex.GetDictionaryIndex();
				SS::Core::Features::CSemanticFeature oSemanticFeature;
				m_pAMConverterSemantic->EnCode(index, &oSemanticFeature);
				resSemanticFeature.m_SemanticType.AssignOR(&oSemanticFeature.m_SemanticType, first);
				resSemanticFeature.m_SPersonType.AssignOR(&oSemanticFeature.m_SPersonType, first);
				resSemanticFeature.m_SPlaceType.AssignOR(&oSemanticFeature.m_SPlaceType, first);
				resSemanticFeature.m_STimeIntervalType.AssignOR(&oSemanticFeature.m_STimeIntervalType, first);
				resSemanticFeature.m_SOrganizationType.AssignOR(&oSemanticFeature.m_SOrganizationType, first);
				resSemanticFeature.m_SServiceTypes.AssignOR(&oSemanticFeature.m_SServiceTypes, first);
				first = false;
			}
		}
	}
}

SMorphoInfo UnitPropertiesOperator::CreateSMorphoInfo(const SS::Core::Features::CMorphoFeature& oMorphoFeature)
{
    SMorphoInfo smi;
	smi.caseType.Assign(oMorphoFeature.m_CaseType);
	smi.genType.Assign(oMorphoFeature.m_GenderType);
	smi.numberType.Assign(oMorphoFeature.m_NumberType);
    smi.animationType.Assign(oMorphoFeature.m_AnimInAnimType);
    smi.partOfSpeech.Assign(oMorphoFeature.m_PartOfSpeechTypes);
	smi.voiceType.Assign(oMorphoFeature.m_VoiceType);
	smi.personType.Assign(oMorphoFeature.m_PersonType);
	smi.shortType.Assign(oMorphoFeature.m_ShortType);
	smi.degreeType.Assign(oMorphoFeature.m_DegreeType);
    return smi;
}

void UnitPropertiesOperator::GetUnitMorfoFeatures(IUnit* pUnit,
												  SS::Core::Features::CMorphoFeature& resMorphoFeature,
												  SMIList *lstMI
												  )
{
	GetUnitMorfoFeaturesFull(pUnit, resMorphoFeature, lstMI);
	if (lstMI)
	{
		SMIList::iterator it = lstMI->begin();
		while (it != lstMI->end())
		{
			SMorphoInfo info = *it;
			if (!info.numberType.IsIncludes(&SS::Core::Features::Values::NumberType.ntSingle))
				it = lstMI->erase(it);
			else
				++it;
		}
	}
}

bool IsContainFeature(
    const vector<SS::Core::Features::CMorphoFeature> &list,
    const SS::Core::Features::CMorphoFeature &oMorphoFeature)
{
    bool contain = false;
    for(vector<SS::Core::Features::CMorphoFeature>::const_iterator it = list.begin();
        !contain && (it != list.end());
        ++it)
    {
        if ((*it).IsIntersectsWith(oMorphoFeature))
            contain = true;
    }
    return contain;
}

void UnitPropertiesOperator::GetUnitMorfoFeaturesFullByMorphoAnalyzer(
    SS::Interface::Core::BlackBox::IUnit* pUnit,
	SS::Core::Features::CMorphoFeature &resMorphoFeature,
    SMIList *lstMI/*= NULL*/,
	vector<SS::Core::Features::CMorphoFeature> *pInclusion/* = NULL*/,
    vector<SS::Core::Features::CMorphoFeature> *pExclusion/* = NULL*/)
{
	if(lstMI)
        lstMI->clear();
	if (GetMorphoInformation(pUnit, resMorphoFeature, lstMI))
	/// есть сохраненная информация
	{ }
	else
	/// прочитаем из словаря
	{
		wstring word = pUnit->GetWord();
		if (word.empty())
			GetUnitMorfoFeaturesFull(pUnit, resMorphoFeature, lstMI, pInclusion, pExclusion);
		else
			GetUnitMorfoFeaturesFullByMorphoAnalyzer(word, resMorphoFeature, lstMI, pInclusion, pExclusion);
	}
}

void UnitPropertiesOperator::GetUnitMorfoFeaturesFullByMorphoAnalyzer(
    const wstring &word,
	SS::Core::Features::CMorphoFeature &resMorphoFeature,
    SMIList *lstMI/*= NULL*/,
	vector<SS::Core::Features::CMorphoFeature> *pInclusion/* = NULL*/,
    vector<SS::Core::Features::CMorphoFeature> *pExclusion/* = NULL*/)
{
	if(lstMI)
        lstMI->clear();
	resMorphoFeature.Undefine();
	if (word.empty())
		return;

	bool first = true;
	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	GetWordInfo(word, &oLst, SS::Dictionary::Types::efzOnlySearch);
	bool isOnlyNominal = !IsAnyUpper(word);
	for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
	{
		SS::Core::Features::CMorphoFeature oMorphoFeature;
		SS::Dictionary::Types::SWordInfo wi = (*it).front();

		SS::Dictionary::Types::TDictNumber dictionaryNumber = wi.m_WordIndex.m_DictIndex.GetDictionaryNumber();
		if (isOnlyNominal &&
			((dictionaryNumber == SS::Dictionary::DATA_TYPE::NAMES::ednRProperName) ||
			(dictionaryNumber == SS::Dictionary::DATA_TYPE::NAMES::ednEProperName)))
		{
			continue;
		}
		SaveMorpho(dictionaryNumber, wi.m_MorphoTypeInfo.m_MorphoInfo, wi.m_SpecialMorpho, first,
			resMorphoFeature, lstMI, pInclusion, pExclusion);
	}
}

void UnitPropertiesOperator::GetUnitMorfoFeaturesFull(
    IUnit* pUnit,
	SS::Core::Features::CMorphoFeature& resMorphoFeature,
	SMIList *lstMI/*= NULL*/,
	vector<SS::Core::Features::CMorphoFeature> *pInclusion/* = NULL*/,
    vector<SS::Core::Features::CMorphoFeature> *pExclusion/* = NULL*/)
{
    if(lstMI)
        lstMI->clear();
	if (GetMorphoInformation(pUnit, resMorphoFeature, lstMI))
	/// есть сохраненная информация
	{ }
	else
	{
		resMorphoFeature.Undefine();
		SS::Interface::Core::BlackBox::IIndex* pIndex;
		SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex;		

		bool first = true;
		wstring value = pUnit->GetWord();
		for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			pDictionaryIndex = pIndex->GetDictionaryIndex();
			if(pDictionaryIndex)
			{
				SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
				SS::Interface::Core::BlackBox::EIndexType oIndexType;
				oIndexType = oDictionaryIndex.GetIndexType();	

				if ((oIndexType == SS::Interface::Core::BlackBox::eitMorphoIndex) ||
					(value.empty() && (oIndexType == SS::Interface::Core::BlackBox::eitCombinatoryIndex)))
				{
					for(IFeature *pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
					{
						SaveMorpho(oDictionaryIndex.GetDictionaryNumber(), pFeature->GetMorphoInfo(), pFeature->GetPartOfSpeech(), first,
							resMorphoFeature, lstMI, pInclusion, pExclusion);
					}
				}
			} // if (pDictionaryIndex)
		} // цикл по индексам
	}
}

void UnitPropertiesOperator::GetUnitMorfoFeaturesFull(
	const wstring &word,
	SS::Core::Features::CMorphoFeature &resMorphoFeature,
    SMIList *lstMI,
    vector<SS::Core::Features::CMorphoFeature> *pInclusion,
    vector<SS::Core::Features::CMorphoFeature> *pExclusion)
{
	if(lstMI)
        lstMI->clear();
	resMorphoFeature.Undefine();
	if (word.empty())
		return;

	bool first = true;
	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	GetWordInfo(word, &oLst, SS::Dictionary::Types::efzFullAddNewWords);
	for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
	{
		SS::Core::Features::CMorphoFeature oMorphoFeature;
		SS::Dictionary::Types::SWordInfo wi = (*it).front();

		SaveMorpho(wi.m_WordIndex.m_DictIndex.GetDictionaryNumber(), wi.m_MorphoTypeInfo.m_MorphoInfo, wi.m_SpecialMorpho, first,
			resMorphoFeature, lstMI, pInclusion, pExclusion);
	}
}

void UnitPropertiesOperator::SaveMorpho(
	const unsigned char uchDictionaryNumber,
	const int iMorphoInfo,
	const int iPartOfSpeech,
	bool &isFirst,
	SS::Core::Features::CMorphoFeature &resMorphoFeature,
    SMIList *lstMI = NULL,
    vector<SS::Core::Features::CMorphoFeature> *pInclusion = NULL,
    vector<SS::Core::Features::CMorphoFeature> *pExclusion = NULL)
{
	SS::Core::Features::CMorphoFeature oMorphoFeature;											
	m_pAMConverter->EnCode(uchDictionaryNumber, iMorphoInfo, iPartOfSpeech, &oMorphoFeature);

	if(pInclusion && !IsContainFeature(*pInclusion, oMorphoFeature))
		return;
	if(pExclusion && IsContainFeature(*pExclusion, oMorphoFeature))
		return;

	resMorphoFeature.m_PartOfSpeechTypes.AssignOR(&oMorphoFeature.m_PartOfSpeechTypes, isFirst);
	resMorphoFeature.m_OfficialType.AssignOR(&oMorphoFeature.m_OfficialType, isFirst);
	resMorphoFeature.m_GenderType.AssignOR(&oMorphoFeature.m_GenderType, isFirst);
	resMorphoFeature.m_NumberType.AssignOR(&oMorphoFeature.m_NumberType, isFirst);
	resMorphoFeature.m_AnimInAnimType.AssignOR(&oMorphoFeature.m_AnimInAnimType, isFirst);
	resMorphoFeature.m_CaseType.AssignOR(&oMorphoFeature.m_CaseType, isFirst);
	resMorphoFeature.m_CommonType.AssignOR(&oMorphoFeature.m_CommonType, isFirst);
	resMorphoFeature.m_VerbForm.AssignOR(&oMorphoFeature.m_VerbForm, isFirst);
	resMorphoFeature.m_VoiceType.AssignOR(&oMorphoFeature.m_VoiceType, isFirst);
	resMorphoFeature.m_ShortType.AssignOR(&oMorphoFeature.m_ShortType, isFirst);
	resMorphoFeature.m_TransitionalType.AssignOR(&oMorphoFeature.m_TransitionalType, isFirst);
	resMorphoFeature.m_TimeType.AssignOR(&oMorphoFeature.m_TimeType, isFirst);
	resMorphoFeature.m_VerbType.AssignOR(&oMorphoFeature.m_VerbType, isFirst);
	resMorphoFeature.m_MoodType.AssignOR(&oMorphoFeature.m_MoodType, isFirst);
	resMorphoFeature.m_AspectType.AssignOR(&oMorphoFeature.m_AspectType, isFirst);
	resMorphoFeature.m_NumericType.AssignOR(&oMorphoFeature.m_NumericType, isFirst);
	isFirst = false;

	if (lstMI)
		lstMI->push_back(CreateSMorphoInfo(oMorphoFeature));
}

bool UnitPropertiesOperator::IsProperUnknown(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	bool result = false;
	if (pUnit)
	{
		result = IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson, SS::Core::Features::Values::TSPersonType.semprstName);
		IIndex* pFirstIndex = pUnit->GetFirstIndex();
		if (pFirstIndex)
			result = result || (pFirstIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase() && pUnit->GetUnknownWord());
	}
	return result;
}

bool UnitPropertiesOperator::IsNominalDict(wstring str)
{
	wstring strLo = GetLowerWord(str);
	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	GetWordInfo(strLo, &oLst, SS::Dictionary::Types::efzFullAddNewWords);
	for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
	{
		SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());
		if (pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednENominalName ||
			pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName)
		{
			return true;
		}
	}
	return false;
}


void UnitPropertiesOperator::GetRusLNForms(SS::Interface::Core::BlackBox::IUnit *pUnit, SMIList& forms)
{
	wstring str = pUnit->GetWord();
	for(int i = 0; i < nSuffixCount; ++i)
	{
		SRusLNSuffixInfo sinfo = sRusLNSiffixes[i];
		wstring curSuffix = L"";
		for(int j = str.size() - 1; j >= 0; --j)
		{
			curSuffix = str[j] + curSuffix;
			if (curSuffix == sinfo.suffix)
			{
				SMorphoInfo smi;
				smi.caseType.Assign(sinfo.caseType);
				smi.genType.Assign(sinfo.genType);
				forms.push_back(smi);
			}
		}
	}	
}

void UnitPropertiesOperator::IntersectMorphoInfoLists(SMIList& formsIn1, SMIList& formsIn2, SMIList& formsOut,
	bool genderInter/* = true*/, bool caseInter/* = true*/, bool numberInter/* = false*/)
{
	if (formsIn1.empty())
	{
		for(SMIList::iterator it = formsIn2.begin(); it != formsIn2.end(); ++it)
			formsOut.push_back(*it);
	}
	else if (formsIn2.empty())
	{
		for(SMIList::iterator it = formsIn1.begin(); it != formsIn1.end(); ++it)
			formsOut.push_back(*it);
	}
	else
	{
		for(SMIList::iterator it1 = formsIn1.begin(); it1 != formsIn1.end(); ++it1)
		{
			SMorphoInfo smi1 = *it1;
			for(SMIList::iterator it2 = formsIn2.begin(); it2 != formsIn2.end(); ++it2)
			{
				SMorphoInfo smi2 = *it2;
				if (smi1.IsIntersectsMorpho(smi2, genderInter, caseInter, numberInter))
				{
					if (caseInter)
						smi1.caseType.AssignAND(&smi2.caseType, true);
					if (genderInter)
						smi1.genType.AssignAND(&smi2.genType, true);
					if (numberInter)
						smi1.numberType.AssignAND(&smi2.numberType, true);

					formsOut.push_back(smi1);
					break;
				}
			}
		}
	}
}

bool UnitPropertiesOperator::IsMorpho(const SMIList &forms,
	SMIList *pFilter, 
	SS::Core::Features::Types::GenderType isGender/* = SS::Core::Features::Values::GenderType.Undefined*/,
	SS::Core::Features::Types::CaseType isCase/* = SS::Core::Features::Values::CaseType.Undefined*/,
	SS::Core::Features::Types::NumberType isNumber/* = SS::Core::Features::Values::NumberType.Undefined*/,
	SS::Core::Features::Types::AnimInAnimType isAnimation/* = SS::Core::Features::Values::AnimInAnimType.Undefined*/)
{
	for (SMIList::const_iterator it = forms.begin(); it != forms.end(); ++it)
	{
		const SMorphoInfo *pInfo = &(*it);
		if (pFilter)
		{
			bool isFilter = true;
			for (SMIList::iterator iter = pFilter->begin(); isFilter && (iter != pFilter->end()); ++iter)
				if (pInfo->IsIntersectsMorpho(*iter, true, true, true))
					isFilter = false;
			if (isFilter)
				continue;
		}
		bool genderTest = isGender.IsUndefined() || isGender.Equal(pInfo->genType);
		bool caseTest = isCase.IsUndefined() || isCase.Equal(pInfo->caseType);
		bool numberTest = isNumber.IsUndefined() || isNumber.Equal(pInfo->numberType);
		bool animationTest = isAnimation.IsUndefined() || isAnimation.Equal(pInfo->animationType);
		if (genderTest && caseTest && numberTest && animationTest)
			return true;
	}
	return false;
}

bool isEndOfStr(wstring strSrc, wstring strEnd)
{
	int count = strSrc.size() - 1;
	if (strSrc.size() <= strEnd.size())
		return false;
	for(int i = strEnd.size() - 1; i >= 0; --i)
	{
		if (strEnd[i] != strSrc[count])
			return false;
		--count;
	}
	return true;
}

wstring UnitPropertiesOperator::GetRusLNBase(SS::Interface::Core::BlackBox::IUnit *pUnit, SMorphoInfo form)
{
	wstring str = pUnit->GetWord();

	for(int i = 0; i < nSuffixCount; ++i)
	{
		SRusLNSuffixInfo sinfo = sRusLNSiffixes[i];		
		if ((sinfo.caseType.Equal(form.caseType) || form.caseType.IsUndefined()) &&
			(sinfo.genType.Equal(form.genType) || form.genType.IsUndefined()) &&
			isEndOfStr(str, sinfo.suffix))
		{
			return str.substr(0, str.size() - sinfo.suffix.size()) + sinfo.process;
		}
	}
	return L"";
}

bool UnitPropertiesOperator::IsSuitableMorphoInfo(
    SS::Dictionary::Types::TWordInfo baseInfo,
    SS::Dictionary::Types::TWordInfo wordInfo,
    bool isVerb, bool isParticiple,
    SS::Core::Features::Types::PartOfSpeechTypes partOfSpeechType,
    SS::Core::Features::Types::GenderType genderType,
    SS::Core::Features::Types::VoiceType voiceType,
	SS::Core::Features::Types::NumberType number,
    bool useCaseTest, bool useNumberTest, bool useRegisterTest, bool useTimeTest)
{
    bool suitable = false;

    SS::Core::Features::CMorphoFeature oMorphoFeature;
    m_pAMConverter->EnCode(
		wordInfo.front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
		wordInfo.front().m_MorphoTypeInfo.m_MorphoInfo,
		wordInfo.front().m_SpecialMorpho,
		&oMorphoFeature);

	if (oMorphoFeature.m_DegreeType.IsUndefined() && (partOfSpeechType.IsUndefined() ||
        oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&partOfSpeechType)))
    {
        bool caseTest = useCaseTest ? oMorphoFeature.m_CaseType.Equal(SS::Core::Features::Values::CaseType.ctNominative) ||
            oMorphoFeature.m_CaseType.Equal(SS::Core::Features::Values::CaseType.ctCommon) : true;
		bool numberTest = useNumberTest ? number.Equal(SS::Core::Features::Values::NumberType.Undefined) ||
			oMorphoFeature.m_NumberType.IsContain(number) : true;
        bool registerTest = true;
        if(useRegisterTest && iswupper(wordInfo.front().m_wWord[0]))
            registerTest = iswupper(baseInfo.front().m_wWord[0]);
        
        if (oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postVerb) && isVerb)
            return suitable;

        if (isParticiple && 
            (useTimeTest && !oMorphoFeature.m_TimeType.Equal(SS::Core::Features::Values::TimeType.tmtPresent)) ||
            !oMorphoFeature.m_VoiceType.Equal(voiceType))
        {
            return suitable;
        }

        if (caseTest && numberTest && registerTest)
	    {
		    if (oMorphoFeature.m_GenderType.IsIntersectsWith(&genderType) || genderType.IsUndefined())
			    suitable = true;
	    }
    }
    return suitable;
}

MorphoTestInfo UnitPropertiesOperator::TestMorphoInfo(
	SS::Dictionary::Types::TWordInfo baseInfo,
	SS::Dictionary::Types::TWordInfo wordInfo, 
	bool isVerb, bool isParticiple,
	SS::Core::Features::Types::PartOfSpeechTypes partOfSpeechType,
    SS::Core::Features::Types::GenderType genderType,
	SS::Core::Features::Types::VoiceType voiceType,
	SS::Core::Features::Types::NumberType number)
{
	MorphoTestInfo result = {0};
	SS::Core::Features::CMorphoFeature oMorphoFeature;
    m_pAMConverter->EnCode(
		wordInfo.front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
		wordInfo.front().m_MorphoTypeInfo.m_MorphoInfo,
		wordInfo.front().m_SpecialMorpho,
		&oMorphoFeature);
	if (partOfSpeechType.IsUndefined() || oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&partOfSpeechType))
	{
		if ((oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postVerb) && isVerb) ||
			(isParticiple && !oMorphoFeature.m_VoiceType.Equal(voiceType)))
		{
			return result;
		}
		result.IsDegreeTestSuccess = oMorphoFeature.m_DegreeType.IsUndefined();
		result.IsCaseTestSuccess = oMorphoFeature.m_CaseType.Equal(SS::Core::Features::Values::CaseType.ctNominative) ||
            oMorphoFeature.m_CaseType.Equal(SS::Core::Features::Values::CaseType.ctCommon);
		result.IsNumberTestSuccess = number.Equal(SS::Core::Features::Values::NumberType.Undefined) ||
			oMorphoFeature.m_NumberType.IsContain(number);
		result.IsRegisterTestSuccess = iswupper(wordInfo.front().m_wWord[0]) ? iswupper(baseInfo.front().m_wWord[0]) : true;
		result.IsGenderTestSuccess = oMorphoFeature.m_GenderType.IsIntersectsWith(&genderType) || genderType.IsUndefined();
		result.IsTimeTestSuccess = isParticiple ? oMorphoFeature.m_TimeType.Equal(SS::Core::Features::Values::TimeType.tmtPresent) : true;
	}
	return result;
}

WordForm UnitPropertiesOperator::GetWordForm(SS::Dictionary::Types::TWordInfo wordInfo)
{
	WordForm result;
	SS::Core::Features::CMorphoFeature oMorphoFeature;
    m_pAMConverter->EnCode(
		wordInfo.front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
		wordInfo.front().m_MorphoTypeInfo.m_MorphoInfo,
		wordInfo.front().m_SpecialMorpho,
		&oMorphoFeature);
	result.form = wordInfo.front().m_wWord;
	result.number = oMorphoFeature.m_NumberType;
	result.gender = oMorphoFeature.m_GenderType;
	return result;
}

WordForm UnitPropertiesOperator::GetWordMainForm(const wstring &word,
	SS::Core::Features::Types::GenderType genderType,
	SS::Core::Features::Types::NumberType number/* = SS::Core::Features::Values::NumberType.ntSingle*/,
       SS::Core::Features::Types::PartOfSpeechTypes partOfSpeechType/* = SS::Core::Features::Values::PartOfSpeechTypes.Undefined*/)
{
	WordForm baseForm(word, number);

	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	GetWordInfo(word, &oLst, SS::Dictionary::Types::efzFullAddNewWords);
	if(oLst.empty())
		return baseForm;

	bool isFindFirst = false;
	bool isFind = false;
	
    WordForm tempForm;
	MorphoTestInfo current = {0};
    vector<bool> isVerbs;
    vector<bool> isParticiples;
	for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
	{
		SS::Core::Features::CMorphoFeature oMorphoMainWord;	

		SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());
		// Преобразует закодированные морфологические характеристики	
		std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;
		bool isVerb = false;
        bool isParticiple = false;
		bool isNeedCaseTest = true;

		for(SS::Dictionary::Types::TWordInfo::iterator it1 = it->begin(); it1 != it->end(); ++it1)
		{
			SS::Dictionary::Types::SWordInfo wi = *it1;
			m_pAMConverter->EnCode(
				wi.m_WordIndex.m_DictIndex.GetDictionaryNumber(),
				wi.m_MorphoTypeInfo.m_MorphoInfo,
				wi.m_SpecialMorpho,					
				&oMorphoMainWord);

			if (oMorphoMainWord.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postVerb))
            {
			    if(!oMorphoMainWord.m_VerbForm.Equal(SS::Core::Features::Values::VerbForm.vfParticiple))
                {
					isVerb = true;
					break;
                }
                else
                    isParticiple = true;
			}
			else if ((pwi1->m_WordIndex.m_DictIndex.GetLanguage() == SS::Core::Types::ealEnglish) &&
				oMorphoMainWord.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
			{
				isNeedCaseTest  = false;
			}
		}
        isVerbs.push_back(isVerb);
        isParticiples.push_back(isParticiple);

        SS::Core::Features::CMorphoFeature oMorphoFeature;
		m_pMorphoAnalyzer->GetWordFormsByIDSource(&pwi1->m_WordIndex.m_DictIndex, &lWordInfoDetect);

		for(TWIList::iterator it1 = lWordInfoDetect.begin(); it1 != lWordInfoDetect.end(); ++it1)
        {
			MorphoTestInfo info = TestMorphoInfo(*it, *it1, isVerb, isParticiple, partOfSpeechType, genderType,
				oMorphoMainWord.m_VoiceType, number);
			bool isSuitableCase = isNeedCaseTest ? info.IsCaseTestSuccess : true;
			if (isSuitableCase && info.IsTimeTestSuccess && info.IsGenderTestSuccess)
			{
				if ((info.IsNumberTestSuccess >= current.IsNumberTestSuccess) &&
					(info.IsRegisterTestSuccess >= current.IsRegisterTestSuccess) &&
					(info.IsDegreeTestSuccess >= current.IsDegreeTestSuccess))
				{
					WordForm form = GetWordForm(*it1);
					if (partOfSpeechType.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric) &&
						!form.gender.Equal(SS::Core::Features::Values::GenderType.gtNeutral))
					{
						continue;
					}
					current = info;
					tempForm = form;
				}
				if (current.IsDegreeTestSuccess && current.IsNumberTestSuccess && current.IsRegisterTestSuccess)
					return tempForm;
			}
        }
	}

	if (tempForm.form.empty())
    {
        int i = -1;
        for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
        {
            ++i;
            SS::Core::Features::CMorphoFeature oMorphoMainWord;
            SS::Dictionary::Types::SWordInfo wi = it->front();
			m_pAMConverter->EnCode(
				wi.m_WordIndex.m_DictIndex.GetDictionaryNumber(),
				wi.m_MorphoTypeInfo.m_MorphoInfo,
				wi.m_SpecialMorpho,					
				&oMorphoMainWord);

		    SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());
		    // Преобразует закодированные морфологические характеристики	
		    std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;
		    
            SS::Core::Features::CMorphoFeature oMorphoFeature;
		    m_pMorphoAnalyzer->GetWordFormsByIDSource(&pwi1->m_WordIndex.m_DictIndex, &lWordInfoDetect);

			for(TWIList::iterator it1 = lWordInfoDetect.begin(); it1 != lWordInfoDetect.end(); ++it1)
			{
				MorphoTestInfo info = TestMorphoInfo(*it, *it1, isVerbs[i], isParticiples[i], partOfSpeechType, genderType,
					oMorphoMainWord.m_VoiceType, number);
				if (info.IsCaseTestSuccess && info.IsGenderTestSuccess && info.IsDegreeTestSuccess &&
					info.IsNumberTestSuccess && info.IsRegisterTestSuccess)
				{
					return GetWordForm(*it1);
				}
			}
		}
	}
    return tempForm.form.empty() ? baseForm : tempForm;
}

WordForm UnitPropertiesOperator::GetMainFormText1(IUnit *pUnit,
	SS::Core::Features::Types::GenderType genderType,
	SS::Core::Features::Types::NumberType number/* = SS::Core::Features::Values::NumberType.ntSingle*/,
    SS::Core::Features::Types::PartOfSpeechTypes partOfSpeechType/* = SS::Core::Features::Values::PartOfSpeechTypes.Undefined*/)
{
	WordForm mainForm(pUnit->GetWord(), number);
	wstring lowerMain = GetLowerWord(mainForm.form);
	if (mainForm.form.empty() || (NumOfPointsInString(mainForm.form) > 1))
	{
		mainForm.form = L"";
        return mainForm;
	}
	if ((mainForm.form == L"US") || (mainForm.form.size()==2 && mainForm.form[1]=='.'))
        return mainForm; // инициалы сразу возращаем
	wstring ending = L"окси";
	if ((lowerMain.size() > ending.size()) && (lowerMain.substr(lowerMain.size() - ending.size()) == ending))
		return mainForm;

	WordForm normalForm;
	vector<wstring> words;
	wstring changeWord;
	wstring constWord;
	if (!IsEnglishWord(pUnit))
		words = GetChangeablePartsByDefis(pUnit);
	int changeableParts = -1;
	if (words.empty())
		changeableParts = 0;
	else if (words.size() == 1)
	{
		changeableParts = 1;
		changeWord = words.front();
	}
	else
	{
		SMIList result;
		if (GetIntersectMorpho(words[0], words[1], result, &changeWord, &constWord))
		{
			if (changeWord.empty())
				changeableParts = 2;
			else
				changeableParts = 1;
		}
		else
			changeableParts = 0;
	}
	switch (changeableParts)
	{
		case 0:
			normalForm = GetWordMainForm(mainForm.form, genderType, number, partOfSpeechType);
			break;
		case 1:
		{
			normalForm = GetWordMainForm(changeWord, genderType, number, partOfSpeechType);
			wstring mainWordForm = mainForm.form;
			int defisIndex = mainWordForm.find(L'-');
			wstring left = mainWordForm.substr(0, defisIndex);
			if (left == changeWord)
				normalForm.form += mainWordForm.substr(defisIndex, mainWordForm.length() - defisIndex);
			else
				normalForm.form.insert(0, mainWordForm.substr(0, defisIndex + 1));
			break;
		}
		case 2:
		{
			genderType = SS::Core::Features::Values::GenderType.Undefined;
			number = SS::Core::Features::Values::NumberType.Undefined;
			WordForm firstWord = GetWordMainForm(words[0], genderType, number, partOfSpeechType);
			number = firstWord.number;
			normalForm.form = firstWord.form + L"-" + GetWordMainForm(words[1], genderType, number, partOfSpeechType).form;
			normalForm.number = SS::Core::Features::Values::NumberType.Undefined;
			break;
		}
		default:
			normalForm = mainForm;
			break;
	}
	return normalForm;
}

bool UnitPropertiesOperator::IsVerbUnit(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
	oMorfoPattern.m_VerbForm = SS::Core::Features::Values::VerbForm.vfInfinitive;
	oMorfoPattern.m_VerbForm.AssignOR(&SS::Core::Features::Values::VerbForm.vfImperative);
	oMorfoPattern.m_VerbForm.AssignOR(&SS::Core::Features::Values::VerbForm.vfImPersonal);
	oMorfoPattern.m_VerbForm.AssignOR(&SS::Core::Features::Values::VerbForm.vfPersonal);
	oMorfoPattern.m_VerbForm.AssignOR(&SS::Core::Features::Values::VerbForm.vfDeepr); 

	SS::Core::Features::CMorphoFeature oMorfoParticiple;
	oMorfoParticiple.Undefine();
	oMorfoParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
	oMorfoParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;			

	if (IsMorpho(pUnit, oMorfoPattern) || IsMorpho(pUnit, oMorfoParticiple))
        return true;
	return false;
}

wstring UnitPropertiesOperator::GetVerbNormText(IUnit *pUnit)
{		
	wstring wbasewordGender = L"";
	wstring wbaseword = L"";
	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	std::list<SS::Dictionary::Types::TWordInfo> oLst1;
	wstring mainword = pUnit->GetWord();
	if (NumOfPointsInString(mainword) > 1)
		return L"";		
	if (mainword.empty())
		return L"";
	if (mainword.size()==2 && mainword[1]=='.')
		return mainword; // инициалы сразу возращаем
	GetWordInfo(mainword, &oLst, SS::Dictionary::Types::efzFullAddNewWords);
	if(oLst.empty())
		return mainword;
	wbaseword = mainword;
	bool isFindFirst = false;		
	bool isFind = false;

	for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
	{			
		SS::Core::Features::CMorphoFeature oMorphoFeature;	

		SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());
		// Преобразует закодированные морфологические характеристики	
		std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;
		m_pMorphoAnalyzer->GetWordFormsByIDSource(&pwi1->m_WordIndex.m_DictIndex, &lWordInfoDetect);
		for(TWIList::iterator it1 = lWordInfoDetect.begin(); it1 != lWordInfoDetect.end(); ++it1)
		{							
			SS::Dictionary::Types::TWordInfo wi = *it1;
			m_pAMConverter->EnCode(
				wi.front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
				wi.front().m_MorphoTypeInfo.m_MorphoInfo,
				wi.front().m_SpecialMorpho,					
				&oMorphoFeature);

			if (oMorphoFeature.m_VerbForm.Equal(SS::Core::Features::Values::VerbForm.vfInfinitive))
				return wi.front().m_wWord;
		}					
	}
	return wbaseword;	
}


wstring UnitPropertiesOperator::GetMainFormText(IUnit *pUnit, SS::Core::Features::Types::GenderType genderType)
{		
	wstring wbasewordGender = L"";
	wstring wbaseword = L"";
	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	std::list<SS::Dictionary::Types::TWordInfo> oLst1;
	wstring mainword = pUnit->GetWord();
	if (mainword.size()==0) return L"";
	if (mainword.size()==2 && mainword[1]=='.') return mainword; // инициалы сразу возращаем
	GetWordInfo(mainword, &oLst, SS::Dictionary::Types::efzFullAddNewWords);
	if(oLst.empty())
		return mainword;

	wbaseword = mainword;
	bool isFindFirst = false;
	bool isFind = false;
	for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
	{			
		SS::Core::Features::CMorphoFeature oMorphoFeature;	
		SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());
		SS::Dictionary::Types::TMorphoInfo tmi;
		SS::Dictionary::Types::TSpecialMorpho tsmi;
		// Преобразует закодированные морфологические характеристики			
		wchar_t wzWord1[100];
		pwi1->m_WordIndex.m_IDForm = 0;
		m_pMorphoAnalyzer->GetWord(&pwi1->m_WordIndex, wzWord1);
		if (genderType.IsUndefined()) return wzWord1;
		m_pMorphoAnalyzer->GetMorphoInfoByIDType(pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
			pwi1->m_MorphoTypeInfo.m_IDType, pwi1->m_WordIndex.m_IDForm, &tmi, &tsmi);
		m_pAMConverter->EnCode(				    
			pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
			tmi,
			tsmi,
			&oMorphoFeature);

		if (oMorphoFeature.m_GenderType.IsIntersectsWith(&genderType)) return wzWord1;
		else if (!isFindFirst) {isFindFirst = true; wbaseword = wzWord1; }
	}
	return wbaseword;	
}

bool UnitPropertiesOperator::isHaveParentCombiNumeric(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
	wstring str = pUnit->GetWord();
	if (str.empty())
		return true;
	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNumeric;
	IUnit* pParent = pUnit->GetParentUnit();
	if (pParent)
	{
		str = pParent->GetWord();
		if (str.empty() && IsMorpho(pParent,oMorfoPattern) && pParent->GetHashKey())
			return true;
	}
	return false;
}

bool UnitPropertiesOperator::IsGoodDateSeq(SS::Interface::Core::BlackBox::IUnit *pUnit1,
										   SS::Interface::Core::BlackBox::IUnit *pUnit2, TUnitList& sentUnits)
{
	TUnitList::iterator it1 = FindUnit(sentUnits, pUnit1);
	TUnitList::iterator it2 = FindUnit(sentUnits, pUnit2);
	while(it1 != it2)
	{
		++it1;
		if (it1 == it2)
			break;
		IUnit* pUnit = *it1;
		wstring str = pUnit->GetWord();
		if (str == L"в")
			return false;
	}
	return true;
}

bool UnitPropertiesOperator::FindInUnits(TUnitList& unitList, SS::Interface::Core::BlackBox::IUnit* pObjUnit)
{
	for(TUnitList::iterator it = unitList.begin(); it!=unitList.end(); ++it)
	{
		IUnit* pUnit = (*it);
		if (pUnit == pObjUnit)
			return true;
	}
	return false;
}

TUnitList::iterator UnitPropertiesOperator::FindUnit(TUnitList& list, SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	for(TUnitList::iterator it = list.begin(); it!=list.end(); ++it)
	{
		if ((*it) == pUnit)
			return it;
	}
	return list.end();
}

void  UnitPropertiesOperator::GetLinearUnits(IUnit* root, list<IUnit*>& allUnits)
{	
	IUnit* pUnit = root;
	allUnits.push_back(root);		
	pUnit = root->GetLeftChildUnit();
	while(pUnit)
	{		
		GetLinearUnits(pUnit, allUnits);
		pUnit = pUnit->GetRightUnit();
	}		
}

bool UnitPropertiesOperator::IsSyntax(IUnit* pUnit, SS::Core::Features::Types::SyntaxCategories oSyntaxCat)
{
	ISyntaxFeature *sf = pUnit->GetSyntaxFeature();
	if (sf)
	{
		SS::Core::Features::Types::SyntaxCategories syntCat = sf->GetSyntaxFeatureAposteriori()->m_SyntaxCategories;
		if (syntCat.IsContain(oSyntaxCat) || oSyntaxCat.Equal(SS::Core::Features::Values::SyntaxCategories.Undefined))
			return true;
	}
	return false;
}

bool UnitPropertiesOperator::IsUndefinedSyntax(IUnit* pUnit)
{
	ISyntaxFeature *sf = pUnit->GetSyntaxFeature();
	if (sf)
	{
		SS::Core::Features::Types::SyntaxCategories syntCat = sf->GetSyntaxFeatureAposteriori()->m_SyntaxCategories;
		if (syntCat.Equal(SS::Core::Features::Values::SyntaxCategories.Undefined))
			return true;
	}
	return false;
}


bool UnitPropertiesOperator::IsParentGeo(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IUnit *pParent = pUnit->GetParentUnit();
	if (pParent!=NULL)
	{		 
		wstring str = pParent->GetWord();
		if (IsSyntax(pParent, SS::Core::Features::Values::SyntaxCategories.scAjunctPlaceSource) ||
			(IsSemantic(pParent,  SS::Core::Features::Values::TSemanticType.smtPlace) && 
			(!pParent->HasChilds() || str.empty()) && !IsSemantic(pParent,  SS::Core::Features::Values::TSemanticType.smtPerson)))
		{
			return true;
		}		
	}
	return false;
}

bool UnitPropertiesOperator::IsParentGeoCombi(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IUnit *pParent = pUnit->GetParentUnit();
	if (pParent)
	{		 
		wstring str = pParent->GetWord();
		if (str.empty() && IsSemantic(pParent,  SS::Core::Features::Values::TSemanticType.smtPlace) &&
			!IsSemantic(pParent,  SS::Core::Features::Values::TSemanticType.smtPerson))			
		{
			return true;
		}		
	}
	return false;
}

bool UnitPropertiesOperator::IsInQuote(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IUnit *pParent = pUnit->GetParentUnit();
	if (IsUnitChild(pParent, pUnit))
		return IsQuoteParentUnit(pParent);
	return false;
}

bool UnitPropertiesOperator::IsQuoteParentUnit(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	wstring str = pUnit->GetWord();
	if (str.empty())
	{
		SS::Interface::Core::BlackBox::IUnit* pChild = pUnit->GetChildList().front();
		if (pChild && (IsQuote(pChild->GetWord()) != EQuoteType::NotQuote))
			return true;
	}
	return false;
}

bool UnitPropertiesOperator::IsInCombi(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IUnit *pParent = pUnit->GetParentUnit();
	if (pParent && IsUnitChild(pParent, pUnit) && IsCombiUnit(pParent))
		return true;
	return false;
}

wstring UnitPropertiesOperator::GetFullWord(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	wstring result = pUnit->GetWord();
	if (result.empty())
	{
		TUnitList list = pUnit->GetChildList();
		unsigned int position = 0;
		for(TUnitList::iterator it = list.begin(); it != list.end(); ++it)
		{
			IUnit *pChild = *it;
			wstring word = GetFullWord(pChild);
			unsigned int childPosition = pChild->GetFirstCharPositionNew();
			if (position < childPosition)
			{
				result += L" ";
				position = childPosition;
			}
			result += word;
			position += word.length();
		}
	}
	return result;
}

bool UnitPropertiesOperator::IsUnitChild(
	SS::Interface::Core::BlackBox::IUnit* pParent,
	SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if (pParent)
	{
		TUnitList list = pParent->GetChildList();
		for(TUnitList::iterator it = list.begin(); it != list.end(); ++it)
			if (pUnit == *it)
				return true;
	}
	return false;
}

bool UnitPropertiesOperator::IsParentInitial(SS::Interface::Core::BlackBox::IUnit* pUnit)
{	
	IUnit* pChildUnit = pUnit->GetLeftChildUnit();
	if (!pChildUnit)
		return false;
	while(pChildUnit)
	{
		if (!pChildUnit->IsInitial())
			return false;
		pChildUnit = pChildUnit->GetRightUnit();		 
	}
	return true;
}

bool UnitPropertiesOperator::IsHavePoint(SS::Interface::Core::BlackBox::IUnit* pUnit)
{	
	wstring str = pUnit->GetWord();
	if (str.size()>1 && str[str.size()-1]==L'.') { return true; }
	return false;
}

bool UnitPropertiesOperator::IsParentHavePoint(SS::Interface::Core::BlackBox::IUnit* pUnit)
{	
	IUnit* pParentUnit = pUnit->GetParentUnit();
	wstring str1 = pUnit->GetWord();
	if (pParentUnit==NULL) return false;
	while(pParentUnit)
	{
		wstring str = pParentUnit->GetWord();

		if (str.size()>1 && str[str.size()-1]==L'.'&&
		   (str1 == L"." || str.find(str1) != size_t(-1))) 
		{ 
			return true; 
		}
		pParentUnit = pParentUnit->GetParentUnit();		 
	}
	return false;
}


bool UnitPropertiesOperator::IsPersonOnlyNameDict(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	SS::Core::Features::CSemanticFeature oSemantic;
	GetUnitSemanticFeatures(pUnit, oSemantic);
	if (oSemantic.m_SemanticType.IsIncludes(&SS::Core::Features::Values::TSemanticType.smtPerson))
	{
		SS::Core::Features::Types::TSPersonType person = oSemantic.m_SPersonType;
		if (person.IsUndefined())
			return false;
		bool isFirstName = person.IsIncludes(&SS::Core::Features::Values::TSPersonType.semprstFirstName);
		bool isLastName = person.IsIncludes(&SS::Core::Features::Values::TSPersonType.semprstLastName);
		bool isMiddleName = person.IsIncludes(&SS::Core::Features::Values::TSPersonType.semprstMiddleName);
		if (isFirstName)
			person.Remove(SS::Core::Features::Values::TSPersonType.semprstFirstName);
		if (isLastName)
			person.Remove(SS::Core::Features::Values::TSPersonType.semprstLastName);
		if (isMiddleName)
			person.Remove(SS::Core::Features::Values::TSPersonType.semprstMiddleName);
		person.AssignOR(&SS::Core::Features::Values::TSPersonType.Undefined);
		if (person.IsUndefined())
			return true;
	}
	return false;
}

bool UnitPropertiesOperator::IsPersonName(SS::Interface::Core::BlackBox::IUnit* pUnit)
{	
	if (pUnit->IsInitial() || (pUnit->GetEvristicLN() || 
		(IsProperUnknown(pUnit) && !IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtOrganization)) ||
		IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstFirstName) ||
		IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstLastName) ||
		IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstMiddleName)))
	{
		return true;
	}
	return false;
}


bool  UnitPropertiesOperator::IsNotHashOrgInsidePerson(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if (pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase())
	{
		return true;
	}

	if  (IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstFirstName) ||
		IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstLastName) ||
		IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstMiddleName))
		return true;

	if (pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==
		SS::Dictionary::DATA_TYPE::NAMES::ednRProperName) return true;		

	return false;
}

EQuoteType UnitPropertiesOperator::IsQuote(const wstring &str)
{
	if (str.size() == 1)
	{
		wchar_t ch = str[0];
		wchar_t chR = (wchar_t)0x201D;
		wchar_t chL = (wchar_t)0x201C;
		if ((ch == chL) || (ch == L'“') || (ch == L'«'))
			return Open;
		else if ((ch == chR) || (ch == L'”') || (ch == L'»'))
			return Close;
		else if ((ch == L'"') || (ch == L'\''))
			return OpenClose;
	}
	return NotQuote;
}

bool UnitPropertiesOperator::IsProper(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
	if (!pUnit)
		return false;

	bool result = false;
	IIndex *index = pUnit->GetFirstIndex();
	wstring word = pUnit->GetWord();
	if (word.empty())
		result = IsProper(pUnit->GetLeftChildUnit());
	if (index && !result)
	{
		SS::Dictionary::Types::TDictionaryIndex dictIndex = index->GetDictionaryIndex()->GetFirst();
		bool isFirstLetterUpper = dictIndex.GetFirstLetterUpperCase();

		result = isFirstLetterUpper || 
			(dictIndex.GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednRProperName) ||
			(dictIndex.GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus);
	}
	return result;
}

bool UnitPropertiesOperator::IsProperByUpper(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
	IIndex *index = pUnit->GetFirstIndex();
	if (index && index->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase())
        return true;
	else
	{
		wstring word = pUnit->GetWord();
		if (word.empty())
		{
			TUnitList list = pUnit->GetChildList();
			for(TUnitList::iterator it = list.begin(); it != list.end(); ++it)
				if (IsProperByUpper(*it))
					return true;
		}
		else
			return IsAnyUpper(word);
	}
	return false;
}

bool UnitPropertiesOperator::IsAnyUpper(const wstring &str)
{
	for (int i = 0; i < str.length(); ++i)
		if (iswupper(str[i]))
			return true;
	return false;
}

bool UnitPropertiesOperator::IsAllUpper(const wstring &str)
{
	for (int i = 0; i < str.length(); ++i)
		if (!iswupper(str[i]))
			return false;
	return str.length() > 0;
}

bool UnitPropertiesOperator::IsFirstSentenceUnit(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
	return pUnit->GetNumber() == 1;
}

bool UnitPropertiesOperator::IsUpperInsideSentence(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
	return !IsFirstSentenceUnit(pUnit) && IsProperByUpper(pUnit);
}

bool UnitPropertiesOperator::IsGeoPretext(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
    wstring str = pUnit->GetWord();
	str = GetLowerWord(str);
    if ((str == L"в") || (str == L"по") || (str == L"к") || (str == L"у") || (str == L"из") ||
        (str == L"за") || (str == L"от") || (str == L"до") || (str == L"под") || (str == L"через") ||
        (str == L"перед") || (str == L"между") || (str == L"над") || (str == L"из-за") || (str == L"на") ||
        (str == L"сквозь") || (str == L"мимо") || (str == L"из-под") || (str == L"вне") ||
        (str == L"близ") || (str == L"above") || (str == L"across") || (str == L"along") ||
        (str == L"alongside") || (str == L"around") || (str == L"aside") || (str == L"at") ||
        (str == L"athwart") || (str == L"atop") || (str == L"behind") || (str == L"below") ||
        (str == L"beneath") || (str == L"beside") || (str == L"between") || (str == L"beyond") ||
        (str == L"by") || (str == L"down") || (str == L"from") || (str == L"in") || (str == L"inside") ||
        (str == L"into") || (str == L"near") || (str == L"on") || (str == L"onto") || (str == L"opposite") ||
        (str == L"out") || (str == L"outside") || (str == L"over") || (str == L"past") || 
        (str == L"through") || (str == L"to") || (str == L"toward") || (str == L"towards") ||
        (str == L"under") || (str == L"underneath") || (str == L"up") || (str == L"via") ||
        (str == L"out of") || (str == L"outside of") || (str == L"in front of"))
    {
        return true;
    }
    return false;
}

bool UnitPropertiesOperator::IsProperDict(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
	IIndex *index =  pUnit->GetFirstIndex();
	if (index==NULL) return false;


	return
		(index->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==
		SS::Dictionary::DATA_TYPE::NAMES::ednRProperName) ||
		(index->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==
		SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus) ;
}

bool UnitPropertiesOperator::IsNominalEng(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
	IIndex *pIndex =  pUnit->GetFirstIndex();
	if (pIndex==NULL) return false;


	return
		(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==
		SS::Dictionary::DATA_TYPE::NAMES::ednENominalName);
}

bool  UnitPropertiesOperator::IsAbbr(IUnit* pUnit)
{
	SS::Interface::Core::BlackBox::IIndex* pIndex;
	SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex;
	SS::Interface::Core::BlackBox::EIndexType oIndexType;
	wstring str = pUnit->GetWord();
	if (str.size() < 2)
		return false;

	for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		pDictionaryIndex = pIndex->GetDictionaryIndex();

		if(pDictionaryIndex)
		{
			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
			oIndexType = oDictionaryIndex.GetIndexType();
			if (oIndexType == SS::Interface::Core::BlackBox::eitSemanticIndex)
			{
				int index = oDictionaryIndex.GetDictionaryIndex();
				SS::Core::Features::CSemanticFeature oSemanticFeature;
				m_pAMConverterSemantic->EnCode(index, &oSemanticFeature);
				if (oSemanticFeature.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtService) &&
					oSemanticFeature.m_SServiceTypes.Equal(SS::Core::Features::Values::TSServiceTypes.sstAcronymCitation))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool UnitPropertiesOperator::isEvristicNumericAdjective(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
    bool result = false;
    SS::Core::Features::CMorphoFeature oMorfoNumeric;
    oMorfoNumeric.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNumeric;
    if(IsMorpho(pUnit, oMorfoNumeric))
    {
        wstring word = GetMainFormText1(pUnit, SS::Core::Features::Values::GenderType.gtMasculine).form;
        if ((word == L"первый") || (word == L"второй") || (word == L"третий") || (word == L"четвертый") ||
            (word == L"пятый") || (word == L"шестой") || (word == L"седьмой") || (word == L"восьмой") ||
            (word == L"девятый") || (word == L"десятый") || (word == L"одиннадцатый") ||
            (word == L"двенадцатый") || (word == L"тринадцатый") || (word == L"четырнадцатый") ||
            (word == L"пятнадцатый") || (word == L"шестнадцатый") || (word == L"семнадцатый") ||
            (word == L"восемнадцатый") || (word == L"девятнадцатый") || (word == L"двадцатый"))
        {
            result = true;
        }
    }
    return result;
}

bool UnitPropertiesOperator::isEvristicAdj(IUnit* pUnit)
{
	wstring str = pUnit->GetWord();
	if (str.size() < 5)
        return false;
	wstring str1 = str.substr(str.size()-4, 4);
	wstring str2 = str.substr(str.size()-3, 3);
	if (str1==L"ский" || str1==L"ским" || /*str1==L"ском" ||*/ str1==L"кому" || str1==L"ская" || 
        /*str1==L"ской" || */str1==L"скую" || str1==L"ское" || str1==L"ному" || str2==L"ный" || 
        str2==L"ным" || /*str2==L"ном" || */str2==L"ная" || str2==L"ной" || str2==L"ную" || str2==L"ого")
	{
		return true;
	}
    else
        return false;
}

bool UnitPropertiesOperator::IsVisualChild(IUnit* pUnit)
{
	IUnit* root = pUnit->GetParentUnit();
	if (root)
	{
		wstring rootstr = root->GetWord();
		wstring str = pUnit->GetWord();
		if (NumOfPointsInString(rootstr) && (rootstr.find(str)==0 || NumOfPointsInString(str)))
			return false;
	}
	return true;	
}

void UnitPropertiesOperator::SortUnits(TUnitList& allUnits)
{
	allUnits.sort(SortByPosInSentFull);
}

void  UnitPropertiesOperator::GetLinearUnitsWithSort(IUnit* root, list<IUnit*>& allUnits, bool isFirst/*=true*/)
{	
	IUnit* pUnit = root;
	wstring s = pUnit->GetWord();
	allUnits.push_back(root);
	pUnit = root->GetLeftChildUnit();

	while(pUnit)
	{
		GetLinearUnitsWithSort(pUnit, allUnits, false);
		pUnit = pUnit->GetRightUnit();
	}
	if (isFirst)
		SortUnits(allUnits);
}

void UnitPropertiesOperator::GetUnitsInCombiWithSort(IUnit *pRoot, list<IUnit*> &allUnits)
{
    allUnits.push_back(pRoot);
	wstring value = pRoot->GetWord();
    if(value.empty())
    {
        IUnit* pUnit = pRoot->GetLeftChildUnit();
		while(pUnit && IsUndefinedSyntax(pUnit))
        {
			GetUnitsInCombiWithSort(pUnit, allUnits);
            pUnit = pUnit->GetRightUnit();
        }
    }
}

bool UnitPropertiesOperator::IsEnglishWord(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IIndex* pIndex = pUnit->GetFirstIndex();
	if (pIndex)
	{
		SS::Dictionary::Types::TDictionaryIndex index = pIndex->GetDictionaryIndex()->GetFirst();
		if (index.GetLanguage() == SS::Core::Types::ealEnglish)
		{
			if (index.GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng)
			{
				wstring word = pUnit->GetWord();
				if (word.empty())
				{
					TUnitList list = pUnit->GetChildList();
					bool isEnglish = true;
					for(TUnitList::iterator it = list.begin(); isEnglish && (it != list.end()); ++it)
						isEnglish = IsEnglishWord(*it);
					return isEnglish;
				}
				else
					return IsEnglishWord(word);
			}
			return true;
		}
	}
	return false;
}

bool UnitPropertiesOperator::IsEnglishWord(const wstring &word)
{
	static wstring allEnglishLetters = L"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsUuVvWwXxYyZz";
	int englishLetterCount = 0;
	for (int i = word.size() - 1; i >= 0; --i)
		if (allEnglishLetters.find(word[i]) != wstring::npos)
			++englishLetterCount;							
	return (word.size() - englishLetterCount) < englishLetterCount;
}

bool UnitPropertiesOperator::IsBaseMorhpo(SS::Interface::Core::BlackBox::IUnit* pUnit,
										  SS::Core::Features::CMorphoFeature oMorfoAnfr)
{
	wstring str = pUnit->GetWord();
	list<SS::Dictionary::Types::TWordInfo> info;
	GetWordInfo(str, &info, SS::Dictionary::Types::efzOnlySearch);

	std::list<SS::Dictionary::Types::TWordInfo> infoDetect;
	for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin();
		it != info.end();
		++it)
	{	
		SS::Core::Features::CMorphoFeature oMorphoFeature;			
		SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());

		infoDetect.clear();
		if(m_pMorphoAnalyzer->GetWordFormsByIDSource(&(it->front().m_WordIndex.m_DictIndex), 
			&(infoDetect)))
		{
			for(std::list<SS::Dictionary::Types::TWordInfo>::iterator itWord = infoDetect.begin();
				itWord != infoDetect.end();
				++itWord)
			{
				pwi1 = &(itWord->front());

				m_pAMConverter->EnCode(
					pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
					pwi1->m_MorphoTypeInfo.m_MorphoInfo,
					pwi1->m_SpecialMorpho,
					&oMorphoFeature);

				if ((oMorfoAnfr.m_PartOfSpeechTypes.IsUndefined() || oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&oMorfoAnfr.m_PartOfSpeechTypes))&&
					(oMorfoAnfr.m_GenderType.IsUndefined() || oMorphoFeature.m_GenderType.IsIntersectsWith(&oMorfoAnfr.m_GenderType))&&
					(oMorfoAnfr.m_NumberType.IsUndefined() || oMorphoFeature.m_NumberType.IsIntersectsWith(&oMorfoAnfr.m_NumberType))&&
					(oMorfoAnfr.m_AnimInAnimType.IsUndefined() || oMorphoFeature.m_AnimInAnimType.IsIntersectsWith(&oMorfoAnfr.m_AnimInAnimType))&&
					(oMorfoAnfr.m_CaseType.IsUndefined() || oMorphoFeature.m_CaseType.IsIntersectsWith(&oMorfoAnfr.m_CaseType)) && 
					(oMorfoAnfr.m_PronounClass.IsUndefined() || oMorphoFeature.m_PronounClass.IsIntersectsWith(&oMorfoAnfr.m_PronounClass)) &&
					(oMorfoAnfr.m_CommonType.IsUndefined() || oMorphoFeature.m_CommonType.IsIntersectsWith(&oMorfoAnfr.m_CommonType)) &&
					(oMorfoAnfr.m_OfficialType.IsUndefined() || oMorphoFeature.m_OfficialType.IsIntersectsWith(&oMorfoAnfr.m_OfficialType)))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool UnitPropertiesOperator::IsParentNonVerb(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IUnit* pParentUnit = pUnit->GetParentUnit();
	if (pParentUnit == NULL) return false;
	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
	if (IsMorpho(pParentUnit,oMorfoPattern)) return false;
	return true;
}

bool UnitPropertiesOperator::GetUnknownAdjNormForm(SS::Interface::Core::BlackBox::IUnit* pUnit)
{ 
	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	SS::Core::Features::CMorphoFeature oMorfoPattern;

	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
	IIndex* pIndex = pUnit->GetFirstIndex();
	if ((pIndex && pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus) ||
		!IsMorpho(pUnit, oMorfoPattern)
	   )
	{
		return false;
	}

	wstring str = pUnit->GetWord();
	wstring strLo = GetLowerWord(str);

	if (strLo.size() < 6)
		return false;

	wstring strSuffix1 = strLo.substr(strLo.size() - 5, 4);
	wstring strSuffix2 = strLo.substr(strLo.size() - 4, 3);
	if (strSuffix1==L"вого" || strSuffix1==L"вому" || strSuffix2==L"вым" || strSuffix2==L"вом" ||
		strSuffix2==L"вая" || strSuffix2==L"вой" || strSuffix2==L"вую")
	{
		// теперь движемся отсекаем по букве и смотрим - известное ли слово
		for(unsigned int i = 1; i < str.size() - 4; ++i)
		{
			wstring sShort = str.substr(i, str.size());
			GetWordInfo(sShort, &oLst, SS::Dictionary::Types::efzFullAddNewWords);
			// нашли известное слово
			if ((&oLst.begin()->front())->m_WordIndex.m_DictIndex.GetDictionaryNumber()!=SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus)
			{
				return true;
			}
		}
	}
	return false;
}


// проверяет наличие апострофа внутри цепочки. Нужно для английского
bool UnitPropertiesOperator::IsPersonApostrof(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IUnit *pChild = pUnit->GetLeftChildUnit();
	if (pChild!=NULL)
	{
		SS::Dictionary::Types::TDictionaryIndex oIndexVal;	
		if (pChild->GetFirstIndex()==NULL) return false;
		pChild->GetFirstIndex()->GetDictionaryIndex()->GetFirst(&oIndexVal);
		if (!oIndexVal.GetFirstLetterUpperCase()) return false;
	}
	while (pChild!=NULL)
	{
		wstring str = pChild->GetWord();
		if (str.size()>0 && (str[0]=='\'' || str[0]=='`')) return true;
		pChild = pChild->GetRightUnit();		
	}
	return false;
}

int UnitPropertiesOperator::GetNominalNounCount(TUnitList& lstUnits)
{
	int nNounCount = 0;
	SS::Core::Features::CMorphoFeature oMorfoPatternNoun;
	oMorfoPatternNoun.Undefine();
	oMorfoPatternNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;

	for(TUnitList::iterator itUnit = lstUnits.begin(); itUnit != lstUnits.end(); ++itUnit)
	{
		if 	(IsMorpho(*itUnit, oMorfoPatternNoun))
			++nNounCount;
	}
	return nNounCount;
}

int UnitPropertiesOperator::GetUnitOffset(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	SS::Interface::Core::BlackBox::IUnit* pChildUnit = pUnit->GetLeftChildUnit();
	int nOffset = pUnit->GetFirstCharPosition();
	while(pChildUnit)
	{
		int nOffsentnew = GetUnitOffset(pChildUnit);	
		if ((nOffsentnew!=-1 && nOffsentnew<nOffset) || nOffset==-1 ) nOffset = nOffsentnew;
		pChildUnit = pChildUnit->GetRightUnit();
	}
	return nOffset;
}

int UnitPropertiesOperator::GetSentenceOffset(SS::Interface::Core::BlackBox::ISentence* pSent)
{		
	SS::Interface::Core::BlackBox::IUnit* pCurUnit = pSent->GetFirstUnit(); 		
	int nOffset = GetUnitOffset(pCurUnit);
	while(pCurUnit)
	{
		pCurUnit = pCurUnit->GetRightUnit();
		if (pCurUnit)
		{
			int nOffsentnew = GetUnitOffset(pCurUnit);	
			if ((nOffsentnew!=-1 && nOffsentnew<nOffset) || nOffset==-1)
                nOffset = nOffsentnew;			
		}
	}
	return nOffset;			
}

bool UnitPropertiesOperator::IsComma(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	wstring strWord = pUnit->GetWord();
	if (strWord == L",")
        return true;
	return false;
}

bool UnitPropertiesOperator::IsCombiUnit(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IIndex* pIndex = pUnit->GetFirstIndex();
	if (pIndex)
	{
		return (pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory) ||
			(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory);
	}
	return false;
}

bool UnitPropertiesOperator::IsCombiUnitParent(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	IUnit* pParent = pUnit->GetParentUnit();
	return pParent && IsCombiUnit(pParent);
}

int UnitPropertiesOperator::GetDefisCount(const wstring &word)
{
	if(word.empty())
        return 0;

    int pos = 0;
    int defisCount = -1;
    while(pos != wstring::npos)
    {
        pos = word.find(L"-", pos + 1);
        ++defisCount;
    }
    return defisCount;
}

bool UnitPropertiesOperator::IsShortWordForm(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
    bool containsDefis = false;
	wstring strWord = pUnit->GetWord();
	if(GetDefisCount(strWord) == 1)
    {
		bool isUpper = true;
		bool haveLetterAfterDefis = false;
		int defisPosition = -1;
		for (unsigned int i = 0; i < strWord.length(); ++i)
		{
			if (strWord[i] == L'-')
				defisPosition = i;
			else if (iswalpha(strWord[i]))
			{
				isUpper = isUpper || iswupper(strWord[i]);
				if (defisPosition >= 0)
					haveLetterAfterDefis = true;
			}
		}

        if (!isUpper && ((defisPosition < 3) || (strWord.length() - defisPosition < 3)))
            containsDefis = haveLetterAfterDefis;
    }
    return containsDefis;
}

bool UnitPropertiesOperator::IsOnlyEnglishLetters(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
    static wstring englishLetters = L"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsUuVvWwXxYyZz";
    wstring str = pUnit->GetWordFull();

    bool onlyEnglish = (str.size() > 0);
    if(onlyEnglish)
    /// первый и последний символы - буквы
    {
        onlyEnglish = (englishLetters.find(str[0]) != wstring::npos) && 
            (englishLetters.find(str[str.size() - 1]) != wstring::npos);
    }
    if(onlyEnglish)
    {
        for(unsigned int i = 0; i < str.size(); ++i)
        {
            bool isEnglish = (englishLetters.find(str[i]) != wstring::npos);
            bool isDigit = iswdigit(str[i]);
            bool isPunct = ((str[i] == L' ') || str[i] == L'.');
            if(isDigit || !(isEnglish || isPunct))
                return false;
        }
    }
    return onlyEnglish;
}

bool UnitPropertiesOperator::IsRomanNumber(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	return IsRomanNumber(pUnit->GetWord());
}

bool UnitPropertiesOperator::IsRomanNumber(const wstring &str)
{
	bool isRoman = !str.empty();
	wstring alphabet = L"IVX";
	for (unsigned int i = 0; isRoman && (i < str.size()); ++i)
		if (alphabet.find(str[i]) == wstring::npos)
			isRoman = false;
    return isRoman;
}

bool UnitPropertiesOperator::IsArabicNumber(const wstring &str)
{
	bool isArabic = !str.empty();
	for (unsigned int i = 0; isArabic && (i < str.size()); ++i)
		if (!iswdigit(str[i]))
			isArabic = false;
	return isArabic;
}

int UnitPropertiesOperator::ConvertFromRomanNumber(const wstring &number)
{
	int result = 0;
	if (IsRomanNumber(number))
	{
		for (unsigned int i = 0; i < number.size(); ++i)
		{
			switch (number[i])
			{
				case 'X':
					result += 10;
					break;
				case 'V':
					result += 5;
					break;
				case 'I':
					if ((i + 1 < number.size()) && ((number[i + 1] == L'X') || (number[i + 1] == L'V')))
						result -= 1;
					else
						result += 1;
					break;
				default: break;
			}
		}
	}
	return result;
}

wstring UnitPropertiesOperator::SpaceNormalize(wstring strSrc)
{
	int spCount = 0;
	for(int i = strSrc.size() - 1; i >= 0; --i)
	{
		if (strSrc[i] != L' ')
			break;
		++spCount;
	}
	wstring resStr = strSrc;
	if (spCount > 0)
		resStr = resStr.substr(0, resStr.size() - spCount);
	return resStr;
}

wstring UnitPropertiesOperator::QuoteNormalize(wstring strSrc)
{
	if (strSrc.size() == 0)
        return L"";
	wstring str1 = L"";
	wstring resStr = L"";
	wstring resStr1 = L"";
	int qCount = 0; 
	int lastQIndex = -1;
	wstring s = L"" ;
	for(int i = strSrc.size() - 1; i >= 0; --i)
	{
		s.clear();
		s += strSrc[i];
		if ((IsQuote(s) != EQuoteType::NotQuote) || (strSrc[i] == L'\''))
		{ 
			qCount++;
			lastQIndex = i;
			resStr = strSrc[i] + resStr;
		}
		else
        {
			if (strSrc[i]==L' ' && i==lastQIndex-1 && (qCount%2)==1)
				continue;
			else
				resStr = strSrc[i] + resStr;
        }
	}

	qCount = 0;
	lastQIndex = 1000;
	for(unsigned int i = 0; i < resStr.size(); ++i)
	{
		s.clear();
		s += resStr[i];
		if (IsQuote(s) != EQuoteType::NotQuote)
		{ 
			++qCount;
			lastQIndex = i;
			resStr1 += resStr[i];
		}
		else
        {
			if (strSrc[i]== L' ' && i==lastQIndex+1 && (qCount%2)==1)
				continue;
			else
				resStr1 += resStr[i];
        }
	}
	wstring resStr2 = L"";
	for(unsigned int i = 0; i < resStr1.size(); ++i)
	{
		s.clear();
		s += resStr1[i];
		if (IsQuote(s) != EQuoteType::NotQuote)
            resStr2 += L"'";
		else
			resStr2 += resStr1[i];
	}

	return resStr2;
}

void RemoveSign(wstring &str, wstring sign)
{
	for (int i = str.find(sign); i != wstring::npos; i = str.find(sign, i))
	{
		if (iswalpha(str[i - 1]) && ((str.size() >= i + sign.size()) || (str[i + sign.size()] == L' ')))
		{
			str.erase(i, sign.size());
			i -= sign.size();
		}
		else
			++i;
	}
}
void UnitPropertiesOperator::RemovePossessiveSign(wstring &str)
{
	RemoveSign(str, L"'s");
	RemoveSign(str, L"s'");
}

wstring UnitPropertiesOperator::GetUnmarkingTextXML(const wstring &srcText, int start, int end)
{
	ATL::CAtlStringW result;
	result += L"<U>";
	result += Get_CDATA_TextXML(srcText, start, end).c_str();
	result += L"</U>";
	return result.GetBuffer();
}

wstring UnitPropertiesOperator::Get_CDATA_TextXML(const wstring &srcText, int start, int end)
{
	ATL::CAtlStringW result;
	result += L"<![CDATA[";
	result += srcText.substr(start, end - start).c_str();
	result += L"]]>";
	return result.GetBuffer();
}

void UnitPropertiesOperator::GetAllWordForms(wstring strWord, set<wstring>& lstForms)
{
	std::list<SS::Dictionary::Types::TWordInfo> oLst;
	GetWordInfo(strWord, &oLst, SS::Dictionary::Types::efzFullAddNewWords);
	for(std::list<SS::Dictionary::Types::TWordInfo>::iterator itForm = oLst.begin();
        itForm != oLst.end();
        ++itForm)
	{
		SS::Dictionary::Types::SWordInfo* pwi = &(itForm->front());
		std::list<SS::Dictionary::Types::TWordInfo> lstWordInfoDetect;
		m_pMorphoAnalyzer->GetWordFormsByIDSource(&pwi->m_WordIndex.m_DictIndex, &lstWordInfoDetect);
		for(TWIList::iterator itWord = lstWordInfoDetect.begin(); itWord != lstWordInfoDetect.end(); ++itWord)
		{
			lstForms.insert(itWord->front().m_wWord);
		}
	}
}

wstring UnitPropertiesOperator::GetLowerWord(const wstring &str)
{
	wstring word = str;
	_wcslwr_s((wchar_t*)word.c_str(), word.length() + 1);
	return word;
}

void UnitPropertiesOperator::AddUnitsToList(TUnitList &to, const TUnitList &from)
{
	for (TUnitList::const_iterator it = from.begin(); it != from.end(); ++it)
		to.push_back(*it);
}

bool UnitPropertiesOperator::IsNumber(const wstring &str)
{
	for (unsigned int i = 0; i < str.length(); ++i)
		if (!iswdigit(str[i]))
			return false;
	return true;
}

vector<wstring> UnitPropertiesOperator::GetChangeablePartsByDefis(SS::Interface::Core::BlackBox::IUnit *pUnit)
{
	vector<wstring> words;

	wstring word = pUnit->GetWord();
	int defisIndex = word.find(L'-');
	if ((defisIndex == wstring::npos) || (word == L"-"))
		return words;
	
	/// считаем, что слово должно быть написано через дефис - переносы обработали раньше

	wstring left = word.substr(0, defisIndex);
	wstring right = word.substr(defisIndex + 1, word.length() - defisIndex - 1);
	bool isRightNumber = IsNumber(right);
	bool isLeftEnglish = IsEnglishWord(left);
	bool isRightEnglish = IsEnglishWord(right);
	
	SS::Core::Features::CMorphoFeature oLeftMorpho;
	SS::Core::Features::CMorphoFeature oRigthMorpho;
	vector<SS::Core::Features::CMorphoFeature> inclusion;

	SS::Core::Features::CMorphoFeature oMorpho;
	GetUnitMorfoFeaturesFullByMorphoAnalyzer(word, oMorpho);
	if (oMorpho.IsUndefined() || isRightNumber || (left.size() == 1) || isLeftEnglish)
	/// слова через дефис нет в словаре
	{
		GetUnitMorfoFeaturesFullByMorphoAnalyzer(left, oLeftMorpho);
		if (isLeftEnglish && isRightEnglish)
		/// английское слово
		{
			if ((left == L"co")   || (left == L"un")    || (left == L"non")  || (left == L"counter") || (left == L"anti") ||
				(left == L"ultra") || (left == L"semi") || (left == L"pre")  || (left == L"pro")     || (left == L"de")   ||
				(left == L"ex")   || (left == L"self")  || (left == L"re")   || (left == L"vice")    || (left == L"mid")  ||
				oLeftMorpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric))
			{
				words.push_back(right);
			}
			else
			{
				GetUnitMorfoFeaturesFullByMorphoAnalyzer(right, oRigthMorpho);
				if (oRigthMorpho.m_OfficialType.IsContain(SS::Core::Features::Values::OfficialType.otPretext) ||
					oRigthMorpho.m_OfficialType.IsContain(SS::Core::Features::Values::OfficialType.otParticle) ||
					oRigthMorpho.m_OfficialType.IsContain(SS::Core::Features::Values::OfficialType.otConjunction) ||
					oRigthMorpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb))
				{
					words.push_back(left);
				}
				else
				{
					words.push_back(left);
					words.push_back(right);
				}
			}
		}
		else if (isRightNumber && (IsAllUpper(left) || (left.size() <= 2)))
		{ }
		else if ((left == L"кое") || (left == L"кой") || (left == L"пол") || IsNumber(left) ||
			(left == L"мини") || (left == L"вице") || (left == L"экс") || (left.size() == 1) || isLeftEnglish)
		/// изменяемая - правая часть
		{
			words.push_back(right);
		}
		else if ((right == L"то") || (right == L"либо") || (right == L"нибудь") || (right == L"ка") ||
			(right == L"де") || (right == L"с") || (right == L"таки") || isRightNumber)
		/// изменяемая - левая часть
		{
			words.push_back(left);
		}
		else if (left == right)
		/// левая и правая части одинаковы
		{
			words.push_back(left);
			words.push_back(right);
		}
		else
		{
			oMorpho.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
			inclusion.push_back(oMorpho);
			GetUnitMorfoFeaturesFullByMorphoAnalyzer(left, oLeftMorpho, 0, &inclusion);
			GetUnitMorfoFeaturesFullByMorphoAnalyzer(right, oRigthMorpho, 0, &inclusion);
			if (!oLeftMorpho.IsUndefined() && !oRigthMorpho.IsUndefined())
			/// левая и правая части - существительные
			{
				words.push_back(left);
				words.push_back(right);
			}
			else
			{
				inclusion.clear();
				oMorpho.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
				inclusion.push_back(oMorpho);
				GetUnitMorfoFeaturesFullByMorphoAnalyzer(left, oLeftMorpho, 0, &inclusion);
				GetUnitMorfoFeaturesFullByMorphoAnalyzer(right, oRigthMorpho, 0, &inclusion);
				if (!oRigthMorpho.IsUndefined() &&
					oLeftMorpho.m_GenderType.Equal(SS::Core::Features::Values::GenderType.gtNeutral) &&
					oLeftMorpho.m_NumberType.Equal(SS::Core::Features::Values::NumberType.ntSingle) &&
					oLeftMorpho.m_ShortType.Equal(SS::Core::Features::Values::ShortType.stShort))
				/// правое - прилагательное, левое - прилагательное единственного числа, среднего рода и краткое
				{
					words.push_back(right);
				}
			}
		}
	}
	return words;
}

bool UnitPropertiesOperator::GetIntersectMorpho(
	const wstring &leftWord,
	const wstring &rightWord,
	SMIList &result,
	wstring *pChangeWord/* = 0*/,
	wstring *pConstWord/* = 0*/,
	bool *pIsAnimation/* = 0*/)
{
	result.clear();
	SMIList left;
	SMIList right;
	SS::Core::Features::CMorphoFeature oMorphoInfo;
	SS::Core::Features::CMorphoFeature oMorpho;
	vector<SS::Core::Features::CMorphoFeature> inclusion;
	oMorpho.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
	inclusion.push_back(oMorpho);
	GetUnitMorfoFeaturesFullByMorphoAnalyzer(leftWord, oMorphoInfo, &left, &inclusion);
	GetUnitMorfoFeaturesFullByMorphoAnalyzer(rightWord, oMorphoInfo, &right, &inclusion);
	IntersectMorphoInfoLists(left, right, result, false, true, true);
	if (result.empty())
	/// пустое пересечение морфологических характеристик
	{
		SMIList *pChange = 0;
		SMIList *pConst = 0;
		if (IsMorpho(right, 0, SS::Core::Features::Values::CaseType.ctNominative))
		{
			pChange = &left;
			pConst = &right;
			if (pChangeWord && pConstWord)
			{
				*pChangeWord = leftWord;
				*pConstWord = rightWord;
			}
		}
		else if (IsMorpho(left, 0, SS::Core::Features::Values::CaseType.ctNominative))
		{
			pChange = &right;
			pConst = &left;
			if (pChangeWord && pConstWord)
			{
				*pChangeWord = rightWord;
				*pConstWord = leftWord;
			}
		}
		else
			return false;
		IntersectMorphoInfoLists(result, *pChange, result);
		if (pIsAnimation)
		{
			*pIsAnimation = IsMorpho(*pConst, 0, SS::Core::Features::Values::GenderType.Undefined,
				SS::Core::Features::Values::CaseType.ctNominative, SS::Core::Features::Values::NumberType.Undefined,
				SS::Core::Features::Values::AnimInAnimType.aiaAnimation);
		}
	}
	else if (pIsAnimation)
		*pIsAnimation = IsMorpho(right, &result, SS::Core::Features::Values::AnimInAnimType.aiaAnimation);
	return true;
}

void UnitPropertiesOperator::SetMorphoInformation(SS::Interface::Core::BlackBox::IUnit *pUnit, SStorageUnitInfo info)
{
	MorphoInformationList::iterator it = m_morphoInfo.find(pUnit);
	if (it == m_morphoInfo.end())
		m_morphoInfo.insert(std::make_pair(pUnit, info));
	else
		(*it).second = info;
}

bool UnitPropertiesOperator::GetMorphoInformation(
	SS::Interface::Core::BlackBox::IUnit *pUnit,
	SS::Core::Features::CMorphoFeature &resMorphoFeature,
	SMIList *lstMI)
{
	bool result = false;
	MorphoInformationList::iterator it = m_morphoInfo.find(pUnit);
	if (it != m_morphoInfo.end())
	/// есть сохраненная информация
	{
		SStorageUnitInfo *pInfo = &(*it).second;
		resMorphoFeature = pInfo->_morpho;
		if (lstMI)
		{
			if (pInfo->_listMI.empty())
				lstMI->push_back(CreateSMorphoInfo(resMorphoFeature));
			else
				IntersectMorphoInfoLists(*lstMI, pInfo->_listMI, *lstMI);
		}
		result = true;
	}
	return result;
}

bool UnitPropertiesOperator::GetWordInfo(
	const wstring &word,
	std::list<SS::Dictionary::Types::TWordInfo> *oLst,
	SS::Dictionary::Types::EFindZone zone)
{
	wstring wordForFind = word.substr(0, SS::Constants::MAX_CHARS_IN_WORD - 1);
	return m_pMorphoAnalyzer->GetWordInfo(wordForFind.c_str(), oLst, zone);
}

vector<wstring> UnitPropertiesOperator::GetSetStrings(set<wstring> collection)
{
	vector<wstring> result;
	for (set<wstring>::iterator it = collection.begin(); it != collection.end(); ++it)
		result.push_back(*it);
	return result;
}

SS::Interface::Core::BlackBox::IUnit* UnitPropertiesOperator::GetUnitByPosition(TUnitList unitList, int position)
{
	for (TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		IUnit *pUnit = *it;
		if (pUnit->GetPosition() == position)
			return pUnit;
	}
	return 0;
}