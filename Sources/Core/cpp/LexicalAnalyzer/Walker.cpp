#include "stdafx.h"
#include "Walker.h"

#include <iostream>


namespace SS
{
namespace LexicalAnalyzer
{
namespace Walker
{

#include "lex.Walker.cc" 
 
/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////
    
CWalker::CWalker() : 
		m_eMode(wmDefault),
		m_pSent(NULL),
		m_pUnit(NULL),
		m_pUnitChecker(NULL),
		m_pIndexLoader(NULL),
		m_pStorage(NULL), 
		m_bQuery(false),
		m_pUnitStorage(NULL),
		m_nProcessed(0),
		m_nInserted(0)
{
}

CWalker::~CWalker()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CWalker interface

void CWalker::SetAnalyseParams(IAnalyseParams* pAnalyseParams)
{
	CAnalyseParams::SetAnalyseParams(pAnalyseParams);
	m_bQuery =	pAnalyseParams->GetAnalyseMod() == eamQuery ||
				pAnalyseParams->GetAnalyseMod() == eamRejectUnimportant;
}

void CWalker::Init(CUnitChecker* pUnitChecker, CIndexLoader* pIndexLoader, IBlackBoxTextStorage* pStorage, 
				   IR::TUnitStorage* pUnitStorage)
{
	m_pUnitChecker = pUnitChecker;
	m_pIndexLoader = pIndexLoader;
	m_pStorage = pStorage;
	m_pUnitStorage = pUnitStorage;
}

void CWalker::Process(IR::CSentence* pSentence, EWalkerMode eMode)
{
	SS_TRY;

	m_pSent = pSentence;
	m_eMode = eMode;

	m_pUnit = m_pSent->GetFirstUnit();
	m_nProcessed = 0;
	m_nInserted = 1;

	yyrestart(NULL);
	BEGIN(GetMode(eMode));
	for(ELexicalPattern lp = (ELexicalPattern) yylex(); lp; lp = (ELexicalPattern) yylex())
		ProcessPattern(lp);

	SS_CATCH(L"");
}

void CWalker::SetNumberSemantic(IR::CSentence* pSent)
{
	SS_TRY;

	for(IR::CUnit* pUnit = pSent->GetFirstUnit(); pUnit; pUnit = pSent->GetNextUnit())
	{
		if(pUnit->GetType(wmDates) == wtNumberYear && !pUnit->GetUnit()->HasChilds())
			m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDateYear);
		else if(pUnit->GetType(wmNumberSemantic) != wtNumber)
			continue;

		if(!pUnit->GetUnit()->HasIndexType(eitMorphoIndex))
		{
			m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNumeric, 
				Values::CaseType.ctCommon, Values::NumberType.ntSingle);
		}

		if(m_bQuery && iswdigit(pUnit->GetUnit()->GetWord()[0]))
			m_pUnitChecker->ChangeFeature(*pUnit, cfAddOrdering);

		// don't put number semantics if it's ordinal number
		if(!m_pUnitChecker->IsOrdinal(*pUnit))
			m_pUnitChecker->AddMeasureIndex(*pUnit, Values::TSMeasureType.msrtNumber);
	}

