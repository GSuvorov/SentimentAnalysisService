#include "StdAfx.h"
#include "Lexica.h"

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;
#include <fstream>

inline bool IsRQuote(LPCTSTR szLexem)
{
	wchar_t ch = szLexem[ 0 ];
	return (ch == (wchar_t)0x201D || ch == L'”' || ch == L'»');
}
inline bool IsQuote(LPCTSTR szLexem)
{
	wchar_t ch = szLexem[ 0 ];
	return (ch == L'\'' || ch == L'"');
}
inline bool IsLQuote(LPCTSTR szLexem)
{
	wchar_t ch = szLexem[ 0 ];
	return (ch == (wchar_t)0x201C || ch == L'“' || ch == L'«');
}
inline bool IsAllQuote(LPCTSTR szLexem)
{
	return (IsLQuote(szLexem) || IsRQuote(szLexem) || IsQuote(szLexem));
}


namespace SS
{
namespace LexicalAnalyzer
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CLexica::CLexica() : 
		m_pStorage(NULL),
		m_bSearch(false),
		m_bQuery(false),
		m_bLite(false),
		m_bUseQueryDeformer(false)
{
	CoInitialize(NULL);
}

CLexica::~CLexica()
{
	CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CLexica::QueryInterface(REFIID refIID, void** pBase)
{
	if(refIID == IID_LexicaPrimary)
		*pBase = (ILexicaPrimary*)(ILexica*) this;
	else if(refIID == IID_Lexica)
		*pBase = (ILexica*) this;
	else if(refIID == IID_Base)
		*pBase = (IBase*) this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CLexica::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// ILexicaPrimary interface

void CLexica::SetLoadManager(ILoadManager* pLoadManager)
{
	SS_TRY;

	if(GetLoadManager())
		return;

	CLexicaPrimary::SetLoadManager(pLoadManager);
	m_pStorage = (IBlackBoxTextStorage*) 
		GetLoadManager()->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);
	m_oUnitChecker.Init(GetLoadManager(), m_pStorage);

	m_oIRLoader.Init(m_pStorage, &m_oIndexLoader, GetLoadManager(), &m_oUnitChecker, 
		static_cast<Reparser::IReparser*>(this), m_oSent.GetUnitStorage());
	m_oWalker.Init(&m_oUnitChecker, &m_oIndexLoader, m_pStorage, m_oSent.GetUnitStorage());

	m_oDocStructureLoader.Init(m_pStorage, &m_oIndexLoader);

	//check ss.dat params
	try
	{
		SS::Interface::Core::CommonServices::ISettingsServer* pSettingsServer =
			(SS::Interface::Core::CommonServices::ISettingsServer*)
			GetLoadManager()->GetInterface(L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);
	//	m_bUseQueryDeformer = pSettingsServer->GetBoolean(L"LinguisticProcessor::UseQueryDeformer") != 0;
		//m_oIndexLoader.SetUseQueryDeformer(m_bUseQueryDeformer);
		m_oIndexLoader.SetUseQueryDeformer(true);
	}
	catch(SS::Interface::Core::CommonServices::IException* pException)
	{
		SAVE_LOG(SS_ERROR AND pException->ToString().c_str() AND __WFUNCTION__);
	}
	catch(...)
	{
		SAVE_LOG(SS_ERROR AND L"Ошибка при обращении к файлу настроек" AND __WFUNCTION__ )
	}

	SS_CATCH(L"");
}

void CLexica::SetText(TNativeTextForIndexation* pNativeText, unsigned int uiStartPosition)
{
	SS_TRY;

	CLexicaPrimary::SetText( pNativeText, uiStartPosition );
	if (uiStartPosition == 0)
	{
		//m_oDocStructureLoader.ParseDocStructure( (BSTR) pNativeText->GetTextStructure() );

		const wchar_t* text = pNativeText->GetTextStructure();
		BSTR text_BSTR = SysAllocString( text );
		m_oDocStructureLoader.ParseDocStructure( text_BSTR );
		SysFreeString( text_BSTR );
	}

	if (!m_bSearch && !(GetAnalyseParams()->GetAnalyseMod() == eamIndexation && GetAnalyseParams()->GetCurrentSystemMode() == ecsmGlobal))
	{
		m_oIndexLoader.NextText();
	}

	m_oPara.Init(m_pStorage);
	m_oIRLoader.Reset();

	SS_CATCH(L"");
}

unsigned int CLexica::LoadNext(bool bIgnoreParagraphs)
{
	SS_TRY;

	if (!CLexicaPrimary::LoadNext(bIgnoreParagraphs))
	{
		m_pBoard->Reset();
		if (!m_oPara.IsEmpty())
		{
			return (m_iCharCount);
		}
		return (0);
	}

	return (m_iCharCount);

	SS_CATCH(L"");
	return (0);
}

SS::Interface::Core::ILexemeBoard* CLexica::GetLexemeBoard()
{
	return CLexicaPrimary::GetLexemeBoard();
}

/////////////////////////////////////////////////////////////////////////////////////
// ILexica interface

void CLexica::SetAnalyseParams(IAnalyseParams* pAnalyseParams)
{
	SS_TRY;

	CAnalyseParams::SetAnalyseParams(pAnalyseParams);
	m_oIndexLoader.Init(GetLoadManager(), m_pStorage, GetAnalyseParams(), m_oSent.GetUnitStorage(), &m_oUnitChecker);
	m_oWalker.SetAnalyseParams(pAnalyseParams);
	m_oIRLoader.SetAnalyseParams(pAnalyseParams);
	m_oUnitChecker.SetAnalyseParams(pAnalyseParams);

	m_bSearch = (pAnalyseParams->GetAnalyseMod() == eamSearch) &&
		        (pAnalyseParams->GetCurrentSystemMode() == ecsmLocal);

	m_bQuery = (pAnalyseParams->GetAnalyseMod() == eamQuery) || 
		       (pAnalyseParams->GetAnalyseMod() == eamRejectUnimportant);

	m_bLite = pAnalyseParams->m_bIsLiteMode;

	SS_CATCH(L""); 
}

void CLexica::AnalyzeDocStructure(IText* pText)
{
	SS_TRY;

	// analyze document structure
	m_oDocStructureLoader.AnalyzeDocStructure(m_pBoard, pText);

	SS_CATCH(L"");
}

void CLexica::EngDeleteSemanticPersonWithoutProper(IR::CSentence *pSent)
{
	if (pSent->GetLanguage()!=ealEnglish) return;
	bool isPrevProper = false;
	IR::CUnit* pUnit = pSent->GetFirstUnit();
    while (pUnit != NULL)
	{
		IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex();
		TDictionaryIndex tind = pIndex->GetDictionaryIndex()->GetFirst();
		if (!tind.GetFirstLetterUpperCase() &&
			m_oUnitChecker.HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson)
		   )
		{
			bool isNextProper = false;			
			IR::CUnit* pNextUnit = pSent->GetNextUnitWithoutMove();
			if (pNextUnit!=NULL)
			{
				IIndex* pIndex1 = pUnit->GetUnit()->GetFirstIndex();
				TDictionaryIndex tind1 = pIndex1->GetDictionaryIndex()->GetFirst();
				if (tind1.GetFirstLetterUpperCase())
				{
					isNextProper = true;
				}
			}
			if (!isNextProper && !isPrevProper)
			{
				isNextProper = false; isPrevProper = false;
				m_oUnitChecker.ClearSemantics(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson);
			}
		}

		isPrevProper = tind.GetFirstLetterUpperCase();
		pUnit = pSent->GetNextUnit();
	}

}

void CLexica::EngEvristicJoinByAndProper(IR::CSentence *pSent)
{
	//if (pSent->GetLanguage()!=ealEnglish) return;
	//IR::CUnit* pUnit = pSent->GetFirstUnit();
	//bool isPrevProper = false;
	//while(pUnit!=NULL)
	//{
	//	wstring str = pUnit->GetUnit()->GetWord();
	//	if ((str==L"and" || str==L"AND") && isPrevProper)
	//	{
	//		IR::CUnit* pNextUnit = pSent->GetNextUnitWithoutMove();
	//		if (pNextUnit!=NULL)
	//		{
	//			IIndex* pIndex1 = pUnit->GetUnit()->GetFirstIndex();
	//			TDictionaryIndex tind1 = pIndex1->GetDictionaryIndex()->GetFirst();
	//			if (tind1.GetFirstLetterUpperCase())
	//			{
	//				//isNextProper = true;
	//				// нашли, надо сливать
	//				pUnitNew = m_oSent.GetUnitStorage()->Allocate();
	//				pUnitNew->SetUnit(m_pStorage->CreateUnit());
	//				pUnitNew->SetWordType(wtUndefined);
	//				m_oUnitChecker.AddPartOfSpeechIndex(*pUnitNew, Values::PartOfSpeechTypes.postNoun, 
	//					Values::CaseType.ctCommon, Values::NumberType.ntSingle);
	//				m_oUnitChecker.AddServiceTypeIndex(*pUnitNew, Values::TSServiceTypes.sstMovie);
	//				pSent->JoinUnits(pUnitNew, 3, 1);				
	//			}
	//		}			
	//	}
	//	pUnit = pSent->GetNextUnit();
	//}	
}

//объединение слов управляющий делами и исполняющий обязанности
void CLexica::SpecialPersonEvristicJoin(IR::CSentence *pSent)
{
	IR::CUnit* pUnit = pSent->GetFirstUnit();
	if (pUnit == NULL)
	{
		return;
	}
	IR::CUnit* pUprUnit = NULL;
	IR::CUnit* pIspUnit = NULL;
	GenderType gender;
	gender.Undefine();

	wstring word;
	while (pUnit != NULL)
	{
		word.assign( pUnit->GetUnit()->GetWord() );
		
		_wcslwr_s((wchar_t*)word.c_str(), word.length() + 1);
		if (word==L"управляющий"  || word==L"управляющего" || word==L"управляющему" ||
			word==L"управляющим"  || word==L"управляющем"  || word==L"исполняющий"  ||
			word==L"исполняющего" || word==L"исполняющему" ||
			word==L"исполняющим"  || word==L"исполняющем"  || word==L"и.")
		{
			pUprUnit = pUnit;
			gender = Values::GenderType.gtMasculine;
		}
		else if (word==L"управляющая" || word==L"управляющую" || word==L"управляющей" ||
			     word==L"исполняющая" || word==L"исполняющую" || word==L"исполняющей")
		{
			pUprUnit = pUnit;
			gender = Values::GenderType.gtFemale;
		}
		else if (pUprUnit != NULL && (word==L"делами" || word==L"обязанности" || word==L"о."))
		{
			IR::CUnit* pUnitNew = m_oSent.GetUnitStorage()->Allocate();
			pUnitNew->SetUnit(m_pStorage->CreateUnit());
			pUnitNew->SetWordType(wtUndefined);
			m_oUnitChecker.AddPartOfSpeechIndexWithAnim(*pUnitNew, Values::PartOfSpeechTypes.postNoun, 
				Values::CaseType.ctCommon, Values::NumberType.ntSingle,gender,SS::Core::Types::ealRussian);
			m_oUnitChecker.AddPersonIndex(*pUnitNew, Values::TSPersonType.semprstProfession);
			pSent->JoinUnits(pUnitNew, 2, 0);
			pUprUnit = NULL;
		}
		else
		{
			pUprUnit = NULL;
		}
		pUnit = pSent->GetNextUnit();
	}
}

//// надо найти слова с большой буквы идут за предлогом географии и согласуются по падежу и удалить у них персону есть и есть
//// и назначить гео
void CLexica::EvristicFindPLaces(IR::CSentence *pSent)
{
	// с предлогом В сочетается иименпительный/винительны падеж географии
	// с предлогом ПО - дательный
	// с предлогом ЧЕРЕЗ - винительный
	// МЕЖДУ - творительный
	// при этом должен рядом находится Locative глагол. Чтобы не было ошибок например между Ивановым и Петровым произошла ссора - здесь не соотв. глагола
	// это

	// глагол движения их словаря (ехать, бежать, идти, двигаться, проезжать, мчаться, посетить и т.д.) + по/в/из + слово с большой буквы 
	if (pSent->GetLanguage() == ealEnglish)
	{
		return;
	}
	IR::CUnit* pUnit = pSent->GetFirstUnit();
	wstring lastPretext = L"";
	while (pUnit)
	{
		bool isPretext = false;
		IUnit *p_Unit = pUnit->GetUnit();
		wstring str = p_Unit->GetWord();
		_wcslwr_s((wchar_t*)str.c_str(), str.length() + 1);
		IIndex* pIndex = p_Unit->GetFirstIndex();
		if (str == L"в")
		{
			lastPretext = L"в";
			isPretext = true;
		}
		else if (str == L"по")
		{
			lastPretext = L"по";
			isPretext = true;
		}
		else if (str == L"через")
		{
			lastPretext = L"через";
			isPretext = true;
		}
		else if (str == L"под")
		{
			lastPretext = L"под";
			isPretext = true;
		}
		else if (pIndex && pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase() &&
		     	 !pUnit->GetUnit()->IsEvristicAbbr() &&
			     !m_oUnitChecker.HasSemantic1(p_Unit,SS::Core::Features::Values::TSemanticType.smtService) &&
			     !m_oUnitChecker.HasSemantic1(p_Unit,SS::Core::Features::Values::TSemanticType.smtOrganization) &&
			     (m_oUnitChecker.HasSemantic1(p_Unit, SS::Core::Features::Values::TSemanticType.smtPlace) ||
			     m_oUnitChecker.isXZ(p_Unit))
			    )
		{
			if (!lastPretext.empty())
			{
				bool isSetSemantic = false;
				if (lastPretext == L"в" &&
					(m_oUnitChecker.HasCase(pIndex,Values::CaseType.ctNominative) ||
					m_oUnitChecker.HasCase(pIndex,Values::CaseType.ctLocative)) &&
					!m_oUnitChecker.HasCase(pIndex,Values::CaseType.ctGenitive)
				   )
				{
					isSetSemantic = true;
				}
				else if (lastPretext == L"по" && m_oUnitChecker.HasCase(pIndex,Values::CaseType.ctDative))
				{
					isSetSemantic = true;
				}
				else if (lastPretext == L"через" && m_oUnitChecker.HasCase(pIndex,Values::CaseType.ctAccusative))
				{
					isSetSemantic = true;
				}
				else if (lastPretext == L"под" && m_oUnitChecker.HasCase(pIndex,Values::CaseType.ctAccusative))
				{
					isSetSemantic = true;
				}

				if (isSetSemantic)
				{
					m_oUnitChecker.ClearSemantics(p_Unit, SS::Core::Features::Values::TSemanticType.smtPlace);
					m_oUnitChecker.AddPlaceIndex(p_Unit, SS::Core::Features::Values::TSPlaceType.Undefined);
				}
			}
		}
		if (!isPretext)
		{
			lastPretext.clear();
		}
		pUnit = pSent->GetNextUnit();
	}

}

inline bool isPoint(wstring str)
{
	return (str[ str.size() - 1 ] == L'.');
}

void CLexica::CreateUnionUnit(IR::CSentence *pSent, EAnalyseLanguage eal, 
                              bool &isEngFind, int &nProcessed, int &nInserted)
{
    int n = nInserted-nProcessed;
	if (!pSent->GetNextUnitWithoutMove() && eal==ealEnglish) 
	{
		//n--;
	}
    IR::CUnit* pUnitNew = m_oSent.GetUnitStorage()->Allocate();
	pUnitNew->SetUnit(m_pStorage->CreateUnit());
	pUnitNew->SetWordType(wtUndefined);
	m_oUnitChecker.AddPartOfSpeechIndex(*pUnitNew, Values::PartOfSpeechTypes.postNoun, 
		Values::CaseType.ctCommon, Values::NumberType.ntSingle);
	m_oUnitChecker.AddServiceTypeIndex(*pUnitNew, Values::TSServiceTypes.sstAcronymCitation);
	pSent->JoinUnits(pUnitNew, nProcessed, n);
	//pUnitNew = NULL;
	nProcessed = 0;
	nInserted = 1;
	isEngFind = false;
	pUnitNew->SetWordType(wtProperName, wmQuotations);
}

void CLexica::EngInRusEvristicJoin(IR::CSentence *pSent)
{
	if (pSent->GetLanguage() != ealRussian)
	{
        return;
	}

	IR::CUnit* pUnit = pSent->GetFirstUnit();
	IR::CUnit* pFirstUnit = pUnit;
	bool isRusFind = false;
	bool isEngFind = false;
	int nProcessed = 0;
	int nInserted = 1;
	int ruCount = 0;

	//m_pUnitStorage->Allocate();
	//pUnit->SetUnit(m_pStorage->CreateUnit());
	//pUnit->SetWordType(wtWord);
    bool containUndef = false;
	bool isPreviousHTTP = false;
	while (pUnit)
	{
		IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex();
		wstring strword = pUnit->GetUnit()->GetWord();
		if (!pIndex)
        {
            pUnit = pSent->GetNextUnit();
            nProcessed = 0;
			nInserted = 1;
			isEngFind = false;
            continue;
        }
		EAnalyseLanguage eal = (pUnit->GetType(wmDates) != wtRomanNumber) ? pIndex->GetDictionaryIndex()->GetFirst().GetLanguage() : ealUndefined;
		
		if (eal != ealUndefined)
		{
			bool isContainLetter = false;
			for (unsigned int i = 0; !isContainLetter && (i < strword.size()); ++i)
			{
				if (iswalpha(strword[i]))
				{
					isContainLetter = true;
				}
			}
			if (!isContainLetter)
			{
				eal = ealUndefined;
			}
		}

		if (eal==ealUndefined && pSent->GetNextUnitWithoutMove() && strword != L"(" && strword != L")" &&
			((strword == L"'") || !IsAllQuote(strword.c_str())) && strword != L",")
		{		
			if (isEngFind && !containUndef && ((strword == L"&") || (strword == L"'")))
            {
                ++nInserted;
                pUnit = pSent->GetNextUnit();

                ++nProcessed;
                containUndef = true;
                continue;
			}
		}
		if (eal==ealRussian || eal==ealUndefined || pUnit->GetUnit()->IsRusInitial() || 
            !pSent->GetNextUnitWithoutMove() || IsAllQuote(strword.c_str()))
		{
			if (eal == ealRussian)
			{
				++ruCount;
			}

			if (!pSent->GetNextUnitWithoutMove() && eal==ealEnglish && strword.size()>0 && 
                !pUnit->GetUnit()->IsRusInitial() && !isPoint(strword) && !IsAllQuote(strword.c_str()))
			{
                isEngFind = true;
                //if (pFirstUnit != pUnit) nInserted++;
			    nProcessed++;
            }

			if (isEngFind)
            {
                if ((nProcessed == 2) && containUndef)
				{
                    --nProcessed;
				}

                int n = nInserted - nProcessed;
	            if (!pSent->GetNextUnitWithoutMove() && eal==ealEnglish) 
	            {
		            //n--;
	            }
				if (!isPreviousHTTP)
				{
					IR::CUnit* pUnitNew = m_oSent.GetUnitStorage()->Allocate();
					pUnitNew->SetUnit(m_pStorage->CreateUnit());
					pSent->JoinUnits(pUnitNew, nProcessed, n);
					pUnitNew->SetWordType(wtUndefined);
					pUnitNew->SetWordType(wtProperName, wmQuotations);
					m_oUnitChecker.AddPartOfSpeechIndex(*pUnitNew, Values::PartOfSpeechTypes.postNoun, 
						Values::CaseType.ctCommon, Values::NumberType.ntSingle);
					m_oUnitChecker.AddServiceTypeIndex(*pUnitNew, Values::TSServiceTypes.sstAcronymCitation);
				}
	            //pUnitNew = NULL;
	            nProcessed = 0;
	            nInserted = 1;
	            isEngFind = false;
                containUndef = false;
            }
			//isRusFind = true;
			//continue;
		}
		else if ((eal == ealEnglish || strword == L"&") && !strword.empty() && !IsAllQuote(strword.c_str()) && !isPoint(strword))
		{
			isEngFind = true;
			++nInserted;
			++nProcessed;
		}
		isPreviousHTTP = false;
		if (eal == ealEnglish)
		{
			_wcslwr_s((wchar_t*)strword.c_str(), strword.length() + 1);
			isPreviousHTTP = (strword == L"http");
		}
   	    pUnit = pSent->GetNextUnit();
	}
}

bool isRegionSpecialPre(wstring str1)
{
	wstring str = str1;
	wstring strLo = str1;
	_wcslwr_s((wchar_t*)strLo.c_str(), strLo.length() + 1);
	if (strLo==L"город" ||strLo==L"города" || strLo==L"городу" ||
		strLo==L"городом" || strLo==L"городе" || strLo==L"городов" || strLo==L"городам" ||strLo==L"городами" ||
		strLo==L"городах" || strLo==L"гор." || str==L"г.")
    {
        return true;
    }

	if (strLo==L"село" ||strLo==L"селом" || strLo==L"селу" ||
		strLo==L"селе" || strLo==L"села" || strLo==L"селами" || strLo==L"селам" || strLo==L"селах" || 
        str==L"с." || strLo==L"сел." || str==L"c.")
    {
        return true;	
    }

	if (strLo==L"деревня" ||strLo==L"деревней" || strLo==L"деревню" || strLo==L"деревне" || 
        strLo==L"деревни" || strLo==L"деревнями" || strLo==L"деревням" || strLo==L"деревнях"
		|| strLo==L"дер." || str==L"д.")
    {
        return true;
    }

	if (strLo==L"поселок" ||strLo==L"поселка" || strLo==L"поселку" || strLo==L"поселке" ||
		strLo==L"поселком" || strLo==L"поселки" || strLo==L"поселками" || strLo==L"поселкам" || 
        strLo==L"поселках" || strLo==L"пос." || str==L"п.")
    {
        return true;
    }

	if (strLo==L"хутор" ||strLo==L"хутора" || strLo==L"хутору" || strLo==L"хуторе" ||
		strLo==L"хутором" || strLo==L"хуторами" || strLo==L"хуторы" || strLo==L"хуторам" ||
		strLo==L"хут." || str==L"х.")
    {
        return true;
    }

    if (strLo==L"море" ||strLo==L"моря" || strLo==L"морем" || strLo==L"морю" || strLo==L"морями")
        return true;

	if (strLo==L"озеро" ||strLo==L"озера" || strLo==L"озером" || strLo==L"озеру" || strLo==L"озерами")
        return true;

	if (strLo==L"государство" ||strLo==L"государства" || strLo==L"государству" ||
		strLo==L"государством" || strLo==L"государстве" || strLo==L"государствами")
    {
        return true;
    }

	if (strLo==L"королевство" ||strLo==L"королевства" || strLo==L"королевству" ||
		strLo==L"королевством" || strLo==L"королевстве")
    {
        return true;
    }

	if (strLo==L"княжество" ||strLo==L"княжеством" || strLo==L"княжеству" || 
        strLo==L"княжества" || strLo==L"княжестве")
    {
        return true;
    }

	if (strLo==L"республика" ||strLo==L"республике" || strLo==L"республикой" ||strLo==L"республики"  ||
		strLo==L"республику")
    {
        return true;
    }

	if (strLo==L"штат" ||strLo==L"штата" || strLo==L"штату" ||strLo==L"штатом"  || strLo==L"штате")
        return true;

	if (strLo==L"провинция" ||strLo==L"провинции" || strLo==L"провинцией" ||strLo==L"провинцию"  ||
	    strLo==L"провинциями" ||  strLo==L"провинциях")
    {
        return true;
    }

	if (strLo==L"парк" ||strLo==L"парку" || strLo==L"парке" || strLo==L"парка" ||
	    strLo==L"парком" ||  strLo==L"парки")
    {
        return true;
    }

	return false;
}

bool isRegionSpecialPost(wstring str)
{
	wstring strLo = str;
	_wcslwr_s((wchar_t*)strLo.c_str(), strLo.length() + 1);
	if (strLo==L"область" ||strLo==L"области" || strLo==L"областью" ||
		strLo==L"областей" || strLo==L"областями") return true;

	if (strLo==L"федерация" ||strLo==L"федерации" || strLo==L"федерацию" ||
		strLo==L"федерацией") return true;

	if (strLo==L"автономный" ||strLo==L"автономная" || strLo==L"автономного" ||
		strLo==L"автономному" || strLo==L"автономной" || strLo==L"автономную" ||
		strLo==L"автономные" || strLo==L"автономных" || strLo==L"автономными") return true;	

	if (strLo==L"ледовитый" ||strLo==L"ледовитого" || strLo==L"ледовитому" ||
		strLo==L"ледовитым") return true;	

     if (strLo==L"автономия" ||strLo==L"автономию" || strLo==L"автономии" ||
		strLo==L"автономией" || strLo==L"автономиями") return true;

	 if (strLo==L"район" ||strLo==L"района" || strLo==L"району" ||
		strLo==L"районом" || strLo==L"районе" || strLo==L"районами") return true;

	 if (strLo==L"кладбище" ||strLo==L"кладбища" || strLo==L"кладбищу" ||
		strLo==L"кладбищем") return true;

	 if (strLo==L"царство" ||strLo==L"цартсвом" || strLo==L"царству" ||
		strLo==L"царстве") return true;


	  if (strLo==L"море" ||strLo==L"моря" || strLo==L"морем" ||
		strLo==L"морю" || strLo==L"морями") return true;

	  if (strLo==L"озеро" ||strLo==L"озера" || strLo==L"озером" ||
		strLo==L"озеру" || strLo==L"озерами") return true;

	  if (strLo==L"океан" ||strLo==L"океанами" || strLo==L"океаном" ||
		strLo==L"океаны" || strLo==L"океану" || strLo==L"океане") return true;

	  if (strLo==L"округ" ||strLo==L"округа" || strLo==L"округу" ||strLo==L"округе"  ||strLo==L"округи" ||
		strLo==L"округом" || strLo==L"округами" || strLo==L"округам" || strLo==L"округах" ) return true;

	  if (strLo==L"пролив" ||strLo==L"пролива" || strLo==L"проливу" ||
		strLo==L"проливом" || strLo==L"проливами" || strLo==L"проливам" ||
		 strLo==L"проливе") return true;

	   if (strLo==L"залив" ||strLo==L"залива" || strLo==L"заливу" ||
		strLo==L"заливом" || strLo==L"заливами" || strLo==L"заливах" ||
		 strLo==L"заливе") return true;

	   if (strLo==L"полуостров" ||strLo==L"полуострова" || strLo==L"полуострову" ||
		   strLo==L"полуостровом" || strLo==L"полуострове") return true;

	   if (strLo==L"низменность" ||strLo==L"низменности" || strLo==L"низменностью") return true;

	   if (strLo==L"горы" ||strLo==L"гор" || strLo==L"горами") return true;

	   if (strLo==L"возвышенность" ||strLo==L"возвышенности" || strLo==L"возвышенностью") return true;

	   if (strLo==L"край" ||strLo==L"края" || strLo==L"краю" ||
		strLo==L"краем" || strLo==L"крае" || strLo==L"краям" || strLo==L"краях" || 
		strLo==L"краями") return true;

	   if (strLo==L"республика" ||strLo==L"республике" || strLo==L"республикой" ||strLo==L"республики"  ||
		strLo==L"республику") return true;

	   return false;


}

void CLexica::MarkRegions(IR::CSentence *pSent)
{
	IR::CUnit* pUnitLast = pSent->GetLastUnit();
	IR::CUnit* pUnit = pSent->GetFirstUnit();
	
	IR::CUnit* pFirstUnit =  pUnit;
	bool isRusFind = false;
	bool isEngFind = false;
	int nProcessed = 0;
	int nInserted = 1;
	int ruCount = 0;

	IR::CUnit* pUnitNew = NULL;
	//m_pUnitStorage->Allocate();
	//pUnit->SetUnit(m_pStorage->CreateUnit());
	//pUnit->SetWordType(wtWord);

	bool isPrevProp = false;	
	list<IR::CUnit*> geoList;

	while (pUnit != NULL)
	{
		wstring str = pUnit->GetUnit()->GetWord();
		IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex();
		if (isRegionSpecialPre(pUnit->GetUnit()->GetWord()))
		{
			pUnit->SetWordType(wtGeoSemanticPre, wmPlaces);
		}
		else if (pIndex!=NULL && pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase()==1 && str.size()>0 &&
			     (m_oUnitChecker.HasSemantic1(pUnit->GetUnit(),SS::Core::Features::Values::TSemanticType.smtPlace)||
			     !m_oUnitChecker.HasSemantic1(pUnit->GetUnit(),SS::Core::Features::Values::TSemanticType.smtPerson) ||
			     pUnit == pUnitLast)
				)
		{			 	
			 pUnit->SetWordType(wtProperName, wmPlaces);
		} 		
		else if (str == L"и")
		{
			pUnit->SetWordType(wtGeoConj, wmPlaces);
		}
		else if (str == L",")
		{
			pUnit->SetWordType(wtComma, wmPlaces);
		}
		else if (pUnit->GetType(wmPlaces)==wtProperName)
		{
			pUnit->SetWordType(wtUndefined, wmPlaces);
		}

		pUnit = pSent->GetNextUnit();
	}

	pUnit = pSent->GetFirstUnit();

	while (pUnit != NULL)
	{
		wstring str = pUnit->GetUnit()->GetWord();
		IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex();
		if (pIndex!=NULL && pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase()==1 && str.size()>0 &&
			m_oUnitChecker.IsAdjectiveEvrisctic(pUnit->GetUnit()))
		{			 	
			 pUnit->SetWordType(wtProperNameUnknown, wmPlaces);
		} 		
		else if (isRegionSpecialPost(pUnit->GetUnit()->GetWord()))
		{
			pUnit->SetWordType(wtGeoSemantic, wmPlaces);
		}
		else if (str == L"и")
		{
			pUnit->SetWordType(wtGeoConj, wmPlaces);
		}
		else if (str == L",")
		{
			pUnit->SetWordType(wtComma, wmPlaces);
		}	
		pUnit = pSent->GetNextUnit();
	}

	/*while(pUnit!=NULL)
	{

		wstring str = pUnit->GetUnit()->GetWord();
		IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex();
		if (pIndex!=NULL && pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase()==1 && str.size()>0 &&
			m_oUnitChecker.IsAdjectiveEvrisctic(pUnit->GetUnit()))
		{			 	
			 geoList.push_back(pUnit);
		} 			 
		else
		{
			if (isRegionSpecial(pUnit->GetUnit()->GetWord()) && geoList.size()!=0)
			{
			  geoList.push_back(pUnit);
			}else
			{
				if (geoList.size()>1)
				{
					for(list<IR::CUnit*>::iterator it = geoList.begin(); it!=geoList.end(); it++)
					{
						if (it == geoList.begin())
						{
							(*it)->SetWordType(wtProperNameUnknown, wmPlaces);
						}else
							(*it)->SetWordType(wtGeoSemantic, wmPlaces);
					}
				}
				geoList.clear();
			}
		}
		
		pUnit = pSent->GetNextUnit();
	}
	if (geoList.size()>1)
	{
		for(list<IR::CUnit*>::iterator it = geoList.begin(); it!=geoList.end(); it++)
		{
			if (it == geoList.begin())
			{
				(*it)->SetWordType(wtProperNameUnknown, wmPlaces);
			}else
				(*it)->SetWordType(wtGeoSemantic, wmPlaces);
		}
	}
	geoList.clear();*/
}

void CLexica::MarkUnknownPersons(IR::CSentence *pSent)
{
	IR::CUnit* pUnit = pSent->GetFirstUnit();
	bool isPossibleLN = false;
	IR::CUnit* pLNUnit = NULL;
	IR::CUnit* pFNUnit = NULL;
	IR::CUnit* pMNUnit = NULL;

	SMIList list;

	bool isFN = false;
	bool isMN = false;
	bool isLN = false;

	while (pUnit != NULL)
	{
		wstring str = pUnit->GetUnit()->GetWord();				
		IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex();		
		if (pIndex!=NULL && pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase()) 
		{
			if (m_oUnitChecker.HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson,
				SS::Core::Features::Values::TSPersonType.semprstMiddleName) || (pUnit->GetUnit()->IsRusInitial() && !isMN)
			   )
			{
				isMN = true;
				pMNUnit = pUnit;
			}
			else if (m_oUnitChecker.HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson,
				     SS::Core::Features::Values::TSPersonType.semprstFirstName) || pUnit->GetUnit()->IsRusInitial())
			{
				isFN = true;
				pFNUnit = pUnit;
			}
			else if (m_oUnitChecker.HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson,
				     SS::Core::Features::Values::TSPersonType.semprstLastName) && !pUnit->GetUnit()->GetEvristicLN())
			{
				isLN = true;
			}
			else if (pIndex != NULL && str.size() > 0 && 
				     (m_oUnitChecker.isXZ(pUnit->GetUnit()) || m_oUnitChecker.IsRusLNForms(pUnit->GetUnit())) &&
				     pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase() == 1 )
			{
				if ((isFN || isMN) && !isLN)
				{
					// можем пометить на объединение
					pUnit->SetWordType(wtFName, wmMultiNames);
					if (pFNUnit!=NULL)pFNUnit->SetWordType(wtFName, wmMultiNames);
					if (pMNUnit!=NULL) pMNUnit->SetWordType(wtFName, wmMultiNames);				
				}
				else
				{
					isPossibleLN = true;
					pLNUnit = pUnit;
				}
				isMN=false;
				isFN =false;
				isLN = false;
				
			}
			else
			{
				if ((isFN || isMN) && !isLN && isPossibleLN)
				{
					// можно пометить на обхединение
					if (pLNUnit!=NULL) pLNUnit->SetWordType(wtFName, wmMultiNames);
					if (pFNUnit!=NULL) pFNUnit->SetWordType(wtFName, wmMultiNames);
					if (pMNUnit!=NULL) pMNUnit->SetWordType(wtFName, wmMultiNames);				
				}
				isPossibleLN = false;
				pLNUnit = NULL;
				isMN=false;
				isFN =false;
				isLN = false;
			}
		}

		pUnit = pSent->GetNextUnit();
	}

	if ((isFN || isMN) && !isLN && isPossibleLN)
	{
		// можно пометить на обхединение
		/*if (pLNUnit!=NULL && pFNUnit!=NULL && pMNUnit!=NULL)
		{
			pLNUnit->SetWordType(wtFName, wmMultiNames);
			pFNUnit->SetWordType(wtFName, wmMultiNames);
			pMNUnit->SetWordType(wtFName, wmMultiNames);
		}*/
		if (pLNUnit!=NULL) pLNUnit->SetWordType(wtFName, wmMultiNames);
		if (pFNUnit!=NULL) pFNUnit->SetWordType(wtFName, wmMultiNames);
		if (pMNUnit!=NULL) pMNUnit->SetWordType(wtFName, wmMultiNames);		
	}


	/* pUnit = pSent->GetFirstUnit();
	 isPossibleLN = false;
	 pLNUnit = NULL;
	 isMN=false;
	 isFN =false;
	 isLN = false;
	 while(pUnit!=NULL)
	 {
		 if (m_oUnitChecker.HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson,
			SS::Core::Features::Values::TSPersonType.semprstMiddleName))
		{
			isMN = true;
			pMNUnit = pUnit;
		}
		if (m_oUnitChecker.HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson,
			SS::Core::Features::Values::TSPersonType.semprstFirstName))
		{
			isFN = true;
			pFNUnit = pUnit;
		}
		 pUnit = pSent->GetNextUnit();
	 }*/
}

