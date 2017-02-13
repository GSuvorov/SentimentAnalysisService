#include "StdAfx.h"
#include ".\LexicaPrimary.h"

#include <iostream>
#include "DocStructureReader.h"


namespace SS
{
namespace LexicalAnalyzer
{

/*
Copyright (C) 1997, 1998 Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111, USA
*/

#include "lex.Lexer.cc" 
 
/////////////////////////////////////////////////////////////////////////////////////
// Helper class used to initialize istream

#ifdef UNICODE
#	define _tstreambuf wstreambuf
#	define _tistream wistream
#else
#	define _tstreambuf streambuf
#	define _tistream istream
#endif

class CStrBuf : public _tstreambuf
{
public:
	CStrBuf() : sz(NULL), iLen(0) { }
	~CStrBuf()
	{
		if(sz)
			delete sz;
	}

	virtual CStrBuf *setbuf(TCHAR* p, streamsize n)
	{
		sz = new TCHAR[n + 1];
		_tcscpy(sz, p);

		setg(sz, sz, sz + n);
		iLen = n;
		return this;
	}

	LPTSTR sz;
	streamsize iLen;
};

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CLexicaPrimary::CLexicaPrimary() : 
		m_szInput(NULL), 
		m_pBoard(NULL),
		m_bIgnoreParagraphs(false),
		m_iWordCount(0),
		m_bAnalysisFinished(false)
{
}

CLexicaPrimary::~CLexicaPrimary()
{
	if(m_pBoard)
		delete m_pBoard;
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CLexicaPrimary::QueryInterface(REFIID refIID, void** pBase)
{
	if(refIID == IID_LexicaPrimary)
		*pBase = this;
	else if(refIID == IID_Base)
		*pBase = (IBase*) this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CLexicaPrimary::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// ILexicaPrimary interface

void CLexicaPrimary::SetLoadManager(ILoadManager* pLoadManager)
{
	SS_TRY;

	if(GetLoadManager())
		return;

	CBaseCoreClass::SetLoadManager(pLoadManager);
	m_pBoard = new CLexemeBoard;

	SS_CATCH(L"");
}

void CLexicaPrimary::SetText(TNativeTextForIndexation* pNativeText, unsigned int uiStartPosition)
{
	SS_TRY;

#ifdef UNICODE
	m_szInput = (LPTSTR) pNativeText->GetText();
#else
	#error("Must copy string here")
#endif

	if(!m_szInput)
		SS_THROW(L"мскебни рейяр!!!");

	// initialize counters
	if(uiStartPosition == 0)
	{
		m_iWordCount = 0;
		m_iCharCount = 0;
	}
	else
	{
		m_iCharCount = uiStartPosition;
	}

	// return to initial start condition
	BEGIN(INITIAL);
	yyrestart(NULL);

	m_bAnalysisFinished = false;
	m_bIgnoreParagraphs = false;

	SS_CATCH(L"");
}

unsigned int CLexicaPrimary::LoadNext(bool bIgnoreParagraphs)
{
	SS_TRY;

	m_bIgnoreParagraphs = bIgnoreParagraphs;
	if(AnalysisFinished())
		return 0;

	m_pBoard->Reset();

	// if there rested some data in ResultBoard, add it to output
	if(!AddToOutput(&m_oResultBoard, L""))
		return m_iCharCount;

	// parse
    int prevCharCount = 0;
	for(ELexemType lt = (ELexemType) yylex(); lt; lt = (ELexemType) yylex())
	{
		CString textStr = yytext;
        LPTSTR text = 0;
		switch(lt)
		{
			case accLongNumber:
				ReparseLongNumber(yytext, yyleng, &m_oResultBoard);
				m_oResultBoard.GetFirstLexem()->LexemType = accFirstComplexNumberPart;
				break;
			case accNumberWord:
				Reparse(yytext, Reparser::rmNumberWord, &m_oResultBoard);
				break;
            case accInitial:
                if ((m_pBoard->GetCount() > 0) && (m_pBoard->GetLastLexem() == accAmpersand) &&
                    (m_iCharCount - prevCharCount == 0))
                {
                    text = new TCHAR[2];
                    text[1] = 0;
                    text[0] = yytext[0];
                    if(!Accept(accSmallWord, text, 1))
                        return m_iCharCount;
                    text[0] = yytext[1];
                    if(!Accept(accPoint, text, 1))
                        return m_iCharCount;
                    delete[] text;
                }
                else if(!Accept(lt, yytext, yyleng))
					return m_iCharCount;
                continue;
                break;
			default:
				if(!Accept(lt, yytext, yyleng))
					return m_iCharCount;
                prevCharCount = m_iCharCount;
				continue;
		}

		if(!AddToOutput(&m_oResultBoard, textStr))
			return m_iCharCount;
	}

	m_bAnalysisFinished = true;
	return m_iCharCount;

	SS_CATCH(L"");
	return 0;
}

ILexemeBoard* CLexicaPrimary::GetLexemeBoard()
{
	return m_pBoard;
}

/////////////////////////////////////////////////////////////////////////////////////
// CLexicaPrimary functions

void CLexicaPrimary::Normalize(LPTSTR szString, LPCTSTR szSymbolsToRemove)
{
	SS_TRY;

	int iLen = 0;
	for(LPTSTR sz = szString, szNewString = szString; *sz; sz++, szNewString++)
	{
		while(*sz && _tcschr(szSymbolsToRemove, *sz))
			sz++;
		*szNewString = *sz;
		iLen++;
	}
	szString[iLen] = 0;

	SS_CATCH(L"");
}

bool CLexicaPrimary::AddToOutput(Reparser::CResultBoard* pBoard, CString szText)
{
	SS_TRY;

	int index = 0;
	int iCount = 0;
	for(Reparser::SLexem* pLexem = pBoard->GetFirstLexem(); pLexem; pLexem = pBoard->GetNextLexem())
	{
		iCount++;

		int tempIndex = szText.Find(pLexem->szText, index);
		if (tempIndex > index)
		{
			m_iCharCount += tempIndex - index;
			index = tempIndex;
		}
		// return false if there is no room in current lexem board
		if(!Accept(pLexem->LexemType, pLexem->szText, pLexem->iLen))
		{
			pBoard->RemoveFirst(iCount);
			return false;
		}
		index += pLexem->iLen;
	}

	pBoard->RemoveFirst(iCount);
	return true;

	SS_CATCH(L"");
	return false;
}

void CLexicaPrimary::ReparseLongNumber(LPCTSTR szText, int iLen, Reparser::CResultBoard* pBoard)
{
	SS_TRY;

	TCHAR sz[8];

	int iBegin = 0,
		iEnd = (iLen % 3) ? (iLen % 3) : 3;
	for(; iEnd <= iLen; iBegin = iEnd, iEnd += 3)
	{
		_tcsncpy(sz, szText + iBegin, iEnd - iBegin);
		sz[iEnd - iBegin] = 0;
		pBoard->AddLexem(sz, accComplexNumberPart);
	}

	SS_CATCH(L"");
}

bool CLexicaPrimary::Accept(ELexemType lt, LPCTSTR sz, int iLen)
{
	SS_TRY;

	m_iCharCount += iLen;

	// add lexem to board, return false if there is no room in current lexem board
	if(!m_pBoard->AddLexeme(sz, lt, m_iWordCount++, m_iCharCount - iLen))
		return false;

	return true;

	SS_CATCH(L"");
	return false;
}

int CLexicaPrimary::LexerInput(YY_CHAR* buf, int max_size)
{
	SS_TRY;

	*buf = *m_szInput;
	if(*m_szInput)
		m_szInput++;
	return 1;

	SS_CATCH(L"");
	return 0;
}

bool CLexicaPrimary::AnalysisFinished()
{
	return m_bAnalysisFinished;
}

void CLexicaPrimary::SetAnalysisFinished(bool bFinished)
{
	m_bAnalysisFinished = bFinished;
}

void CLexicaPrimary::Reparse(LPCTSTR szText, Reparser::EReparseMode eMode, Reparser::CResultBoard* pBoard)
{
	SS_TRY;

	if(!*szText)
		return;

	// use the same lexer as for whole text parsing, but in another mode
	if(eMode == Reparser::rmRestore)
		yy_push_state(RESTORE);
	else if(eMode == Reparser::rmNumberWord)
		yy_push_state(NUMBERWORD);
	else
	{
		assert(false);
		return;
	}

	// back up values
	LPCTSTR szOldInput = m_szInput;
	yy_buffer_state* pOldState = yy_current_buffer;

	// initialize stream
	CStrBuf oBuff;
	oBuff.pubsetbuf((LPTSTR) szText, (int) _tcslen(szText));
	_tistream oIstream(&oBuff);

	// set new parser state
	m_szInput = oBuff.sz;
	yy_buffer_state* pNewState = yy_create_buffer((istream*) &oIstream, (int) oBuff.iLen);
	yy_switch_to_buffer(pNewState);

	// parse
	for(ELexemType lt = (ELexemType) yylex(); lt; lt = (ELexemType) yylex())
		pBoard->AddLexem(yytext, lt, yyleng);

	// restore old parser state
	yy_delete_buffer(yy_current_buffer);
	yy_switch_to_buffer(pOldState);
	m_szInput = szOldInput;
	yy_pop_state();

	SS_CATCH(L"");
}

}
}