	SS_CATCH(L"");
}

void CWalker::SetBeRepresentSemantic(IR::CSentence* pSent)
{
	SS_TRY;

	if(pSent->GetLanguage() != ealRussian)
		return;

	for(IR::CUnit* pUnit = pSent->GetFirstUnit(); pUnit; pUnit = pSent->GetNextUnit())
	{
		if(pUnit->GetType(wmDefault) == wtTire)
			m_pUnitChecker->AddBeSemanticIndex(*pUnit, Values::TSVBeType.tsvbtRepresent);
	}

	SS_CATCH(L"");
}

void CWalker::SetNounToUnknownWords(IR::CSentence* pSent)
{
	SS_TRY;

	if(pSent->GetLanguage() != ealEnglish)
		return;

	for(IR::CUnit* pUnit = pSent->GetFirstUnit(); pUnit; pUnit = pSent->GetNextUnit())
	{
		IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex();
		if(pIndex && pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber() == ednUnknownEng)
		{
			if(wcslen(pUnit->GetUnit()->GetWord()) >= 2)
				m_pUnitChecker->ChangeFeature(*pUnit, cfSetNounCommonSingle);
		}
	}

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CWalker funtcions

int CWalker::LexerInput(char* buf, int max_size)
{
	if(!m_pUnit)
		return 0;

	m_nInserted++;

	*buf = (char) m_pUnit->GetType(m_eMode);
	m_pUnit = m_pSent->GetNextUnit();
	return 1;
}

bool IsComplexUnit(IUnit* pUnit)
{
	if (!pUnit)
		return false;

	wstring word = pUnit->GetWord();
	return word.empty()	&& (pUnit->GetChildCount() > 1);
}

void CWalker::ProcessPattern(ELexicalPattern lp)
{
	SS_TRY;

	list<CaseType> lstCases;

	bool isOnlyPerson = true;
	bool isOrgSemantic = false;
	bool isPlaceSemantic = false;
	bool isPersonSemantic = false;
	bool isXZ = false;
	bool isPronounOrOfficial = false;

	m_nProcessed += yyleng;
	IR::CUnit* pCurrentUnit = m_pSent->GetUnit(m_nInserted - m_nProcessed);
	if( (lp == lpWord) || ( (yyleng == 1) && ( (lp != lpList) && (lp != lpElevenDigitsPhoneNumber) && ( !IsComplexUnit( pCurrentUnit->GetUnit() ) || (lp == lpOrdinalNumber) )) ) )
		return;

	IR::CUnit* pUnit = m_pUnitStorage->Allocate();
	pUnit->SetUnit(m_pStorage->CreateUnit());
	pUnit->SetWordType(wtWord);

	IR::CUnit* pGenNoun = NULL;
	IR::CUnit* pDativeNoun = NULL;

  	switch(lp)
	{
		case lpTimeInterval:
			m_pUnitStorage->Release(pUnit, true);
			break;

		case lpFiveDigitsPhoneNumber:
		case lpSixDigitsPhoneNumber:
		case lpSevenDigitsPhoneNumber:
		case lpTenDigitsPhoneNumber:
		case lpElevenDigitsPhoneNumber:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddServiceTypeIndex(*pUnit, Values::TSServiceTypes.sstTelephoneNumber);
			if (lp == lpFiveDigitsPhoneNumber)
				pUnit->GetUnit()->SetPatternName(L"FiveDigitsPhone");
			else if (lp == lpSixDigitsPhoneNumber)
				pUnit->GetUnit()->SetPatternName(L"SixDigitsPhone");
			else if (lp == lpSevenDigitsPhoneNumber)
				pUnit->GetUnit()->SetPatternName(L"SevenDigitsPhone");
			else if (lp == lpTenDigitsPhoneNumber)
				pUnit->GetUnit()->SetPatternName(L"TenDigitsPhone");
			else if (lp == lpElevenDigitsPhoneNumber)
				pUnit->GetUnit()->SetPatternName(L"ElevenDigitsPhone");
			break;

		case lpDecade:
		case lpDecade1:
		case lpDecade2:
			m_pUnitStorage->Release(pUnit, true);
			break;

		case lpWeekEnd:
			m_pUnitStorage->Release(pUnit, true);
			break;

		case lpM:
			m_pUnitStorage->Release(pUnit, true);
			break;

		case lpYearPart1:
		case lpYearPart2:
			m_pUnitStorage->Release(pUnit, true);
			break;

		case lpDayTime:
			m_pUnitStorage->Release(pUnit, true);
			break;

		case lpMI:
		case lpMI1:
		case lpMI2:
		case lpMI3:
			m_pUnitStorage->Release(pUnit, true);
			break;

		case lpF:
		case lpFI:
		case lpIO:
		case lpFIO:
		{
			bool isNeedJoin = true;
			for(int i = 0; i < yyleng; ++i)
			/// все юниты должны быть из словар€ имен собственных
			{
				IUnit *pSentUnit = m_pSent->GetUnit(m_nInserted - m_nProcessed + i)->GetUnit();
				isNeedJoin = m_pUnitChecker->IsProper(pSentUnit) || pSentUnit->IsInitial();
				if (!isNeedJoin)
				{
					if (m_pUnitChecker->isXZ(pSentUnit))
						continue;
					else
						break;
				}
			}
			if(isNeedJoin)
			{
				if(lp == lpFIO)
					ProcessNames(pUnit, true, true, true);
				else if(lp == lpIO)
					ProcessNames(pUnit, true, false, true);
				else if(lp == lpFI)
					ProcessNames(pUnit, true, true, false);
				else if (lp == lpF)
					ProcessNames(pUnit, false, true, false);
			}
			else
				m_pUnitStorage->Release(pUnit, true);
			break;
		}

		case lpYM:
			ProcessDates(pUnit, true, true, false, false);
			pUnit->GetUnit()->SetPatternName(L"YM");
			break;

		case lpDM:			
		case lpDM1:
			ProcessDates(pUnit, false, true, true, false);
			if (lp == lpDM)
				pUnit->GetUnit()->SetPatternName(L"DM");
			else
				pUnit->GetUnit()->SetPatternName(L"DM1");
			break;

		case lpDMW:
		case lpDMW1:
		case lpDMW2:
			ProcessDates(pUnit, false, true, true, true);
			if (lp == lpDMW)
				pUnit->GetUnit()->SetPatternName(L"DMW");
			else if (lp == lpDMW1)
				pUnit->GetUnit()->SetPatternName(L"DMW1");
			else if (lp == lpDMW2)
				pUnit->GetUnit()->SetPatternName(L"DMW2");
			break;

		case lpDMY:
		case lpDMY1:
		case lpDMY2:
		case lpDMY3:
		case lpDMY4:
		case lpDMY5:
		case lpDMY6:
		case lpYMD:
		{
			bool isDate = true;
			if ((lp == lpDMY6) && (yyleng > 4))
			{
				wstring firstSep = m_pSent->GetUnit(m_nInserted - m_nProcessed + 1)->GetUnit()->GetWord();
				wstring lastSep = m_pSent->GetUnit(m_nInserted - m_nProcessed + 3)->GetUnit()->GetWord();
				isDate = (firstSep == lastSep);
			}
			if (isDate)
			{
				ProcessDates(pUnit, true, true, true, false);
	 			pUnit->SetWordType(wtDMY);
				if (lp == lpDMY)
					pUnit->GetUnit()->SetPatternName(L"DMY");
				else if (lp == lpDMY1)
					pUnit->GetUnit()->SetPatternName(L"DMY1");
				else if (lp == lpDMY2)
					pUnit->GetUnit()->SetPatternName(L"DMY2");
				else if (lp == lpDMY3)
					pUnit->GetUnit()->SetPatternName(L"DMY3");
				else if (lp == lpDMY4)
					pUnit->GetUnit()->SetPatternName(L"DMY4");
				else if (lp == lpDMY5)
					pUnit->GetUnit()->SetPatternName(L"DMY5");
				else if (lp == lpDMY6)
					pUnit->GetUnit()->SetPatternName(L"DMY6");
				else
					pUnit->GetUnit()->SetPatternName(L"YMD");
			}
			else
				m_pUnitStorage->Release(pUnit, true);
			break;
		}

		case lpDMYW:
		case lpDMYW1:
		case lpDMYW2:
		case lpDMYW3:
		case lpDMYW4:
			ProcessDates(pUnit, true, true, true, true);
			if (lp == lpDMYW)
				pUnit->GetUnit()->SetPatternName(L"DMYW");
			else if (lp == lpDMYW1)
				pUnit->GetUnit()->SetPatternName(L"DMYW1");
			else if (lp == lpDMYW2)
				pUnit->GetUnit()->SetPatternName(L"DMYW2");
			else if (lp == lpDMYW3)
				pUnit->GetUnit()->SetPatternName(L"DMYW3");
			else 
				pUnit->GetUnit()->SetPatternName(L"DMYW4");
			break;

		case lpYear:
		case lpYear1:
		case lpYear2:
			ProcessDates(pUnit, true, false, false, false);
			if (lp == lpYear)
				pUnit->GetUnit()->SetPatternName(L"Year");
			else if (lp == lpYear1)
				pUnit->GetUnit()->SetPatternName(L"Year1");
			else if (lp == lpYear2)
				pUnit->GetUnit()->SetPatternName(L"Year2");
			break;

		case lpNumericYear:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddNumericPartOfSpeechIndex(*pUnit, Values::NumericType.ntOrdering);			
			m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDateYear);
			pUnit->GetUnit()->SetPatternName(L"NumericYear");
			break;

		case lpCentury:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			CheckAndSetGenitive(*pUnit);

			m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitCentury);
			m_pUnitChecker->ClearChildrenSemantics(*pUnit, Values::TSemanticType.smtTimeInterval);
			pUnit->GetUnit()->SetPatternName(L"Century");
			break;

		case lpDI:
			ProcessDates(pUnit, true, true, true, false);
			pUnit->GetUnit()->SetPatternName(L"DI");
	 		//pUnit->SetWordType(wtDMY);			
			break;

		case lpDYI:
			ProcessDates(pUnit, true, true, true, false);
			pUnit->GetUnit()->SetPatternName(L"DYI");
	 		//pUnit->SetWordType(wtDMY);			
			break;

		case lpQuotationSimple:
        case lpQuotation:
		{
			wstring firstQuote = m_pSent->GetUnit(yyleng)->GetUnit()->GetWord();
			wstring lastQuote = m_pSent->GetUnit(1)->GetUnit()->GetWord();
			if ((firstQuote != lastQuote) && 
				!(((firstQuote == L"Ф") && (lastQuote == L"Ф")) || ((firstQuote == L"Ђ") && (lastQuote == L"ї")) ||
				((firstQuote[0] == (wchar_t)0x201C) && (lastQuote[0] == (wchar_t)0x201D))))
			{
				m_pUnitStorage->Release(pUnit, true);
				break;
			}
		}
			if (lp == lpQuotationSimple)
			{
				IR::CUnit* pMainUnit = m_pSent->GetUnit(m_nInserted - m_nProcessed + 1);
				m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, *pMainUnit);
				m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed); 
			}
			else
			{
				pUnit->SetWordType(wtQuotation);
				if(yyleng == 3)
				{
					IR::CUnit* pMainUnit = m_pSent->GetUnit(m_nInserted - m_nProcessed + 1);
					if(m_bQuery)
					{
						m_pUnitChecker->AddServiceTypeIndex(*pMainUnit, Values::TSServiceTypes.sstAcronymCitation);
						m_pUnitStorage->Release(pUnit, true);
						m_pSent->RemoveUnits(m_nInserted - m_nProcessed + 2, 1);
						m_pSent->RemoveUnits(m_nInserted - m_nProcessed, 1);
					}
					else
					{
						m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
							Values::CaseType.ctCommon, Values::NumberType.ntSingle, m_pSent->GetLanguage());
						m_pUnitChecker->AddServiceTypeIndex(*pUnit, Values::TSServiceTypes.sstAcronymCitation);
						m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed); 
					}
				}
				else
				{
					m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
						Values::CaseType.ctCommon, Values::NumberType.ntSingle, m_pSent->GetLanguage());	
					m_pUnitChecker->AddServiceTypeIndex(*pUnit, Values::TSServiceTypes.sstAcronymCitation);
					m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
					if(m_bQuery)
						m_pUnitChecker->RemoveExtremeChildren(*pUnit);
				}
			}
			break;

		case lpOrg:
			ProcessOrganizations(pUnit);
			break;

		case lpCityCountry:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			CheckAndSetGenitive(*pUnit);

			m_pUnitChecker->AddPlaceIndex(*pUnit, Values::TSPlaceType.pltCity);
			m_pUnitChecker->AddPlaceIndex(*pUnit, Values::TSPlaceType.pltCountry);
			break;

		case lpStateCountry:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			CheckAndSetGenitive(*pUnit);

			m_pUnitChecker->AddPlaceIndex(*pUnit, Values::TSPlaceType.pltState);
			m_pUnitChecker->AddPlaceIndex(*pUnit, Values::TSPlaceType.pltCountry);
			break;

		case lpCityState:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			CheckAndSetGenitive(*pUnit);

			m_pUnitChecker->AddPlaceIndex(*pUnit, Values::TSPlaceType.pltCity);
			m_pUnitChecker->AddPlaceIndex(*pUnit, Values::TSPlaceType.pltState);
			break;

		case lpGeoSemantic:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddPlaceIndex(*pUnit, Values::TSPlaceType.Undefined);
			m_pUnitChecker->CopyFeaturesFromChildren(*pUnit, Values::PartOfSpeechTypes.postNoun);	
			break;
			
		case lpComplexNumber:
			pUnit->SetWordType(wtNumber);
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pIndexLoader->FillMorphoInfoToComplexNumber(*pUnit);
			break;

		case lpFloat:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNumeric, 
				Values::CaseType.ctCommon, Values::NumberType.ntSingle);

			pUnit->SetWordType(wtNumber);
			break;

		case lpInetLocation:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
				Values::CaseType.ctCommon, Values::NumberType.ntSingle);

			m_pUnitChecker->AddPlaceIndex(*pUnit, Values::TSPlaceType.Undefined);
			break;

		case lpTime1:
		case lpTime:
			{
				bool isTime = true;
				for(int i = 0; i < yyleng; ++i)
				{
					IUnit *pSentUnit = m_pSent->GetUnit(m_nInserted - m_nProcessed + (yyleng - 1 - i))->GetUnit();
					
					if(i%2 == 0)
					{
						int number = _wtoi(pSentUnit->GetWord());
						//более 23 часов; более 59 минут/секунд
						if((i == 0 && number > 23) || (number > 59))
						{
							isTime = false;
							break;
						}
					}
				}

				if(isTime)
				{
					m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
					m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNumeric, 
											Values::CaseType.ctCommon, Values::NumberType.ntSingle);
					m_pUnitChecker->ClearChildrenSemantics(*pUnit, Values::TSemanticType.Undefined);
					m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDayTime);
				}
				else 
				{
					m_pUnitStorage->Release(pUnit, true);
				}
			
				break;
			}
			
		case lpMeasureDay:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
				Values::CaseType.ctCommon, Values::NumberType.ntSingle);

			m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDateDay);
			m_pUnitChecker->AddMeasureIndex(*pUnit, Values::TSMeasureType.msrtTime);
			break;

		case lpMeasureMonth:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
				Values::CaseType.ctCommon, Values::NumberType.ntSingle);

			m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDateMonth);
			m_pUnitChecker->AddMeasureIndex(*pUnit, Values::TSMeasureType.msrtTime);
			break;

		case lpNounSequence:			
			pGenNoun = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmInSequences,  wtDativeAndGenNoun);

			if(pGenNoun)
			{
				m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, *pGenNoun);
				m_pSent->JoinUnits(pUnit, yyleng-1, m_nInserted - m_nProcessed);
			}			

			break;

		case lpLSequence:
        {
            bool personOnlyInitial = true;
			bool isRussian = m_pSent->GetLanguage() == ealRussian;
			bool isCanJoin = true;
			for(int i = 0; i < yyleng; ++i)
			{
				IR::CUnit* pUnitSeq = m_pSent->GetUnit(m_nInserted - m_nProcessed + i);
				IUnit *pSeqUnit = pUnitSeq->GetUnit();
				if (pSeqUnit->HasChilds())
					isOnlyPerson = false;

				if (m_pUnitChecker->HasPartOfSpeechType(pSeqUnit, SS::Core::Features::Values::PartOfSpeechTypes.postOfficial) ||
					m_pUnitChecker->HasPartOfSpeechType(pSeqUnit, SS::Core::Features::Values::PartOfSpeechTypes.postPronoun))	
					isPronounOrOfficial = true;
				if (m_pUnitChecker->HasSemantic1(pSeqUnit, SS::Core::Features::Values::TSemanticType.smtPerson, SS::Core::Features::Values::TSPersonType.semprstFirstName) ||
					m_pUnitChecker->HasSemantic1(pSeqUnit, SS::Core::Features::Values::TSemanticType.smtPerson, SS::Core::Features::Values::TSPersonType.semprstLastName) ||
					m_pUnitChecker->HasSemantic1(pSeqUnit, SS::Core::Features::Values::TSemanticType.smtPerson, SS::Core::Features::Values::TSPersonType.semprstMiddleName))
                {
                    personOnlyInitial = false;
                    isPersonSemantic = true;
                }
                else if (pSeqUnit->IsInitial())
					isPersonSemantic = true;
				if (m_pUnitChecker->HasSemantic1(pSeqUnit, SS::Core::Features::Values::TSemanticType.smtOrganization) &&
					!pSeqUnit->IsInitial())
				{
					isOrgSemantic = true;
				}
				if (m_pUnitChecker->HasSemantic1(pSeqUnit, SS::Core::Features::Values::TSemanticType.smtPlace) &&
					!pSeqUnit->IsInitial())
				{
					isPlaceSemantic = true;
				}
				if (m_pUnitChecker->isXZ(pSeqUnit))
					isXZ = true;
				else if (isRussian && m_pUnitChecker->HasPartOfSpeechType(pSeqUnit, SS::Core::Features::Values::PartOfSpeechTypes.postVerb))
					isCanJoin = false;
			}
			
            if(isPersonSemantic && personOnlyInitial)
            {
                isPersonSemantic = false;
                isOrgSemantic = true;
            }

			bool flag1 = m_pSent->HaveCoincidentIndexes(yyleng, m_nInserted - m_nProcessed, eitSemanticIndex);
			bool flag2 = m_pSent->GetUnitCount(yyleng, m_nInserted - m_nProcessed, eitSemanticIndex) <= 1;
			bool flag3 = !m_pUnitChecker->UnitHasCase(*m_pSent->GetUnit(m_nInserted - m_nProcessed + yyleng - 1),
				Values::CaseType.ctGenitive);
			if (isCanJoin && ((isOnlyPerson && isRussian) || (!isRussian && (flag1 || (flag2 && flag3)))))
			{
				m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
				if(isRussian)
				{
					m_pUnitChecker->CopyFeaturesFromChildren(*pUnit);	
					if (isXZ)
					{
						m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, pUnit->GetUnit()->GetRightChildUnit());
						m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
							Values::CaseType.ctNominative, Values::NumberType.ntSingle);
						m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
							Values::CaseType.ctGenitive, Values::NumberType.ntSingle);
						m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
							Values::CaseType.ctAccusative, Values::NumberType.ntSingle);
						m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
							Values::CaseType.ctDative, Values::NumberType.ntSingle);
						m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
							Values::CaseType.ctInstrumental, Values::NumberType.ntSingle);
						m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
							Values::CaseType.ctLocative, Values::NumberType.ntSingle);
					}
				}
				else					
					CheckAndSetGenitive(*pUnit);					
				
				/*if (isPlaceSemantic)
					m_pUnitChecker->AddPlaceIndex(pUnit->GetUnit(), SS::Core::Features::Values::TSPlaceType.Undefined);   					   
				if (isPersonSemantic)
					m_pUnitChecker->AddPersonIndex(pUnit->GetUnit(), SS::Core::Features::Values::TSPersonType.semprstLastName);					   					      
				if (isOrgSemantic)
					m_pUnitChecker->AddOrganizationIndex(pUnit->GetUnit(), SS::Core::Features::Values::TSOrganizationType.Undefined);
				
				if (isXZ && !isPersonSemantic && !isOrgSemantic)
                    m_pUnitChecker->AddPersonIndex(pUnit->GetUnit(), SS::Core::Features::Values::TSPersonType.semprstLastName);
				if (!isXZ && !isPlaceSemantic && !isPersonSemantic && !isPronounOrOfficial)
                    m_pUnitChecker->AddOrganizationIndex(pUnit->GetUnit(), SS::Core::Features::Values::TSOrganizationType.Undefined);*/
			}
			else
				m_pUnitStorage->Release(pUnit, true);
			break;
        }
			
		case lpGenitiveEng:
			if(m_bQuery)
			{
				m_pUnitStorage->Release(pUnit, true);
				pUnit = m_pSent->GetUnit(m_nInserted - m_nProcessed + yyleng - 1);
				if( m_pUnitChecker->HasPartOfSpeechType(*pUnit, Values::PartOfSpeechTypes.postNoun) ||
					m_pUnitChecker->HasPartOfSpeechType(*pUnit, Values::PartOfSpeechTypes.Undefined))
				{
					// process nouns
					m_pSent->RemoveUnits(m_nInserted - m_nProcessed, yyleng - 1);
					m_pUnitChecker->ChangeFeature(*pUnit, cfSetGenitive);
				}
				else if(m_pUnitChecker->HasPartOfSpeechType(*pUnit, Values::PartOfSpeechTypes.postPronoun) ||
						m_pUnitChecker->HasPartOfSpeechType(*pUnit, Values::PartOfSpeechTypes.postAdverb))
				{
					// process combinations like "what's", "who's"
					m_pSent->RemoveUnits(m_nInserted - m_nProcessed, yyleng - 2);
					pUnit = m_pSent->GetUnit(m_nInserted - m_nProcessed);
					pUnit->GetUnit()->SetWord(L"is");
					pUnit->SetWordType(wtWord);
					pUnit->RemoveIndexes(eitMorphoIndex);
					pUnit->RemoveIndexes(eitSemanticIndex);
					m_pIndexLoader->FillMorphoInfo(*pUnit, L"is");
					m_pIndexLoader->FillSemanticInfo(*pUnit);
				}
			}
			else
			{
				pUnit->SetWordType(m_pSent->GetUnit(m_nInserted - m_nProcessed + yyleng - 1)->
					GetType(wmLSequences), wmLSequences);

				m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);

				m_pUnitChecker->AddPartOfSpeechIndex(pUnit->GetUnit(), Values::PartOfSpeechTypes.postNoun, 
					Values::CaseType.ctGenitive, Values::NumberType.ntSingle);			

				m_pIndexLoader->CopySemanticsFromChildren(*pUnit);

				// надо проверить - если от дочерних семантика не перенесена (ее просто нет),
				// то Noun's можно считать персоной 
				if (!m_pUnitChecker->HasSemantic(pUnit->GetUnit(), Values::TSemanticType.smtPerson) &&
					!m_pUnitChecker->HasSemantic(pUnit->GetUnit(), Values::TSemanticType.smtOrganization) &&
					!m_pUnitChecker->HasSemantic(pUnit->GetUnit(), Values::TSemanticType.smtPlace))
				{
					wstring word = pUnit->GetUnit()->GetLeftChildUnit()->GetWord();			
					m_pUnitChecker->AddPersonIndex(pUnit->GetUnit(), Values::TSPersonType.Undefined);					
				}

				m_pUnitChecker->FillSemanticTypes(pUnit);													
			}
			break;

		case lpOrdinalNumber:
			if(yyleng == 1)
			{
				//?
				//Ќадо выделить в метод и вызывать в Walker'e и в IRLoader'e ?
	//				if(len <= 4)
	//{
	//	int i = _ttoi(sz);
	//	// for dates detecting
	//	if(i >= 1 && i <= 31)
	//		pUnit->SetWordType(wtNumberDay, wmDates);
	//	else if(i > MIN_YEAR_NUMBER && i < MAX_YEAR_NUMBER)
	//		pUnit->SetWordType(wtNumberYear, wmDates);
	//	else
	//		pUnit->SetWordType(wtNumber, wmDates);
	//}
			}
			if(m_bQuery)
			{
				m_pUnitStorage->Release(pUnit, true);
				m_pSent->RemoveUnits(m_nInserted - m_nProcessed, yyleng - 1);
				m_pUnitChecker->ChangeFeature(*m_pSent->GetUnit(m_nInserted - m_nProcessed), cfSetOrdering);
			}
			else
			{
				//сохранение параметров дл€ дат
				pUnit->SetWordType(m_pSent->GetUnit(m_nInserted - m_nProcessed + yyleng - 1)->GetType(wmDates), 
					wmDates);
				m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
				IUnit* pUnit1 = pUnit->GetUnit()->GetRightChildUnit();
				wstring numEnd = L"";
				if (pUnit1!=NULL) numEnd = pUnit1->GetWord();
				m_pUnitChecker->AddNumericPartOfSpeechIndex(*pUnit, Values::NumericType.ntOrdering, numEnd);
			}
			break;

		case lpNegVerb:
		{
			IR::CUnit* pNegVerb = m_pSent->GetUnit(m_nInserted - m_nProcessed + yyleng - 1);
			LPCWSTR sz = GetNegativeVerb(pNegVerb->GetUnit()->GetWord());
			if(!sz)
			{
				m_pUnitStorage->Release(pUnit, true);
				break;
			}

			// fill top unit's morphology ("would")
			pUnit->GetUnit()->SetWord(sz);
			m_pIndexLoader->FillMorphoInfo(*pUnit, sz);
			m_pUnitChecker->ChangeFeature(*pUnit, cfSetNegative);

			// add positive morphology to first unit ("wouldn")
			pNegVerb->RemoveIndexes(eitSemanticIndex);
			m_pIndexLoader->FillMorphoInfo(*pNegVerb, sz);

			// add "not" morphology to negative particle ("t")
			IR::CUnit* pNegParticle = m_pSent->GetUnit(m_nInserted - m_nProcessed);
			pNegParticle->RemoveIndexes(eitSemanticIndex);
			m_pIndexLoader->FillMorphoInfo(*pNegParticle, _T("not"));

			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			break;
		}

		case lpSimpleSequence:
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
				Values::CaseType.ctCommon, Values::NumberType.ntSingle);
			m_pUnitChecker->AddOrganizationIndex(pUnit->GetUnit(), SS::Core::Features::Values::TSOrganizationType.Undefined);
			break;

		case lpSimpleFraction:
			pUnit->SetWordType(wtNumber);
			m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
			m_pUnitChecker->AddNumericPartOfSpeechIndex(*pUnit, Values::NumericType.ntCount);
			break;

		case lpList:
		{
			pUnit->GetUnit()->SetWord(L"what");
			pUnit->GetUnit()->SetPosition(0);
			m_pIndexLoader->FillMorphoInfo(*pUnit, L"what");
			m_pIndexLoader->FillSemanticInfo(*pUnit);

			IR::CUnit* pUnit2 = m_pUnitStorage->Allocate();
			pUnit2->SetUnit(m_pStorage->CreateUnit());
			pUnit2->SetWordType(wtWord);
			pUnit2->GetUnit()->SetWord(L"are");
			pUnit2->GetUnit()->SetPosition(0);
			m_pIndexLoader->FillMorphoInfo(*pUnit2, L"are");
			m_pIndexLoader->FillSemanticInfo(*pUnit2);

			IR::CUnit* rUnits[2] = { pUnit, pUnit2 };
			m_pSent->ReplaceUnits(rUnits, 2, yyleng, m_nInserted - m_nProcessed);
			break;
		}

		default:
			assert(false);
	}

	SS_CATCH(L"");
}