bool CLexica::AnalyzeSentencesJoin(ISentence* pPrevSent, ISentence* pCurSent)
{
    bool isJoin = false;
    if (pPrevSent && pCurSent)
    {
        IUnit* pLastUnit = pPrevSent->GetLastUnit();
        IUnit* pFirstUnit = pCurSent->GetFirstUnit();
		int lastCharPosition = pLastUnit->GetFirstCharPositionNew();
        int firstCharPosition = pFirstUnit->GetFirstCharPositionNew();
		wstring lastWord = pLastUnit->GetWord();
        wstring word = pFirstUnit->GetWord();
		bool isLastCombi = false;
		if (lastWord.empty())
		{
			isLastCombi = true;
			IUnit *pChild = pLastUnit->GetRightChildUnit();
			if (pChild)
			{
				lastCharPosition = pChild->GetFirstCharPositionNew();
                lastWord = pChild->GetWord();
			}
		}
		else
		{
			isLastCombi = lastWord.find(L'.') > 0;
		}
		lastCharPosition += lastWord.size() - 1;
		bool isCombinatoryPerson = false;
        if (word.empty())
        {
			isCombinatoryPerson = m_oUnitChecker.HasSemantic1(pFirstUnit, SS::Core::Features::Values::TSemanticType.smtPerson);
            IUnit *pChild = pFirstUnit->GetLeftChildUnit();
            if (pChild)
			{
                word = pChild->GetWord();
			}
        }
		bool isNear = (firstCharPosition - lastCharPosition == 1);
		bool isPersonCombi = isLastCombi && (isCombinatoryPerson || m_oUnitChecker.IsProper(pFirstUnit));
		wstring wrongSymbols = L"—-–";
		bool isWrongStart = (word.size() == 1) && (wrongSymbols.find(word[0]) != wstring::npos);
		if ((lastWord != L";") && !isWrongStart && (isNear || (!word.empty() && iswalpha(word[0]) && (iswlower(word[0]) || isPersonCombi))))
        {
            JoinSentences(pLastUnit, pFirstUnit);
            isJoin = true;
        }
    }
    return isJoin;
}

