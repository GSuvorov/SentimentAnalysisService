#include "stdafx.h"
#include "ITextFile.h"

#include <assert.h>

#include "..\..\ASSInterface\Constants.h"
using namespace SS::Constants;


namespace SS
{
namespace Core
{
namespace BlackBox
{
namespace XML
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CITextFile::CITextFile(IBlackBoxTextStorage* pStorage) :
		m_pStorage(pStorage)
{
	m_oFeatureHelper.Init(pStorage->GetLoadManager());
}

CITextFile::~CITextFile()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CITextFile interface

_bstr_t CITextFile::GetXML(IText* pText)
{
	if(!CreateDocument())
		return L"";

	xml2::IXMLDOMNodePtr pRoot = AddNode(m_pDoc, L"TestFile");
	if(pRoot == NULL)
		return L"";

	WriteText(pText, pRoot);
	return m_pDoc->Getxml();
}

void CITextFile::SetXML(IText* pText, BSTR bstrXML)
{
	if(!CreateDocument())
		return;

	m_pDoc->loadXML(_bstr_t(bstrXML));
	ReadText(pText);
}

void CITextFile::WriteTestFile(BSTR bstrTestFileName, IText* pText, LPCTSTR szXsl)
{
	if(!CreateDocument())
		return;

	xml2::IXMLDOMNodePtr pRoot = AddNode(m_pDoc, L"TestFile");
	if(pRoot == NULL)
		return;

	AddNode(pRoot, L"Question", m_bstrQuestion);
	AddCDataSection(pRoot, L"SearchText", m_bstrText);
	AddNode(pRoot, L"DocStructure", m_bstrDocStructure);

	WriteText(pText, pRoot);

	if(szXsl)
		Transform(szXsl, bstrTestFileName);
	else
		m_pDoc->save(bstrTestFileName);
}

void CITextFile::TransformResult(BSTR bstrTestFileName, LPCTSTR szXsl)
{
	if(m_pDoc == NULL)
		return;

	Transform(szXsl, bstrTestFileName);
}

IText* CITextFile::ReadTestFile(BSTR bstrTestFileName)
{
	if(!CreateDocument())
		return NULL;

	m_pDoc->load(bstrTestFileName);

	m_bstrQuestion = GetTagText(L"TestFile/Question");
	m_bstrText = GetTagText(L"TestFile/SearchText");
	m_bstrDocStructure = GetTagText(L"TestFile/DocStructure");

	IText* pText = m_pStorage->CreateText();
	ReadText(pText);

	return pText;
}

void CITextFile::SetQuestion(BSTR bstrQuestion)
{
	m_bstrQuestion = bstrQuestion;
}

void CITextFile::SetText(BSTR bstrText)
{
	m_bstrText = bstrText;
}

void CITextFile::SetDocStructure(BSTR bstrDocStructure)
{
	m_bstrDocStructure = bstrDocStructure;
}

BSTR CITextFile::GetQuestion()
{
	return m_bstrQuestion;
}

BSTR CITextFile::GetText()
{
	return m_bstrText;
}

BSTR CITextFile::GetDocStructure()
{
	return m_bstrDocStructure;
}

/////////////////////////////////////////////////////////////////////////////////////
// CITextFile functions (writing test file)

void CITextFile::WriteText(IText* pText, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pTextRoot = AddNode(pRoot, L"Text");
	for(IParagraph* pPara = pText->GetFirstParagraph(); pPara; pPara = pPara->GetNextParagraph())
		WriteParagraph(pPara, pTextRoot);
}

void CITextFile::WriteParagraph(IParagraph* pPara, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pParaRoot = AddNode(pRoot, L"Paragraph");
	SetXMLAttr(pParaRoot, L"Relevance", L"int", _variant_t(pPara->GetRelevance() / RELEVANCE_MULTIPLIER));
	SetXMLAttr(pParaRoot, L"Number", L"int", _variant_t(pPara->GetNumber()));
	if(pPara->GetType() != tctText)
	{
		SetXMLAttr(pParaRoot, L"Type", L"int", _variant_t(pPara->GetType()));
		SetXMLAttr(pParaRoot, L"Level", L"int", _variant_t(pPara->GetLevel()));
	}

	for(ISentence* pSent = pPara->GetFirstSentence(); pSent; pSent = pSent->GetNextSentence())
		WriteSentence(pSent, pParaRoot);
}

void CITextFile::WriteSentence(ISentence* pSent, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pSentRoot = AddNode(pRoot, L"Sentence");
	SetXMLAttr(pSentRoot, L"Relevance", L"int", _variant_t(pSent->GetRelevance() / RELEVANCE_MULTIPLIER));

	for(IUnit* pUnit = pSent->GetFirstUnit(); pUnit; pUnit = pUnit->GetRightUnit())
		WriteUnit(pUnit, pSentRoot);
}

void CITextFile::WriteUnit(IUnit* pParentUnit, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pUnitRoot = AddNode(pRoot, L"Unit", (BSTR) pParentUnit->GetWord());

	SetXMLAttr(pUnitRoot, L"WordPos", L"int", _variant_t(pParentUnit->GetPosition()));
	SetXMLAttr(pUnitRoot, L"CharPos", L"int", _variant_t(pParentUnit->GetFirstCharPosition()));

	if(pParentUnit->GetHighlightType() != hltNone)
		SetXMLAttr(pUnitRoot, L"Highlight", L"int", _variant_t(pParentUnit->GetHighlightType()));
	if(m_oFeatureHelper.IsTitleUnit(pParentUnit))
		SetXMLAttr(pUnitRoot, L"Title", L"int", _variant_t(1));

	for(IIndex* pIndex = pParentUnit->GetFirstIndex(); pIndex; pIndex = pParentUnit->GetNextIndex())
		WriteIndex(pIndex, pUnitRoot);

	for(IUnit* pUnit = pParentUnit->GetLeftChildUnit(); pUnit; pUnit = pUnit->GetRightUnit())
		WriteUnit(pUnit, pUnitRoot);
}

void CITextFile::WriteIndex(IIndex* pIndex, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pIndexRoot = AddNode(pRoot, L"Index");

	WriteDictionaryIndex(pIndex->GetDictionaryIndex(), pIndexRoot);
	for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
		WriteFeature(pFeature, pIndex, pIndexRoot);
}

void CITextFile::WriteDictionaryIndex(IDictionaryIndex* pIndex, xml2::IXMLDOMNodePtr pRoot)
{
	USES_CONVERSION;

	TDictionaryIndex oInd;
	for(bool b = pIndex->GetFirst(&oInd); b; b = pIndex->GetNext(&oInd))
	{
		xml2::IXMLDOMNodePtr pDictIdx = AddNode(pRoot, L"DictIdx");

		// save index as int
		SetXMLAttr(pDictIdx, L"Val", L"int", _variant_t(oInd.GetDictionaryIndex()));

		// save index as string
		string str = m_oFeatureHelper.GetDescription(oInd);
		SetXMLAttr(pDictIdx, L"Descr", L"string", _variant_t(A2W(str.c_str())));
	}
}

void CITextFile::WriteFeature(IFeature* pFeature, IIndex* pParentIndex, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pFeatureRoot = AddNode(pRoot, L"Feature");

	USES_CONVERSION;
	BSTR bstr = A2W(m_oFeatureHelper.GetDescription(pFeature, pParentIndex).c_str());
	SetXMLAttr(pFeatureRoot, L"Descr", L"string", _variant_t(bstr));

	SetXMLAttr(pFeatureRoot, L"midx", L"int", _variant_t(pFeature->GetMorphoIndex()));
	SetXMLAttr(pFeatureRoot, L"minf", L"int", _variant_t(pFeature->GetMorphoInfo()));
	SetXMLAttr(pFeatureRoot, L"pos", L"int", _variant_t(pFeature->GetPartOfSpeech()));
}

/////////////////////////////////////////////////////////////////////////////////////
// CITextFile functions (reading test file)

void CITextFile::ReadText(IText* pText)
{
	xml2::IXMLDOMNodeListPtr pList = m_pDoc->selectNodes(L"TestFile/Text/Paragraph");
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		IParagraph* pPara = m_pStorage->CreateParagraph();
		pText->AddParagraph(pPara);

		ReadParagraph(pPara, pNode);
	}
}

void CITextFile::ReadParagraph(IParagraph* pPara, xml2::IXMLDOMNodePtr pTextNode)
{
	pPara->SetRelevance(GetIntAttribute(pTextNode, L"Relevance", 0) * RELEVANCE_MULTIPLIER);
	pPara->SetNumber(GetIntAttribute(pTextNode, L"Number", 0));
	pPara->SetType((ETextCutType) GetIntAttribute(pTextNode, L"Type", (int) tctText));
	pPara->SetLevel(GetIntAttribute(pTextNode, L"Level", 0));

	xml2::IXMLDOMNodeListPtr pList = pTextNode->selectNodes(L"Sentence");
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		ISentence* pSent = m_pStorage->CreateSentence();
		pPara->AddSentence(pSent);

		ReadSentence(pSent, NULL, pNode);
	}
}

bool CITextFile::ReadSentence(ISentence* pSentParent, IUnit* pUnitParent, xml2::IXMLDOMNodePtr pTextNode)
{
	if(pSentParent)
		pSentParent->SetRelevance(GetIntAttribute(pTextNode, L"Relevance") * RELEVANCE_MULTIPLIER);

	xml2::IXMLDOMNodeListPtr pList = pTextNode->selectNodes(L"Unit");
	if(pList->Getlength() == 0)
		return false;

	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		IUnit* pUnit = ReadUnit(pNode);
		if(pUnitParent)
			pUnitParent->AddUnitVertical(pUnit);
		else if(pSentParent)
			pSentParent->AddUnit(pUnit);
		else
			assert(false);

		ReadSentence(NULL, pUnit, pNode);
	}