void CWalker::ProcessOrganizations(IR::CUnit* pUnit)
{
	SS_TRY;

	// find the main unit of organization sequence (unit with wtOrg type)
	IR::CUnit* unit = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmOrganizations, wtOrg);
	if (!unit) unit = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmOrganizations, wtLWord);
	if (!unit) unit = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmOrganizations, wtSpecificOrg);
	if (!unit) unit = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmOrganizations, wtOrgAbbr);
    if (!unit) return;

	IUnit* pMainUnit = unit->GetUnit();

	// join units
	m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);

	// set case according to main unit
	if(pMainUnit!=NULL && m_pUnitChecker->UnitHasCase(pMainUnit, Values::CaseType.ctGenitive))
	{
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
			Values::CaseType.ctGenitive, Values::NumberType.ntSingle);
	}
	else 
	{
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
			Values::CaseType.ctCommon, Values::NumberType.ntSingle);
	}

	// copy organization indexes from main unit, and clear semantics to children
	if(!m_pUnitChecker->CopyIndexes(pMainUnit, *pUnit, Values::TSemanticType.smtOrganization))
	  if (!m_pUnitChecker->CopyIndexes(pMainUnit, *pUnit, Values::TSemanticType.smtService))
		m_pUnitChecker->CopyIndexes(pMainUnit, *pUnit, Values::TSemanticType.smtPlace);
