#include "StdAfx.h"
#include ".\LexemeBoard.h"
#include ".\LexicaPrimary.h"

#include <atlconv.h>

namespace SS
{
namespace LexicalAnalyzer
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CLexemeBoard::CLexemeBoard(unsigned int uiCapacity) : 
	m_iCurrLexem(-1),
	m_uiTop(0)
{
	m_vLexems.resize(uiCapacity);
	for (unsigned int i = 0; i < uiCapacity; i++)
	{
		m_vLexems[i] = new SLexem();
	}
}

CLexemeBoard::~CLexemeBoard()
{
	for (TLexemVect::iterator it = m_vLexems.begin(); it != m_vLexems.end(); ++it)
	{
		delete *it;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// ILexemeBoard interface

unsigned int CLexemeBoard::GetCapacity()
{
	SS_TRY;

	return (unsigned int) m_vLexems.capacity();

	SS_CATCH(L"");
	return 0;
}

unsigned int CLexemeBoard::GetCount()
{
	SS_TRY;

	return m_uiTop;

	SS_CATCH(L"");
	return 0;
}

unsigned int CLexemeBoard::MoveFirst()
{
	SS_TRY;

	if (m_uiTop == 0)
		return 0;

	m_iCurrLexem = 0;
	return 1;

	SS_CATCH(L"");
	return 0;
}

unsigned int CLexemeBoard::MoveNext()
{
	SS_TRY;

	if (m_iCurrLexem == m_uiTop)
		return 0;

	m_iCurrLexem++;
	if (m_iCurrLexem != m_uiTop)
		return 1;
	else
		return 0;

	SS_CATCH(L"");
	return 0;
}

LPCWSTR CLexemeBoard::GetLexeme()
{
	SS_TRY;

	if (m_iCurrLexem == m_uiTop)
		return NULL;

	USES_CONVERSION;
	return T2CW(m_vLexems[m_iCurrLexem]->szText);

	SS_CATCH(L"");
	return NULL;
}

LPCSTR CLexemeBoard::GetLexemeA()
{
	SS_TRY;

	if (m_iCurrLexem == m_uiTop)
		return NULL;

	USES_CONVERSION;
	return T2CA(m_vLexems[m_iCurrLexem]->szText);

	SS_CATCH(L"");
	return NULL;
}

unsigned int CLexemeBoard::GetAbsoluteIndex()
{
	SS_TRY;

	if (m_iCurrLexem == m_uiTop)
		return -1;

	return m_vLexems[m_iCurrLexem]->iAbsPosition;

	SS_CATCH(L"");
	return -1;
}

unsigned int CLexemeBoard::GetAbsoluteSymbolIndex()
{
	SS_TRY;

	if (m_iCurrLexem == m_uiTop)
		return -1;

	return m_vLexems[m_iCurrLexem]->iAbsCharPosition;

	SS_CATCH(L"");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// CLexemeBoard interface

ELexemType CLexemeBoard::GetLexemType()
{
	if (m_uiTop == 0)
		return accUndefined;

	return m_vLexems[m_iCurrLexem]->LexemType;
}

bool CLexemeBoard::IsEnd()
{
	return m_iCurrLexem == m_uiTop;
}

bool CLexemeBoard::MoveLast()
{
	if (m_uiTop == 0)
		return false;
	m_iCurrLexem = m_uiTop - 1;
	return true;
}

bool CLexemeBoard::MovePrev()
{
	if (m_iCurrLexem == -1)
		return false;

	m_iCurrLexem--;
	return (m_iCurrLexem != -1);
}

void CLexemeBoard::Reset()
{
	m_uiTop = 0;
	m_iCurrLexem = -1;
}

bool CLexemeBoard::AddLexeme(LPCTSTR szText, ELexemType LexemType, int iAbsPosition, int iAbsCharPosition)
{
	SLexem* pLexem = m_vLexems[m_uiTop++];
	pLexem->szText = szText;
	pLexem->LexemType = LexemType;
	pLexem->iAbsPosition = iAbsPosition;
	pLexem->iAbsCharPosition = iAbsCharPosition;
	pLexem->iHeaderLevelBegin = 0;
	pLexem->iHeaderLevelEnd = 0;

	return (m_uiTop < m_vLexems.capacity());
}

ELexemType CLexemeBoard::GetFirstLexem()
{
	if (m_uiTop == 0)
		return accUndefined;
	return m_vLexems[0]->LexemType;
}

ELexemType CLexemeBoard::GetLastLexem()
{
	if (m_uiTop == 0)
		return accUndefined;
	return m_vLexems[m_uiTop - 1]->LexemType;
}

unsigned int CLexemeBoard::GetFirstLexemPosition()
{
	if (m_uiTop == 0)
		return -1;
	return m_vLexems[0]->iAbsPosition;
}

unsigned int CLexemeBoard::GetLastLexemPosition()
{
	if (m_uiTop == 0)
		return -1;
	return m_vLexems[m_uiTop - 1]->iAbsPosition;
}

ELexemType CLexemeBoard::GetLexemType(int nToLeft)
{
	assert(m_iCurrLexem - nToLeft >= 0);
	assert(m_iCurrLexem - nToLeft < (int) m_uiTop);
	return m_vLexems[m_iCurrLexem - nToLeft]->LexemType;
}

LPCTSTR CLexemeBoard::GetLexemString(int nToLeft)
{
	assert(m_iCurrLexem - nToLeft >= 0);
	assert(m_iCurrLexem - nToLeft < (int) m_uiTop);
	return m_vLexems[m_iCurrLexem - nToLeft]->szText;
}

UINT CLexemeBoard::GetLexemPosition(int nToLeft)
{
	assert(m_iCurrLexem - nToLeft >= 0);
	assert(m_iCurrLexem - nToLeft < (int) m_uiTop);
	return m_vLexems[m_iCurrLexem - nToLeft]->iAbsPosition;
}

UINT CLexemeBoard::GetLexemSymbolPosition(int nToLeft)
{
	assert(m_iCurrLexem - nToLeft >= 0);
	assert(m_iCurrLexem - nToLeft < (int) m_uiTop);
	return m_vLexems[m_iCurrLexem - nToLeft]->iAbsCharPosition;
}

void CLexemeBoard::SetLexemeType(ELexemType eType)
{
	if (m_iCurrLexem == m_uiTop)
		return;

	m_vLexems[m_iCurrLexem]->LexemType = eType;
}

void CLexemeBoard::SetHeaderLevelBegin(int iHeaderLevel)
{
	assert(m_iCurrLexem >= 0);
	assert(m_iCurrLexem < (int) m_uiTop);

	m_vLexems[m_iCurrLexem]->iHeaderLevelBegin = iHeaderLevel;
}

int CLexemeBoard::GetHeaderLevelBegin()
{
	assert(m_iCurrLexem >= 0);
	assert(m_iCurrLexem < (int) m_uiTop);

	return m_vLexems[m_iCurrLexem]->iHeaderLevelBegin;
}

void CLexemeBoard::SetHeaderLevelEnd(int iHeaderLevel)
{
	assert(m_iCurrLexem >= 0);
	assert(m_iCurrLexem < (int) m_uiTop);

	m_vLexems[m_iCurrLexem]->iHeaderLevelEnd = iHeaderLevel;
}

int CLexemeBoard::GetHeaderLevelEnd()
{
	assert(m_iCurrLexem >= 0);
	assert(m_iCurrLexem < (int) m_uiTop);

	return m_vLexems[m_iCurrLexem]->iHeaderLevelEnd;
}

}
}
