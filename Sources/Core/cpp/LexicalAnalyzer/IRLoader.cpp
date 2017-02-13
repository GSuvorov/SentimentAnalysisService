#include "stdafx.h"
#include "IRLoader.h"

#include "IndexLoader.h"
#include <iostream>

using namespace SS::Core::Types::IndexationSearch;


namespace SS
{
namespace LexicalAnalyzer
{

#include "lex.IRLoader.cc"

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CIRLoader::CIRLoader() : 
		m_pStorage(NULL),
		m_pIndexLoader(NULL),
		m_pUnitChecker(NULL),
		m_bIgnoreBreakers(false),
		m_bSentFinished(true),
		m_pReparser(NULL),
		m_bQuery(false),
		m_bSearch(false),
		m_bLite(false),
		m_pLexems(NULL),
		m_iCurrentHeaderLevel(0),		
		m_bTitleMode(false),
		m_pUnitStorage(NULL),
		m_uiCurrentPosition(0),
		m_pSent(NULL),
		m_nProcessed(0),
		m_nInserted(0)
{
	sentCount = 0;
}

CIRLoader::~CIRLoader()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CIRLoader interface

void CIRLoader::SetAnalyseParams(IAnalyseParams* pAnalyseParams)
{
	CAnalyseParams::SetAnalyseParams(pAnalyseParams);

	m_bSearch = (pAnalyseParams->GetAnalyseMod() == eamSearch) &&
		        (pAnalyseParams->GetAnalyseResultEx() == TSearchParams::saNormal);

	m_bQuery =	(pAnalyseParams->GetAnalyseMod() == eamQuery) ||
				(pAnalyseParams->GetAnalyseMod() == eamRejectUnimportant);

	m_bLite = pAnalyseParams->m_bIsLiteMode;
}

void CIRLoader::Init(IBlackBoxTextStorage* pStorage, CIndexLoader* pIndexLoader, ILoadManager* pLoadManager,
					 CUnitChecker* pUnitChecker, Reparser::IReparser* pReparser, IR::TUnitStorage* pUnitStorage)
{
	SS_TRY;

	m_pStorage     = pStorage;
	m_pIndexLoader = pIndexLoader;
	m_pUnitChecker = pUnitChecker;
	m_pReparser    = pReparser;
	m_pUnitStorage = pUnitStorage;

	SS_CATCH(L"");
}

void CIRLoader::Reset()
{
	SS_TRY;

	m_nProcessed = 0;
	m_nInserted = 0;
	m_uiCurrentPosition = 0;
	m_bIgnoreBreakers = false;
	m_bSentFinished = true;
	m_iCurrentHeaderLevel = 0;
	m_bTitleMode = false;

	yyrestart(NULL);

	SS_CATCH(L"");
}

void CIRLoader::SetIgnoreBreakers(bool bSet)
{
	m_bIgnoreBreakers = bSet;
}

inline bool IsLetter(wchar_t symbol)
{
	return iswalpha(symbol);
}
inline bool IsSpace(wchar_t symbol)
{
	return iswspace(symbol);
}
inline bool IsNotSpace(wchar_t symbol)
{
	return !IsSpace(symbol);
}

bool CIRLoader::IsStringContainSymbol(const wstring &sentence, int start, int end, std::function<bool(const wchar_t)> predicate)
{
	int length = sentence.length();
	for (int i = start; (i < length) && (i < end); ++i)
	{
		if (predicate(sentence[i]))
		{
			return true;
		}
	}
	return false;
}

bool CIRLoader::IsNeedAddSign(int nToLeft)
{
	ELexemType lt = m_pLexems->GetLexemType(nToLeft);
	assert(lt == accSign);
	wstring str = m_pLexems->GetLexemString(m_nInserted - m_nProcessed + yyleng);
	if ( str.size() == 1 )
	{
		wchar_t ch = str[ 0 ];
		if ( (ch == L'\r') || (ch == (wchar_t)0x2000 /*longSpace*/) )
		{
			return false;
		}
	}
	return true;
}

int CIRLoader::MissSpaces(const wstring &sentence, int startShift)
{
	int shift = startShift;
	int length = sentence.length();
	while (shift < length)
	{
		if (IsSpace(sentence[shift]))
			++shift;
		else
			break;
	}
	return shift;
}

int CIRLoader::UpdateShift(const wstring *pSentence, int offset, int count, int &shift)
{
	int unitCount = 0;
	if (pSentence)
	{
		for (int i = count + offset; i > offset; --i)
		{
			wstring str = m_pLexems->GetLexemString(i);
			ELexemType lt = m_pLexems->GetLexemType(i);
			int index = pSentence->find(str, shift);
			if ((lt == accTire) || (lt == accApostroph))
			{
				if ((index != wstring::npos) && !IsStringContainSymbol(*pSentence, shift, index, IsNotSpace))
				{
					shift = index;
				}
				else
				{
					shift = MissSpaces(*pSentence, shift);
				}
				shift += str.length();
				++unitCount;
			}
			else if (lt == accParaBreak)
			{
				break;
			}
			else if (str == L"\r")
			{
				++unitCount;
			}
			else
			{
				/*if (!shift && (index > shift))
				/// пробельный символ в начале предложения
				{
					index = wstring::npos;
					return unitCount;
				}*/

				bool isLetter = false;
				if (index != wstring::npos)
				{
					isLetter = IsStringContainSymbol(*pSentence, shift, index, IsLetter);
				}

				if (isLetter || (index == wstring::npos))
				/// возможно неправильное определение сдвига слова
				{
					wstring subStr = L"";
					int tempShift = shift;
					for (int defisIndex = str.find(L"-"); defisIndex != wstring::npos; defisIndex = str.find(L"-"))
					{
						index = wstring::npos;
						subStr = str.substr(0, defisIndex);
						str = str.substr(defisIndex + 1, str.length() - defisIndex - 1);
						if (!subStr.empty())
						{
							index = pSentence->find(subStr, tempShift);
						}
						if (index != wstring ::npos)
						{
							tempShift = index + subStr.length();
						}
						else
						{
							tempShift = wstring::npos;
							break;
						}
					}
					if ((tempShift != wstring::npos) && !str.empty())
					{
						index = pSentence->find(str, tempShift);

						isLetter = IsStringContainSymbol(*pSentence, tempShift, index, IsLetter);
						if (!isLetter && (index != wstring ::npos))
							tempShift = index + str.length();
						else
							tempShift = wstring::npos;
					}
					if (tempShift != wstring::npos)
					{
						shift = tempShift;
						++unitCount;
					}
					else
					{
						break;
					}
				}
				else
				{
					++unitCount;
					shift = index + str.length();
				}
			}
		}
	}
	return unitCount;
}

CIRLoader::EBreakReason CIRLoader::LoadSentence(IR::CSentence* pSent, CLexemCollection* pLexems)
{
	SS_TRY;
	
	m_pSent = pSent;
	m_pLexems = pLexems;
	IR::CUnit* pUnit = pSent->GetFirstUnit();

	if (m_bSentFinished)
		m_pSent->Reset();

	m_bSentFinished = true;
	for (ELexemType lt = (ELexemType) yylex(); lt; lt = (ELexemType) yylex())
	{
		// real lexems (that are present in LexemCollection)
		m_nProcessed += yyleng;
		bool isAdded = false;
		switch ( lt )
		{
			case accAbbrev:
				HandleAbbrevGroup(m_nInserted - m_nProcessed, yyleng);
				isAdded = true;
				break;
			case accParaBreak:
				isAdded = true;
				break;
			case accBrokenWord:
				HandleBrokenWord(m_nInserted - m_nProcessed);
				isAdded = true;
				break;
			case accUnclosedSentence:
			{
				int offset = m_nInserted - m_nProcessed + 2;
				ELexemType lexemType = m_pLexems->GetLexemType(offset);
				bool isNeedAdd = (lexemType == accSign) ? IsNeedAddSign(offset) : true;
				if (isNeedAdd)
					AddToSentence(offset);
				isAdded = true;
				break;
			}
			case accSign:
				isAdded = !IsNeedAddSign(m_nInserted - m_nProcessed + yyleng);
				break;
			case accComma:
			case accColon:
			case accPoint:
			case accSentBreak:
			case accApostroph:
				if (m_bLite && m_bQuery)
					continue;
				break;
		}
		if (!isAdded)
			AddToSentence(m_nInserted - m_nProcessed, yyleng);
	}

	SS_CATCH(L"");

	m_bSentFinished = false;
	return brBoardEnd;
}

CIRLoader::EBreakReason CIRLoader::Load(IR::CSentence* pSent, CLexemCollection* pLexems, wstring *pFullSentence)
{
	SS_TRY;
	
	int sentenceShift = 0;
	int sentenceLength = pFullSentence ? pFullSentence->length() : -1;
	m_pSent = pSent;
	
	m_pLexems = pLexems;
	IR::CUnit* pUnit = pSent->GetFirstUnit();

	if (m_bSentFinished)
	{
		m_pSent->Reset();
	}

	m_bSentFinished = true;
	if (sentenceLength == 0)
	{
		return brSentBreak;
	}
	for (ELexemType lt = (ELexemType) yylex(); lt; lt = (ELexemType) yylex())
	{
		if ((lt == accBreak) || (lt == accTitleBreak))
		// pseudolexems (that are not present in LexemCollection)
		{
			if (pFullSentence)
			{ 
				; 
			}
			else if (lt == accBreak)
			{
				return brSentBreak;
			}
			else if (lt == accTitleBreak)
			{
				return brTitle;
			}
		}

		// real lexems (that are present in LexemCollection)
		m_nProcessed += yyleng;
		switch ( lt )
		{
			case accComma:
			case accColon:
			case accPoint:
			case accSentBreak:
			case accSign:
			case accApostroph:
				if (m_bLite && m_bQuery)
					continue;
				break;
		}
		if (pFullSentence)
		/// известно содержимое предложения
		{
			int tempShift = sentenceShift;
			int count = UpdateShift(pFullSentence, m_nInserted - m_nProcessed, yyleng, tempShift);
			if ((count == yyleng) || ((count > 0) && (lt == accUnclosedSentence)))
			/// найдены все юниты или найден перевод каретки
			{
				bool isAdded = false;
				switch(lt)
				{
					case accAbbrev:
						HandleAbbrevGroup(m_nInserted - m_nProcessed, yyleng);
						isAdded = true;
						break;
					case accBrokenWord:
						HandleBrokenWord(m_nInserted - m_nProcessed);
						isAdded = true;
						break;
					case accUnclosedSentence:
					{
						int offset = m_nInserted - m_nProcessed + 2;
						ELexemType lexemType = m_pLexems->GetLexemType(offset);
						bool isNeedAdd = (lexemType == accSign) ? IsNeedAddSign(offset) : true;
						if (tempShift != sentenceShift)
						{
							sentenceShift = tempShift;
						}
						else if (isNeedAdd)
						{
							int shift = 0;
							wstring str;
							if (sentenceShift > 0)
							{
								shift = m_pLexems->GetLexemSymbolPosition(offset);
								str = m_pLexems->GetLexemString(offset + 1);
								shift -= m_pLexems->GetLexemSymbolPosition(offset + 1) + str.length();
							}
							str = m_pLexems->GetLexemString(offset);
							sentenceShift += shift + str.length();
						}
						if (isNeedAdd)
							AddToSentence(offset);
						isAdded = true;
						break;
					}
					case accSign:
					{
						isAdded = !IsNeedAddSign(m_nInserted - m_nProcessed + yyleng);
						break;
					}
				}
				if (!isAdded)
				{
					AddToSentence(m_nInserted - m_nProcessed, yyleng);
					sentenceShift = tempShift;
				}
				else if (sentenceShift != tempShift)
				{
					UpdateShift(pFullSentence, m_nInserted - m_nProcessed, yyleng, sentenceShift);
				}
			}
			else
			{
				if ((count == 0) && ((lt == accSign) || (lt == accUnclosedSentence) || (lt == accSentBreak) || (lt == accParaBreak)))
				{	
					;
				}
				else
				{
					m_nProcessed -= yyleng - count;
					if (count > 0)
					{
						AddToSentence(m_nInserted - m_nProcessed, count);
						UpdateShift(pFullSentence, m_nInserted - m_nProcessed, count, sentenceShift);
					}
					yyless(count);
				}
			}
			if (sentenceShift == sentenceLength)
			{
				return brSentBreak;
			}
		}
		else
		{
			switch ( lt )
			{
				case accAbbrev:
					if (HandleAbbrevGroup(m_nInserted - m_nProcessed, yyleng) && !m_bIgnoreBreakers)
						return brSentBreak;
					continue;
				case accParaBreak:
					if (!m_bIgnoreBreakers && !m_bTitleMode)
						return brParaBreak;
					continue;
				case accUnclosedSentence:
					AddToSentence(m_nInserted - m_nProcessed + 2);
					/*FillUnit(m_pSent->AddUnit(), _T("."), accPoint, 
						m_pLexems->GetLexemPosition(m_nInserted - m_nProcessed + 1), 
						m_pLexems->GetLexemSymbolPosition(m_nInserted - m_nProcessed + 1));*/
					if (!m_bIgnoreBreakers && !m_bTitleMode)
						return brParaBreak;
					continue;
				case accBrokenWord:
					HandleBrokenWord(m_nInserted - m_nProcessed);
					continue;
				case accComma:
				case accColon:
				case accPoint:
				case accSentBreak:
				case accSign:
				case accApostroph:
					if (m_bLite && m_bQuery)
						continue;
					break;
			}

			AddToSentence(m_nInserted - m_nProcessed, yyleng);
			if (lt == accPoint || lt == accSentBreak)
			{
				if (!m_bIgnoreBreakers)
				{
					sentCount++;
					return brSentBreak;
				}
			}
			/*if((m_SentenceMaxUnitCount > 0) && (m_pSent->GetUnitCount() >= m_SentenceMaxUnitCount))
			{
				sentCount++;
				return brSentBreak;
			}*/
		}
	}

	SS_CATCH(L"");

	m_bSentFinished = false;
	return brBoardEnd;
}

bool CIRLoader::IsFinished()
{
	// if m_bSentFinished == false, then flex doesn't contain any backup info
	return !m_bSentFinished;
}

int CIRLoader::GetHeaderLevel()
{
	return m_iCurrentHeaderLevel;
}

/////////////////////////////////////////////////////////////////////////////////////
// CIRLoader functions

int CIRLoader::LexerInput(char* buf, int max_size)
{
	SS_TRY;

	if (m_pLexems->IsEnd())
	{
		return 0;
	}

	if (m_pLexems->GetHeaderLevelBegin() != 0 || m_pLexems->GetHeaderLevelEnd() != 0)
	{
		m_iCurrentHeaderLevel = m_pLexems->GetHeaderLevelEnd();
		m_bTitleMode = m_pLexems->GetHeaderLevelBegin() != 0;
		m_pLexems->SetHeaderLevelBegin(0);
		m_pLexems->SetHeaderLevelEnd(0);
		*buf = (char) accTitleBreak;
		return 1;
	}

	/*if(m_pSent->GetUnitCount() + (m_nInserted - m_nProcessed) >= MAX_WORDS_IN_SENTENCE)
	{
		*buf = (char) accBreak;
		return 1;
	}*/

	m_nInserted++;
	*buf = (char) m_pLexems->GetLexemType();
	m_pLexems->MoveNext();

	SS_CATCH(L"");
	return 1;
}

void CheckNominalOnly(IR::CUnit* pUnit1)
{
	bool isNom = false;
	bool isProp = false;
	IUnit* pUnit = pUnit1->GetUnit();
	for (IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		if (pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==SS::Dictionary::DATA_TYPE::NAMES::ednRProperName ||
			pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus)
		{
			isProp = true;
		}
		if (pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName) 
		{
			isNom = true;
		}
	}

	if ( isNom && !isProp ) 
	{
		pUnit->SetIsNominalOnly(true);
	}
	else
	{
		pUnit->SetIsNominalOnly(false);
	}
}

inline bool IsRQuote(LPCTSTR szLexem)
{
	wchar_t ch = szLexem[ 0 ];
	return (ch == (wchar_t)0x201D || ch == L'”' || ch == L'»');
}
inline bool IsQuote(LPCTSTR szLexem)
{
	wchar_t ch = szLexem[ 0 ];
	return (ch == L'\'' ||  ch == L'"');
}
inline bool IsLQuote(LPCTSTR szLexem)
{
	wchar_t ch = szLexem[ 0 ];
	return (ch == (wchar_t)0x201C || ch == L'“' || ch == L'«');
}

void CIRLoader::FillUnit(IR::CUnit* pUnit, LPCTSTR szLexem, ELexemType lt, UINT uiCurrPos, UINT uiSymbolIndex)
{
	SS_TRY
	{
		USES_CONVERSION;
		if (!pUnit->GetUnit())
		{
			// indexation & query
			pUnit->SetUnit(m_pStorage->CreateUnit());
			m_pIndexLoader->FillMorphoInfo(*pUnit, szLexem);
		}

		pUnit->GetUnit()->SetWord(T2CW(szLexem));
		pUnit->GetUnit()->SetPosition(uiCurrPos);
		pUnit->GetUnit()->SetFirstCharPosition(uiSymbolIndex);
	
		if (pUnit->GetUnit()->IsEvristicAbbr())
		{
			lt = accAcronymCitation;
		}
		else
		{
			const wchar_t* word = pUnit->GetUnit()->GetWord();
			if (m_pIndexLoader->IsRomanOrdinal(word))
			{
				pUnit->SetWordType(wtRomanNumber, wmDates);
			}
		}

		IIndex* pIndex = NULL;
		wstring str = szLexem;

		// process lexems
		switch ( lt )
		{	
			case accWord:				
				SetLWord(pUnit, szLexem);
				FillWordIndexes(pUnit);	
				//CheckNominalOnly(pUnit);
				if (m_pUnitChecker->HasSemantic1(pUnit->GetUnit(),SS::Core::Features::Values::TSemanticType.smtOrganization)||
					m_pUnitChecker->HasProfession(pUnit->GetUnit())
				   )
				{
					pUnit->SetWordType(wtWord, wmLSequences);
				}
				pIndex = pUnit->GetUnit()->GetFirstIndex();

				if (pIndex != NULL && 
					m_pUnitChecker->HasSemantic1(pUnit->GetUnit(),SS::Core::Features::Values::TSemanticType.smtPlace) &&
					!m_pUnitChecker->HasSemantic1(pUnit->GetUnit(),SS::Core::Features::Values::TSemanticType.smtPerson) &&
					pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase()
				   )					
				{
					pUnit->SetWordType(wtGeoSemantic, wmLSequences);
				}

				
				if (pIndex != NULL && pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase() &&
					m_pUnitChecker->isXZ(pUnit->GetUnit())
				   )
				{
					pUnit->SetWordType(wtProperNameUnknown, wmNames);
				}

				break;

			case accAcronymCitation:
				if (pUnit->HasIndexes(ednNumeric) || 
					pUnit->HasIndexes(ednRomanNumeric) ||
					(_wcsicmp(pUnit->GetUnit()->GetWord(), L"I") == 0) 
				   )				
				{
					pUnit->SetWordType(wtRomanNumber, wmDates);					
				}
				/*else
				{
					if (!m_pUnitChecker->HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson) &&
					!m_pUnitChecker->HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPlace))
					{
						m_pUnitChecker->ChangeFeature(*pUnit, cfSetNounCommonSingle);					
						m_pUnitChecker->AddServiceTypeIndex(*pUnit, Values::TSServiceTypes.sstAcronymCitation);
					}
				}*/
					
				pUnit->SetWordType(wtWord);				
				pUnit->SetWordType(wtProperName, wmQuotations);

				DetectSTerminated(pUnit, szLexem);

				m_pIndexLoader->FillSemanticInfo(*pUnit);		

				if (!m_pUnitChecker->HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPlace)  &&
					!pUnit->GetUnit()->IsInitial()
				   )
				{
			   	  pUnit->SetWordType(wtOrgAbbr, wmOrganizations);
				}
				else
				{
					pUnit->SetWordType(wtGeoSemantic, wmOrganizations);
				}
				
				if (!pUnit->HasIndexes(ednNumeric) && 
					!pUnit->HasIndexes(ednRomanNumeric) &&
					!m_pUnitChecker->HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPerson) &&
					!m_pUnitChecker->HasSemantic1(pUnit->GetUnit(), SS::Core::Features::Values::TSemanticType.smtPlace)
				   )									
				{
					m_pUnitChecker->ChangeFeature(*pUnit, cfSetNounCommonSingle);					
					m_pUnitChecker->AddServiceTypeIndex(*pUnit, Values::TSServiceTypes.sstAcronymCitation);
				}		

				if (pUnit->GetUnit()->IsInitial())
				{
					pUnit->SetWordType(wtInitial, wmNames);
					pUnit->SetWordType(wtLWord, wmLSequences);
				}

				m_pUnitChecker->FillSemanticTypes(pUnit);
				break;

			case accSmallWord:
				if(_tcslen(szLexem) <= 3)
				{
					SetLWord(pUnit, szLexem);					
					if (!PredefinedBeginEndDateSmallWord(pUnit, szLexem) && 
					    !DetectPredefinedSmallword(pUnit, szLexem)
					   )
					{
						DetectSTerminated(pUnit, szLexem);
						pUnit->SetWordType(wtSmallWord, wmDates);
						pUnit->SetWordType(wtSmallWord, wmOrganizations, false);
						if (pUnit->GetType(wmLSequences) == wtLWord)
						{
							pUnit->SetWordType(wtLSmallWord, wmLSequences);
						}
						else
						{
							pUnit->SetWordType(wtSmallWord, wmLSequences);
						}
					}

					FillWordIndexes(pUnit);
					//CheckNominalOnly(pUnit);
				}
				else
				{
					// these are not really small words, only used to detect abbreviations
					SetLWord(pUnit, szLexem);
					DetectSTerminated(pUnit, szLexem);
					FillWordIndexes(pUnit);
					//CheckNominalOnly(pUnit);
					IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex();
					if (pIndex != NULL && pIndex->GetDictionaryIndex()->GetFirst().GetLanguage()==SS::Core::Types::ealRussian)
					{
						if (m_pUnitChecker->HasSemantic1(pUnit->GetUnit(),SS::Core::Features::Values::TSemanticType.smtOrganization)||
						    m_pUnitChecker->HasProfession(pUnit->GetUnit())
						   )
						{
							pUnit->SetWordType(wtWord, wmLSequences);
						}
						if (pIndex != NULL && 
							m_pUnitChecker->HasSemantic1(pUnit->GetUnit(),SS::Core::Features::Values::TSemanticType.smtPlace) &&
						    !m_pUnitChecker->HasSemantic1(pUnit->GetUnit(),SS::Core::Features::Values::TSemanticType.smtPerson) &&
						    pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase()
						   )
						{
							pUnit->SetWordType(wtGeoSemantic, wmLSequences);
						}
					}

					if (_tcsicmp(szLexem, _T("делам")) == 0)
					{						
						pUnit->SetWordType(wtNounSeqRoot, wmInSequences);			
					}
				}

				if (_tcsicmp(szLexem, _T("Тел.")) == 0 || _tcsicmp(szLexem, _T("тел.")) == 0 
					|| _tcsicmp(szLexem, _T("Тел")) == 0 || _tcsicmp(szLexem, _T("тел")) == 0 
					|| _tcsicmp(szLexem, _T("Т.")) == 0 || _tcsicmp(szLexem, _T("т.")) == 0 
					|| _tcsicmp(szLexem, _T("Т")) == 0 || _tcsicmp(szLexem, _T("т")) == 0
					|| _tcsicmp(szLexem, _T("моб.")) == 0 || _tcsicmp(szLexem, _T("моб")) == 0
					|| _tcsicmp(szLexem, _T("сот.")) == 0 || _tcsicmp(szLexem, _T("сот")) == 0) 
				{
					pUnit->SetWordType(wtTelephoneWord,wmTelephones);
				}

				if (_tcsicmp(szLexem, _T("доб.")) == 0 || _tcsicmp(szLexem, _T("доб")) == 0 )
				{
					pUnit->SetWordType(wtTelephoneAdditionWord, wmTelephones);
				}

				break;

			case accEngWord:
				SetLWord(pUnit, szLexem);
				DetectSTerminated(pUnit, szLexem);
				FillWordIndexes(pUnit);
				break;

			case accPoint:
				pUnit->SetWordType(wtWord);
				pUnit->SetWordType(wtPoint, wmReparsedItems);
				pUnit->SetWordType(wtPoint, wmLSequences);
				pUnit->SetWordType(wtPoint, wmTelephones);
				pUnit->SetWordType(wtNonSyntax, wmNonSyntax);
				break;

			case accSentBreak:
				pUnit->SetWordType(wtWord);
				pUnit->SetWordType(wtNonSyntax, wmNonSyntax);
				break;
			
			case accNumber:
				SetNumber(pUnit, szLexem, false);
				pUnit->SetWordType(wtNumber);
				break;

			case accOrdinalNumber:
				pUnit->SetWordType(wtOrdinalNumber);
				SetNumber(pUnit, szLexem, true);
				break;

			case accInitial:
				pUnit->SetWordType(wtInitial);
				break;

			case accComma:
				pUnit->SetWordType(wtComma);
				pUnit->SetWordType(wtComma, wmInSequences);
				break;

			case accApostroph:
				pUnit->SetWordType(wtApostroph);
				pUnit->SetWordType(wtQuote, wmQuotations);
				break;

			case accAmpersand:
				pUnit->SetWordType(wtAmpersand);
				break;

			case accDefis:
				pUnit->SetWordType(wtDefis);
				break;

			case accTire:
				pUnit->SetWordType(wtTire);
				break;

			case accRightBracket:
				pUnit->SetWordType(wtRightBracket);
				pUnit->SetWordType(wtRightBracket, wmReparsedItems);
				break;

			case accLeftBracket:
				pUnit->SetWordType(wtLeftBracket);
				pUnit->SetWordType(wtLeftBracket, wmReparsedItems);
				pUnit->SetWordType(wtLeftBracket, wmReparsedItems);
				break;

			case accLQuote:
				if (IsQuote(szLexem))
				{
					pUnit->SetWordType(wtQuote);
					pUnit->SetWordType(wtQuote, wmQuotations);
				}
				else if (IsRQuote(szLexem))
				{
					pUnit->SetWordType(wtRQuote);
					pUnit->SetWordType(wtRQuote, wmQuotations);
				}
				else
				{
					pUnit->SetWordType(wtLQuote);
					pUnit->SetWordType(wtLQuote, wmQuotations);
				}
				break;
			case accRQuote:
				pUnit->SetWordType(wtRQuote);
				pUnit->SetWordType(wtRQuote, wmQuotations);
				break;

			case accSlash:
				pUnit->SetWordType(wtSlash);
				break;

			case accColon:
				pUnit->SetWordType(wtColon);
				break;

			case accURLPart:
				SetLWord(pUnit, szLexem); 
				pUnit->SetWordType(wtSmallWord, wmDates);
				pUnit->SetWordType(wtURLPart, wmReparsedItems);
				FillWordIndexes(pUnit);
				break;

			case accURL:
				SetLWord(pUnit, szLexem); 
				pUnit->SetWordType(wtURL, wmReparsedItems);
				break;

			case accAt:
				pUnit->SetWordType(wtAt, wmReparsedItems);
				pUnit->SetWordType(wtWord);
				break;

			case accSign:
				pUnit->SetWordType(wtWord);
				if(_tcsicmp(pUnit->GetUnit()->GetWord(), _T("/"))==0)
					pUnit->SetWordType(wtSlash);
				if(_tcsicmp(pUnit->GetUnit()->GetWord(), _T("+"))==0)
					pUnit->SetWordType(wtPlusSign,wmTelephones);
				break;

			default:
			{
				//szLexem, ELexemType lt, UINT uiCurrPos, UINT uiSymbolIndex;
				//---ATL::CAtlString text = szLexem;
				//---int length = 256 + text.GetLength();
				int length = 256 + _tcslen( szLexem );
				wchar_t *pMessage = new wchar_t[ length ];
				memset(pMessage, 0, length * sizeof(wchar_t));
				swprintf(pMessage, L"Неправильный тип лексемы '%d', лексема '%s', WordPosition = '%d', SymbolIndex = '%d'",
								   lt, szLexem, uiCurrPos, uiSymbolIndex);
				SAVE_LOG(SS_ERROR AND pMessage AND __WFUNCTION__ );
				delete [] pMessage;
			}
		}
	}
	SS_CATCH(L"");
}

