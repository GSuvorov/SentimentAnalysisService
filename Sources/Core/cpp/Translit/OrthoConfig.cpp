#include "stdafx.h"
#include "OrthoConfig.h"

#include "ConfigReader.h"
#include <atlconv.h>


/////////////////////////////////////////////////////////////////////////////////////
// Bison global functions

#define YYSTYPE wstring
#include "OrthoConfig.tab.h"

#define YYPARSE_PARAM parm
#define YYLEX_PARAM parm

namespace OrthoConfig
{

void yyerror(char* sz)
{
	USES_CONVERSION;
	SAVE_LOG(A2W(sz));
}

int yylex(YYSTYPE* pValue, YYLTYPE* pLoc, void* pParam)
{
	CConfigReader* pReader = (CConfigReader*) pParam;
	return pReader->GetNextLexem(pValue, pLoc);
}

void AddRus(void* pParam, YYSTYPE& oRus)
{
	CConfigReader* pReader = (CConfigReader*) pParam;
	pReader->AddRus(oRus.c_str(), MAX_SYMBOL_WEIGHT);
}

void AddRus(void* pParam, YYSTYPE& oRus, YYSTYPE& oWeight)
{
	CConfigReader* pReader = (CConfigReader*) pParam;
	pReader->AddRus(oRus.c_str(), _wtoi(oWeight.c_str()));
}

void AddLat(void* pParam, YYSTYPE& oLat)
{
	CConfigReader* pReader = (CConfigReader*) pParam;
	pReader->AddLat(oLat.c_str());
}

#define YYERROR_VERBOSE
#include "OrthoConfig.tab.c"

}

/////////////////////////////////////////////////////////////////////////////////////
// External functions

int ParseOrthoConfig(void* pParam)
{
	return OrthoConfig::yyparse(pParam);
}
