#include "StdAfx.h"
#include ".\smquestionanalysis.h"
#include "SMManager.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{


CSMQuestionAnalysis::CSMQuestionAnalysis(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CSMQuestionAnalysis::~CSMQuestionAnalysis(void)
{
}
SMorphoDescription * CSMQuestionAnalysis::GetUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(pUnit==NULL) pUnit=m_pCurrentUnit;
	return m_Frame.GetUnitDescription(pUnit);
}
void CSMQuestionAnalysis::AddUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit,SMorphoDescription & oDescription )
{
	m_Frame.AddUnitDescription(pUnit,oDescription);
}
bool CSMQuestionAnalysis::Init(IText * pIText)
{
	if(!CSMSentenceExpert::Init(pIText)) return false;

	SS::Core::Features::CSemanticFeature oSemanticFeature;
	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtMeasure;
	oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtNumber;
	m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semNumber);

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstName;
	m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semName);

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstLastName;
	m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semLastName);

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstFirstName;
	m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semFirstName);

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtVerb;
	oSemanticFeature.m_SVerbType=Values::TSVerbType.semvtBecome;
	m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semBecome);

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtVerb;
	oSemanticFeature.m_SVerbType=Values::TSVerbType.semvtBe;
	m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semVBe);

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtVerb;
	oSemanticFeature.m_SVerbType=Values::TSVerbType.semvtAcquirePossess;
	m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_semVPossess);

	return true;
}

bool CSMQuestionAnalysis::IsLegalLexem(IUnit* pUnit)
{
	TCHAR * sz_Word;
	sz_Word=(TCHAR*)pUnit->GetWord();
	if (!sz_Word || sz_Word[0]=='\"' || sz_Word[0]=='.' || sz_Word[0]==',' || sz_Word[0]==';' || 
		sz_Word[0]=='?' || sz_Word[0]==':' /*|| sz_Word[0]==0*/ ) return false;
	return true;
}

void CSMQuestionAnalysis::ProcessSentence()
{
	SMorphoDescription oDescription;

	do
	{
		if ( IsLegalLexem(m_pCurrentUnit) /*!m_LexemType.IsSymbol()*/ 
			)
		{
			oDescription._oMorphoFeature.Undefine();
			oDescription._oMorphoFeature.m_PartOfSpeechTypes.SetValue(0);

			CollectMorphoFeature(oDescription._oMorphoFeature);

			oDescription._oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.Undefined);
			//if (!m_LexemType.Iseng() && !m_LexemType.Isrus()  && !SpecialTypeEquals(ustWordCombination) )
			//	oDescription._oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);

			m_Frame.AddUnitDescription(m_pCurrentUnit,oDescription);

			if(!GetNextUnit()) break;
		}else if (!IsSyntax())
		{
			if(!HideCurrentUnit()) break;
		}else
		{
			oDescription._oMorphoFeature.Undefine();
			oDescription._oMorphoFeature.m_PartOfSpeechTypes.SetValue(0);
			m_Frame.AddUnitDescription(m_pCurrentUnit,oDescription);
			if(!GetNextUnit()) break;
		}
	}while(true);
}

void CSMQuestionAnalysis::AddAllPlaceSemantic(IUnit * pHead)
{
	SS::Dictionary::Types::TDictionaryIndex lVal=0;
	SS::Core::Features::CSemanticFeature oSemanticFeature;

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;

	oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltState;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltCity;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltContinent;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltCountry;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltProvince;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltHotel;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltIsland;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SPlaceType.Undefine();
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);

}
void CSMQuestionAnalysis::AddAllOrganisationSemantic(IUnit * pHead)
{
	SS::Dictionary::Types::TDictionaryIndex lVal=0;
	SS::Core::Features::CSemanticFeature oSemanticFeature;

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtOrganization;

	oSemanticFeature.m_SOrganizationType=Values::TSOrganizationType.orgtCompany;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SOrganizationType=Values::TSOrganizationType.orgtBank;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SOrganizationType=Values::TSOrganizationType.orgtFund;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
	oSemanticFeature.m_SOrganizationType=Values::TSOrganizationType.orgtInternationalOrganization;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
}