void CIRLoader::FillWordIndexes(IR::CUnit* pUnit)
{	
	int qq = sizeof(IR::CUnit);

	m_pIndexLoader->FillSemanticInfo(*pUnit);
	m_pIndexLoader->FillSemanticInfoBySuffix(*pUnit);

	ResolveNameAmbiguity(pUnit);
	/*if (pUnit->HasIndexes(ednUnknownRus) || pUnit->HasIndexes(ednUnknownEng))
	{
		m_pUnitChecker->AddServiceTypeIndex(*pUnit, Values::TSServiceTypes.sstWine);
	}*/
	//m_pIndexLoader->FillSemanticVERB(*pUnit);
	m_pUnitChecker->AddAdjectiveIndexEvrisctic(pUnit->GetUnit());
	m_pUnitChecker->AddRusLastNameIndexEvrisctic(pUnit->GetUnit());	m_pUnitChecker->FillMorphoTypes(pUnit);
	m_pUnitChecker->FillSemanticTypes(pUnit);
}

void CIRLoader::ResolveNameAmbiguity(IR::CUnit* pUnit)
{
	if (m_pSent->IsFirstUnit(pUnit) || m_bQuery)
	{
		return;
	}

	EWordType wt = pUnit->GetType(wmDefault);
	switch ( wt )
	{
		case wtLWord:
			if (pUnit->HasIndexes(ednRProperName))
				pUnit->RemoveIndexes(ednRNominalName);
			else if (pUnit->HasIndexes(ednEProperName))
				pUnit->RemoveIndexes(ednENominalName);
			break;

		case wtWord:
			if (pUnit->HasIndexes(ednRNominalName))
				pUnit->RemoveIndexes(ednRProperName);
			else if (pUnit->HasIndexes(ednENominalName))
				pUnit->RemoveIndexes(ednEProperName);
			break;
	}
}