//	pUnit->RemoveChildrenIndexes(eitSemanticIndex);

	SS_CATCH(L"");
}

void CWalker::ProcessNames(IR::CUnit* pUnit, bool bFirst, bool bLast, bool bMiddle)
{
	SS_TRY;

	if(	m_bQuery && 
		!m_pSent->CheckRangeWordType(yyleng, m_nInserted - m_nProcessed, wmDefault, wtWord, false))
	{
		m_pUnitStorage->Release(pUnit, true);
		return; 
	}

	IR::CUnit* pLNameUnit = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmNames, wtLName);
	if (!pLNameUnit)
		pLNameUnit = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmNames, wtName);
	if (!pLNameUnit)
		pLNameUnit = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmNames, wtFName);
	if (!pLNameUnit)
		pLNameUnit = m_pSent->GetUnit(yyleng, m_nInserted - m_nProcessed, wmNames, wtMName);
	if(pLNameUnit)
	{
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, *pLNameUnit);
		m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
	}
	else
	{
		m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, pUnit->GetUnit()->GetRightChildUnit());
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
			Values::CaseType.ctNominative, Values::NumberType.ntSingle);
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
			Values::CaseType.ctGenitive, Values::NumberType.ntSingle);
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
			Values::CaseType.ctAccusative, Values::NumberType.ntSingle);
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
			Values::CaseType.ctDative, Values::NumberType.ntSingle);
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
			Values::CaseType.ctInstrumental, Values::NumberType.ntSingle);
		m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun,
			Values::CaseType.ctLocative, Values::NumberType.ntSingle);
	}

	if(bFirst)
		m_pUnitChecker->AddPersonIndex(*pUnit, Values::TSPersonType.semprstFirstName);
	if(bLast)
		m_pUnitChecker->AddPersonIndex(*pUnit, Values::TSPersonType.semprstLastName);
	if(bMiddle)
		m_pUnitChecker->AddPersonIndex(*pUnit, Values::TSPersonType.semprstMiddleName);

	SS_CATCH(L"");
}

