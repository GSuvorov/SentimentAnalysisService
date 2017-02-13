#include "StdAfx.h"
#include ".\smfeaturecompute.h"
#include "..\assinterface\TDictionaryNames.h"
namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{


CSMFeatureCompute::CSMFeatureCompute(void) : m_bMuchMany(false), m_semFirstName(0), m_semLastName(0), m_semPlaceIndf(0)
{
	m_bInit = false;
	m_sDescription=_T(__FUNCTION__);
}

CSMFeatureCompute::~CSMFeatureCompute(void)
{
}

bool CSMFeatureCompute::Init(IText * pIText)
{
	SS_TRY
	{
		if(!CSMSentenceExpert::Init(pIText)) return false;

		if(!m_bInit)
		{
			m_bInit = true;

			SS::Core::Features::CSemanticFeature oSemanticFeature;

			/*oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstAgent;
			m_pIAMCSemantic->Code(&oSemanticFeature,&m_semAgent);*/

			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstFirstName;
			m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semFirstName);

			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
			oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstLastName;
			m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semLastName);

			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;
			oSemanticFeature.m_SPersonType=Values::TSPersonType.Undefined;
			m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semPlaceIndf);
		}
		
		return true;
	}
	SS_CATCH(L"")
}

bool CSMFeatureCompute::AnalizeSentence()
{
	SS_TRY
	{

		if(!GetFirstUnit()) 
			return true;

		UINT uiIndex, uiDicNumber;
		wstring str;
		bool bEng=false,bRus=false, bUpper, bAddSemanticUnknown,bLastName;
		SS::Core::Features::CSemanticFeature oSemanticFeature;
		IDictionaryIndex * pDIndex;
		//EIndexType eit;
		//SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber dn; 
		SS::Core::Types::EAnalyseLanguage eal;

		do
		{
			m_pCurrentUnit->ClearInfo();

			str=m_pCurrentUnit->GetWord();
			m_StringService.LowerSz((TCHAR*)str.c_str());
			m_bMuchMany=m_StringService.StrEqual(str.c_str(),_T("many")) ||m_StringService.StrEqual(str.c_str(),_T("much"));
			bUpper=m_LexemType.IsUpperCase();
			bAddSemanticUnknown=false;
			bLastName=false;

			if(!GetFirstIndex()) 
			{
				continue;
			}
			do
			{
				pDIndex=m_pCurrentIndex->GetDictionaryIndex();
				uiIndex=pDIndex->GetFirst().GetId();
				//uiIndex=pDIndex->GetFirst();
				//uiDicNumber=uiIndex>>24;
				//eit = pDIndex->GetIndexType();
				uiDicNumber = pDIndex->GetFirst().GetDictionaryNumber();
				eal = pDIndex->GetFirst().GetLanguage();

				bEng=false;
				bRus=false;

				switch(eal) 
				{
				case SS::Core::Types::ealEnglish:
					bEng = true;
					break;
				case SS::Core::Types::ealRussian:
					bRus = true;
					break;
				default:
					break;
				};
				
				/*switch(uiDicNumber)
				{
				case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng :
				case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory :
				case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName :
				case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName :
				case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng :
					bEng=true;
					break;
				case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus :
				case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory :
				case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName :
				case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName :
				case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus :
					bRus=true;
					break;
				}*/
				if(bRus || bEng)
				{
					IFeature * pFeature;
					if(!(pFeature=m_pCurrentIndex->GetFirstFeature())) 
						continue;
					do
					{
						m_MorphoFeature.Undefine();
						m_pIAMCMorpho->EnCode(/*uiIndex>>24*/uiDicNumber,pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&m_MorphoFeature);
						if(bRus)
						{
							ProcessRusFeature(m_MorphoFeature);
						}
						else 
							if(bEng)
							{
								ProcessEngFeature(m_MorphoFeature);
							}

					}while((pFeature=m_pCurrentIndex->GetNextFeature()));
				}

				if (uiDicNumber==SS::Dictionary::DATA_TYPE::NAMES::ednSemantic)
				{
					oSemanticFeature.Undefine();
					m_pIAMCSemantic->EnCode(uiIndex/*uiIndex&0x00ffffff*/,&oSemanticFeature);

					if( oSemanticFeature.m_SemanticType.Equal(Values::TSemanticType.smtPlace)||
						oSemanticFeature.m_SemanticType.Equal(Values::TSemanticType.smtOrganization)) 
					{
						m_pCurrentUnit->GetInfo()->_PlaceType.AssignOR(&oSemanticFeature.m_SPlaceType);
					}
					else if( oSemanticFeature.m_SemanticType.Equal(Values::TSemanticType.smtTimeInterval) ) 
					{
						m_pCurrentUnit->GetInfo()->_TimeType.AssignOR(&oSemanticFeature.m_STimeIntervalType);
					}
					else if (oSemanticFeature.m_SPersonType.Equal(Values::TSPersonType.semprstFirstName)||
						oSemanticFeature.m_SPersonType.Equal(Values::TSPersonType.semprstLastName)||
						oSemanticFeature.m_SPersonType.Equal(Values::TSPersonType.semprstMiddleName))
					{
						AddFeature(efcSemanticName);
						if(oSemanticFeature.m_SPersonType.Equal(Values::TSPersonType.semprstLastName))
							bLastName = true;
					}else if (oSemanticFeature.m_SemanticType.Equal(Values::TSemanticType.smtMeasure))
					{
						AddFeature(efcSemanticMeasure);
					}else if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtService) && 
						oSemanticFeature.m_SServiceTypes.IsContain(Values::TSServiceTypes.sstAcronymCitation))
					{
						AddFeature(efcSemanticCitation);
					}

					if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtVerb))
					{
						AddFeature(efcSemanticVerb);
						if (
							oSemanticFeature.m_SVBeType.IsContain(Values::TSVBeType.tsvbtRepresent)||
							oSemanticFeature.m_SVerbType.IsContain(Values::TSVerbType.semvtBecome)
							)
						{
							AddFeature(efcRepresentBecome);
						}
					}
					else{
						AddFeature(efcSemanticCommon);
					}
					
				}else if ( (uiDicNumber==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng||
					uiDicNumber==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus) /*&& 
					bUpper*/
					)
				{
					if(bRus)
					{
						m_MorphoFeature.Undefine();
						CollectMorphoFeature(m_MorphoFeature);
						if(!m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun))
						{
							break;
						}
					}
					AddFeature(efcSemanticName);
					AddFeature(efcSemanticCommon);
					switch(m_pCurrentSentence->GetLanguage()) 
					{
					case SS::Core::Types::ealEnglish:
						AddFeature(efEClearProper);
					break;
					};
					bAddSemanticUnknown=true;

					break;
		
				}else if (uiDicNumber==SS::Dictionary::DATA_TYPE::NAMES::ednEProperName)
				{
					AddFeature(efEClearProper);
				}
			}while(GetNextIndex());

			if (bAddSemanticUnknown)
			{
				m_pCurrentUnit->GetInfo()->_PlaceType.AssignOR(&oSemanticFeature.m_SPlaceType);
				AddSemanticIndex(m_semFirstName+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24));
				AddSemanticIndex(m_semLastName+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24));
				AddSemanticIndex(m_semPlaceIndf+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24));
			}
			if(bLastName)
				Up2SymbolNextLastName();
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}