bool CIRLoader::HandleAbbrevGroup(int iOffset, int iLength)
{
	SS_TRY;

	for (int iToLeft = yyleng + iOffset; iToLeft > iOffset; )
	{
		bool isFindAbbrev = FindAbbreviation(iOffset, iToLeft);
		bool isLastPoint = (m_pLexems->GetLexemType(iToLeft + 1) == accPoint);
		if (m_pLexems->GetLexemType(iToLeft + 1) == accPoint)
		{
			if (iOffset != iToLeft)
			{
				yyless(iLength + iOffset - iToLeft);
				assert(iToLeft - iOffset >= 0);
				m_nProcessed -= iToLeft - iOffset;
			}
			return true;
		}
	}

	SS_CATCH(L"");
	return false;
}

bool CIRLoader::FindAbbreviation(int iOffset, int& iToLeft)
{
	SS_TRY;

	for (int iLen = iToLeft - iOffset; iLen > 1; iLen--)
	{
		ATL::CAtlString& sz = m_pLexems->GetString(iLen, iToLeft);
		if (m_pIndexLoader->FillMorphoInfo(NULL, sz, true))
		{
			IR::CUnit* pUnit = m_pUnitStorage->Allocate();
			FillUnit(pUnit, sz, accWord, UINT_MAX, UINT_MAX);

			AddToSentence(iToLeft - iLen, iLen);
			m_pSent->GetLastUnit(); // this sets CSentence's internal pointer
			if(m_pSent->CheckRangeWordType(iLen, 0, wmDefault, wtInitial) && iLen <= 2)
			{
				pUnit->SetWordType(wtInitial, wmNames);
			}
			m_pSent->JoinLastUnits(pUnit, iLen);

			iToLeft -= iLen;
			return true;
		}
	}

	iToLeft--;
	AddToSentence(iToLeft, 1);

	SS_CATCH(L"");
	return false;
}

