#include "stdafx.h"
#include "Text.h"

#include "Paragraph.h"

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;

#include "GetXML\ITextFile.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CText::CText(IBlackBoxTextStorage* pStorage) :
		m_pTextFeature(NULL),
		m_pStorage(pStorage)
{
}

CText::~CText()
{
	delete m_pTextFeature;
}

/////////////////////////////////////////////////////////////////////////////////////
// IText interface

ITextFeature* CText::GetTextFeature()
{
	if(!m_pTextFeature)
		m_pTextFeature = new CTextFeature;
	return m_pTextFeature;
}

void CText::AddParagraph(IParagraph* pParagraph)
{
	AddChild(dynamic_cast<CParagraph*>(pParagraph));
}

IParagraph* CText::GetFirstParagraph()
{
	return dynamic_cast<CParagraph*>(GetFirstChild());
}

IParagraph* CText::GetLastParagraph()
{
	return dynamic_cast<CParagraph*>(GetLastChild());
}

void CText::ClearText()
{
	ReleaseChildren();
}

void CText::ReleaseText()
{
	ReleaseItem();
}

void CText::Normalize()
{
	for(IParagraph* pPara = GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
	{
		for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		{
			for(CSentenceIterator it = pSent->GetFirstUnit(); *it; ++it)
				CopyIndexesToChildren(*it);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// XML functions

void CText::GetXML(std::wstring* pwXML)
{
	XML::CITextFile oFile(m_pStorage);
	*pwXML = oFile.GetXML(this);
}

void CText::SetXML(const std::wstring* pwXML)
{
	ClearText();
	XML::CITextFile oFile(m_pStorage);
	oFile.SetXML(this, (BSTR) (void*) pwXML->c_str());
}

/////////////////////////////////////////////////////////////////////////////////////
// CText functions

void CText::CopyIndexesToChildren(IUnit* pUnit)
{
	if(!pUnit->HasChilds())
		return;

	for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		if(pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() == eitMorphoIndex)
			continue;

		for(IUnit* pChild = pUnit->GetLeftChildUnit(); pChild; pChild = pChild->GetRightUnit())
		{
			if(!FindIndex(pChild, pIndex))
				pChild->AddIndex(Clone(pIndex));
		}
	}

	for(IUnit* pChild = pUnit->GetLeftChildUnit(); pChild; pChild = pChild->GetRightUnit())
		pChild->SetObligatory(pUnit->IsObligatory());
}

IIndex* CText::Clone(IIndex* pIndex)
{
	IIndex* pNewIndex = m_pStorage->CreateIndex();
	pNewIndex->GetDictionaryIndex()->AppendIndex(pIndex->GetDictionaryIndex()->GetFirst());

	for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
	{
		IFeature* pNewFeature = m_pStorage->CreateFeature();
		pNewFeature->SetMorphoIndex(pFeature->GetMorphoIndex());
		pNewFeature->SetMorphoInfo(pFeature->GetMorphoInfo());
		pNewFeature->SetPartOfSpeech(pFeature->GetPartOfSpeech());

		pNewIndex->AddFeature(pNewFeature);
	}

	return pNewIndex;
}

bool CText::FindIndex(IUnit* pUnit, IIndex* pIndex)
{
	TDictionaryIndex oIndex1 = pIndex->GetDictionaryIndex()->GetFirst();
	for(IIndex* p = pUnit->GetFirstIndex(); p; p = pUnit->GetNextIndex())
	{
		TDictionaryIndex oIndex2 = p->GetDictionaryIndex()->GetFirst();
		if(oIndex1 == oIndex2)
			return true;
	}

	return false;
}

}
}
}