void CSMQuestionAnalysis::AddAllTimeIntervalSemantic(IUnit * pHead)
{
	SS::Dictionary::Types::TDictionaryIndex lVal=0;
	SS::Core::Features::CSemanticFeature oSemanticFeature;

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtTimeInterval;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);


	oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateYear;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);

	oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateDay;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);

	oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateMonth;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);

	oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDayTime;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);

	oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitCentury;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
}
void CSMQuestionAnalysis::AddAllPersonSemantic(IUnit * pHead, bool bCountry)
{
	SS::Dictionary::Types::TDictionaryIndex lVal=0;
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstFirstName;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);

	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstAgent;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstLastName;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstMiddleName;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstNationality;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstProfession;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstName;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	////
	if(bCountry)
	{
		oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;
		oSemanticFeature.m_SPlaceType=Values::TSPlaceType.pltCountry;
		m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
		lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
		AddSemanticIndex(lVal,pHead);
	}

}
void CSMQuestionAnalysis::AddAllPersonSemanticInAnimate(IUnit * pHead)
{
	SS::Dictionary::Types::TDictionaryIndex lVal=0;
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPerson;
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstAgent;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstNationality;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstProfession;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
	//oSemanticFeature.m_SPersonType=Values::TSPersonType.semprstName;
	//m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	//AddSemanticIndex(lVal,pHead);
}

void CSMQuestionAnalysis::AddAcroCitationSemantic(IUnit * pHead)
{
	SS::Dictionary::Types::TDictionaryIndex lVal=0;
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	oSemanticFeature.m_SemanticType.Assign(Values::TSemanticType.smtService);
	oSemanticFeature.m_SServiceTypes.Assign(Values::TSServiceTypes.sstAcronymCitation);
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);	
}


void CSMQuestionAnalysis::SetSyntaxFiltered(IUnit* pHead,IUnit* pPrep,bool bPlace)
{
	if (!pHead || !pPrep) return ;
	list<Types::SyntaxCategories> l_Cat;
	wstring str=pPrep->GetWord();

	m_StringService.LowerSz((TCHAR*)str.c_str());
	/*char buf[50];
	wcstombs(buf,str.c_str(),50);
	string str2(buf);*/

	if( m_pSMManager->GetOfficialVirtualSyntax()->GetByFormPrepositionSyntaxCategories(&str,&l_Cat) )
	{
		if (FilterSyntax(l_Cat,!bPlace))
		{
			AddSyntaxCategoryIndices(l_Cat,pHead);
		}
	}
}

void CSMQuestionAnalysis::AddToBeIndex(IUnit* pHead,Types::TSVBeType  tsType)
{
	if (!pHead) return;
	SS::Dictionary::Types::TDictionaryIndex lVal=0;
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtVerb;
	oSemanticFeature.m_SVerbType=Values::TSVerbType.semvtBe;
	oSemanticFeature.m_SVBeType=tsType;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal,pHead);
}
void CSMQuestionAnalysis::SetToBeIndex(IUnit* pHead,Types::TSVBeType  tsType)
{
	if (!pHead) return;
	SS::Dictionary::Types::TDictionaryIndex lVal=0;
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtVerb;
	oSemanticFeature.m_SVerbType=Values::TSVerbType.semvtBe;
	oSemanticFeature.m_SVBeType=tsType;
	m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
	lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
	AddSemanticIndex(lVal, pHead);
}