void CLexica::JoinSentences(IUnit* pLeft, IUnit* pForAdd)
{
    for (IUnit *pCurrent = pForAdd; pCurrent;)
    {
        int lastCharPosition = pLeft->GetFirstCharPositionNew();
        int firstCharPosition = pCurrent->GetFirstCharPositionNew();
        
        IUnit* pNext = pCurrent->Pop();
        pLeft->AddUnitHorizontal(pCurrent);
        
        firstCharPosition = pCurrent->GetFirstCharPositionNew();

        /*IUnit* pChild = pCurrent->GetLeftChildUnit();
        if(pChild)
        {
            IUnit* pNextChild = pChild->Pop();
            pLeft->AddUnitVertical(pChild);
            JoinSentences(pLeft->GetRightChildUnit(), pNextChild);
        }*/
        pLeft = pLeft->GetRightUnit();

        pCurrent = pNext;
    }
}

wstring* CLexica::GetFirstSentenceOfParagraph(vector<vector<wstring>> *pParagraphs, vector<vector<wstring>>::iterator &pIt, vector<wstring>::iterator &sIt)
{
	wstring *pSentence = 0;
	if (pParagraphs && (pIt != pParagraphs->end()))
	{
		sIt = pIt->begin();
		if (sIt != pIt->end())
		{
			pSentence = &(*sIt);
		}
		else
		{
			++pIt;
			pSentence = GetFirstSentenceOfParagraph(pParagraphs, pIt, sIt);
		}
	}
	return pSentence;
}