bool CIRLoader::WrongDefisCorrection(wstring &word)
{
	if (word == L"-")
	{
		return false;
	}
	vector<wstring> parts;
	int oldPos = 0;
	for (int pos = word.find(L'-'); pos != wstring::npos; pos = word.find(L'-', pos + 1))
	{
		parts.push_back(word.substr(oldPos, pos - oldPos));
		oldPos = pos + 1;
	}
	parts.push_back(word.substr(oldPos, word.length() - oldPos));

	if (parts.size() == 1) /// нет дефиса
	{
		return false;
	}

	unsigned int cycle = 0;
	while (cycle < parts.size())
	{
		wstring form = L"";
		for (unsigned int i = 0; i < parts.size(); ++i)
		{
			if ((cycle != 0) && (i == cycle))
				form += L"-";
			form += parts[i];
		}
		if (!form.empty() && !m_pIndexLoader->IsRomanNumber(form) && m_pIndexLoader->FillMorphoInfo(NULL, form.c_str(), true))
		/// проверка формы на наличие в словаре
		{
			word = form;
			return true;
		}
		++cycle;
	}
	return false;
}

void CIRLoader::HandleBrokenWord(int iOffset)
{
	SS_TRY;

	ATL::CAtlStringW text = m_pLexems->GetLexemString(iOffset + yyleng);
	text.Append(_T("-"));
	text.Append(m_pLexems->GetLexemString(iOffset + 1));
	wstring word = text;
	if (WrongDefisCorrection(word))
	{
		IR::CUnit* pUnit = m_pSent->AddUnit();
		FillUnit(pUnit, word.c_str(), accWord, m_pLexems->GetLexemPosition(iOffset + yyleng), 
			m_pLexems->GetLexemSymbolPosition(iOffset + yyleng));
	}
	else
	{
		// add first part of broken word, and reparse the rest
		AddToSentence(iOffset + yyleng);
		m_nProcessed -= yyleng - 1;
		yyless(1);
	}

	SS_CATCH(L"");
}