bool CSMQuestionAnalysis::ProcessVerbSemantic(IUnit* pVerb,SS::Core::Features::CSemanticFeature& oSemanticFeature,list<SS::Dictionary::Types::TDictionaryIndex>& l_SemValues)
{
	SS::Core::Features::CSemanticFeature oSemanticFeature2;
	SS::Dictionary::Types::TDictionaryIndex lVal;
	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtMeasure;
	oSemanticFeature.m_SVerbType.SetValue(0);
	set<long> oSemSet;

	IIndex * pIndex=pVerb->GetFirstIndex();
	UINT uiVal;
	if (pIndex)
	{
		do{
			if(IsSemanticIndex(pIndex))
			{
				//uiVal=(pIndex->GetDictionaryIndex()->GetFirst())&0x00ffffff;
				uiVal = pIndex->GetDictionaryIndex()->GetFirst().GetId();
				m_pIAMCSemantic->EnCode(uiVal,&oSemanticFeature2);
				if (oSemanticFeature2.m_SemanticType.Equal(Values::TSemanticType.smtVerb)) 
				{
					if (oSemanticFeature2.m_SVerbType.Equal(Values::TSVerbType.semvtCost))
					{
						oSemanticFeature.m_SMeasureType.AssignOR(&Values::TSMeasureType.msrtPrice);

						oSemanticFeature2.Undefine();
						oSemanticFeature2.m_SemanticType=Values::TSemanticType.smtMeasure;
						oSemanticFeature2.m_SMeasureType=Values::TSMeasureType.msrtPrice;

						m_pIAMCSemantic->Code(&oSemanticFeature2,&lVal);

						lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
						oSemSet.insert(lVal.GetDictionaryIndex());

					}else if (oSemanticFeature2.m_SVerbType.Equal(Values::TSVerbType.semvtWeigh))
					{
						oSemanticFeature.m_SMeasureType.AssignOR(&Values::TSMeasureType.msrtWeight);

						oSemanticFeature2.Undefine();
						oSemanticFeature2.m_SemanticType=Values::TSemanticType.smtMeasure;
						oSemanticFeature2.m_SMeasureType=Values::TSMeasureType.msrtWeight;

						m_pIAMCSemantic->Code(&oSemanticFeature2,&lVal);
						lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
						oSemSet.insert(lVal.GetDictionaryIndex());
					}
				}

			}
		}while((pIndex=pVerb->GetNextIndex()));
	}
	l_SemValues.assign(oSemSet.begin(),oSemSet.end());
	return !l_SemValues.empty();
}
void CSMQuestionAnalysis::SetSingleSyntax(IUnit* pUnit)
{
	if(!pUnit) return;
	pUnit->Pop();
	m_pCurrentSentence->AddUnit(pUnit);
	//SetSyntaxType(usntEntity2,pUnit);
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
	//m_pMorphoService->CollectSemanticFeature(m_pIAMCSemantic,pUnit->m_MorphoInfo,oSemanticFeature,l_Sem);
	//m_pMorphoService->FilterSemanticTypes(m_pIAMCSemantic,Values::TSemanticType.smtMeasure,l_Sem);
	RemoveSemanticIndicesFromUnit(l_Sem,pUnit);
	KeepSpecifedSemanticTypes(Values::TSemanticType.smtMeasure,l_Sem);

	//pUnit->m_SyntaxInfo.SetSemantic(l_Sem);
	SetSyntaxCategory(pUnit,Values::SyntaxCategories.scAjunct);
	SetSemanticIndices(l_Sem,pUnit);
}
void CSMQuestionAnalysis::StrictQuery()
{
	if(!GetFirstUnit()) return;
	
	IUnit * pHead=m_pIBlackBoxFactory->CreateUnit();
	pHead->JoinUpInterval(GetFirstUnit(),GetLastUnit());
	m_pCurrentUnit=pHead;
	AddSyntaxCategoryIndex(Values::SyntaxCategories.scMainEntity);
	AddSyntaxCategory(pHead,Values::SyntaxCategories.scMainEntity);

}

void CSMQuestionAnalysis::SimpleQuery()
{
	if(!GetFirstUnit()) return;

	IUnit * pHead=m_pIBlackBoxFactory->CreateUnit();
	pHead->JoinUpInterval(GetFirstUnit(),GetLastUnit());
	m_pCurrentUnit=pHead;
	//AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunct);
	AddSyntaxCategory(pHead,Values::SyntaxCategories.scAjunct);

}

void CSMQuestionAnalysis::MoveUnitToPredicate(IUnit * pUnit, IUnit * pAfter)
{
	if(pAfter && pUnit)
	{
		pUnit->Pop();
		pAfter->AddUnitHorizontal(pUnit);
	}
}


			}
		}
	}
}