bool CLexica::AddSentence(CIRLoader::EBreakReason br, int maxUnitCount, bool isParagraphEnd, bool isSimple, int mode)
{
	int count = m_oSent.GetUnitCount();
	if (count > 0)
	{
		if (br == CIRLoader::brBoardEnd && !AnalysisFinished())
		{
			// m_oSent is only beginning of a sentence, which ends in the next block
			return false;
		}
		else
		{
			// first set language, because after all joins it can become undefinable
			m_oSent.SetLanguage();					
			if (!m_bLite)
			{
				if (isSimple)
				{
					if (mode == 0)
					{
						m_oIndexLoader.FillCombinatoryInfo(&m_oSent);
					}
					else if (mode == 1)
					{
						m_oWalker.Process(&m_oSent, wmReparsedItems);
						m_oIndexLoader.FillNumberCombinatory(&m_oSent);
						m_oWalker.Process(&m_oSent, wmDates);
					}
				}
				else
				{
					EngInRusEvristicJoin(&m_oSent);

					// joins by combinatory dictionary
					m_oIndexLoader.FillCombinatoryInfo(&m_oSent);

					// парсим телефонные номера перед NumberCombinatory
					m_oWalker.Process(&m_oSent, wmTelephones);

					// join reparsed entities
					m_oWalker.Process(&m_oSent, wmReparsedItems);

					// join all numerics, written as words
					m_oIndexLoader.FillNumberCombinatory(&m_oSent);

					// make all other joins
					m_oWalker.Process(&m_oSent, wmQuotations);

				 	SpecialPersonEvristicJoin(&m_oSent);

					EvristicFindPLaces(&m_oSent);
					m_oWalker.Process(&m_oSent, wmDates);

					MarkRegions(&m_oSent);
					m_oWalker.Process(&m_oSent, wmPlaces);
					//m_oWalker.Process(&m_oSent, wmOrganizations);
					MarkUnknownPersons(&m_oSent);
					m_oWalker.Process(&m_oSent, wmMultiNames);
					m_oWalker.Process(&m_oSent, wmNames);
					m_oWalker.Process(&m_oSent, wmLSequences);
					//m_oWalker.Process(&m_oSent, wmInSequences);
				}
			}
			if (m_bQuery)
			{
				m_oSent.RemoveUnits(wtApostroph);
			}

			if (!m_bLite)
			{
				// number semantics must be filled only for not joined units
				m_oWalker.SetNumberSemantic(&m_oSent);
				m_oWalker.SetBeRepresentSemantic(&m_oSent);
			}
			m_oWalker.SetNounToUnknownWords(&m_oSent);
		}

		vector<ISentence*> sentences = m_oSent.GetSentences(m_pStorage, maxUnitCount, m_bQuery);
		for (vector<ISentence*>::iterator it = sentences.begin(); it != sentences.end(); ++it)
		{
			m_oPara.AddSentence(*it);
		}
	}
	else
	{
		return false;
	}

	if (!m_bIgnoreParagraphs)
	{
		if (isParagraphEnd || (br == CIRLoader::brParaBreak))
		{
			m_oPara.Break();
		}
		else if (br == CIRLoader::brTitle)
		{
			m_oPara.Break(m_oIRLoader.GetHeaderLevel());
		}
	}
	return true;
}

