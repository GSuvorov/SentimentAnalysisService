#include "stdafx.h"
#include "NumericParser.h"

#include "..\..\ASSInterface\Constants.h"
using namespace SS::Constants;


namespace SS  
{
namespace Dictionary  
{
namespace NumberCombinatory
{

/////////////////////////////////////////////////////////////////////////////////////
// Global Bison definitions

#define YYPARSE_PARAM parm
#define YYLEX_PARAM parm


void yyerror(char* sz)
{
}

int yylex(YYSTYPE* pValue, SLocation* pLoc, void* pParam)
{
	CNumericParser* pParser = (CNumericParser*) pParam;
	return pParser->GetNextLexem(pValue, pLoc);
}

YYSTYPE SetResult(YYSTYPE iRes, void* pParam, SLocation& oLoc)
{
	CNumericParser* pParser = (CNumericParser*) pParam;
	pParser->SetResult(iRes, &oLoc);
	return iRes;
}


#ifdef _DEBUG
	#define YYERROR_VERBOSE
#endif
#include "NumberGrammar.tab.c"

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CNumericParser::CNumericParser() : m_Res(0), m_iCur(0), m_bUnknown(false)
{
	memset(&m_oLoc, 0, sizeof(SLocation));
}

CNumericParser::~CNumericParser()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CNumericParser functions

void CNumericParser::Parse(void* pParam, int iMaxSize)
{
	// init internal variables
	m_iCur = 0;
	m_oLoc.iFirst = 0;
	m_oLoc.iLast = -1;

	// parse
	int iAux = m_iCur;
	while(m_iCur < iMaxSize)
	{
		if(yyparse(this) == 0)
		{
			AddToOutput(pParam, m_oLoc.iFirst, m_oLoc.iLast, m_Res);
			continue;
		}

		// if we have already parsed starting with this position, don't reparse it
		if(iAux == m_oLoc.iLast + 1)
			continue;

		// reparse starting with the lexem, following last accepted input
		m_iCur = m_oLoc.iLast + 1;
		AddToOutput(pParam, m_oLoc.iFirst, m_oLoc.iLast, m_Res);
		iAux = m_iCur;
	}
}

int CNumericParser::GetNextLexem(YYSTYPE* pValue, SLocation* pLoc)
{
	pLoc->iFirst = m_iCur;
	pLoc->iLast = m_iCur;

	return GetCurrentLexem(m_iCur++, pValue);
}

void CNumericParser::SetResult(YYSTYPE Res, SLocation* pLoc)
{
	m_Res = Res;
	m_oLoc = *pLoc;
}

}
}
}