void CWalker::ProcessDates(IR::CUnit* pUnit, bool bYear, bool bMonth, bool bNumber, bool bDay)
{
	SS_TRY;

	m_pSent->JoinUnits(pUnit, yyleng, m_nInserted - m_nProcessed);
	m_pUnitChecker->AddPartOfSpeechIndex(*pUnit, Values::PartOfSpeechTypes.postNoun, 
		Values::CaseType.ctCommon, Values::NumberType.ntSingle);
	m_pUnitChecker->ClearChildrenSemantics(*pUnit, Values::TSemanticType.smtTimeInterval);

	if(bYear)
		m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDateYear);
	if(bMonth)
		m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDateMonth);
	if(bNumber)
		m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDateNumber);
	if(bDay)
		m_pUnitChecker->AddDateIndex(*pUnit, Values::TSTimeIntervalType.semtitDateDay);

	SS_CATCH(L"");
}

void CWalker::CheckAndSetGenitive(IUnit* pUnit)
{
	SS_TRY;

	if(m_pUnitChecker->UnitHasCase(pUnit->GetRightChildUnit(), Values::CaseType.ctGenitive))
	{
		m_pUnitChecker->AddPartOfSpeechIndex(pUnit, Values::PartOfSpeechTypes.postNoun, 
			Values::CaseType.ctGenitive, Values::NumberType.ntSingle);
	}
	else
	{
		m_pUnitChecker->AddPartOfSpeechIndex(pUnit, Values::PartOfSpeechTypes.postNoun, 
			Values::CaseType.ctCommon, Values::NumberType.ntSingle);
	}

	SS_CATCH(L"");
}