void CLexica::AnalyzeNextSentence(IText* pText, bool isSimple/* = false*/, int mode/* = 1*/)
{
	SS_TRY;

	m_oIRLoader.SetIgnoreBreakers(m_bQuery);
	if (IsStrictQuery() && !m_bUseQueryDeformer)
	{
		AnalyzeStrictQuery(pText);
		return;
	}

	CLexemCollection oLexems;
	if (m_bSearch)
	{
		// in m_bSearch mode we don't use ILexicaPrimary, so make like analysis is finished
		SetAnalysisFinished(true);
		// reset IRLoader and init paragraph, because in m_bSearch mode SetText is not called
		m_oIRLoader.Reset();
		m_oPara.Init(m_pStorage);

		oLexems.Init(pText, static_cast<Reparser::IReparser*>(this), &m_oPara, &m_oIndexLoader);
	}
	else
	{
		oLexems.Init(m_pBoard, static_cast<Reparser::IReparser*>(this));
	}

	m_oPara.SetText(pText);
	
	int maxUnitCount = isSimple ? 0 : MAX_WORDS_IN_SENTENCE;
	if (oLexems.MoveFirst())
	{
		// do until oLexems and m_oIRLoader's internal data are both exhausted
		while (!oLexems.IsEnd() || !m_oIRLoader.IsFinished())
		{
			CIRLoader::EBreakReason br = m_oIRLoader.LoadSentence(&m_oSent, &oLexems);
			if (!AddSentence(br, maxUnitCount, false, isSimple, mode))
			{
				break;
			}
		}

		m_oPara.Finalize(AnalysisFinished());
	}
	else
	{
		m_oPara.Finalize(true);
	}

	if (m_bLite)
	{
		SetQuestionWords(pText);
	}

	SS_CATCH(L"");
}