int CIRLoader::AddToSentence(int iOffset)
{
	SS_TRY;

	IR::CUnit* pIRUnit = NULL;
	LPCTSTR szLexem = NULL;
	ELexemType lt = accUndefined;
	UINT uiPosition = -1;
	UINT uiSymbolPosition = -1;
	IUnit* pUnit = NULL;
	m_pLexems->GetData(iOffset, &pUnit, &szLexem, &lt, &uiPosition, &uiSymbolPosition);
	wstring word = szLexem;
	WrongDefisCorrection(word);
	pIRUnit = m_pSent->AddUnit();
	pIRUnit->SetUnit(pUnit);
	FillUnit(pIRUnit, word.c_str(), lt, uiPosition, uiSymbolPosition);
	return word.length();

	SS_CATCH(L"");
}

void CIRLoader::AddToSentence(int iOffset, int nCount)
{
	SS_TRY;

	for (int i = nCount + iOffset; i > iOffset; i--)
	{
		AddToSentence(i);
	}
	SS_CATCH(L"");
}

void CIRLoader::SetLWord(IR::CUnit* pUnit, LPCTSTR sz)
{
	SS_TRY;

	EWordType wt = _istupper(sz[0]) ? wtLWord : wtWord;

	pUnit->SetWordType(wt);

	if (m_pSent->IsFirstUnit(pUnit))
	{
		/*if(_istlower(sz[0]))
		{
			wstring str(sz);
			str[0] = _totupper(sz[0]);
			pUnit->GetUnit()->SetWord(str.c_str());
		}*/

		pUnit->SetWordType(wtWord, wmLSequences);		
		pUnit->SetWordType(wtWord, wmOrganizations);
	}
	else
	{	
		//if (!m_pUnitChecker->HasProfession(pUnit->GetUnit()))
		//{
		pUnit->SetWordType(wt, wmLSequences);		
		//}
		pUnit->SetWordType(wt, wmOrganizations);
	}

	SS_CATCH(L"");
}

