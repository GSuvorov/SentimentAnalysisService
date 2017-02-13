#include "stdafx.h"
#include "TranslitConfig.h"

#include "ConfigReader.h"
#include <atlconv.h>


/////////////////////////////////////////////////////////////////////////////////////
// Bison global functions

#define YYPARSE_PARAM parm
#define YYLEX_PARAM parm

namespace TranslitConfig
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
#include "ConfigReader.tab.c"

}

/////////////////////////////////////////////////////////////////////////////////////
// External functions

int ParseTranslitConfig(void* pParam)
{
	return TranslitConfig::yyparse(pParam);
}
