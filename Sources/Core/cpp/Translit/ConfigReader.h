#pragma once

#include <string>
#include <list>
using namespace std;

#include "WordBreaker.h"
#include "TransitionTable.h"

#include "TranslitConfig.h"
#include "OrthoConfig.h"


class CConfigReader
{
public:
	CConfigReader();
	~CConfigReader();

public:
	bool FillTable(LPCWSTR szFile, CTransitionTable* pTable, bool bTranslit);

public:
	int GetNextLexem(YYSTYPE* pszValue, YYLTYPE* pLoc);
	void AddRus(LPCWSTR szRus, int iWeight);
	void AddLat(LPCWSTR szLat);

private:
	bool OpenConfigFile(LPCWSTR szFile);
	void Close();

	int GetLexem(ELexemType eType);

private:
	HANDLE m_hFile;
	wstring m_strFile;
	CWordBreaker* m_pBreaker;
	CTransitionTable* m_pTable;
	bool m_bTranslit;

	bool m_bFirst;
	bool m_bError;

	struct SWeightString
	{
		int iWeight;
		wstring wstr;
	};

	YYLTYPE m_oLoc;

	typedef list<SWeightString> TRusList;
	TRusList m_lRus;
};