void CIRLoader::SetNumberType(IR::CUnit* pUnit, EWordType type)
{
	pUnit->SetWordType(type, wmTelephones);
	pUnit->SetWordType(type, wmReparsedItems);
	pUnit->SetWordType(type, wmLSequences);
}

inline EWordType CIRLoader::GetNumberType(int len)
{
	switch (len)
	{
	case 1:
		return wtOneDigit;

	case 2:
		return wtTwoDigits;

	case 3:
		return wtThreeDigits;

	case 4:
		return wtFourDigits;

	case 5:
		return wtFiveDigits;

	case 6:
		return wtSixDigits;

	case 7:
		return wtSevenDigits;

	case 8:
		return wtEightDigits;

	case 9:
		return wtNineDigits;

	case 10:
		return wtTenDigits;

	case 11:
		return wtElevenDigits;

	default:
		return wtNumber;
	}
}

void CIRLoader::SetNumber(IR::CUnit* pUnit, LPCTSTR sz, bool bOrdinal)
{
	SS_TRY;
	
	int len = _tcslen(sz);		
	if (!bOrdinal) 
	{
		// for joining literal numbers
		pUnit->SetTag(m_pIndexLoader->GetNumericValue(*pUnit));

		// for joining complex numbers
		EWordType numberType = GetNumberType(len);
		SetNumberType(pUnit, numberType);
	}
	else
	{
		//получить длину числа без учета окончания
		len = m_pIndexLoader->GetCharBeforeNumber(sz);
	}

	if (len <= 4)
	{
		int i = _ttoi(sz);
		// for dates detecting
		if (i >= 1 && i <= 31)
			pUnit->SetWordType(wtNumberDay, wmDates);
		else if (i > MIN_YEAR_NUMBER && i < MAX_YEAR_NUMBER)
			pUnit->SetWordType(wtNumberYear, wmDates);
		else
			pUnit->SetWordType(wtNumber, wmDates);
	}
	SS_CATCH(L"");
}