	return true;
}

IUnit* CITextFile::ReadUnit(xml2::IXMLDOMNodePtr pTextNode)
{
	IUnit* pUnit = m_pStorage->CreateUnit();
	ReadIndex(pUnit, pTextNode);

	pUnit->SetFirstCharPosition(GetIntAttribute(pTextNode, L"CharPos"));
	pUnit->SetPosition(GetIntAttribute(pTextNode, L"WordPos"));

	xml2::IXMLDOMNodePtr	pTextChild = pTextNode->selectSingleNode(L"text()"),
							pNode = pTextNode->selectSingleNode(L"Unit");
	_bstr_t bstrWord = pTextChild ? pTextChild->Gettext() : L"";
	if(*bstrWord.GetBSTR() != 0 || pNode == NULL)
	{
		int iHighlight = GetIntAttribute(pTextNode, L"Highlight", hltNone);
		int iTitle = GetIntAttribute(pTextNode, L"Title");

		pUnit->SetWord(bstrWord);
		pUnit->SetHighlightType((EHighlightType) iHighlight);
		if(iTitle != -1)
			m_oFeatureHelper.SetTitleUnit(pUnit);
	}

	return pUnit;
}

void CITextFile::ReadIndex(IUnit* pUnit, xml2::IXMLDOMNodePtr pTextNode)
{
	xml2::IXMLDOMNodeListPtr pList = pTextNode->selectNodes(L"Index");
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		IIndex* pIndex = m_pStorage->CreateIndex();
		pUnit->AddIndex(pIndex);

		ReadDictionaryIndex(pIndex, pNode);
		ReadFeature(pIndex, pNode);
	}
}

void CITextFile::ReadDictionaryIndex(IIndex* pIndex, xml2::IXMLDOMNodePtr pTextNode)
{
	xml2::IXMLDOMNodeListPtr pList = pTextNode->selectNodes(L"DictIdx");
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		int iValue = GetIntAttribute(pNode, L"Val");
		pIndex->GetDictionaryIndex()->AppendIndex(TDictionaryIndex(iValue));
	}
}

void CITextFile::ReadFeature(IIndex* pIndex, xml2::IXMLDOMNodePtr pTextNode)
{
	xml2::IXMLDOMNodeListPtr pList = pTextNode->selectNodes(L"Feature");
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		int iMorphoIndex = GetIntAttribute(pNode, L"midx", 0);
		int iMorphoInfo = GetIntAttribute(pNode, L"minf", 0);
		int iPartOfSpeech = GetIntAttribute(pNode, L"pos", 0);

		IFeature* pFeature = m_pStorage->CreateFeature();
		pFeature->SetMorphoIndex(iMorphoIndex);
		pFeature->SetMorphoInfo(iMorphoInfo);
		pFeature->SetPartOfSpeech(iPartOfSpeech);

		pIndex->AddFeature(pFeature);
	}
}

}
}
}
}