void CLexica::AnalyzeNext(IText* pText, vector<vector<wstring>> *pParagraphs, bool isSimple/* = false*/, int mode/* = 1*/)
{
	SS_TRY;

	m_oIRLoader.SetIgnoreBreakers(m_bQuery);
	if (IsStrictQuery() && !m_bUseQueryDeformer)
	{
		AnalyzeStrictQuery(pText);
		return;
	}

	CLexemCollection oLexems;
	if (m_bSearch)
	{
		// in m_bSearch mode we don't use ILexicaPrimary, so make like analysis is finished
		SetAnalysisFinished(true);
		// reset IRLoader and init paragraph, because in m_bSearch mode SetText is not called
		m_oIRLoader.Reset();
		m_oPara.Init(m_pStorage);

		oLexems.Init(pText, static_cast<Reparser::IReparser*>(this), &m_oPara, &m_oIndexLoader);
	}
	else
	{
		oLexems.Init(m_pBoard, static_cast<Reparser::IReparser*>(this));
	}

	m_oPara.SetText(pText);
	vector<vector<wstring>>::iterator pIt;
	vector<wstring>::iterator sIt;
	wstring *pCurrentSentence = 0;
	int sentenceIndex;
	if (pParagraphs)
	/// переходим на первое предложение
	{
#ifdef WITH_LOG
		ofstream filestr("D:\\NLP\\Lingvistic2012\\log.txt", fstream::app);
		filestr << "paragraphs: " << pParagraphs->size() << std::endl;
#endif

		pIt = pParagraphs->begin();
		pCurrentSentence = GetFirstSentenceOfParagraph(pParagraphs, pIt, sIt);
#ifdef WITH_LOG
		if (pCurrentSentence)
			filestr << "paragraph sentences: " << pIt->size() << std::endl;
		filestr.close();
#endif
		sentenceIndex = 0;
	}
	
	int maxUnitCount = isSimple ? 0 : MAX_WORDS_IN_SENTENCE;
	if (oLexems.MoveFirst())
	{
        ISentence* pPrevSent = 0;
		// do until oLexems and m_oIRLoader's internal data are both exhausted
		while (!oLexems.IsEnd() || !m_oIRLoader.IsFinished())
		{
			CIRLoader::EBreakReason br = m_oIRLoader.Load(&m_oSent, &oLexems, pCurrentSentence);

			bool isParagraphEnd = false;
			if (pParagraphs && pCurrentSentence)
			/// переходим на следующее предложение
			{
#ifdef WITH_LOG
				ofstream filestr("D:\\NLP\\Lingvistic2012\\log.txt", fstream::app);
				filestr << "sentence №" << sentenceIndex << std::endl;
#endif
				++sentenceIndex;

				pCurrentSentence = 0;
				++sIt;
				if (sIt == pIt->end())
				{
					isParagraphEnd = true;
					++pIt;
					pCurrentSentence = GetFirstSentenceOfParagraph(pParagraphs, pIt, sIt);
					
					sentenceIndex = 0;
#ifdef WITH_LOG
					if (pCurrentSentence)
						filestr << "paragraph sentences: " << pIt->size() << std::endl;
#endif
				}
				else
				{
					pCurrentSentence = &(*sIt);
				}
#ifdef WITH_LOG
				filestr.close();
#endif
			}

			if (!AddSentence(br, maxUnitCount, isParagraphEnd, isSimple, mode))
			{
				break;
			}
		}

		m_oPara.Finalize(AnalysisFinished());
	}
	else
	{
		m_oPara.Finalize(true);
	}

	if (m_bLite)
	{
		SetQuestionWords(pText);
	}

	SS_CATCH(L"");
}