bool CIRLoader::PredefinedBeginEndDateSmallWord(IR::CUnit* pUnit, LPCTSTR szLexem)
{
	if (_tcsicmp(szLexem, _T("с"))==0 ||
		_tcsicmp(szLexem, _T("С"))==0 ||
		_tcsicmp(szLexem, _T("по"))==0 ||
		_tcsicmp(szLexem, _T("По"))==0 ||
		_tcsicmp(szLexem, _T("from"))==0 ||
		_tcsicmp(szLexem, _T("to"))==0 ||
		_tcsicmp(szLexem, _T("до"))==0 ||
		_tcsicmp(szLexem, _T("До"))==0
	   )
	{
		pUnit->SetWordType(wtDatePreposition, wmDates);
		pUnit->SetWordType(wtDatePreposition, wmReparsedItems);
		return true;
	}
	return false;

}

bool CIRLoader::DetectPredefinedSmallword(IR::CUnit* pUnit, LPCTSTR szLexem)
{
	SS_TRY;
	
	if (m_pIndexLoader->IsOrdinalParticle(szLexem))
	{
		pUnit->SetWordType(wtOrdinalParticle, wmReparsedItems);
		pUnit->SetWordType(wtOrdinalParticle, wmDates);
	}
	else if (_tcsicmp(szLexem, _T("д")) == 0 ||
			 _tcsicmp(szLexem, _T("фон")) == 0 ||
			 _tcsicmp(szLexem, _T("ди")) == 0 ||
			 _tcsicmp(szLexem, _T("ван")) == 0 ||
			 _tcsicmp(szLexem, _T("да")) == 0 ||
			 _tcsicmp(szLexem, _T("де")) == 0 ||
			 _tcsicmp(szLexem, _T("von")) == 0 ||
			 _tcsicmp(szLexem, _T("di")) == 0 ||
			 _tcsicmp(szLexem, _T("van")) == 0 ||
			 _tcsicmp(szLexem, _T("da")) == 0 ||
			 _tcsicmp(szLexem, _T("de")) == 0 ||
			 _tcsicmp(szLexem, _T("d")) == 0
		    )
	{
		pUnit->SetWordType(wtNameParticle, wmNames);
		pUnit->SetWordType(wtNameParticle, wmMultiNames);
	}
	else if (_tcsicmp(szLexem, _T("s")) == 0)
	{
		pUnit->SetWordType(wtSmallWord, wmDates);
		pUnit->SetWordType(wtLetterS, wmReparsedItems);
		pUnit->SetWordType(wtLetterS, wmDates);
	}
	else if (_tcsicmp(szLexem, _T("to")) == 0)
	{
		pUnit->SetWordType(wtTo, wmDates);
	}
	else if (_tcsicmp(szLexem, _T("t")) == 0)
	{
		pUnit->SetWordType(wtSmallWord, wmDates);
		pUnit->SetWordType(wtLetterT, wmReparsedItems);
	}
	else if (_tcsicmp(szLexem, _T("of")) == 0)
	{
		pUnit->SetWordType(wtGenConj, wmDates);
		pUnit->SetWordType(wtGenConj, wmNames);
		pUnit->SetWordType(wtGenConj, wmOrganizations);
	}
	else if (_tcsicmp(szLexem, _T("и")) == 0)
	{
		pUnit->SetWordType(wtGenConj, wmInSequences);
	}
	else if (_tcsicmp(szLexem, _T("по")) == 0)
	{		
		pUnit->SetWordType(wtNounSeqRoot, wmInSequences);			
	}
	else
	{
		// predefined small word not found
		return false;
	}

	SS_CATCH(L"");
	return true;
}