void CSMFeatureCompute::ProcessEngFeature(SS::Core::Features::CMorphoFeature & oMorphoFeature)
{
	SS_TRY
	{
		if (oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun)) 
		{
			AddFeature(efcNoun);
		}
		else if (oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdjective)||
			oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdverb)) 
		{
			AddFeature(efcAdverbAdjective);
		}
		else if (oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb)) 
		{
			AddFeature(efcVerb);
			if(oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfInfinitive))
			{
				AddFeature(efEVerbInfinitive);
			}
			else if (oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple)&&
				oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent))
			{
				AddFeature(efEVerbParticiplePresent);
			}
		}
		else if (oMorphoFeature.m_OfficialType.Equal(Values::OfficialType.otPretext))
		{
			AddFeature(efcPretext);
		}
		else if(/*IsQuestionMode() &&*/ 
			oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun)&&
			oMorphoFeature.m_PronounGroup.IsContain(Values::PronounGroup.pngNoun)&&
			oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncIndividual))
		{
			AddFeature(efcNoun);
		}

		if (
			oMorphoFeature.m_PartOfSpeechTypes.GetValue()>1 && 
			!oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun) &&
			!(
				oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun) &&
				(
					oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncIndividual) || 
					oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncPossessive)
				) &&
				(
					oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctAccusative) || 
					oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctNominative)
				) &&
				oMorphoFeature.m_PronounGroup.IsContain(Values::PronounGroup.pngNoun)
			)
			)  
		{
			AddFeature(efENotOnlyNoun);
		}


		if(oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctGenitive)) 
		{
			AddFeature(efECaseGenetive);
		}else if 
			( 
			oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postOfficial)&&
			oMorphoFeature.m_OfficialType.Equal(Values::OfficialType.otDeterminers)||			
			oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postPronoun) && 
			oMorphoFeature.m_PronounGroup.Equal(Values::PronounGroup.pngAdjective) &&
			!m_bMuchMany
			)
		{
			AddFeature(efEDetPronounAdjective);
		}
	}
	SS_CATCH(L"")
}

