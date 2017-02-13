#pragma once


namespace SS  
{
namespace Dictionary  
{
namespace NumberCombinatory
{

/////////////////////////////////////////////////////////////////////////////////////
// Global Bison definitions

#define YYSTYPE unsigned __int64

struct SLocation
{
	int iFirst;
	int iLast;
};

#define YYLTYPE SLocation
#define YYLLOC_DEFAULT(Current, Rhs, N) \
	Current.iLast = Rhs[N].iLast;

#include "NumberGrammar.tab.h"

/////////////////////////////////////////////////////////////////////////////////////
// CNumericParser

class CNumericParser
{
public:
	CNumericParser();
	~CNumericParser();

public:
	void Parse(void* pParam, int iMaxSize);

	int GetNextLexem(YYSTYPE* pType, SLocation* pLoc);
	void SetResult(YYSTYPE Res, SLocation* pLoc);

protected:
	virtual int GetCurrentLexem(int iPosition, YYSTYPE* pValue) = 0;
	virtual void AddToOutput(void* pParam, int iFirst, int iLast, YYSTYPE Result) = 0;

private:
	/// current result
	YYSTYPE m_Res;
	/// current result location
	SLocation m_oLoc;
	/// current lexem number
	int m_iCur;

	bool m_bUnknown;
};

}
}
}