void CIRLoader::DetectSTerminated(IR::CUnit* pUnit, LPCTSTR szLexem)
{
	SS_TRY;

	TCHAR tcLastSymbol = szLexem[_tcslen(szLexem) - 1];
	if(_tcschr(_T("SsXx"), tcLastSymbol))
		pUnit->SetWordType(wtSTerminated, wmReparsedItems);
	else
		pUnit->SetWordType(wtEngWord, wmReparsedItems);

	SS_CATCH(L"");
}

void CIRLoader::DetectQueryWord(IR::CUnit* pUnit)
{
	SS_TRY;

	if (!m_bQuery)
	{
		return;
	}

	LPCWSTR szLexem = pUnit->GetUnit()->GetWord();
	if (m_pSent->IsFirstUnit(pUnit) &&
		(_tcsicmp(szLexem, L"list") == 0 || _tcsicmp(szLexem, L"name") == 0)
	   )
	{
		pUnit->SetWordType(wtQListWord, wmQuery);
	}
	else if (_tcsicmp(szLexem, L"the") == 0)
	{
		pUnit->SetWordType(wtQThe, wmQuery);
	}
	else if (_tcsicmp(szLexem, L"names") == 0)
	{
		pUnit->SetWordType(wtQNames, wmQuery);
	}
	else if (_tcsicmp(szLexem, L"of") == 0)
	{
		pUnit->SetWordType(wtQOf, wmQuery);
	}
	else if (_tcsicmp(szLexem, L"what") == 0)
	{
		pUnit->SetWordType(wtQWhat, wmQuery);
	}
	else if (_tcsicmp(szLexem, L"are") == 0)
	{
		pUnit->SetWordType(wtQAre, wmQuery);
	}

	SS_CATCH(L"");
}

}
}