void CLexica::FillSynonyms(IText* pText)
{
	SS_TRY;

	if (m_bLite && !m_bQuery)
	{
		return;
	}

	if (!GetAnalyseParams()->GetAnalyseDepthParams()->IsSynonims())
	{
		return;
	}

	for (IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		for (ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		{
			if (pSent->GetLanguage() == ealRussian)
			{
				continue;
			}

			for (IUnit* pUnit = pSent->GetFirstUnit(); pUnit; pUnit = pUnit->GetRightUnit())
			{
				for (CSentenceIterator it = pUnit; *it != pUnit->GetRightUnit(); ++it)
				{
					m_oIndexLoader.FillSynonims(*it);
				}
			}
		}
	}

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CLexica functions

bool CLexica::IsStrictQuery()
{
	SS_TRY;

	if (!m_bQuery)
	{
		return false;
	}

	IAnalyseParams* pParams = GetAnalyseParams();
	if (!pParams)
	{
		return false;
	}
	if (pParams->GetAnalyseMod() == eamStrictQuery || pParams->GetAnalyseMod() == eamOnlyKeyWords)
	{
		return true;
	}

	// check for quotation marks
	ELexemType	ltFirst = m_pBoard->GetFirstLexem(),
				ltLast = m_pBoard->GetLastLexem();
	if (ltFirst == accLQuote && (ltLast == accRQuote || ltLast == accLQuote))
	{
		GetAnalyseParams()->SetAnalyseMod(eamStrictQuery);
		return true;
	}

	// check if it's an interrogative sentence
	if (IsOnlyKeywordsSentence())
	{
		GetAnalyseParams()->SetAnalyseMod(eamOnlyKeyWords);
		return true;
	}

	SS_CATCH(L"");
	return false;
}

bool CLexica::IsOnlyKeywordsSentence()
{
	SS_TRY;

	if (m_sQueryWordsIDSources.empty())
	{
		m_oIndexLoader.FillQueryWordSet(&m_sQueryWordsIDSources);
	}

	int i = 0;
	for (unsigned int ui = m_pBoard->MoveFirst(); ui; ui = m_pBoard->MoveNext())
	{
		if (i++ > 2)
		{
			break;
		}

		LPCWSTR sz = m_pBoard->GetLexeme();
		UINT idSource = m_oIndexLoader.GetQueryWordSourceID(sz);
		if (m_sQueryWordsIDSources.find(idSource) != m_sQueryWordsIDSources.end())
		{
			return false;
		}
	}

	i = 0;
	for (bool b = m_pBoard->MoveLast(); b; b = m_pBoard->MovePrev())
	{
		if (i++ > 2)
		{
			break;
		}

		LPCWSTR sz = m_pBoard->GetLexeme();
		UINT idSource = m_oIndexLoader.GetQueryWordSourceID(sz);
		if (m_sQueryWordsIDSources.find(idSource) != m_sQueryWordsIDSources.end())
		{
			return false;
		}
	}

	SS_CATCH(L"");
	return true;
}

void CLexica::AnalyzeStrictQuery(IText* pText)
{
	SS_TRY;

	// reset analyze params and reinit index loader
	IAnalyseParams* pParams = GetAnalyseParams();
	pParams->GetAnalyseDepthParams()->SetIsMorpho(true);
	pParams->GetAnalyseDepthParams()->SetIsSemantic(true);
	pParams->GetAnalyseDepthParams()->SetIsSyntax(false);
	pParams->GetAnalyseDepthParams()->SetIsSynonims(false);

	m_oIndexLoader.Init(GetLoadManager(), m_pStorage, pParams, m_oSent.GetUnitStorage(), &m_oUnitChecker);

	m_oPara.SetText(pText);
	if (m_pBoard->MoveFirst())
	{
		// reset and load sentence
		m_oSent.Reset();

		CLexemCollection oLexems;
		oLexems.Init(m_pBoard, static_cast<Reparser::IReparser*>(this));
		m_oIRLoader.Load(&m_oSent, &oLexems, 0);

		// must not result more than one sentence
		assert(m_pBoard->IsEnd() && m_oIRLoader.IsFinished());

		// set language
		m_oSent.SetLanguage();
		if (pParams->GetAnalyseMod() == eamStrictQuery)
		{
			// if first and last units are quotation marks, remove them
			IR::CUnit	*pFirstUnit = m_oSent.GetFirstUnit(),
						*pLastUnit = m_oSent.GetLastUnit();
			if (pFirstUnit->GetType(wmDefault) == wtQuote && pLastUnit->GetType(wmDefault) == wtQuote)
			{
				m_oSent.RemoveUnit(pFirstUnit);
				m_oSent.RemoveUnit(pLastUnit);
			}

			IR::CUnit* pUnit = m_oSent.GetUnitStorage()->Allocate();
			pUnit->SetUnit(m_pStorage->CreateUnit());
			m_oSent.JoinSentence(pUnit);
			m_oUnitChecker.AddSyntaxCategoryIndex(*pUnit, Values::SyntaxCategories.scMainEntity);
		}
		else if(pParams->GetAnalyseMod() == eamOnlyKeyWords)
		{
			pParams->GetAnalyseDepthParams()->SetIsSemantic(true);
		}

		// create ISentence from IR::CSentence and add it to current paragraph
		m_oPara.AddSentence(m_oSent.GetSentence(m_pStorage));
	}

	m_oPara.Finalize(true);
	AnalizeNewQuestionStructures(pText);

	SS_CATCH(L"");
}

void CLexica::SetQuestionWords(IText* pText)
{
	SS_TRY;

	if (!m_bQuery)
	{
		return;
	}

	IAnalyseParams* pParams = GetAnalyseParams();
	if (!pParams)
	{
		return;
	}
	if (pParams->GetAnalyseMod() == eamStrictQuery || pParams->GetAnalyseMod() == eamOnlyKeyWords)
	{
		return;
	}

	for (IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		for (ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		{
			int i = 0;
			for (CLeafIterator it = pSent->GetFirstUnit(); *it; ++it)
			{
				if (i++ > 2)
				{
					break;
				}

				UINT idSource = m_oIndexLoader.GetQueryWordSourceID((*it)->GetWord());
				if (m_sQueryWordsIDSources.find(idSource) != m_sQueryWordsIDSources.end())
				{
					(*it)->CreateSyntaxFeature()->SetIsQuestion();
				}
			}
		}
	}

	SS_CATCH(L"");
}

void CLexica::AnalizeNewQuestionStructures(IText* pText)
{
	SS_TRY;

	if (!m_bQuery)
	{
		return;
	}

	for (IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		for (ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		{
			for (CLeafIterator it = pSent->GetFirstUnit(); *it; ++it)
			{
				IUnit* pUnit = m_oUnitChecker.SetKeywordSyntax(*it);
				if (pUnit)
				{
					pSent->AddUnit(pUnit);
					return;
				}
			}
		}
	}

	SS_CATCH(L"");
}

void CLexica::SetSemanticType(IUnit* pUnit, SS::Core::Features::Types::TSemanticType semanticType)
{
	if (m_oUnitChecker.HasSemantic1(pUnit, SS::Core::Features::Values::TSemanticType.smtTimeInterval))
	{
		return;
	}
	m_oUnitChecker.ClearSemantics(pUnit, semanticType);
	if (!semanticType.IsUndefined() && !m_oUnitChecker.HasSemantic1(pUnit, semanticType))
	{
		if (semanticType.Equal(SS::Core::Features::Values::TSemanticType.smtOrganization))
			m_oUnitChecker.AddOrganizationIndex(pUnit, SS::Core::Features::Values::TSOrganizationType.orgtCompany);
		else if (semanticType.Equal(SS::Core::Features::Values::TSemanticType.smtPerson))
			m_oUnitChecker.AddPersonIndex(pUnit, SS::Core::Features::Values::TSPersonType.semprstName);
		else if (semanticType.Equal(SS::Core::Features::Values::TSemanticType.smtPlace))
			m_oUnitChecker.AddPlaceIndex(pUnit, SS::Core::Features::Values::TSPlaceType.Undefined);
		else if (semanticType.Equal(SS::Core::Features::Values::TSemanticType.smtEvent))
			m_oUnitChecker.AddSemanticIndex(pUnit, SS::Core::Features::Values::TSemanticType.smtEvent);
		else if (semanticType.Equal(SS::Core::Features::Values::TSemanticType.smtProduct))
			m_oUnitChecker.AddSemanticIndex(pUnit, SS::Core::Features::Values::TSemanticType.smtProduct);
	}
}

bool CLexica::IsSemanticType(IUnit* pUnit, SS::Core::Features::Types::TSemanticType semanticType)
{
	return m_oUnitChecker.HasSemantic1(pUnit, semanticType);
}

}
}
