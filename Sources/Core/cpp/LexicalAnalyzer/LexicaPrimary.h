#pragma once

#include "..\ASSInterface\ILexicaLib.h"


using namespace SS::Interface::Core::LexicalAnalyze;
using namespace SS::Interface::Core::ResourceManagers;
using namespace SS::Core::Types::IndexationSearch;
using namespace SS::Interface;

#include "LexemeBoard.h"
#include "IReparser.h"


namespace SS
{
namespace LexicalAnalyzer
{

#define YY_CHAR_DEFINED
#ifdef UNICODE
	typedef wchar_t YY_CHAR;
#else
	typedef char YY_CHAR;
#endif

#undef yyFlexLexer
#define yyFlexLexer LexerFlexLexer
#include "FlexLexerUnicode.h"

class CLexicaPrimary : 
		public SS::Interface::Core::ILexicaPrimary,
		public yyFlexLexer,
		public Reparser::IReparser
{
public:
	CLexicaPrimary();
	virtual ~CLexicaPrimary();

// IBase
public:
	virtual HRESULT QueryInterface(REFIID refIID, void** pBase);
	virtual ULONG Release();

// ILexicaPrimary
public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	virtual void SetText(TNativeTextForIndexation* pNativeText, unsigned int uiStartPosition = 0);
	virtual unsigned int LoadNext(bool bIgnoreParagraphs = false);
	virtual ILexemeBoard* GetLexemeBoard();

private:
	/// removes tabs and spaces from sz in place
	virtual void Normalize(LPTSTR szString, LPCTSTR szSymbolsToRemove);
	bool AddToOutput(Reparser::CResultBoard* pBoard, CString szText);

	void ReparseLongNumber(LPCTSTR szText, int iLen, Reparser::CResultBoard* pBoard);

protected:
	virtual bool Accept(ELexemType lt, LPCTSTR sz, int iLen);
	virtual int LexerInput(YY_CHAR* buf, int max_size);

	void SetAnalysisFinished(bool bFinished);
	bool AnalysisFinished();

// IReparser
protected:
	virtual void Reparse(LPCTSTR szText, Reparser::EReparseMode eMode, Reparser::CResultBoard* pBoard);

protected:
	/// current lexem board
	CLexemeBoard* m_pBoard;

	bool m_bIgnoreParagraphs;

private:
	/// the text to be parsed
	LPCTSTR m_szInput;
	/// current word number
	int m_iWordCount;
	/// storage for reparsed entities
	Reparser::CResultBoard m_oResultBoard;
	bool m_bAnalysisFinished;
};

}
}
