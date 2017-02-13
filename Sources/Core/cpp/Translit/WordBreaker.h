#pragma once

/////////////////////////////////////////////////////////////////////////////////////
// Flex definitions

#define YY_CHAR_DEFINED
typedef wchar_t YY_CHAR;

#undef yyFlexLexer
#define yyFlexLexer WordBreakerFlexLexer

#include <iostream>
using namespace std;
#include "FlexLexerUnicode.h"


/////////////////////////////////////////////////////////////////////////////////////
// CLexer definitions

enum ELexemType
{
	ltNone,
	ltWord,
	ltNoWord,

	ltLatNum,
	ltRus,
	ltWeight,
	ltBreaker,
};

/////////////////////////////////////////////////////////////////////////////////////
// Parsing types

enum EParseMode
{
	pmTranslit,
	pmTranslitConfig,
	pmOrtho,
	pmOrthoConfig,
};

/////////////////////////////////////////////////////////////////////////////////////
// CLexer

class CWordBreaker : public yyFlexLexer
{
public:
	CWordBreaker(EParseMode eMode);
	virtual ~CWordBreaker();

public:
	ELexemType GetFirstLexem(LPCWSTR szText);
	ELexemType GetNextLexem();
	LPCWSTR GetCurrLexemString();
	int GetCurrLexemLength();
	int GetCharPos();
	int GetLineNum();

protected:
	virtual int LexerInput(YY_CHAR* buf, int max_size);

private:
	LPCWSTR m_szInput;
	int m_iMode;
};
