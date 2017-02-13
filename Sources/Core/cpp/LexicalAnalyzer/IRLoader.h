#pragma once

#include "InternalRepresentation.h"
#include "LexemeBoard.h"

#include "..\ASCInterface\defines.h"
#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASSInterface\ICommon.h"
using namespace SS::Interface::Core::Common;
using namespace SS::Interface::Core::MainAnalyse;

#include "Unit.h"
#include "UnitChecker.h"

#include "IReparser.h"
#include "LexemCollection.h"
#include <functional>

namespace SS
{
namespace LexicalAnalyzer
{

#undef yyFlexLexer
#define yyFlexLexer IRLoaderFlexLexer
#include "FlexLexer.h"

class CIRLoader : 
		public yyFlexLexer,
		public CAnalyseParams
{
public:
	CIRLoader();
	~CIRLoader();

	enum EBreakReason
	{
		brBoardEnd,
		brSentBreak,
		brParaBreak,
		brMaxPos,
		brTitle,
	};

// CAnalyseParams
public:
	virtual void SetAnalyseParams(IAnalyseParams* pAnalyseParams);

public:
	/// initialization, must be made only once
	void Init(IBlackBoxTextStorage* pStorage, CIndexLoader* pIndexLoader, ILoadManager* pLoadManager, 
		CUnitChecker* pUnitChecker, Reparser::IReparser* pReparser, IR::TUnitStorage* pUnitStorage);
	/// resets loader's internal state, should be called in ILexica::SetText()
	void Reset();
	/// if true, ignores sentence and paragraph breakers
	void SetIgnoreBreakers(bool bSet);
	/// fills pSent and moves pBoard to the beginning of the next sentence
	EBreakReason Load(IR::CSentence* pSent, CLexemCollection* pLexems, wstring *pFullSentence);
	EBreakReason LoadSentence(IR::CSentence* pSent, CLexemCollection* pLexems);
	/// returns true if IRLoader doesn't contain any backup information
	bool IsFinished();

	int GetHeaderLevel();

private:
	int UpdateShift(const wstring *pSentence, int offset, int count, int &shift);
	bool IsStringContainSymbol(const wstring &sentence, int start, int end, std::function<bool(const wchar_t)> predicate);
	bool IsNeedAddSign(int nToLeft);
	int MissSpaces(const wstring &sentencee, int startShift);

	virtual int LexerInput(char* buf, int max_size);

	void FillUnit(IR::CUnit* pUnit, LPCTSTR szLexem, ELexemType lt, UINT uiCurrPos, UINT uiSymbolIndex);
	void FillWordIndexes(IR::CUnit* pUnit);
	void ResolveNameAmbiguity(IR::CUnit* pUnit);

	bool HandleAbbrevGroup(int iOffset, int iLength);
	bool FindAbbreviation(int iOffset, int& iToLeft);
	void HandleBrokenWord(int iOffset);

	int AddToSentence(int iOffset);
	void AddToSentence(int iOffset, int nCount);

	void SetLWord(IR::CUnit* pUnit, LPCTSTR sz);
	void SetNumber(IR::CUnit* pUnit, LPCTSTR sz, bool bOrdinal);
	EWordType GetNumberType(int len);
	void SetNumberType(IR::CUnit* pUnit, EWordType type);

	bool DetectPredefinedSmallword(IR::CUnit* pUnit, LPCTSTR szLexem);
	void DetectSTerminated(IR::CUnit* pUnit, LPCTSTR szLexem);
	void DetectQueryWord(IR::CUnit* pUnit);

	bool PredefinedBeginEndDateSmallWord(IR::CUnit* pUnit, LPCTSTR szLexem);

	bool WrongDefisCorrection(wstring &word);
private:
	/// object for creating black box items
	IBlackBoxTextStorage* m_pStorage;
	IR::TUnitStorage* m_pUnitStorage;
	/// object for filling unit indexes
	CIndexLoader* m_pIndexLoader;
	/// object for detecting or filling semantic categories
	CUnitChecker* m_pUnitChecker;
	/// if true, ignores sentence and paragraph breakers
	bool m_bIgnoreBreakers;
	bool m_bTitleMode;
	/// absolute number of current LexemBoard item
	UINT m_uiCurrentPosition;

	CLexemCollection* m_pLexems;
	IR::CSentence* m_pSent;

	/// count of units, processed by flex
	int m_nProcessed;
	/// count of units, pushed to flex
	int m_nInserted;

	bool m_bSentFinished;

	Reparser::IReparser* m_pReparser;

	bool m_bQuery;
	bool m_bSearch;
	bool m_bLite;

	int m_iCurrentHeaderLevel;

	int sentCount;
};

}
}
