#include "StdAfx.h"
#include "StringService.h"
#include "tchar.h"



namespace SS
{
	namespace Semantic
	{

CStringService::CStringService(void)
{
}

CStringService::~CStringService(void)
{
}

bool CStringService::StrEqual(const TCHAR* sz1, const TCHAR* sz2)
{
	int i = 0;
	while(sz1[i] == sz2[i]){
		if(sz1[i] == 0) return true;
		i++;
	}
	return false;
}

void CStringService::LowerFirstChar(TCHAR* szWord)
{
	TCHAR chbuf[2];
	TCHAR* szbuf = chbuf;

	szbuf[1] = 0;
	szbuf[0] = szWord[0];
	szbuf = _tcslwr(szbuf);
	szWord[0] = szbuf[0];//опускаем первую букву
}

void CStringService::UpperFirstChar(TCHAR* szWord)
{
	TCHAR chbuf[2];
	TCHAR* szbuf = chbuf;

	szbuf[1] = 0;
	szbuf[0] = szWord[0];
	szbuf = _tcsupr(szbuf);
	szWord[0] = szbuf[0];//опускаем первую букву
}

void CStringService::LowerSz(TCHAR* szWord)
{
	szWord = _tcslwr(szWord);
}

void CStringService::UpperSz(TCHAR* szWord)
{
	szWord = _tcsupr(szWord);
}
	}
}