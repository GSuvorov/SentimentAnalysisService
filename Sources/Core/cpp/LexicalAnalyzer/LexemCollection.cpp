#include "stdafx.h"
#include "LexemCollection.h"


namespace SS
{
namespace LexicalAnalyzer
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CLexemCollection::CLexemCollection() : 
		m_pBoard(NULL),
		m_pReparser(NULL),
		m_iCurrUnit(-1)
{
	m_vUnits.reserve(1000);
}

CLexemCollection::~CLexemCollection()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CLexemCollection interface

void CLexemCollection::Init(IText* pText, Reparser::IReparser* pReparser, CParagraph* pBackupPara,
							CIndexLoader* pIndexLoader)
{
	SS_TRY;

	m_pBoard = NULL;
	m_pReparser = pReparser;
	m_iCurrUnit = -1;

	m_vUnits.erase(m_vUnits.begin(), m_vUnits.end());
	for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		if(!pPara->GetFirstSentence() || pPara->GetType() == tctTable)
			continue;

		// back up relevance to restore it in new IText
		pBackupPara->AddData(pPara->GetRelevance(), pPara->GetNumber(), pPara->GetType(), pPara->GetLevel(), 
			pPara->GetIndexTypeName());

		for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		{
			for(IUnit* pUnit = pSent->GetFirstUnit(); pUnit; )
			{
				IUnit* pNewUnit = pUnit;
				pUnit = pUnit->Pop();
				pIndexLoader->FillWord(pNewUnit);
				m_vUnits.push_back(pNewUnit);
			}
		}

		if(pPara != pText->GetLastParagraph())
			m_vUnits.push_back(NULL);
	}

	pText->ClearText();

	SS_CATCH(L"");
}

void CLexemCollection::Init(CLexemeBoard* pBoard, Reparser::IReparser* pReparser)
{
	m_pBoard = pBoard;
	m_pReparser = pReparser;
}

bool CLexemCollection::IsEnd()
{
	return m_pBoard ? m_pBoard->IsEnd() : m_iCurrUnit >= (int) m_vUnits.size();
}

unsigned int CLexemCollection::MoveFirst()
{
	if(m_pBoard)
		return m_pBoard->MoveFirst();

	m_iCurrUnit = 0;
	return m_vUnits.empty() ? 0 : 1;
}

unsigned int CLexemCollection::MoveNext()
{
	if(m_pBoard)
		return m_pBoard->MoveNext();

	return (int) m_vUnits.size() >= ++m_iCurrUnit;
}

ATL::CAtlString CLexemCollection::GetString(int nCount, int nToLeft)
{
	ATL::CAtlString result;
	for(int i = 0; i < nCount; i++)
		result.Append(GetLexemString(nToLeft - i));
	return result;
}

ELexemType CLexemCollection::GetLexemType(int nToLeft)
{
	LPCTSTR szLexem = GetLexemString(nToLeft);
	if(!szLexem)
		return accParaBreak;

	m_pReparser->Reparse(szLexem, Reparser::rmRestore, &m_oResultBoard);
	if(m_oResultBoard.IsEmpty())
		return accWord;

	ELexemType lt = m_oResultBoard.GetFirstLexem()->LexemType;
	m_oResultBoard.Clear();
	if(!m_pBoard && lt == accParaBreak)
		return accWord;

	return lt;
}

LPCTSTR CLexemCollection::GetLexemString(int nToLeft)
{
	if(m_pBoard)
		return m_pBoard->GetLexemString(nToLeft);

	if(!m_vUnits[m_iCurrUnit - nToLeft])
		return NULL;

	USES_CONVERSION;
	return W2CT(m_vUnits[m_iCurrUnit - nToLeft]->GetWord());
}

UINT CLexemCollection::GetLexemPosition(int nToLeft)
{
	if(m_pBoard)
		return m_pBoard->GetLexemPosition(nToLeft);
	if(!m_vUnits[m_iCurrUnit - nToLeft])
		return -1;
	return m_vUnits[m_iCurrUnit - nToLeft]->GetPosition();
}

UINT CLexemCollection::GetLexemSymbolPosition(int nToLeft)
{
	if(m_pBoard)
		return m_pBoard->GetLexemSymbolPosition(nToLeft);
	if(!m_vUnits[m_iCurrUnit - nToLeft])
		return -1;
	wstring word = m_vUnits[m_iCurrUnit - nToLeft]->GetWord();
	return word.length();
}

IUnit* CLexemCollection::GetUnit(int nToLeft)
{
	if(m_pBoard)
		return NULL;
	return m_vUnits[m_iCurrUnit - nToLeft];
}

void CLexemCollection::SetHeaderLevelBegin(int iHeaderLevel)
{
	if(m_pBoard)
		m_pBoard->SetHeaderLevelBegin(iHeaderLevel);
}

int CLexemCollection::GetHeaderLevelBegin()
{
	if(m_pBoard)
		return m_pBoard->GetHeaderLevelBegin();
	else
		return 0;
}

void CLexemCollection::SetHeaderLevelEnd(int iHeaderLevel)
{
	if(m_pBoard)
		m_pBoard->SetHeaderLevelEnd(iHeaderLevel);
}

int CLexemCollection::GetHeaderLevelEnd()
{
	if(m_pBoard)
		return m_pBoard->GetHeaderLevelEnd();
	else
		return 0;
}

void CLexemCollection::GetData(int nToLeft, IUnit** ppUnit, LPCTSTR* pszLexem, ELexemType* plt, 
							   UINT* puiPosition, UINT* puiSymbolPosition)
{
	SS_TRY;

	if(m_pBoard)
	{
		*pszLexem = m_pBoard->GetLexemString(nToLeft);
		*puiPosition = m_pBoard->GetLexemPosition(nToLeft);
		*puiSymbolPosition = m_pBoard->GetLexemSymbolPosition(nToLeft);
		*ppUnit = NULL;
	}
	else
	{
		USES_CONVERSION;
		*pszLexem = m_vUnits[m_iCurrUnit - nToLeft]->GetWord();
		*puiPosition = m_vUnits[m_iCurrUnit - nToLeft]->GetPosition();
		*puiSymbolPosition = m_vUnits[m_iCurrUnit - nToLeft]->GetFirstCharPosition();
		*ppUnit = m_vUnits[m_iCurrUnit - nToLeft];
	}

	m_pReparser->Reparse(*pszLexem, Reparser::rmRestore, &m_oResultBoard);
	if(m_oResultBoard.IsEmpty())
		*plt = accWord;
	else
	{
		*plt = m_oResultBoard.GetFirstLexem()->LexemType;
		m_oResultBoard.Clear();
	}

	SS_CATCH(L"");
}

}
}
