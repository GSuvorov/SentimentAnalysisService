#include "stdafx.h"
#include "ConfigReader.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CConfigReader::CConfigReader() :
		m_hFile(INVALID_HANDLE_VALUE),
		m_pBreaker(NULL),
		m_pTable(NULL),
		m_bTranslit(true),
		m_bFirst(true),
		m_bError(false)
{
	ZeroMemory(&m_oLoc, sizeof(YYLTYPE));
}

CConfigReader::~CConfigReader()
{
	Close();
	delete m_pBreaker;
}

/////////////////////////////////////////////////////////////////////////////////////
// CConfigReader interface

bool CConfigReader::FillTable(LPCWSTR szFile, CTransitionTable* pTable, bool bTranslit)
{
	SS_TRY;

	if(!OpenConfigFile(szFile))
		return false;

	m_bTranslit = bTranslit;
	m_bError = false;
	m_bFirst = true;
	m_pTable = pTable;

	if(!m_pBreaker)
		m_pBreaker = new CWordBreaker(bTranslit ? pmTranslitConfig : pmOrthoConfig);

	int iResult = bTranslit ? ParseTranslitConfig(this) : ParseOrthoConfig(this);
	if(iResult != 0)
	{
		wchar_t szMessage[256] = { 0 };
		swprintf(szMessage, L"Ст: %d, Кол: %d - синтаксическая ошибка", m_oLoc.first_line, m_oLoc.first_column);
		SAVE_LOG(szMessage);
		return false;
	}

	SS_CATCH(L"");
	return !m_bError;
}

/////////////////////////////////////////////////////////////////////////////////////
// Functions called by bison

int CConfigReader::GetNextLexem(YYSTYPE* pszValue, YYLTYPE* pLoc)
{
	ELexemType lt = ltNone;
	if(m_bFirst)
	{
		lt = m_pBreaker->GetFirstLexem(m_strFile.c_str());
		m_bFirst = false;
	}
	else
		lt = m_pBreaker->GetNextLexem();

	pLoc->first_line = pLoc->last_line = m_pBreaker->GetLineNum();
	pLoc->first_column = m_pBreaker->GetCharPos();
	pLoc->last_column = pLoc->first_column + m_pBreaker->GetCurrLexemLength();
	m_oLoc = *pLoc;

	*pszValue = m_pBreaker->GetCurrLexemString();
	return GetLexem(lt);
}

void CConfigReader::AddRus(LPCWSTR szRus, int iWeight)
{
	SWeightString oRus = { iWeight, szRus };
	m_lRus.push_back(oRus);
}

void CConfigReader::AddLat(LPCWSTR szLat)
{
	for(TRusList::iterator it = m_lRus.begin(); it != m_lRus.end(); ++it)
		m_bError = m_bError || !m_pTable->AddLat(szLat, it->wstr.c_str(), it->iWeight);
	m_lRus.clear();
}

/////////////////////////////////////////////////////////////////////////////////////
// CConfigReader functions

bool CConfigReader::OpenConfigFile(LPCWSTR szFile)
{
	SS_TRY;

	Close();
	m_hFile = CreateFileW(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		SAVE_LOG(L"Невозможно открыть файл с таблицей транслитерации");
		return false;
	}

	DWORD dwSize = GetFileSize(m_hFile, NULL);
	if(dwSize > 10000)
	{
		SAVE_LOG(L"Размер файла транслитерации не может превышать 10000 байт");
		return false;
	}

	LPWSTR szString = (LPWSTR) alloca(dwSize + 2);
	if(!ReadFile(m_hFile, (void*) szString, dwSize, &dwSize, NULL))
	{
		SAVE_LOG(L"Невозможно считать данные из файла транслитерации");
		return false;
	}

	if(*szString != 0xfeff)
	{
		SAVE_LOG(L"Файл транслитерации должен быть в юникоде");
		return false;
	}

	szString[dwSize / sizeof(wchar_t)] = 0;
	m_strFile = ++szString;

	SS_CATCH(L"");
	return true;
}

void CConfigReader::Close()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

int CConfigReader::GetLexem(ELexemType eType)
{
	if(m_bTranslit)
	{
		switch(eType)
		{
			case ltLatNum:
				return t_lat;
			case ltRus:
				return t_rus;
			case ltWeight:
				return t_weight;
			case ltBreaker:
				return t_breaker;
			case ltNone:
				return 0;
			default:
				return t_unknown;
		}
	}
	else
	{
		switch(eType)
		{
			case ltRus:
				return (m_pBreaker->GetCharPos() - m_pBreaker->GetCurrLexemLength() == 1) ? o_startrus : o_rus;
			case ltWeight:
				return o_weight;
			case ltBreaker:
				return o_breaker;
			case ltNone:
				return 0;
			default:
				return o_unknown;
		}
	}
}