void CSMFeatureCompute::ProcessRusFeature(SS::Core::Features::CMorphoFeature & oMorphoFeature)
{
	SS_TRY
	{
		if (oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun)) 
		{
			AddFeature(efcNoun);
		}
		else if (oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdjective)||
			oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdverb)) 
		{
			AddFeature(efcAdverbAdjective);
		}
		else if (oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb)) 
		{
			AddFeature(efcVerb);
			if( (oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfPersonal) ||
				 oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) && oMorphoFeature.m_ShortType.IsContain(Values::ShortType.stShort)
				)
				&&
				oMorphoFeature.m_VoiceType.IsContain(Values::VoiceType.vtPassive)
			  )
			{
				AddFeature(efRPassive);
			}

			if(oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfDeepr))
			{
				AddFeature(efRVerbDeepr);
			}
		}
		else if (oMorphoFeature.m_OfficialType.Equal(Values::OfficialType.otPretext)) 
		{
			AddFeature(efcPretext);
		}
		else if(/*IsQuestionMode() &&*/ 
			oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun)&&
			oMorphoFeature.m_PronounGroup.IsContain(Values::PronounGroup.pngNoun)
			)
		{
			AddFeature(efcNoun);
		}

		if(oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctNominative)) 
		{
			AddFeature(efRCaseNominative);
		}
		else if (oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctInstrumental))
		{
			AddFeature(efRCaseInstrumental);
		}
		else if (oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctAccusative))
		{
			AddFeature(efRCaseAccusative);
		}
		else if (oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctDative))
		{
			AddFeature(efRCaseDative);
		}
		else if (oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctLocative))
		{
			AddFeature(efRCaseLocative);
		}

		if (oMorphoFeature.m_AnimInAnimType.IsContain(Values::AnimInAnimType.aiaAnimation))
		{
			AddFeature(efRAnimate);
		}
		
		Types::CaseType oCaseType;
		oCaseType.FillUp();
		if(oMorphoFeature.m_CaseType.GetValue()==oCaseType.GetValue())
		{
			AddFeature(efRCaseAll);
		}


		if(oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctGenitive)) 
		{
			AddFeature(efRCaseGenetive);
		}
	}
	SS_CATCH(L"")
}

void CSMFeatureCompute::Up2SymbolNextLastName()
{
	if(!m_pCurrentUnit) return;

	Mem();
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	if(GetPrevUnit() && CollectSemanticFeature(oSemanticFeature))
	{
		if(oSemanticFeature.m_SServiceTypes.IsContain(Values::TSServiceTypes.sstAcronymCitation) && 
			m_LexemType.GetLexemLength()==2)
		{
			list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
			RemoveSemanticIndicesFromUnit(l_Sem);
			AddSemanticIndex(m_semFirstName+(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24));
			GotoMem();
			return;
		}
	}
	GotoMem();
	return;
}

		}
	}
}