#include "stdafx.h"
#include "Paragraph.h"

#include "Sentence.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CParagraph::CParagraph() :
		m_uiLevel(0),
		m_uiNumber(0),
		m_iRelevance(0),
		m_eTextCutType(tctText)
{
}

CParagraph::~CParagraph()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IParagraph interface

void CParagraph::AddSentence(ISentence* pSentence)
{
	AddChild(dynamic_cast<CSentence*>(pSentence));
}

IParagraph* CParagraph::GetNextParagraph()
{
	return dynamic_cast<CParagraph*>(GetRight());
}

IParagraph* CParagraph::GetPrevParagraph()
{
	return dynamic_cast<CParagraph*>(GetLeft());
}

ISentence* CParagraph::GetFirstSentence()
{
	return dynamic_cast<CSentence*>(GetFirstChild());
}

ISentence* CParagraph::GetLastSentence()
{
	return dynamic_cast<CSentence*>(GetLastChild());
}

void CParagraph::ClearParagraph()
{
	ReleaseChildren();
	m_uiLevel = 0;
	m_uiNumber = 0;
	m_iRelevance = 0;
	m_eTextCutType = tctText;
}

IParagraph* CParagraph::ReleaseParagraph()
{
	return dynamic_cast<CParagraph*>(ReleaseItem());
}

void CParagraph::SetIndexTypeName(const wchar_t* wszName)
{
	m_strIndexTypeName = wszName;
}

const wchar_t* CParagraph::GetIndexTypeName()
{
	return m_strIndexTypeName.c_str();
}

void CParagraph::SetRelevance(int iRelevance)
{
	m_iRelevance = iRelevance;
}

int CParagraph::GetRelevance()
{
	return m_iRelevance;
}

ETextCutType CParagraph::GetType()
{
	return m_eTextCutType;
}

void CParagraph::SetType(ETextCutType eTextCutType)
{
	m_eTextCutType = eTextCutType;
}

unsigned int CParagraph::GetLevel()
{
	return m_uiLevel;
}

void CParagraph::SetLevel(unsigned int uiLevel)
{
	m_uiLevel = uiLevel;
}

unsigned int CParagraph::GetNumber()
{
	return m_uiNumber;
}

void CParagraph::SetNumber(unsigned int uiNumber)
{
	m_uiNumber = uiNumber;
}

}
}
}
