#include "stdafx.h"
#include "TextStorage.h"

#include "Text.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Unit.h"
#include "Index.h"
#include "Feature.h"
#include "TextFeature.h"
#include "ObjectDescr.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CTextStorage::CTextStorage()
{
}

CTextStorage::~CTextStorage()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CTextStorage::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID == IID_BlackBoxTextStorage)
		*pBase = (IBlackBoxTextStorage*) this;
	else if(pIID == IID_Base)
		*pBase = (SS::Interface::IBase*) this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CTextStorage::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// IBlackBoxTextStorage interface

IText* CTextStorage::CreateText()
{
	return new CText(this);
}

IParagraph* CTextStorage::CreateParagraph()
{
	return new CParagraph;
}

ISentence* CTextStorage::CreateSentence()
{
	return new CSentence;
}

IUnit* CTextStorage::CreateUnit()
{
	return new CUnit;
}

IIndex* CTextStorage::CreateIndex()
{
	return new CIndex;
}

IFeature* CTextStorage::CreateFeature()
{
	return new CFeature;
}

ITextFeature* CTextStorage::CreateTextFeature()
{
	return new SS::Core::BlackBox::CTextFeature;
}

IObjectDescr*	CTextStorage::CreateObjectDescr()
{
	return new SS::Core::BlackBox::CObjectDescr();
}

}
}
}
