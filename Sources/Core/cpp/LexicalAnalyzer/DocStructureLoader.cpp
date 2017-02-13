#include "stdafx.h"
#include "DocStructureLoader.h"


namespace SS
{
namespace LexicalAnalyzer
{

/////////////////////////////////////////////////////////////////////////////////////
// Constructors
/////////////////////////////////////////////////////////////////////////////////////

CDocStructureLoader::CDocStructureLoader() :
		m_pBoard(NULL),
		m_pStorage(NULL),
		m_pIndexLoader(NULL),
		m_pText(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CDocStructureLoader interface

void CDocStructureLoader::Init(IBlackBoxTextStorage* pStorage, CIndexLoader* pIndexLoader)
{
	m_pStorage = pStorage;
	m_pIndexLoader = pIndexLoader;
}

void CDocStructureLoader::ParseDocStructure(BSTR bstrDocStructure)
{
	SS_TRY;

	m_oRangeList.Clear();
	if(!bstrDocStructure)
		return;

	CDocStructureReader oDocStructure;
	oDocStructure.ParseXml(bstrDocStructure, &m_oRangeList);
	m_oRangeList.MoveFirst();

	SS_CATCH(L"");
}

void CDocStructureLoader::AnalyzeDocStructure(CLexemeBoard* pBoard, IText* pText)
{
	SS_TRY;

	m_pBoard = pBoard;
	m_pText = pText;
	for(UINT ui = m_pBoard->MoveFirst(); ui; ui = m_pBoard->MoveNext())
		NextWord();

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CDocStructureLoader functions

void CDocStructureLoader::NextWord()
{
	SS_TRY;

	CommonContainers::ITextFeature* pFeature = m_pText->GetTextFeature();
	int nCharBegin = m_pBoard->GetAbsoluteSymbolIndex(),
		nCharEnd = nCharBegin + (int) wcslen(m_pBoard->GetLexeme()),
		nWord = m_pBoard->GetAbsoluteIndex();

	// set begins of all ranges, that start with current word
	SStyle* pStyle = m_oRangeList.SetWordBegin(nWord, nCharEnd);
	for(; pStyle; pStyle = m_oRangeList.SetWordBegin(nWord, nCharEnd))
	{
		if(pStyle->RangeType == rtHeader)
			m_pBoard->SetHeaderLevelBegin(pStyle->iLevel - m_oRangeList.GetMinLevel() + 1);
	}

	// set ends of all ranges, that start with current word
	pStyle = m_oRangeList.SetWordEnd(nWord, nCharBegin, nCharEnd);
	for(; pStyle; pStyle = m_oRangeList.SetWordEnd(nWord, nCharBegin, nCharEnd))
	{
		if(pStyle->nWordBegin < 0)
			continue;

		if(pStyle->RangeType == rtHeader)
			m_pBoard->SetHeaderLevelEnd(pStyle->iLevel - m_oRangeList.GetMinLevel() + 1);
	}

	SS_CATCH(L"");
}

}
}