LPCTSTR CWalker::GetNegativeVerb(LPCTSTR szLexem)
{
	SS_TRY;

	if(_tcsicmp(szLexem, _T("t")) == 0)
		return _T("not");
	else if(_tcsicmp(szLexem, _T("wouldn")) == 0)
		return _T("would");
	else if(_tcsicmp(szLexem, _T("shouldn")) == 0)
		return _T("should");
	else if(_tcsicmp(szLexem, _T("don")) == 0)
		return _T("do");
	else if(_tcsicmp(szLexem, _T("doesn")) == 0)
		return _T("does");
	else if(_tcsicmp(szLexem, _T("isn")) == 0)
		return _T("is");
	else if(_tcsicmp(szLexem, _T("won")) == 0)
		return _T("will");
	else if((_tcsicmp(szLexem, _T("ain")) == 0) || (_tcsicmp(szLexem, _T("aren")) == 0))
		return _T("are");
	else if(_tcsicmp(szLexem, _T("didn")) == 0)
		return _T("did");
	else if(_tcsicmp(szLexem, _T("haven")) == 0)
		return _T("have");
	//else if(_tcsicmp(szLexem, _T("can")) == 0)
	//	return _T("can");
	else if(_tcsicmp(szLexem, _T("couldn")) == 0)
		return _T("could");
	else if(_tcsicmp(szLexem, _T("weren")) == 0)
		return _T("were");
	else if(_tcsicmp(szLexem, _T("wasn")) == 0)
		return _T("was");
	else if(_tcsicmp(szLexem, _T("hadn")) == 0)
		return _T("had");

	SS_CATCH(L"");
	return NULL;
}

