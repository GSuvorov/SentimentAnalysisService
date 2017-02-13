#include "stdafx.h"
#include "WordBreaker.h"

#include "lex.WordBreaker.cc"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CWordBreaker::CWordBreaker(EParseMode eMode) :
		m_szInput(NULL)
{
	switch(eMode)
	{
		case pmTranslit:
		case pmOrtho:
			m_iMode = MODE_TEXT;
			break;

		case pmTranslitConfig:
		case pmOrthoConfig:
			m_iMode = MODE_CONFIG;
			break;

		default:
			m_iMode = MODE_TEXT;
			break;
	}
}

CWordBreaker::~CWordBreaker()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CWordBreaker interface

ELexemType CWordBreaker::GetFirstLexem(LPCWSTR szText)
{
	m_iCharCount = 1;
	yylineno = 1;
	m_szInput = szText;
	yyrestart(NULL);
	BEGIN(m_iMode);

	ELexemType eType = (ELexemType) yylex();
	m_iCharCount += yyleng;
	return eType;
}

ELexemType CWordBreaker::GetNextLexem()
{
	ELexemType eType = (ELexemType) yylex();
	m_iCharCount += yyleng;
	return eType;
}

LPCWSTR CWordBreaker::GetCurrLexemString()
{
	return yytext;
}

int CWordBreaker::GetCurrLexemLength()
{
	return yyleng;
}

int CWordBreaker::GetCharPos()
{
	return m_iCharCount;
}

int CWordBreaker::GetLineNum()
{
	return yylineno;
}

/////////////////////////////////////////////////////////////////////////////////////
// CWordBreaker functions

int CWordBreaker::LexerInput(YY_CHAR* buf, int max_size)
{
	*buf = *m_szInput;
	if(*m_szInput)
		m_szInput++;
	return 1;
}