int CWalker::GetMode(EWalkerMode eMode)
{
	switch(eMode)
	{
		case wmQuotations:
			return QUOTATIONS;
		case wmDates:
			return DATES;
		case wmNames:
			return NAMES;
		case wmOrganizations:
			return ORGANIZATIONS;
		case wmPlaces:
			return PLACES;
		case wmReparsedItems:
			return REPARSEDITEMS;
		case wmLSequences:
			return LSEQUENCES;
		case wmQuery:
			return QUERY;
		case wmMultiNames:
			return MULTINAMES;
		case wmInSequences:
			return INSEQUENCES; 
		case wmTelephones:
			return TELEPHONES;
		default:
			assert(false);
			return INITIAL;
	}
}

EWalkerMode CWalker::GetMode(int iMode)
{
	switch(iMode)
	{
		case QUOTATIONS:
			return wmQuotations;
		case DATES:
			return wmDates;
		case NAMES:
			return wmNames;
		case ORGANIZATIONS:
			return wmOrganizations;
		case PLACES:
			return wmPlaces;
		case REPARSEDITEMS:
			return wmReparsedItems;
		case LSEQUENCES:
			return wmLSequences;
		case QUERY:
			return wmQuery;
		case MULTINAMES:
			return wmMultiNames;
		case INSEQUENCES:
			return wmInSequences;
		case TELEPHONES:
			return wmTelephones;
		default:
			assert(false);
			return wmDefault;
	}
}

}
}
}
